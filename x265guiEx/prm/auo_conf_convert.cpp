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

//x264guiExの設定ファイル、CONF_VIDEO構造体
typedef struct {
	BOOL   afs;                      //自動フィールドシフトの使用
	BOOL   afs_bitrate_correction;   //afs & 2pass時、ドロップ数に応じてビットレートを補正
	BOOL   auo_tcfile_out;           //auo側でタイムコードを出力する
	DWORD  check_keyframe;           //キーフレームチェックを行う (CHECK_KEYFRAME_xxx)
	int    priority;                 //x264のCPU優先度(インデックス)
	char   stats[MAX_PATH_LEN];      //x264用ステータスファイルの場所
	char   tcfile_in[MAX_PATH_LEN];  //x264 tcfile-in用タイムコードファイルの場所
	char   cqmfile[MAX_PATH_LEN];    //x264 cqmfileの場所
	char   cmdex[CMDEX_MAX_LEN];     //追加コマンドライン
	int    __yc48_colormatrix_conv;  //YC48の色変換 (使用されていません)
	DWORD  check;                //自動マルチパス時のチェックの種類(AMPLIMIT_FILE_SIZE/AMPLIMIT_BITRATE)
	double limit_file_size;      //自動マルチパス時のファイルサイズ制限(MB)
	double limit_bitrate;        //自動マルチパス時のビットレート制限(kbps)
	BOOL   input_as_lw48;            //LW48モード
} CONF_VIDEO_OLD; //動画用設定(x264以外)


void guiEx_config::convert_x264stg_to_x265stg(CONF_GUIEX *conf, const BYTE *dat) {
	const CONF_GUIEX *old_data = (CONF_GUIEX *)dat;
	init_CONF_GUIEX(conf, FALSE);

	//まずそのままコピーするブロックはそうする
#define COPY_BLOCK(block, block_idx) { memcpy(&conf->block, ((BYTE *)old_data) + old_data->block_head_p[block_idx], min(sizeof(conf->block), old_data->block_size[block_idx])); }
	COPY_BLOCK(aud, 2);
	COPY_BLOCK(mux, 3);
	COPY_BLOCK(oth, 4);
#undef COPY_BLOCK

	const CONF_VIDEO_OLD *vid_old = (CONF_VIDEO_OLD *)(((BYTE *)old_data) + old_data->block_head_p[1]);

	//vidブロックの復元
	conf->vid.enc_type = 0;
#define COPY_VID_VALUE(x) { conf->vid.x = vid_old->x; }
#define COPY_AMP_VALUE(x) { conf->vid.amp_x264.x = vid_old->x; }
#define COPY_VID_STR(x) { strcpy_s(conf->vid.x, _countof(conf->vid.x), vid_old->x); }
	COPY_VID_VALUE(afs);
	COPY_VID_VALUE(afs_bitrate_correction);
	COPY_VID_VALUE(auo_tcfile_out);
	COPY_VID_VALUE(check_keyframe);
	COPY_VID_VALUE(priority);
	COPY_VID_STR(stats);
	COPY_VID_STR(tcfile_in);
	COPY_VID_STR(cqmfile);
	COPY_VID_STR(cmdex);
	COPY_AMP_VALUE(check);
	COPY_AMP_VALUE(limit_file_size);
	COPY_AMP_VALUE(limit_bitrate);
	COPY_VID_VALUE(input_as_lw48);
#undef COPY_VID_VALUE
#undef COPY_AMP_VALUE
#undef COPY_VID_STR

	//x264オプションの復元
	char *x264_all_cmd = ((char *)old_data) + old_data->block_head_p[5];
	set_cmd_to_conf(x264_all_cmd, &conf->x26x[0], ENC_TYPE_X264);
}
