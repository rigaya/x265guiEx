//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include "auo_util.h"
#include "auo_conf.h"
#include "auo_options.h"

//x265guiEx ～3.34βの設定ファイル、CONF_VIDEO構造体
typedef struct {
	DWORD amp_check;            //チェックの種類(AMPLIMIT_FILE_SIZE/AMPLIMIT_BITRATE)
	double amp_limit_file_size; //ファイルサイズ制限(MB)
	double amp_limit_bitrate;   //ビットレート制限(kbps)
} CONF_AMP;

typedef struct {
	int    enc_type;                 //x264/x265の選択
	BOOL   afs;                      //自動フィールドシフトの使用
	BOOL   afs_bitrate_correction;   //afs & 2pass時、ドロップ数に応じてビットレートを補正
	BOOL   auo_tcfile_out;           //auo側でタイムコードを出力する
	DWORD  check_keyframe;           //キーフレームチェックを行う (CHECK_KEYFRAME_xxx)
	int    priority;                 //x264のCPU優先度(インデックス)
	char   stats[MAX_PATH_LEN];      //x264用ステータスファイルの場所
	char   tcfile_in[MAX_PATH_LEN];  //x264 tcfile-in用タイムコードファイルの場所
	char   cqmfile[MAX_PATH_LEN];    //x264 cqmfileの場所
	char   cmdex[CMDEX_MAX_LEN];     //追加コマンドライン

	//配列としてもアクセスできるようにする
	union {
		CONF_AMP amp[2];
		struct {
			CONF_AMP amp_x264;       //自動マルチパス時
			CONF_AMP amp_x265;       //自動マルチパス時
		};
	};
	BOOL   input_as_lw48;            //LW48モード
	char   parallel_div_info[64];    //プロセス並列モード時に使用する情報
} CONF_VIDEO_OLD; //動画用設定(x264以外)

void guiEx_config::convert_x26xstg_to_x265stg(CONF_GUIEX *conf, const void *dat) {
	const CONF_GUIEX *old_data = (const CONF_GUIEX *)dat;
	init_CONF_GUIEX(conf, FALSE);

	//まずそのままコピーするブロックはそうする
#define COPY_BLOCK(block, block_idx) { memcpy(&conf->block, ((BYTE *)old_data) + old_data->block_head_p[block_idx], min(sizeof(conf->block), old_data->block_size[block_idx])); }
	COPY_BLOCK(x265, 2);
	COPY_BLOCK(aud, 3);
	COPY_BLOCK(mux, 4);
	COPY_BLOCK(oth, 5);
#undef COPY_BLOCK

	const CONF_VIDEO_OLD *vid_old = (CONF_VIDEO_OLD *)(((BYTE *)old_data) + old_data->block_head_p[0]);
	
#define COPY_VID_VALUE(x) { conf->vid.x = vid_old->x; }
#define COPY_AMP_VALUE(x) { conf->vid.x = vid_old->amp_x265.x; }
#define COPY_VID_STR(x) { strcpy_s(conf->vid.x, _countof(conf->vid.x), vid_old->x); }
	//vidブロックの復元
	COPY_VID_VALUE(afs);
	COPY_VID_VALUE(afs_bitrate_correction);
	COPY_VID_VALUE(auo_tcfile_out);
	COPY_VID_VALUE(check_keyframe);
	COPY_VID_VALUE(priority);
	COPY_VID_STR(stats);
	COPY_VID_STR(tcfile_in);
	COPY_VID_STR(cqmfile);
	COPY_VID_STR(cmdex);
	COPY_AMP_VALUE(amp_check);
	COPY_AMP_VALUE(amp_limit_file_size);
	COPY_AMP_VALUE(amp_limit_bitrate);
	COPY_VID_VALUE(input_as_lw48);
#undef COPY_VID_VALUE
#undef COPY_AMP_VALUE
#undef COPY_VID_STR
}
