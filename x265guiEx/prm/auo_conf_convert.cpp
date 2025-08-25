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
    double amp_limit_bitrate_upper;   //ビットレート制限(kbps)
} CONF_AMP;


void guiEx_config::convert_x265stgv2_to_x265stgv4(CONF_GUIEX_OLD *conf) {
    conf->enc.qp_compress = 0.60f;
    strcpy_s(conf->header.conf_name, CONF_NAME_OLD4);

    convert_x265stgv3_to_x265stgv4(conf);
}

void guiEx_config::convert_x265stgv3_to_x265stgv4(CONF_GUIEX_OLD *conf) {
    static const DWORD OLD_FLAG_AFTER  = 0x01;
    static const DWORD OLD_FLAG_BEFORE = 0x02;

    char bat_path_before_process[1024];
    char bat_path_after_process[1024];
    strcpy_s(bat_path_after_process,  conf->oth.batfiles[0]);
    strcpy_s(bat_path_before_process, conf->oth.batfiles[2]);
    
    DWORD old_run_bat_flags = conf->oth.run_bat;
    conf->oth.run_bat  = 0x00;
    conf->oth.run_bat |= (old_run_bat_flags & OLD_FLAG_BEFORE) ? RUN_BAT_BEFORE_PROCESS : 0x00;
    conf->oth.run_bat |= (old_run_bat_flags & OLD_FLAG_AFTER)  ? RUN_BAT_AFTER_PROCESS  : 0x00;

    memset(&conf->oth.batfiles[0], 0, sizeof(conf->oth.batfiles));
    strcpy_s(conf->oth.batfile.before_process, bat_path_before_process);
    strcpy_s(conf->oth.batfile.after_process,  bat_path_after_process);
    strcpy_s(conf->header.conf_name, CONF_NAME_OLD5);
}

CONF_VIDEO guiEx_config::conf_video_conv(const CONF_VIDEO_OLD& vid_old) {
    CONF_VIDEO vid;
    vid.afs = vid_old.afs;
    vid.afs_bitrate_correction = vid_old.afs_bitrate_correction;
    vid.auo_tcfile_out = vid_old.auo_tcfile_out;
    vid.check_keyframe = vid_old.check_keyframe;
    vid.priority = vid_old.priority;
    _tcscpy_s(vid.stats, char_to_tstring(vid_old.stats, CP_THREAD_ACP).c_str());
    _tcscpy_s(vid.tcfile_in, char_to_tstring(vid_old.tcfile_in, CP_THREAD_ACP).c_str());
    _tcscpy_s(vid.cqmfile, char_to_tstring(vid_old.cqmfile, CP_THREAD_ACP).c_str());
    _tcscpy_s(vid.cmdex, char_to_tstring(vid_old.cmdex, CP_THREAD_ACP).c_str());
    vid.amp_check = vid_old.amp_check;
    vid.amp_limit_file_size = vid_old.amp_limit_file_size;
    vid.amp_limit_bitrate_upper = vid_old.amp_limit_bitrate_upper;
    vid.amp_limit_bitrate_lower = vid_old.amp_limit_bitrate_lower;
    vid.input_as_lw48 = vid_old.input_as_lw48;
    _tcscpy_s(vid.parallel_div_info, char_to_tstring(vid_old.parallel_div_info, CP_THREAD_ACP).c_str());
    _tcscpy_s(vid.analysis_file, char_to_tstring(vid_old.analysis_file, CP_THREAD_ACP).c_str());
    vid.sync_process_affinity = vid_old.sync_process_affinity;
    return vid;
}