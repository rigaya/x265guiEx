//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#include <Windows.h>
#include <tlhelp32.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "auo.h"
#include "auo_version.h"
#include "auo_system.h"
#include "auo_audio.h"
#include "auo_frm.h"
#include "auo_mux.h"
#include "auo_util.h"
#include "auo_pipe.h"
#include "auo_encode.h"
#include "auo_runbat.h"
#include "auo_process_parallel.h"

#define PARALLEL_INFO_HEAD "x264/x265guiEx_parallel_info"
#define PARALLEL_INFO_BASE PARALLEL_INFO_HEAD"_v%d_id_%d_divided_to_%d_parts"

static const int PARALLEL_INFO_VER = 1;

typedef struct parallel_info_t {
	int version;
	int id;
	int div_count;
} parallel_info_t;

static parallel_info_t parallel_info_default() {
	parallel_info_t info;
	info.version   = PARALLEL_INFO_VER;
	info.id        = 0;
	info.div_count = 1;
	return info;
}

static parallel_info_t parallel_info_parse(const char *info_string) {
	parallel_info_t info = parallel_info_default();
	int _ver, _id, _div_count;
	if (3 == sscanf_s(info_string, PARALLEL_INFO_BASE, &_ver, &_id, &_div_count)
		&& _ver == PARALLEL_INFO_VER) {
		info.id = _id;
		info.div_count = _div_count;
	}
	return info;
}

int parallel_info_get_div_max(const char *info_string) {
	return parallel_info_parse(info_string).div_count;
}

static void parallel_info_write(char *info_string, size_t buf_size, const parallel_info_t *info) {
	sprintf_s(info_string, buf_size, PARALLEL_INFO_BASE, info->version, info->id, info->div_count);
}
void parallel_info_write(char *info_string, size_t buf_size, int div_count) {
	sprintf_s(info_string, buf_size, PARALLEL_INFO_BASE, PARALLEL_INFO_VER, 0, div_count);
}

typedef struct TASK_INFO {
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
private:
	enum {
		TASK_WAIT  = 0,
		TASK_READY = 1,
		TASK_ABORT = 2,
	};
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
		task.oip.savefile            = NULL;
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
		if (NULL == buffer) {
			ret = AUO_RESULT_ERROR; log_write("映像ファイル結合用のメモリ確保に失敗しました。\n");
		} else if (fopen_s(&fp_write, first_filename, "ab") || NULL == fp_write) {
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
					remove(add_filename);
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
					if (buffer[0] == '#') {
						continue;
					} else if (1 != sscanf_s(buffer, "%lf", &value)) {
						ret = AUO_RESULT_ERROR;
					} else {
						//タイムコードを整数値に変換
						timecode.push_back(file_offset + (int)(value * fps * 0.001 + 0.5));
					}
				}
				fclose(fp_read);

				//ファイルの最後の5フレームから、最終フレームの時間を推定してfile_offsetの計算を行う
				if (timecode.size()) {
					int sum = 0;
					const DWORD scan_start = (std::max)(1u, timecode.size()-5);
					for (DWORD i_frame = scan_start; i_frame < timecode.size(); i_frame++) {
						sum += timecode[i_frame] - timecode[i_frame-1];
					}
					file_offset = timecode[timecode.size()-1] + (int)(sum / (double)(timecode.size() - scan_start) + 0.5);
				}
			}
		}
		if (!ret) {
			char tc_filename[MAX_PATH_LEN];
			sprintf_s(tc_filename, _countof(tc_filename), "%s_%d_%d%s", i_task->filebase, i_task->pe.div_max, i_task->pe.div_max, ext);
			apply_appendix(tc_filename, _countof(tc_filename), tc_filename, i_task->pe.append.tc);

			//もともとのx_x.timecodeのファイルをoldに除けておく
			char tc_filename_old[MAX_PATH_LEN];
			sprintf_s(tc_filename_old, _countof(tc_filename_old), "%s_%d_%d_old%s", i_task->filebase, i_task->pe.div_max, i_task->pe.div_max, ext);
			apply_appendix(tc_filename_old, _countof(tc_filename_old), tc_filename_old, i_task->pe.append.tc);
			if (PathFileExists(tc_filename_old))
				remove(tc_filename_old);
			rename(tc_filename, tc_filename_old);

			FILE *tcfile = NULL;
			if (fopen_s(&tcfile, tc_filename, "w") || NULL == tcfile) {
				ret = AUO_RESULT_ERROR; log_writef("結合タイムコードファイルのオープンに失敗しました。: %s\n", tc_filename);
			} else {
				//出力
				const double tm_multi = 1000.0 / fps;
				fprintf(tcfile, "# timecode format v2\n");
				for (auto time : timecode) {
					fprintf(tcfile, "%.6lf\n", time * tm_multi);
				}
				fclose(tcfile);
				log_writef("結合タイムコードファイルを出力しました。: %s\n", tc_filename);
			}
		}
		for (int i = 0; !ret && i < i_task->pe.div_max; i++) {
			//タイムコードの結合が成功していたら後片付け
			char tc_filename_old[MAX_PATH_LEN] = { 0 };
			sprintf_s(tc_filename_old, _countof(tc_filename_old), "%s_%d_%d%s%s", i_task->filebase, i+1, i_task->pe.div_max, (i+1 == i_task->pe.div_max) ? "_old" : "", ext);
			apply_appendix(tc_filename_old, _countof(tc_filename_old), tc_filename_old, i_task->pe.append.tc);
			if (PathFileExists(tc_filename_old))
				remove(tc_filename_old);
		}
		return ret;
	}
	AUO_RESULT task_run(TASK_INFO *i_task) {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		log_writef("%sの処理を行います。\n", i_task->filebase);

		//savefileへのポインタを設定
		i_task->oip.savefile = i_task->savefile;

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
			i_task->pe.div_max = 1; //move_temporary_filesを実行するため
			ret |= move_temporary_files(&i_task->conf, &i_task->pe, i_task->sys_dat, &i_task->oip, ret);
		}

		if (!ret) ret |= run_bat_file(&i_task->conf, &i_task->oip, &i_task->pe, i_task->sys_dat, RUN_BAT_AFTER);
		
		log_writef("%sの処理%s\n", i_task->filebase, (ret) ? "でエラーが発生しました。" : "を終了しました。");

		return ret;
	}
	AUO_RESULT task_abort(TASK_INFO *i_task) {
		for (int i = 0; i < i_task->pe.div_max; i++) {
			char check_filename[MAX_PATH_LEN] = { 0 };
			sprintf_s(check_filename, _countof(check_filename), "%s_%d_%d_abort.txt", i_task->filebase, i+1, i_task->pe.div_max);
			if (PathFileExists(check_filename))
				remove(check_filename);
		}
		return AUO_RESULT_ABORT;
	}
	int task_check(TASK_INFO *i_task) {
		int task_status = TASK_READY;
		const char *ext = PathFindExtension(i_task->pe.temp_filename);
		for (int i = 0; task_status != TASK_WAIT && i < i_task->pe.div_max; i++) {
			char check_filename[MAX_PATH_LEN] = { 0 };
			sprintf_s(check_filename, _countof(check_filename), "%s_%d_%d_tmp%s", i_task->filebase, i+1, i_task->pe.div_max, ext);
			if (!PathFileExists(check_filename)) {
				task_status = TASK_WAIT;
				sprintf_s(check_filename, _countof(check_filename), "%s_%d_%d_abort.txt", i_task->filebase, i+1, i_task->pe.div_max);
				if (PathFileExists(check_filename))
					task_status = TASK_ABORT;
			}
		}
		return task_status;
	}
	AUO_RESULT task_check(BOOL *task_empty) {
		AUO_RESULT ret = AUO_RESULT_SUCCESS;
		int task_processed = 0;
		for (auto i_task : task_list) {
			int task_status = task_check(&i_task);
			if (TASK_WAIT != task_status) {
				ret |= (TASK_READY == task_status) ? task_run(&i_task) : task_abort(&i_task);
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

//Aviutlのバッチファイル名の一覧を作成する
static std::vector<std::string> get_bat_aup_list() {
	std::vector<std::string> list;
	HANDLE hFind = NULL;
	WIN32_FIND_DATA win32fd;
	
	char aviutl_dir[MAX_PATH_LEN] = { 0 };
	char bat_aup_base[MAX_PATH_LEN] = { 0 };
	get_aviutl_dir(aviutl_dir, _countof(aviutl_dir));
	PathCombineLong(bat_aup_base, _countof(bat_aup_base), aviutl_dir, "batch*.aup");

	if (INVALID_HANDLE_VALUE != (hFind = FindFirstFile(bat_aup_base, &win32fd))) {
		do {
			if (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char bat_aup_fullpath[MAX_PATH_LEN] = { 0 };
				PathCombineLong(bat_aup_fullpath, _countof(bat_aup_fullpath), aviutl_dir, win32fd.cFileName);
				list.push_back(bat_aup_fullpath);
			}
		} while (FindNextFile(hFind, &win32fd));
	}

	return list;
}

static int get_conf_from_bat_aup(parallel_info_t *info, const char *bat_aup) {
	int ret = AUO_RESULT_SUCCESS;
	*info = parallel_info_default();
	//FILE_SHARE_READ | FILE_SHARE_WRITEを両方指定して読み込むことで、Aviutlがすでに開いているバッチも読み取ることができる
	HANDLE h_file = CreateFile(bat_aup, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h_file) {
		ret = AUO_RESULT_ERROR;
	} else {
		DWORD bytes_read = 0;
		const DWORD filesize = GetFileSize(h_file, NULL);
		char *buffer = (char *)malloc(filesize);
		const char *header = NULL;
		if (   NULL == buffer
			|| 0 == ReadFile(h_file, buffer, filesize, &bytes_read, NULL)
			|| filesize != bytes_read
			|| NULL == (header = (const char *)find_data(buffer, filesize, PARALLEL_INFO_HEAD, strlen(PARALLEL_INFO_HEAD)))) {
			ret = AUO_RESULT_ERROR;
		} else {
			*info = parallel_info_parse(header);
		}
		if (buffer) free(buffer);
		CloseHandle(h_file);
	}
	return ret;
}

//現在使用されていないparallel_idを取得する
int parallel_task_get_unused_parallel_id() {
	HANDLE hFind = NULL;
	WIN32_FIND_DATA win32fd = { 0 };
	
	char aviutl_dir[MAX_PATH_LEN] = { 0 };
	char bat_aup_base[MAX_PATH_LEN] = { 0 };
	get_aviutl_dir(aviutl_dir, _countof(aviutl_dir));
	PathCombineLong(bat_aup_base, _countof(bat_aup_base), aviutl_dir, "batch*.aup");

	//parallel_idのリストを作成
	std::vector<int> id_list;
	if (INVALID_HANDLE_VALUE != (hFind = FindFirstFile(bat_aup_base, &win32fd))) {
		do {
			if (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				parallel_info_t info = parallel_info_default();
				char bat_aup_fullpath[MAX_PATH_LEN] = { 0 };
				PathCombineLong(bat_aup_fullpath, _countof(bat_aup_fullpath), aviutl_dir, win32fd.cFileName);
				if (AUO_RESULT_SUCCESS == get_conf_from_bat_aup(&info, bat_aup_fullpath) && 1 < info.div_count)
					id_list.push_back(info.id);
			}
		} while (FindNextFile(hFind, &win32fd));
	}

	//IDを検索し、使われていないものを返す
	int unused_id = 1;
	if (id_list.size())
		for ( ; ; unused_id++)
			if (id_list.end() == std::find(id_list.begin(), id_list.end(), unused_id))
				break;

	return unused_id;
}

void parallel_task_set_unused_parallel_info(char *info_string, size_t buf_size) {
	parallel_info_t info = parallel_info_parse(info_string);
	info.id = parallel_task_get_unused_parallel_id();
	parallel_info_write(info_string, buf_size, &info);
}

//現在使用されていないAviutlのバッチファイル名を取得する
static std::string get_bat_aup_new(std::string current_bat_aup) {
	int current_id = 0;
	std::string new_bat_aup;
	if (1 == sscanf_s(PathFindFileName(current_bat_aup.c_str()), "batch%d.aup", &current_id)) {
		bool batch_exists = true;
		char aviutl_dir[MAX_PATH_LEN] = { 0 };
		char bat_filename[MAX_PATH_LEN] = { 0 };
		get_aviutl_dir(aviutl_dir, _countof(aviutl_dir));
		for (int id = current_id + 1; batch_exists && id < 100; id++) {
			sprintf_s(bat_filename, _countof(bat_filename), "%s\\batch%d.aup", aviutl_dir, id);
			batch_exists &= !!PathFileExists(bat_filename);
		}
		for (int id = 0; batch_exists && id < current_id; id++) {
			sprintf_s(bat_filename, _countof(bat_filename), "%s\\batch%d.aup", aviutl_dir, id);
			batch_exists &= !!PathFileExists(bat_filename);
		}
		new_bat_aup = bat_filename;
	}
	return new_bat_aup;
}

//ファイル名: bat_aup_origのバッチファイルから、
//ファイル名: bat_aup_filename_newのバッチファイルを作成
//その際バッチファイルの中のsavefile_origをsavefile_newで置き換える
static int create_new_bat_aup_file(const char *bat_aup_filename_new, const char *bat_aup_orig, const char *savefile_new, const char *savefile_orig) {
	int ret = AUO_RESULT_SUCCESS;
	std::ifstream inputFile(bat_aup_orig, std::ios::in | std::ios::binary);
	std::ofstream outputFile(bat_aup_filename_new, std::ios::out | std::ios::binary);
	if (!inputFile.good() || !outputFile.good()) {
		ret = AUO_RESULT_ERROR;
	} else {
		std::vector<char> bat_aup_orig_data((size_t)inputFile.seekg(0, std::ios::end).tellg());
		inputFile.seekg(0, std::ios::beg).read(&bat_aup_orig_data[0], static_cast<std::streamsize>(bat_aup_orig_data.size()));
		char *ptr = (char *)find_data(&bat_aup_orig_data[0], bat_aup_orig_data.size(), savefile_orig, strlen(savefile_orig) + 1);
		if (NULL != ptr) {
			strcpy_s(ptr, MAX_PATH, savefile_new);
			outputFile.write(&bat_aup_orig_data[0], bat_aup_orig_data.size());
		}
	}
	return ret;
}

//自分のバッチファイルを探す
static std::string get_bat_aup_of_self(const CONF_GUIEX *conf, std::vector<std::string>& bat_aup_list) {
	const parallel_info_t own_parallel_info = parallel_info_parse(conf->vid.parallel_div_info);

	std::string bat_aup_file_orig;
	for (auto bat_aup : bat_aup_list) {
		parallel_info_t info = parallel_info_default();
		if (AUO_RESULT_SUCCESS == get_conf_from_bat_aup(&info, bat_aup.c_str())
			&& 1 < info.div_count
			&& own_parallel_info.id == info.id) {
			bat_aup_file_orig = bat_aup;
			break;
		}
	}
	return bat_aup_file_orig;
}

//同じパスから実行されているAviutlのプロセスIDリストを返す
static std::vector<DWORD> get_current_running_aviutl_process_id() {
	char aviutl_path[MAX_PATH_LEN] = { 0 };
	GetModuleFileName(NULL, aviutl_path, _countof(aviutl_path));
	const char *aviutl_filename = PathFindFileName(aviutl_path);
	std::vector<DWORD> aviutl_process_id;

	HANDLE h_snapshot = NULL;

	if (INVALID_HANDLE_VALUE != (h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0))) {
		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(h_snapshot, &pe32)) {
			do {
				if (0 == _stricmp(pe32.szExeFile, aviutl_filename)) {
					HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
					if (NULL != h_process) {
						HMODULE module_list[4096] = { 0 };
						DWORD module_count = 0;
						if (EnumProcessModules(h_process, module_list, sizeof(module_list), &module_count)) {
							char exe_fullpath[MAX_PATH_LEN];
							GetModuleFileNameEx(h_process, module_list[0], exe_fullpath, _countof(exe_fullpath));
							if (0 == _stricmp(exe_fullpath, aviutl_path)) {
								aviutl_process_id.push_back(pe32.th32ProcessID);
							}
						}
						CloseHandle(h_process);
					}
				}
			} while (Process32Next(h_snapshot, &pe32));
		}
		CloseHandle(h_snapshot);
	}
	return aviutl_process_id;
}

//自分と同じAviutlを立ち上げ、バッチ出力の開始
static AUO_RESULT load_aviutl_and_run_bat_task(const PRM_ENC *pe, DWORD *pid) {
	AUO_RESULT ret = AUO_RESULT_SUCCESS;
	PROCESS_INFORMATION pi = { 0 };
	char aviutl_cmd[MAX_PATH_LEN]  = { 0 };
	char aviutl_path[MAX_PATH_LEN] = { 0 };
	GetModuleFileName(NULL, aviutl_path, _countof(aviutl_path));
	sprintf_s(aviutl_cmd, _countof(aviutl_cmd), "\"%s\" -bs", aviutl_path); //バッチ出力の開始
	PathRemoveFileSpec(aviutl_path);

	if (RP_SUCCESS != RunProcess(aviutl_cmd, aviutl_path, &pi, NULL, GetPriorityClass(pe->h_p_aviutl), FALSE, TRUE))
		ret = AUO_RESULT_ERROR;
	
	for (int i = 0; i < 20 && WAIT_TIMEOUT == WaitForInputIdle(pi.hProcess, LOG_UPDATE_INTERVAL); i++)
		log_process_events();

	*pid = pi.dwProcessId;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return ret;
}

//自分と同じAviutlを立ち上げ、バッチ出力の開始
static AUO_RESULT aviutl_add_bat_task(const char *bat_file, const char *savefile) {
	AUO_RESULT ret = AUO_RESULT_SUCCESS;
	PROCESS_INFORMATION pi = { 0 };
	char aviutl_cmd[MAX_PATH_LEN]  = { 0 };
	char aviutl_path[MAX_PATH_LEN] = { 0 };
	GetModuleFileName(NULL, aviutl_path, _countof(aviutl_path));
	sprintf_s(aviutl_cmd, _countof(aviutl_cmd), "\"%s\" \"%s\" -bp \"%s\" -q", aviutl_path, bat_file, savefile); //バッチ登録して終了
	PathRemoveFileSpec(aviutl_path);

	if (RP_SUCCESS != RunProcess(aviutl_cmd, aviutl_path, &pi, NULL, BELOW_NORMAL_PRIORITY_CLASS, TRUE, TRUE))
		ret = AUO_RESULT_ERROR;
	
	while (WAIT_TIMEOUT == WaitForInputIdle(pi.hProcess, LOG_UPDATE_INTERVAL))
		log_process_events();

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	for (int i = 0; i < 1000 / LOG_UPDATE_INTERVAL; i++) {
		log_process_events();
		Sleep(LOG_UPDATE_INTERVAL);
	}

	return ret;
}

static AUO_RESULT add_master_batfile_to_aviutl(int div_count, const char *savefile) {
	char new_savefile[MAX_PATH_LEN] ={ 0 };
	char src_bat_file[MAX_PATH_LEN] ={ 0 };
	char appendix[MAX_APPENDIX_LEN] ={ 0 };
	sprintf_s(appendix, _countof(appendix), "_%d_%d%s", div_count, div_count, PathFindExtension(savefile));
	apply_appendix(new_savefile, _countof(new_savefile), savefile, appendix);
	apply_appendix(src_bat_file, _countof(src_bat_file), new_savefile, ".aup");
	return aviutl_add_bat_task(src_bat_file, new_savefile);
}

static HWND get_top_window_handle_from_process_id(HWND hwnd_last, DWORD target_process_id) {
	HWND result = NULL;
	for (HWND hwnd = (hwnd_last) ? GetNextWindow(hwnd_last, GW_HWNDNEXT) : GetTopWindow(NULL); NULL != hwnd && NULL == result; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {
		if (hwnd != hwnd_last && 0 == GetWindowLong(hwnd, GWL_HWNDPARENT) && IsWindowVisible(hwnd)) {
			DWORD window_process_id = 0;
			GetWindowThreadProcessId(hwnd, &window_process_id);
			if (window_process_id == target_process_id)
				result = hwnd;
		}
	}
	return result;
}

static HWND get_bat_list_hwnd(HWND hwnd_aviutl) {
	log_process_events();
	Sleep(LOG_UPDATE_INTERVAL);
	static const int max_wait = 2000 / LOG_UPDATE_INTERVAL;
	HWND hwnd_bat_list = NULL;
	for (int i = 0; NULL == hwnd_bat_list && i < max_wait; i++) {
		HWND hwnd_dialog = FindWindowEx(NULL, NULL, "#32770", NULL);
		for (; NULL != hwnd_dialog; hwnd_dialog = FindWindowEx(NULL, hwnd_dialog, "#32770", NULL)) {
			if (hwnd_aviutl == (HWND)GetWindowLong(hwnd_dialog, GWL_HWNDPARENT)) {
				char buf[256] = { 0 };
				GetWindowText(hwnd_dialog, buf, _countof(buf));
				if (0 == strcmp(buf, "バッチ出力リスト")) {
					hwnd_bat_list = hwnd_dialog;
					break;
				}
			}
		}
		log_process_events();
		Sleep(LOG_UPDATE_INTERVAL);
	}
	return hwnd_bat_list;
}

//すでにバッチ出力リスト画面が開かれているAviutlがあれば、それを再利用する
//バッチ出力リストを閉じて再び開くことで更新を書ける
static int aviutl_restart_bat_task(DWORD target_process_id) {
	int run_started = 0;
	HWND hwnd_bat_list = NULL, hwnd_start_button = NULL;

	if (GetCurrentProcessId() != target_process_id) {

		for (HWND hwnd_aviutl = get_top_window_handle_from_process_id(NULL, target_process_id); NULL != hwnd_aviutl;
			hwnd_aviutl = get_top_window_handle_from_process_id(hwnd_aviutl, target_process_id)) {
			if (NULL != (hwnd_bat_list = get_bat_list_hwnd(hwnd_aviutl))
				&& NULL != (hwnd_start_button = FindWindowEx(hwnd_bat_list, NULL, "Button", "開始"))) {
				for (int i = 0; i < 2000 / LOG_UPDATE_INTERVAL; i++) {
					log_process_events();
					Sleep(LOG_UPDATE_INTERVAL);
				}
				run_started = 1;
				//「開始」ボタンが有効 (= バッチ出力中でない) なら再開する
				// 自分自身に対しては、強制的に更新をかける
				if (IsWindowEnabled(hwnd_start_button)) {
					//一度閉じる
					SendMessage(hwnd_bat_list, WM_CLOSE, 0, 0);
					Sleep(200);
					log_process_events();

					//再び開くことでバッチリストを更新する
					SendMessage(hwnd_aviutl, WM_COMMAND, 1025, 0);
					Sleep(200);
					log_process_events();

					hwnd_bat_list = get_bat_list_hwnd(hwnd_aviutl);
					hwnd_start_button = FindWindowEx(hwnd_bat_list, NULL, "Button", "開始");
					PostMessage(hwnd_start_button, WM_LBUTTONDOWN, MK_LBUTTON, 0);
					PostMessage(hwnd_start_button, WM_LBUTTONUP, MK_LBUTTON, 0);
					log_process_events();
				}
				break;
			}
		}
	}
	return run_started;
}

static bool check_parallel_process_compatible(const CONF_GUIEX *conf, const PRM_ENC *pe) {
	bool result = true;
	if (1 != pe->total_pass) {
		result = false; write_log_auo_line(LOG_INFO, "マルチパスエンコードでは、分割エンコードは利用できません。");
	} else if (conf->x26x[conf->vid.enc_type].use_tcfilein) {
		result = false; write_log_auo_line(LOG_INFO, "分割エンコードとtcfile-inは併用できません。");
	}
	return result;
}

AUO_RESULT parallel_task_check(CONF_GUIEX *conf, OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
	AUO_RESULT ret = AUO_RESULT_SUCCESS;
	pe->div_num = 0;
	pe->div_max = 1;
	if (!sys_dat->exstg->s_local.enable_process_parallel)
		return ret;

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
		|| 2 != sscanf_s(ptr, "_%d_%d", &pe->div_num, &pe->div_max)
		|| !check_range(pe->div_num, 0, pe->div_max)) {
		pe->div_num = 0;
		pe->div_max = 1;
	} else {
		pe->div_num--; //"1"始まりから"0"始まりへ変換する
	}

	log_process_events();

	if (1 >= pe->div_max && oip->flag & OUTPUT_INFO_FLAG_BATCH) {
		parallel_info_t info = parallel_info_parse(conf->vid.parallel_div_info);
		if (1 < info.div_count && check_parallel_process_compatible(conf, pe)) {
			//分割バッチファイル自動生成
			//現在のAviutlのバッチファイル名のリストを作成
			std::vector<std::string> bat_aup_list = get_bat_aup_list();
			//自分のバッチファイル名を探す
			std::string bat_aup_file_orig = get_bat_aup_of_self(conf, bat_aup_list);
			if (0 == bat_aup_file_orig.size()) {
				write_log_auo_line(LOG_INFO, "自分のバッチファイルの検索に失敗しました。");
				ret = AUO_RESULT_ERROR;
			} else {
				//新たなバッチファイルを作成
				const BOOL auto_run_all    = PROCESS_PARALLEL_AUTO_RUN_ALL    == sys_dat->exstg->s_local.process_parallel_mode;
				const BOOL auto_run_master = PROCESS_PARALLEL_AUTO_RUN_MASTER == sys_dat->exstg->s_local.process_parallel_mode;
				for (int i = 0; !ret && i < info.div_count - auto_run_all; i++) {
					char savefile_new[MAX_PATH_LEN] = { 0 };
					char appendix[MAX_APPENDIX_LEN] = { 0 };
					sprintf_s(appendix, _countof(appendix), "_%d_%d%s", i+1, info.div_count, ".aup");
					apply_appendix(savefile_new, _countof(savefile_new), oip->savefile, appendix);
					std::string bat_aup_filename_new = (auto_run_all) ? get_bat_aup_new(bat_aup_file_orig) : savefile_new;
					change_ext(savefile_new, _countof(savefile_new), PathFindExtension(oip->savefile));

					if (AUO_RESULT_SUCCESS != (ret |= create_new_bat_aup_file(bat_aup_filename_new.c_str(), bat_aup_file_orig.c_str(), savefile_new, oip->savefile))) {
						write_log_auo_line(LOG_INFO, "分割エンコードのバッチファイルの作成に失敗しました。");
					} else {
						write_log_auo_line_fmt(LOG_INFO, "分割エンコードのバッチファイルを作成しました。(%d / %d)", i+1, info.div_count);
					}
				}
				if (auto_run_all) {
					const std::vector<DWORD> current_running_aviutl_process_id = get_current_running_aviutl_process_id();
					int bat_running_count = 1; //最初の1は自分の分
					for (auto aviutl_process_id : current_running_aviutl_process_id) {
						if (bat_running_count <= info.div_count && aviutl_restart_bat_task(aviutl_process_id)) {
							write_log_auo_line_fmt(LOG_INFO, "並列でのバッチ出力を開始しました。(%d / %d, PID: %d)", bat_running_count, info.div_count, aviutl_process_id);
							bat_running_count++;
						}
					}
					for (int i = bat_running_count; !ret && i < info.div_count; i++) {
						DWORD process_id = 0;
						if (AUO_RESULT_SUCCESS != (ret |= load_aviutl_and_run_bat_task(pe, &process_id))) {
							write_log_auo_line(LOG_INFO, "Aviutlの起動に失敗しました。");
						} else {
							write_log_auo_line_fmt(LOG_INFO, "Aviutlを起動し、並列でのバッチ出力を開始しました。(%d / %d, PID: %d)", i, info.div_count, process_id);
						}
					}
					if (!ret) {
						//自分が最後のプロセスとして振る舞う
						pe->div_max = info.div_count;
						pe->div_num = info.div_count - 1;
						char appendix[MAX_APPENDIX_LEN] = { 0 };
						sprintf_s(appendix, _countof(appendix), "_%d_%d%s", pe->div_num+1, pe->div_max, PathFindExtension(oip->savefile));
						apply_appendix(pe->temp_filename, _countof(pe->temp_filename), pe->temp_filename, appendix);
					}
				} else {
					if (auto_run_master)
						add_master_batfile_to_aviutl(info.div_count, oip->savefile);
					pe->div_max = info.div_count;
					pe->div_num = -1;
					ret |= AUO_RESULT_ABORT;
				}
			}
			log_process_events();
		}
	}
	if (!ret && 1 < pe->div_max) {
		if (!check_parallel_process_compatible(conf, pe)) {
			ret = AUO_RESULT_ERROR;
		} else {
			write_log_auo_line_fmt(LOG_INFO, "分割エンコードを行います。(%d / %d, PID: %d)", pe->div_num + 1, pe->div_max, GetCurrentProcessId());
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

AUO_RESULT parallel_task_add(const CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat, AUO_RESULT ret) {
	if (1 >= pe->div_max)
		return AUO_RESULT_SUCCESS;

	if (ret & (AUO_RESULT_ABORT | AUO_RESULT_ERROR)) {
		if (0 <= pe->div_num) {
			//abortファイルを作る
			char appendix[MAX_APPENDIX_LEN] = { 0 };
			char abort_file[MAX_PATH_LEN] = { 0 };
			sprintf_s(appendix, _countof(appendix), "_%d_%d_abort.txt", pe->div_num+1, pe->div_max);
			apply_appendix(abort_file, _countof(abort_file), pe->temp_filename, appendix);
			HANDLE hnd = CreateFile(abort_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE != hnd) {
				CloseHandle(hnd);
			}
		}
		return ret;
	}

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

	const char *MES_BASE = ""
		"は並列処理の待機中です。\n"
		"ここでAviutlを終了すると並列処理は完了しませんが、\n"
		"Aviutlを終了しますか ?";

	char message[1024] = { 0 };
	strcpy_s(message, _countof(message), auo_name);
	strcat_s(message, _countof(message), MES_BASE);

	if (   0 == g_controller->get_current_task_waiting()
		|| IDOK == MessageBox(NULL, message, auo_full_name, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING)) {
		delete g_controller;
		g_controller = NULL;
		return AUO_RESULT_SUCCESS;
	}

	return AUO_RESULT_WARNING;
}
