//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#ifndef _AUO_PROCESS_PARALLEL_H_
#define _AUO_PROCESS_PARALLEL_H_

#include "output.h"
#include "auo.h"
#include "auo_settings.h"
#include "auo_options.h"
#include "auo_conf.h"
#include "auo_system.h"

enum {
	PROCESS_PARALLEL_AUTO_RUN_ALL    = 0, //すべての分割プロセスを自動実行
	PROCESS_PARALLEL_AUTO_RUN_MASTER = 1, //マスタープロセスのみ自動実行(遅延実行)し、その他はバッチファイルを出力先に
	PROCESS_PARALLEL_AUTO_RUN_NONE   = 2, //分割プロセスのバッチファイルを出力先に
};

int parallel_info_get_div_max(const char *info_string);
void parallel_info_write(char *info_string, size_t nSize, int div_max);

AUO_RESULT parallel_task_check(CONF_GUIEX *conf, OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat);
AUO_RESULT parallel_task_add(const CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat, AUO_RESULT ret);
AUO_RESULT parallel_task_close();

void parallel_task_set_unused_parallel_info(char *info_string, size_t buf_size);

#endif //_AUO_PROCESS_PARALLEL_H_
