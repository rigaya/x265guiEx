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

AUO_RESULT parallel_task_check(CONF_GUIEX *conf, OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat);
AUO_RESULT parallel_task_add(const CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat);
AUO_RESULT parallel_task_close();


#endif //_AUO_PROCESS_PARALLEL_H_
