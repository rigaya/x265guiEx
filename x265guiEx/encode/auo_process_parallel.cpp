//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#include <Windows.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <utility>
#include <vector>
#include "auo.h"
#include "auo_version.h"
#include "auo_system.h"
#include "auo_audio.h"
#include "auo_frm.h"
#include "auo_mux.h"
#include "auo_util.h"
#include "auo_encode.h"
#include "auo_runbat.h"

typedef struct {
	int id;
	CONF_GUIEX conf;
	OUTPUT_INFO oip;
	PRM_ENC pe;
	const SYSTEM_DATA *sys_dat;
	char savefile[MAX_PATH_LEN]; //保存ファイル名
	char filebase[MAX_PATH_LEN]; //ファイルベース名
} TASK_INFO;

class TaskController {
private:
	char task_log[MAX_PATH_LEN];       //ログファイル名
	std::vector<TASK_INFO> task_list;  //処理するタスクのリスト
	HANDLE thread;                     //待機・監視スレッド
	HANDLE he_abort;                   //終了スレッド
	CRITICAL_SECTION critical_section; //同期用
public:
	TaskController::TaskController() {
		thread = NULL;
		he_abort = NULL;
		memset(task_log, 0, sizeof(task_log));
	}
	TaskController::~TaskController() {
		thread_close();
		task_list.empty();
		memset(task_log, 0, sizeof(task_log));
	}
	int get_current_task_waiting() {
		return task_list.size();
	}
	AUO_RESULT task_add(const CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
		if (thread_start())
			return AUO_RESULT_ERROR;

		TASK_INFO task = { 0 };
		task.conf = *conf;
		task.oip = *oip;
		task.pe = *pe;
		task.sys_dat = sys_dat;
		get_filebase_from_tmp_name(task.filebase, _countof(task.filebase), pe->temp_filename);

		//oipの修正
		//ポインタで指定しているものはすべて無効となるので、あらかじめ無効としておく
		task.oip.func_get_flag       = NULL;
		task.oip.func_get_audio      = NULL;
		task.oip.func_get_video      = NULL;
		task.oip.func_get_video_ex   = NULL;
		task.oip.func_is_abort       = NULL;
		task.oip.func_rest_time_disp = NULL;
		task.oip.func_update_preview = NULL;
		task.oip.savefile = task.savefile;
		strcpy_s(task.savefile, _countof(task.savefile), oip->savefile);
		
		//task_listに追加するあたりはスレッド間での競合を阻止する
		EnterCriticalSection(&critical_section);
		task.id = get_id_unsed();
		task_list.push_back(task);
		LeaveCriticalSection(&critical_section);

		return AUO_RESULT_SUCCESS;
	}
private:
	//めんどくさくないスレッドの立て方
	static unsigned int __stdcall thread_func_wrapper(void *obj) {
		TaskController *control = reinterpret_cast<TaskController *>(obj);
		return control->thread_func();
	}
	//使われていないID番号を取得
	int get_id_unsed() {
		int check_id = 0;
		bool used = true;
		for ( ; used; check_id++) {
			used = false;
			for (auto i_task : task_list) {
				used |= (check_id == i_task.id);
			}
		}
		return check_id;
	}
	void get_filebase_from_tmp_name(char *filebase, size_t nSize, const char *tempfile) {
		const char *ptr = PathFindExtension(tempfile);
		if (ptr) {
			for (int i = 0; ptr && i < 3; i++) {
				ptr--;
				for (; *ptr != '_'; ptr--) {
					if (ptr == tempfile) {
						ptr = NULL;
						break;
					}
				}
			}
			//int a = 0, b = 0;
			//if (2 != sscanf(ptr, "_%d_%d_tmp", &a, &b)) {
			//	b = 0;
			//}
			strncpy_s(filebase, nSize, tempfile, ptr - tempfile);
		}
	}
	AUO_RESULT thread_func() {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		BOOL task_empty = 0;
		for (;;) {
			if (WAIT_TIMEOUT != WaitForSingleObject(he_abort, 5000)) {
				he_abort = NULL;
				break;
			}
			ret |= task_check(&task_empty);
			if (task_empty)
				SetEvent(he_abort); //自殺
		}
		return ret;
	}
	AUO_RESULT thread_start() {
		if (he_abort)
			return AUO_RESULT_SUCCESS;

		if (thread)
			thread_close();

		if (   NULL == (he_abort = (HANDLE)CreateEvent(NULL, false, false, NULL))
			|| NULL == (thread = (HANDLE)_beginthreadex(NULL, 0, thread_func_wrapper, this, 0, NULL)))
			return AUO_RESULT_ERROR;

		InitializeCriticalSection(&critical_section);

		return AUO_RESULT_SUCCESS;
	}
	void thread_close() {
		if (he_abort)
			SetEvent(he_abort);
		if (thread) {
			WaitForSingleObject(thread, INFINITE);
			CloseHandle(thread);
		}
		if (he_abort) {
			CloseHandle(he_abort);
		}
		DeleteCriticalSection(&critical_section);
		thread = NULL;
		he_abort = NULL;
	}
	int log_write(const char* mes) {
		int ret = 0;
		if (!str_has_char(task_log))
			return ret;
		FILE *fp = NULL;
		if (0 == fopen_s(&fp, task_log, "a") && NULL != fp) {
			ret = fprintf(fp, "%s", mes);
			fclose(fp);
		}
		return ret;
	}
	AUO_RESULT log_writef(const char *format, ... ) {
		AUO_RESULT ret = 0;
		if (!str_has_char(task_log))
			return ret;
		va_list args;
		int len;
		char *buffer;
		va_start(args, format);
		len = _vscprintf(format, args) + 1; // _vscprintf doesn't count terminating '\0'
		buffer = (char *)malloc(len * sizeof(buffer[0]));
		vsprintf_s(buffer, len, format, args);
		ret = log_write(buffer);
		free(buffer);
		return ret;
	}
	AUO_RESULT combine_files(TASK_INFO *i_task) {
		AUO_RESULT  ret = AUO_RESULT_SUCCESS;
		const char *ext = PathFindExtension(i_task->pe.temp_filename);
		const int buf_max_bytes = 64 * 1024 * 1024; //64MB
		char first_filename[MAX_PATH_LEN];
		sprintf_s(first_filename, _countof(first_filename), "%s_%d_%d_tmp%s", i_task->filebase, 1, i_task->pe.div_max, ext);
		FILE *fp_write = NULL;
		BYTE *buffer = (BYTE *)malloc(buf_max_bytes);
		if (NULL == buffer || fopen_s(&fp_write, first_filename, "ab") || NULL == fp_write) {
			ret = AUO_RESULT_ERROR; log_writef("結合する映像ファイル(%d/%d)のオープンに失敗しました。: %s\n", 1, i_task->pe.div_max, first_filename);
		} else {
			//1_xのファイルの後に、残りのファイルを追記していく
			for (int i = 1; !ret && i < i_task->pe.div_max; i++) {
				char add_filename[MAX_PATH_LEN];
				sprintf_s(add_filename, _countof(add_filename), "%s_%d_%d_tmp%s", i_task->filebase, i+1, i_task->pe.div_max, ext);
				FILE *fp_read = NULL;
				if (fopen_s(&fp_read, add_filename, "rb") || NULL == fp_read) {
					ret = AUO_RESULT_ERROR; log_writef("結合する映像ファイル(%d/%d)のオープンに失敗しました。: %s\n", i+1, i_task->pe.div_max, add_filename);
				} else {
					int bytes_read = 0;
					do {
						bytes_read = fread(buffer, 1, buf_max_bytes, fp_read);
						if (bytes_read)
							fwrite(buffer, 1, bytes_read, fp_write);
					} while (bytes_read == buf_max_bytes);
					fclose(fp_read);
				}
			}
		}
		if (fp_write)
			fclose(fp_write);
		if (buffer)
			free(buffer);

		if (!ret) {
			//動画ファイル名を修正
			sprintf_s(i_task->pe.temp_filename, _countof(i_task->pe.temp_filename), "%s_%d_%d%s", i_task->filebase, i_task->pe.div_max, i_task->pe.div_max, ext);
			if (PathFileExists(i_task->pe.temp_filename))
				remove(i_task->pe.temp_filename);
			rename(first_filename, i_task->pe.temp_filename);
			log_writef("映像ファイルを結合しました。結合ファイル: %s\n", i_task->pe.temp_filename);
		}

		return ret;
	}
	AUO_RESULT combine_timecodes(TASK_INFO *i_task) {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		if (0 == i_task->conf.vid.afs)
			return ret;

		//タイムコードは一度整数に戻して格納していく
		std::vector<int> timecode;
		const char *ext = PathFindExtension(i_task->pe.temp_filename);
		const double fps = i_task->oip.rate * 4.0 / (double)i_task->oip.scale;
		int file_offset = 0;
		for (int i = 0; !ret && i < i_task->pe.div_max; i++) {
			char tc_filename[MAX_PATH_LEN];
			sprintf_s(tc_filename, _countof(tc_filename), "%s_%d_%d%s", i_task->filebase, i+1, i_task->pe.div_max, ext);
			apply_appendix(tc_filename, _countof(tc_filename), tc_filename, i_task->pe.append.tc);
			FILE *fp_read = NULL;
			if (fopen_s(&fp_read, tc_filename, "r") || NULL == fp_read) {
				ret = AUO_RESULT_ERROR; log_writef("結合するタイムコードファイル(%d/%d)のオープンに失敗しました。: %s\n", i+1, i_task->pe.div_max, tc_filename);
			} else {
				char buffer[1024] = { 0 };
				while (!ret && fgets(buffer, _countof(buffer)-1, fp_read)) {
					double value = 0.0;
					if (1 != sscanf_s(buffer, "%lf", &value)) {
						ret = AUO_RESULT_ERROR;
					} else {
						//タイムコードを整数値に変換
						timecode.push_back(file_offset + (int)(value * fps * 0.001 + 0.5));
					}
				}
				fclose(fp_read);

				//ファイルの最後の5フレームから、最終フレームの時間を推定してfile_offsetの計算を行う
				int sum = 0;
				const int max_frame_minus_5 = timecode.size()-5;
				for (DWORD i_frame = max_frame_minus_5; i_frame < timecode.size(); i_frame++) {
					sum += timecode[i_frame];
				}
				file_offset = timecode[timecode.size()-1] + (int)((double)(sum - timecode[max_frame_minus_5]) / 5.0 + 0.5);
			}
		}
		if (!ret) {
			char tc_filename[MAX_PATH_LEN];
			sprintf_s(tc_filename, _countof(tc_filename), "%s_%d_%d%s", i_task->filebase, i_task->pe.div_max, i_task->pe.div_max, ext);
			apply_appendix(tc_filename, _countof(tc_filename), tc_filename, i_task->pe.append.tc);
			FILE *tcfile = NULL;
			if (fopen_s(&tcfile, tc_filename, "w") || NULL == tcfile) {
				ret = AUO_RESULT_ERROR; log_writef("結合タイムコードファイルのオープンに失敗しました。: %s\n", tc_filename);
			} else {
				{
					//もともとのx_x.timecodeのファイルをoldに除けておく
					char tc_filename_old[MAX_PATH_LEN];
					sprintf_s(tc_filename_old, _countof(tc_filename_old), "%s_%d_%d_old%s", i_task->filebase, i_task->pe.div_max, i_task->pe.div_max, ext);
					apply_appendix(tc_filename_old, _countof(tc_filename_old), tc_filename_old, i_task->pe.append.tc);
					if (PathFileExists(tc_filename_old))
						remove(tc_filename_old);
					rename(tc_filename, tc_filename_old);
				}
				//出力
				const double tm_multi = 1000.0 / fps;
				fprintf(tcfile, "# timecode format v2\r\n");
				for (auto time : timecode) {
					fprintf(tcfile, "%.6lf\n", time * tm_multi);
				}
				fclose(tcfile);
				log_writef("結合タイムコードファイルを出力しました。: %s\n", tc_filename);
			}
		}
		return ret;
	}
	AUO_RESULT task_run(TASK_INFO *i_task) {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		log_writef("%sの処理を行います。\n", i_task->filebase);

		//結合
		if (!ret) ret |= combine_files(i_task);

		//タイムコード結合
		if (!ret) ret |= combine_timecodes(i_task);

		//mux以降の後回しにした処理
		log_writef("%sをmuxします。\n", i_task->filebase);
		i_task->pe.muxer_to_be_used = check_muxer_to_be_used(&i_task->conf, i_task->pe.video_out_type, (i_task->oip.flag & OUTPUT_INFO_FLAG_AUDIO) != 0);
		if (!ret) ret |= mux(&i_task->conf, &i_task->oip, &i_task->pe, i_task->sys_dat);

		if (!ret) {
			//分割エンコード用のappendixを取り除いて一時ファイルのコピーを行う
			char ext[64];
			strcpy_s(ext, _countof(ext), PathFindExtension(i_task->savefile));
			sprintf_s(i_task->savefile, _countof(i_task->savefile), "%s%s", i_task->filebase, ext);
			ret |= move_temporary_files(&i_task->conf, &i_task->pe, i_task->sys_dat, &i_task->oip, ret);
		}

		if (!ret) ret |= run_bat_file(&i_task->conf, &i_task->oip, &i_task->pe, i_task->sys_dat, RUN_BAT_AFTER);
		
		log_writef("%sの処理%s\n", i_task->filebase, (ret) ? "でエラーが発生しました。" : "を終了しました。");

		return ret;
	}
	bool task_check(TASK_INFO *i_task) {
		bool all_files_fin = true;
		const char *ext = PathFindExtension(i_task->pe.temp_filename);
		for (int i = 0; all_files_fin && i < i_task->pe.div_max; i++) {
			char check_filename[MAX_PATH_LEN];
			sprintf_s(check_filename, _countof(check_filename), "%s_%d_%d_tmp%s", i_task->filebase, i+1, i_task->pe.div_max, ext);
			all_files_fin &= !!PathFileExists(check_filename);
		}
		return all_files_fin;
	}
	AUO_RESULT task_check(BOOL *task_empty) {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		int task_processed = 0;
		for (auto i_task : task_list) {
			if (task_check(&i_task)) {
				ret |= task_run(&i_task);
				task_delete(i_task.id);
				task_processed++;
				break;
			}
		}
		EnterCriticalSection(&critical_section);
		//何らかの処理をして残りのタスクがゼロの時、TRUEとする
		*task_empty = (0 == task_list.size()) && (task_processed > 0);
		LeaveCriticalSection(&critical_section);
		return ret;
	}
	void task_delete(int id) {
		foreach (std::vector<TASK_INFO>, it_task, &task_list) {
			if (id == it_task->id) {
				EnterCriticalSection(&critical_section);
				task_list.erase(it_task);
				LeaveCriticalSection(&critical_section);
				break;
			}
		}

	}
};

static TaskController *g_controller = NULL;

AUO_RESULT parallel_task_check(CONF_GUIEX *conf, OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
	AUO_RESULT ret = AUO_RESULT_SUCCESS;
	pe->div_num = 0;
	pe->div_max = 1;
	if (sys_dat->exstg->s_local.enable_process_parallel) {
		const char *ptr = PathFindExtension(pe->temp_filename);
		for (int i = 0; ptr && i < 2; i++) {
			ptr--;
			for (; *ptr != '_'; ptr--) {
				if (ptr == pe->temp_filename) {
					ptr = NULL;
					break;
				}
			}
		}
		if (NULL == ptr
			|| 2 != sscanf_s(ptr, "_%d_%d_tmp", &pe->div_num, &pe->div_max)
			|| !check_range(pe->div_num, 1, pe->div_max)) {
			pe->div_num = 0;
			pe->div_max = 1;
		} else {
			pe->div_num--; //"1"始まりから"0"始まりへ変換する
		}
	}
	if (1 < pe->div_max) {
		if (1 != pe->total_pass) {
			write_log_auo_line(LOG_INFO, "マルチパスエンコードでは、分割エンコードは利用できません。");
			if (pe->div_num < pe->div_max)
				ret = AUO_RESULT_ERROR;
		} else if (conf->x26x[conf->vid.enc_type].use_tcfilein) {
			write_log_auo_line(LOG_INFO, "分割エンコードとtcfile-inは併用できません。");
			ret = AUO_RESULT_ERROR;
		} else {
			write_log_auo_line_fmt(LOG_INFO, "分割エンコードを行います。(%d / %d)", pe->div_num + 1, pe->div_max);
			//キーフレーム関係の機能は非対応
			if (conf->vid.check_keyframe) {
				write_log_auo_line(LOG_WARNING, "分割エンコードに伴い、キーフレームの検出は無効化されます。");
				conf->vid.check_keyframe = CHECK_KEYFRAME_NONE;
			}
			//分割エンコード結合後に行う処理を無効化する
			conf->oth.run_bat &= ~RUN_BAT_AFTER;
			pe->muxer_to_be_used = MUXER_DISABLED;
			//raw出力後にバイナリ結合し、それをmuxする
			change_ext(pe->temp_filename, _countof(pe->temp_filename), (conf->vid.enc_type == ENC_TYPE_X264) ? ".264" : ".265");
			//pe->div_num == pe->div_max 以外では音声出力を行わない
			if (pe->div_num+1 < pe->div_max) {
				oip->flag &= ~OUTPUT_INFO_FLAG_AUDIO;
			}
		}
	}
	return ret;
}

AUO_RESULT parallel_task_add(const CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
	if (1 >= pe->div_max)
		return AUO_RESULT_SUCCESS;

	//ファイル名に"_tmp"を追加して改名
	const char *ext = PathFindExtension(pe->temp_filename);
	char appendix[MAX_APPENDIX_LEN] = { 0 };
	strcpy_s(appendix, _countof(appendix), "_tmp");
	if (ext) {
		strcat_s(appendix, _countof(appendix), ext);
	}
	char temp_filename_orig[MAX_PATH_LEN];
	strcpy_s(temp_filename_orig, _countof(temp_filename_orig), pe->temp_filename);
	apply_appendix(pe->temp_filename, _countof(pe->temp_filename), pe->temp_filename, appendix);
	if (PathFileExists(pe->temp_filename))
		remove(pe->temp_filename);
	rename(temp_filename_orig, pe->temp_filename);

	//最後のファイルでない場合はここで終わり
	if (pe->div_num+1 < pe->div_max)
		return AUO_RESULT_SUCCESS;

	if (NULL == g_controller) {
		g_controller = new TaskController();
	}
	return g_controller->task_add(conf, oip, pe, sys_dat);
}

AUO_RESULT parallel_task_close() {
	if (NULL == g_controller)
		return AUO_RESULT_SUCCESS;

	const char *MES = ""
		AUO_NAME"は並列処理の待機中です。\n"
		"ここでAviutlを終了すると並列処理は完了しませんが、\n"
		"Aviutlを終了しますか ?";

	if (   0 == g_controller->get_current_task_waiting()
		|| IDOK == MessageBox(NULL, MES, AUO_FULL_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING)) {
		delete g_controller;
		g_controller = NULL;
		return AUO_RESULT_SUCCESS;
	}

	return AUO_RESULT_WARNING;
}
