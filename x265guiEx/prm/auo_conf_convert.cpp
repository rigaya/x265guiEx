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

//x264guiEx 2.03の設定ファイル、CONF_VIDEO構造体
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
	DWORD  check;                    //自動マルチパス時のチェックの種類(AMPLIMIT_FILE_SIZE/AMPLIMIT_BITRATE)
	double limit_file_size;          //自動マルチパス時のファイルサイズ制限(MB)
	double limit_bitrate;            //自動マルチパス時のビットレート制限(kbps)
	BOOL   input_as_lw48;            //LW48モード
} CONF_VIDEO_OLD; //動画用設定(x264以外)

//x264guiEx 2.03の設定ファイル、CONF_X264構造体
typedef struct {
	BOOL    use_highbit_depth;
	int     output_csp;
	int     pass;
	BOOL    slow_first_pass;
	BOOL    use_auto_npass;
	int		auto_npass;
	BOOL    nul_out;
	int     preset;
	int     tune;
	int     profile;
	int     rc_mode;
	int     bitrate;
	int     crf;
	int     qp;
	float   ip_ratio;
	float   pb_ratio;
	int     qp_min;
	int     qp_max;
	int     qp_step;
	float   qp_compress;
	int     chroma_qp_offset;
	BOOL    mbtree;
	int     rc_lookahead;
	int     vbv_bufsize;
	int     vbv_maxrate;
	int     aq_mode;
	float   aq_strength;
	FLOAT2  psy_rd;
	int     scenecut;
	int     keyint_min;
	int     keyint_max;
	BOOL    open_gop;
	BOOL    cabac;
	int     bframes;
	int     b_adapt;
	int     b_bias;
	int     b_pyramid;
	int     slice_n;
	BOOL    use_deblock;
	INT2    deblock;
	BOOL    interlaced;
	BOOL    tff;
	DWORD   mb_partition;
	BOOL	dct8x8;
	int     me;
	int     subme;
	int     me_range;
	BOOL    chroma_me;
	int     direct_mv;
	int     ref_frames;
	BOOL    mixed_ref;
	BOOL    weight_b;
	int     weight_p;
	int     noise_reduction;
	BOOL    no_fast_pskip;
	BOOL    no_dct_decimate;
	int     trellis;
	int     cqm;
	int     colormatrix;
	int     colorprim;
	int     transfer;
	int     input_range;
	INT2    sar;
	int     h264_level;
	int     videoformat;
	BOOL    aud;
	BOOL    pic_struct;
	int     nal_hrd;
	BOOL    bluray_compat;
	int     threads;
	int     sliced_threading;
	int     log_mode;
	BOOL    psnr;
	BOOL    ssim;
	BOOL    use_tcfilein;
	BOOL    use_timebase;
	INT2    timebase;
	BOOL    disable_progress;
	int     lookahead_threads;
} CONF_X264_OLD;

void guiEx_config::convert_x264stg_to_x265stg(CONF_GUIEX *conf, const BYTE *dat) {
	const CONF_GUIEX *old_data = (CONF_GUIEX *)dat;
	init_CONF_GUIEX(conf, FALSE);

	//まずそのままコピーするブロックはそうする
#define COPY_BLOCK(block, block_idx) { memcpy(&conf->block, ((BYTE *)old_data) + old_data->block_head_p[block_idx], min(sizeof(conf->block), old_data->block_size[block_idx])); }
	COPY_BLOCK(aud, 2);
	COPY_BLOCK(mux, 3);
	COPY_BLOCK(oth, 4);
#undef COPY_BLOCK
	
	const CONF_X264_OLD *x264_old = (CONF_X264_OLD  *)(((BYTE *)old_data) + old_data->block_head_p[0]);
	const CONF_VIDEO_OLD *vid_old = (CONF_VIDEO_OLD *)(((BYTE *)old_data) + old_data->block_head_p[1]);

	conf->vid.enc_type = 0;
#define COPY_X264_VALUE(x) { conf->x264.x = x264_old->x; }
#define COPY_VID_VALUE(x) { conf->vid.x = vid_old->x; }
#define COPY_AMP_VALUE(x) { conf->vid.amp_x264.x = vid_old->x; }
#define COPY_VID_STR(x) { strcpy_s(conf->vid.x, _countof(conf->vid.x), vid_old->x); }
	//x264ブロックの復元
	conf->x264.bit_depth = (x264_old->use_highbit_depth) ? 16 : 8;
	COPY_X264_VALUE(output_csp);
	COPY_X264_VALUE(pass);
	COPY_X264_VALUE(slow_first_pass);
	COPY_X264_VALUE(use_auto_npass);
	COPY_X264_VALUE(auto_npass);
	COPY_X264_VALUE(nul_out);
	COPY_X264_VALUE(preset);
	COPY_X264_VALUE(tune);
	COPY_X264_VALUE(profile);
	COPY_X264_VALUE(rc_mode);
	COPY_X264_VALUE(bitrate);
	COPY_X264_VALUE(crf);
	COPY_X264_VALUE(qp);
	COPY_X264_VALUE(ip_ratio);
	COPY_X264_VALUE(pb_ratio);
	COPY_X264_VALUE(qp_min);
	COPY_X264_VALUE(qp_max);
	COPY_X264_VALUE(qp_step);
	COPY_X264_VALUE(qp_compress);
	COPY_X264_VALUE(chroma_qp_offset);
	COPY_X264_VALUE(mbtree);
	COPY_X264_VALUE(rc_lookahead);
	COPY_X264_VALUE(vbv_bufsize);
	COPY_X264_VALUE(vbv_maxrate);
	COPY_X264_VALUE(aq_mode);
	COPY_X264_VALUE(aq_strength);
	COPY_X264_VALUE(psy_rd);
	COPY_X264_VALUE(scenecut);
	COPY_X264_VALUE(keyint_min);
	COPY_X264_VALUE(keyint_max);
	COPY_X264_VALUE(open_gop);
	COPY_X264_VALUE(cabac);
	COPY_X264_VALUE(bframes);
	COPY_X264_VALUE(b_adapt);
	COPY_X264_VALUE(b_bias);
	COPY_X264_VALUE(b_pyramid);
	COPY_X264_VALUE(slice_n);
	COPY_X264_VALUE(use_deblock);
	COPY_X264_VALUE(deblock);
	COPY_X264_VALUE(interlaced);
	COPY_X264_VALUE(tff);
	COPY_X264_VALUE(mb_partition);
	COPY_X264_VALUE(dct8x8);
	COPY_X264_VALUE(me);
	COPY_X264_VALUE(subme);
	COPY_X264_VALUE(me_range);
	COPY_X264_VALUE(chroma_me);
	COPY_X264_VALUE(direct_mv);
	COPY_X264_VALUE(ref_frames);
	COPY_X264_VALUE(mixed_ref);
	COPY_X264_VALUE(weight_b);
	COPY_X264_VALUE(weight_p);
	COPY_X264_VALUE(noise_reduction);
	COPY_X264_VALUE(no_fast_pskip);
	COPY_X264_VALUE(no_dct_decimate);
	COPY_X264_VALUE(trellis);
	COPY_X264_VALUE(cqm);
	COPY_X264_VALUE(colormatrix);
	COPY_X264_VALUE(colorprim);
	COPY_X264_VALUE(transfer);
	COPY_X264_VALUE(input_range);
	COPY_X264_VALUE(sar);
	conf->x264.h26x_level = x264_old->h264_level;
	COPY_X264_VALUE(videoformat);
	COPY_X264_VALUE(aud);
	COPY_X264_VALUE(pic_struct);
	COPY_X264_VALUE(nal_hrd);
	COPY_X264_VALUE(bluray_compat);
	COPY_X264_VALUE(threads);
	COPY_X264_VALUE(sliced_threading);
	COPY_X264_VALUE(log_mode);
	COPY_X264_VALUE(psnr);
	COPY_X264_VALUE(ssim);
	COPY_X264_VALUE(use_tcfilein);
	COPY_X264_VALUE(use_timebase);
	COPY_X264_VALUE(timebase);
	COPY_X264_VALUE(disable_progress);
	COPY_X264_VALUE(lookahead_threads);

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
	COPY_AMP_VALUE(check);
	COPY_AMP_VALUE(limit_file_size);
	COPY_AMP_VALUE(limit_bitrate);
	COPY_VID_VALUE(input_as_lw48);
#undef COPY_VID_VALUE
#undef COPY_AMP_VALUE
#undef COPY_VID_STR
}
