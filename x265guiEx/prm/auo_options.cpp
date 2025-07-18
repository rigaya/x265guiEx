﻿//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <Math.h>
#include <limits.h>
#include <vector>

#include "h264_level.h"
#include "auo_util.h"
#include "auo_conf.h"
#include "auo_options.h"


//オプションの種類(色々便利なので1から始める)
enum {
    OPTION_TYPE_BOOL = 1,
    OPTION_TYPE_BOOL_REVERSE,
    OPTION_TYPE_INT,
    OPTION_TYPE_FLOAT,
    OPTION_TYPE_INT2,
    OPTION_TYPE_FLOAT2,
    OPTION_TYPE_BOOL2_REVERSE,
    OPTION_TYPE_LIST,
    //個別
    OPTION_TYPE_CRF,
    OPTION_TYPE_BITRATE,
    OPTION_TYPE_QP,
    OPTION_TYPE_KEYINT,
    OPTION_TYPE_DEBLOCK,
    OPTION_TYPE_CQM,
    OPTION_TYPE_TCFILE_IN,
    OPTION_TYPE_OUTPUT_DEPTH,
    OPTION_TYPE_PASS,
    OPTION_TYPE_MB_PARTITION,
    OPTION_TYPE_TFF,
    OPTION_TYPE_BFF,
    OPTION_TYPE_TIMEBASE,
    OPTION_TYPE_LEVEL,
    OPTION_TYPE_ANALYSE,
    OPTION_TYPE_RC,
    OPTION_TYPE_AQ,
    OPTION_TYPE_INTERLACED,
    OPTION_TYPE_PSY,
    OPTION_TYPE_SAR_X265,
    OPTION_TYPE_LOSSLESS,
    OPTION_TYPE_FIRST_PASS,
    OPTION_TYPE_FIRST_PASS_R,
};

//値を取らないオプションタイプのリスト
static const DWORD OPTION_NO_VALUE[] = {
    OPTION_TYPE_BOOL,
    OPTION_TYPE_BOOL_REVERSE,
    OPTION_TYPE_BOOL2_REVERSE,
    OPTION_TYPE_TFF,
    OPTION_TYPE_BFF,
    OPTION_TYPE_DEBLOCK,
    OPTION_TYPE_FIRST_PASS,
    OPTION_TYPE_FIRST_PASS_R,
    NULL
};

//オプション解析用の設定
static guiEx_settings *ex_stg;

static X265_OPTIONS x265_options_table[] = {
    { "output-depth",     "",   OPTION_TYPE_OUTPUT_DEPTH,  NULL,                 offsetof(CONF_ENC, bit_depth      ) },
    { "input-csp",        "",   OPTION_TYPE_LIST,          list_output_csp_x265, offsetof(CONF_ENC, output_csp     ) },
    { "pass",             "",   OPTION_TYPE_PASS,          NULL,                 offsetof(CONF_ENC, pass           ) },
    { "slow-firstpass",   "",   OPTION_TYPE_FIRST_PASS,    NULL,                 offsetof(CONF_ENC, slow_first_pass) },
    { "no-slow-firstpass","",   OPTION_TYPE_FIRST_PASS_R,  NULL,                 offsetof(CONF_ENC, slow_first_pass) },
    //{ "stats",            "",   OPTION_TYPE_STATS,         NULL,                 NULL                                  },
    { "preset",          "p",   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, preset         ) },
    { "tune",            "t",   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, tune           ) },
    { "profile",         "P",   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, profile        ) },
    { "crf",              "",   OPTION_TYPE_CRF,           NULL,                 NULL                                 },
    { "bitrate",           "",  OPTION_TYPE_BITRATE,       NULL,                 NULL                                 },
    { "qp",               "q",  OPTION_TYPE_QP,            NULL,                 NULL                                 },
    { "lossless",          "",  OPTION_TYPE_LOSSLESS,      NULL,                 NULL                                 },
    //{ "ipratio",          "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, ip_ratio       ) },
    //{ "pbratio",          "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, pb_ratio       ) },
    //{ "qpmin",            "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_min         ) },
    //{ "qpmax",            "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_max         ) },
    //{ "qpstep",           "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_step        ) },
    { "qcomp",            "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, qp_compress    ) },
    { "cbqpoffs",         "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, cbqpoffs       ) },
    { "crqpoffs",         "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, crqpoffs       ) },
    //{ "chroma-qp-offset", "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, chroma_qp_offset) },
    //{ "no-mbtree",        "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, mbtree         ) },
    //{ "mbtree",           "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, mbtree         ) },
    { "rc-lookahead",     "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rc_lookahead   ) },
    { "vbv-bufsize",      "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, vbv_bufsize    ) },
    { "vbv-maxrate",      "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, vbv_maxrate    ) },
    { "aq-mode",          "",   OPTION_TYPE_INT,           list_aq,              offsetof(CONF_ENC, aq_mode        ) },
    { "aq-strength",      "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, aq_strength    ) },
    { "psy-rd",           "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { "no-psy-rd",        "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { "psy-rdoq",         "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, psy_rdoq       ) },
    { "no-psy-rdoq",      "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, psy_rdoq       ) },
    { "rdoq-level",       "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rdoq_level     ) },
    { "no-rdoq-level",    "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, rdoq_level     ) },
    //{ "psy",              "",   OPTION_TYPE_PSY,           NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    //{ "no-psy",           "",   OPTION_TYPE_BOOL2_REVERSE, NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { "scenecut",         "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, scenecut       ) },
    { "no-scenecut",      "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, scenecut       ) },
    { "keyint",           "I",  OPTION_TYPE_KEYINT,        NULL,                 offsetof(CONF_ENC, keyint_max     ) },
    { "min-keyint",       "i",  OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, keyint_min     ) },
    { "no-open-gop",      "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, open_gop       ) },
    { "open-gop",         "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, open_gop       ) },
    //{ "no-cabac",         "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, cabac          ) },
    //{ "cabac",            "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, cabac          ) },
    { "bframes",          "b",  OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, bframes        ) },
    { "b-adapt",          "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, b_adapt        ) },
    { "bframe-bias",      "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, b_bias         ) },
    { "b-pyramid",        "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, b_pyramid      ) },
    { "no-b-pyramid",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, b_pyramid      ) },
    //{ "slices",           "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, slice_n        ) },
    //{ "no-interlace",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, interlaced     ) },
    { "deblock",          "f",  OPTION_TYPE_DEBLOCK,       NULL,                 NULL                                 },
    { "no-deblock",       "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, use_deblock    ) },
    { "no-sao",           "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, sao            ) },
    { "sao",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, sao            ) },
    { "no-interlace",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, interlaced     ) },
    { "interlace",        "",   OPTION_TYPE_LIST,          list_interlace_x265,  offsetof(CONF_ENC, interlaced     ) },
    //{ "bff",              "",   OPTION_TYPE_BFF,           NULL,                 NULL                                 },
    //{ "partitions",       "A",  OPTION_TYPE_MB_PARTITION,  NULL,                 offsetof(CONF_ENC, mb_partition   ) },
    //{ "no-8x8dct",        "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, dct8x8         ) },
    //{ "8x8dct",           "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, dct8x8         ) },
    { "no-wpp",           "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, wpp            ) },
    { "wpp",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, wpp            ) },
    { "ctu",             "s",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, ctu            ) },
    { "tu-intra-depth",   "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, tu_intra_depth ) },
    { "tu-inter-depth",   "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, tu_inter_depth ) },
    { "me",               "",   OPTION_TYPE_LIST,          list_me_x265,         offsetof(CONF_ENC, me             ) },
    { "subme",           "m",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, subme          ) },
    { "merange",          "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, me_range       ) },
    { "no-rect",          "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, rect_mp        ) },
    { "rect",             "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, rect_mp        ) },
    { "no-amp",           "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, asymmnteric_mp ) },
    { "amp",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, asymmnteric_mp ) },
    { "limit-refs",       "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, limit_refs     ) },
    { "no-limit-modes",   "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, limit_modes    ) },
    { "limit-modes",      "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, limit_modes    ) },
    //{ "no-early-skip",    "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, early_skip     ) },
    //{ "early-skip",       "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, early_skip     ) },
    //{ "no-chroma-me",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, chroma_me      ) },
    //{ "chroma-me",        "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, chroma_me      ) },
    //{ "direct",           "",   OPTION_TYPE_LIST,          list_direct,          offsetof(CONF_ENC, direct_mv      ) },
    { "ref",              "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, ref_frames     ) },
    //{ "no-mixed-ref",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, mixed_ref      ) },
    //{ "mixed-ref",        "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, mixed_ref      ) },
    { "max-merge",        "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, max_merge      ) },
    { "rskip",            "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, recursion_skip ) },
    { "no-weightb",       "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, weight_b       ) },
    { "weightb",          "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, weight_b       ) },
    { "no-weightp",       "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, weight_p       ) },
    { "weightp",         "w",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, weight_p       ) },
    //{ "rdpenalty",        "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rdpenalty      ) },
    //{ "no-tskip",         "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, tskip          ) },
    //{ "tskip",            "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, tskip          ) },
    //{ "no-tskip-fast",    "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, tskip_fast     ) },
    //{ "tskip-fast",       "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, tskip_fast     ) },
    //{ "no-strong-intra-smoothing","",OPTION_TYPE_BOOL_REVERSE,NULL,              offsetof(CONF_ENC, strong_intra_smooth ) },
    //{ "strong-intra-smoothing",   "",OPTION_TYPE_BOOL,        NULL,              offsetof(CONF_ENC, strong_intra_smooth ) },
    { "rd",               "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rd             ) },
    //{ "nr",               "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, noise_reduction) },
    //{ "no-fast-pskip",    "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, no_fast_pskip  ) },
    //{ "no-dct-decimate",  "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, no_dct_decimate) },
    //{ "trellis",          "t",  OPTION_TYPE_INT,           list_trellis,         offsetof(CONF_ENC, trellis        ) },
    //{ "cqm",              "",   OPTION_TYPE_CQM,           list_cqm,             offsetof(CONF_ENC, cqm            ) },
    //{ "cqmfile",          "",   OPTION_TYPE_CQMFILE,       NULL,                 NULL                                 },
    { "colormatrix",      "",   OPTION_TYPE_LIST,          list_colormatrix,     offsetof(CONF_ENC, colormatrix    ) },
    { "colorprim",        "",   OPTION_TYPE_LIST,          list_colorprim,       offsetof(CONF_ENC, colorprim      ) },
    { "transfer",         "",   OPTION_TYPE_LIST,          list_transfer,        offsetof(CONF_ENC, transfer       ) },
    { "range",            "",   OPTION_TYPE_LIST,          list_range,           offsetof(CONF_ENC, input_range    ) },
    { "sar",              "",   OPTION_TYPE_SAR_X265,      list_sar_x265,        offsetof(CONF_ENC, sar            ) },
    { "extended-sar",     "",   OPTION_TYPE_SAR_X265,      NULL,                 offsetof(CONF_ENC, sar            ) },
    //{ "level",            "",   OPTION_TYPE_LEVEL,         list_x264guiEx_level, offsetof(CONF_ENC, h264_level     ) },
    { "videoformat",      "",   OPTION_TYPE_LIST,          list_videoformat,     offsetof(CONF_ENC, videoformat    ) },
    //{ "aud",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, aud            ) },
    //{ "pic-struct",       "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pic_struct     ) },
    //{ "nal-hrd",          "",   OPTION_TYPE_LIST,          list_nal_hrd,         offsetof(CONF_ENC, nal_hrd        ) },
    //{ "bluray-compat",    "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, bluray_compat  ) },
    { "qg-size",          "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qg_size          ) },
    { "pools",            "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, pools          ) },
    { "frame-threads",   "F",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, frame_threads  ) },
    //{ "lookahead-threads","",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, lookahead_threads) },
    //{ "sliced-threads",   "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, sliced_threading) },
    //{ "log-level",        "",   OPTION_TYPE_LIST,          list_log_type,        offsetof(CONF_ENC, log_mode       ) },
    //{ "psnr",             "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, psnr           ) },
    //{ "ssim",             "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, ssim           ) },
    //{ "tcfile-in",        "",   OPTION_TYPE_TCFILE_IN,     NULL,                 NULL                                 },
    //{ "timebase",         "",   OPTION_TYPE_TIMEBASE,      NULL,                 NULL                                 },
    //{ "progress",         "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, disable_progress) },
    //{ "no-progress",      "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, disable_progress) },
    { "no-cutree",        "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, cutree         ) },
    { "cutree",           "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, cutree         ) },
    { "no-pmode",         "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, pmode          ) },
    { "pmode",            "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pmode          ) },
    { "no-pme",           "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, pme            ) },
    { "pme",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pme            ) },
    { "no-aq-motion",     "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, aq_motion      ) },
    { "aq-motion",        "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, aq_motion      ) },
    { "no-ssim-rd",       "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, ssim_rd        ) },
    { "ssim-rd",          "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, ssim_rd        ) },
    //{ "analysis-reuse-mode", "", OPTION_TYPE_BOOL,        NULL,                 offsetof(CONF_ENC, analysis_reuse) },
    { "analysis-reuse-level", "", OPTION_TYPE_INT,         NULL,                 offsetof(CONF_ENC, analysis_reuse_level) },
    //{ "refine-intra",     "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, refine_intra)         },
    //{ "refine-inter",     "",   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, refine_inter)         },
    { "no-hevc-aq",       "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, hevc_aq) },
    { "hevc-aq",          "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, hevc_aq) },
    { "no-svt",           "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, svt) },
    { "svt",              "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, svt) },
    { "fades",            "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, fades) },
    { "no-fades",         "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, fades) },
    { "hist-scenecut",    "",   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, hist_scenecut) },
    { "no-hist-scenecut", "",   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, hist_scenecut) },
    { "hist-threshold",   "",   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, hist_threshold) },
    { "rskip-edge-threshold", "", OPTION_TYPE_INT,         NULL,                 offsetof(CONF_ENC, rskip_edge_threshold) },
    { NULL,               NULL, NULL,                      NULL,                 NULL                                 },
};

static BOOL auo_strtol(int *i, const char *str, DWORD len) {
    char *eptr = NULL;
    int v;
    BOOL ret = TRUE;
    if (len == NULL) len = ULONG_MAX;
    if (*str != '{') {
        v = strtol(str, &eptr, 0);
        if (*eptr == '\0' || (DWORD)(eptr - str) == len) { *i = v; } else { ret = FALSE; }
    } else {
        str++;
        BOOL multi = (*str == '*');
        v = strtol(str + multi, &eptr, 0);
        if (*eptr == '}') { (multi) ? *i *= v : *i += v; } else { ret = FALSE; }
    }
    return ret;
}

static BOOL auo_strtof(float *f, const char *str, DWORD len) {
    char *eptr = NULL;
    float v;
    BOOL ret = TRUE;
    if (len == NULL) len = ULONG_MAX;
    if (*str != '{') {
        v = (float)strtod(str, &eptr);
        if (*eptr == '\0' || (DWORD)(eptr - str) == len) { *f = v; } else { ret = FALSE; }
    } else {
        str++;
        BOOL multi = (*str == '*');
        v = (float)strtod(str + multi, &eptr);
        if (*eptr == '}') { (multi) ? *f *= v : *f += v; } else { ret = FALSE; }
    }
    return ret;
}

static BOOL auo_parse_int(int *i, const char *value, DWORD len) {
    BOOL ret;
    if ((ret = auo_strtol(i, value, len)) == FALSE) {
        size_t str_len = strlen(value);
        if (*value == '[' && value[str_len -1] == ']') {
            const char *a, *b, *c;
            if ((a = strstr(value, "if>")) != NULL && (b = strstr(value, "else")) != NULL) {
                int v;
                c = a + strlen("if>");
                ret |= auo_strtol(&v, c, b-c);
                b += strlen("else");
                if (*i > v)
                    c = value+1, str_len = a - c;
                else
                    c = b, str_len = (value + str_len - 1) - c;
                ret &= auo_strtol(i, c, str_len);
            }
        }
    }
    return ret;
}

static BOOL auo_parse_float(float *f, const char *value, DWORD len) {
    BOOL ret;
    if ((ret = auo_strtof(f, value, len)) == FALSE) {
        size_t val_len = strlen(value);
        if (*value == '[' && value[val_len -1] == ']') {
            const char *a, *b, *c;
            if ((a = strstr(value, "if>")) != NULL && (b = strstr(value, "else")) != NULL) {
                float v;
                c = a + strlen("if>");
                ret |= auo_strtof(&v, c, b-c);
                b += strlen("else");
                if (*f > v)
                    c = value+1, val_len = a - c;
                else
                    c = b, val_len = (value + val_len - 1) - c;
                ret &= auo_strtof(f, c, val_len);
            }
        }
    }
    return ret;
}

//以下部分的にwarning C4100を黙らせる
//C4100 : 引数は関数の本体部で 1 度も参照されません。
#pragma warning( push )
#pragma warning( disable: 4100 )

static BOOL set_bool(void *b, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (value) {
        int i = -1;
        if (FALSE != (ret = auo_parse_int(&i, value, NULL)))
            if (FALSE != (ret = check_range(i, FALSE, TRUE)))
                *(int *)b = i;
    } else {
        *(BOOL*)b = TRUE;
    }
    return ret;
}

static BOOL set_bool_reverse(void *b, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (value) {
        int i = -1;
        if (FALSE != (ret = auo_parse_int(&i, value, NULL)))
            if (FALSE != (ret = check_range(i, FALSE, TRUE)))
                *(int *)b = !i;
    } else {
        *(BOOL*)b = FALSE;
    }
    return ret;
}
static BOOL set_int(void *i, const char *value, const ENC_OPTION_STR *list) {
    return auo_parse_int((int *)i, value, NULL);
}

static BOOL set_float(void *f, const char *value, const ENC_OPTION_STR *list) {
    return auo_parse_float((float *)f, value, NULL);
}

static BOOL set_int2(void *i, const char *value, const ENC_OPTION_STR *list) {
    const size_t len = strlen(value);
    //一度値をコピーして分析
    BOOL ret = FALSE;
    for (size_t j = 0; j < len; j++) {
        if (value[j] == ':' || value[j] == '|' || value[j] == ',' || value[j] == '/') {
            ret = TRUE;
            if (!(j == strlen("<unset>") && _strnicmp(value, "<unset>", strlen("<unset>")) == NULL))
                ret &= auo_parse_int(&((INT2 *)i)->x, value, j);
            if (_stricmp(&value[j+1], "<unset>") != NULL)
                ret &= auo_parse_int(&((INT2 *)i)->y, &value[j+1], 0);
            break;
        }
    }
    return ret;
}

static BOOL set_bool2_reverse(void *b, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (value) {
        INT2 i_value = { 0, 0 };
        if (FALSE != (ret = set_int2(&i_value, value, NULL))) {
            if (i_value.x == 0) ((INT2*)b)->x = FALSE;
            if (i_value.y == 0) ((INT2*)b)->y = FALSE;
        }
    } else {
        ((INT2*)b)->x = FALSE;
        ((INT2*)b)->y = FALSE;
    }
    return TRUE;
}

static BOOL set_float2(void *f, const char *value, const ENC_OPTION_STR *list) {
    const size_t len = strlen(value);
    BOOL ret = FALSE;
    for (size_t j = 0; j < len; j++) {
        if (value[j] == ':' || value[j] == '|' || value[j] == ',' || value[j] == '/') {
            ret = TRUE;
            if (!(j == strlen("<unset>") && _strnicmp(value, "<unset>", strlen("<unset>")) == NULL))
                ret &= auo_parse_float(&((FLOAT2 *)f)->x, value, j);
            if (_stricmp(&value[j+1], "<unset>") != NULL)
                ret &= auo_parse_float(&((FLOAT2 *)f)->y, &value[j+1], 0);
            break;
        }
    }
    return ret;
}

static BOOL set_list(void *i, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    for (int j = 0; list[j].name; j++) {
        if (_stricmp(value, list[j].name) == NULL) {
            *(int*)i = j;
            ret = TRUE;
            break;
        }
    }
    //数値での指定に対応X265_RC_CRF
    if (!ret) {
        int k = -1;
        if (FALSE != (ret = auo_parse_int(&k, value, NULL))) {
            //取得した数が、listに存在するか確認する
            ret = FALSE;
            for (int i_check = 0; list[i_check].name; i_check++) {
                if (i_check == k) {
                    *(int*)i = k;
                    ret = TRUE;
                    break;
                }
            }
        }
    }
    return ret;
}
static BOOL set_crf(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = X265_RC_CRF;
    float f = 23.0f;
    auo_strtof(&f, value, NULL);
    ((CONF_ENC *)cx)->crf = (int)(f * 100 + 0.5);
    return TRUE;
}
static BOOL set_bitrate(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = X265_RC_BITRATE;
    return auo_strtol(&((CONF_ENC *)cx)->bitrate, value, NULL);
}
static BOOL set_qp(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = X265_RC_QP;
    return auo_strtol(&((CONF_ENC *)cx)->qp, value, NULL);
}
static BOOL set_lossless(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = X265_RC_QP;
    ((CONF_ENC *)cx)->qp = -1;
    return TRUE;
}
static BOOL set_keyint(void *i, const char *value, const ENC_OPTION_STR *list) {
    if ((*(int*)i = _stricmp(value, "infinite")) != NULL)
        return auo_parse_int((int *)i, value, NULL);
    return TRUE;
}
static BOOL set_deblock(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    if (NULL == value) {
        ((CONF_ENC *)cx)->use_deblock = TRUE;
        ret = TRUE;
    } else {
        const int BUF_LEN = 128;
        const char *a = NULL;
        size_t len = strlen(value);
        if (*value == '[' && value[len-1] == ']' && NULL != (a = strstr(value, "if_on")) && BUF_LEN >= a - value && ((CONF_ENC *)cx)->use_deblock) {
            char tmp[BUF_LEN] = { 0 };
            memcpy(tmp, value + 1, a - value - 1);
            ret = set_int2(&((CONF_ENC *)cx)->deblock, tmp, list);
        } else if (FALSE != (ret = set_int2(&((CONF_ENC *)cx)->deblock, value, list))) {
            ((CONF_ENC *)cx)->use_deblock = TRUE;
        }
    }
    return ret;
}
static BOOL set_mb_partitions(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    *(DWORD*)cx = MB_PARTITION_NONE;
    if (stristr(value, "all")) {
        *(DWORD*)cx = MB_PARTITION_ALL;
    } else if (stristr(value, "none")) {
        ;//なにもしない
    } else {
        BOOL fin = FALSE;
        for (const char *p = value, *q = value; !fin; p++) {
            if (*p == '\0') fin = TRUE;
            if (fin || *p == ',' || *p == '/' || *p == ';' || *p == ':') {
                int len = p - q;
                if (     !_strnicmp(q, "p8x8", len))
                    *(DWORD*)cx |= MB_PARTITION_P8x8;
                else if (!_strnicmp(q, "b8x8", len))
                    *(DWORD*)cx |= MB_PARTITION_B8x8;
                else if (!_strnicmp(q, "p4x4", len))
                    *(DWORD*)cx |= MB_PARTITION_P4x4;
                else if (!_strnicmp(q, "i8x8", len))
                    *(DWORD*)cx |= MB_PARTITION_I8x8;
                else if (!_strnicmp(q, "i4x4", len))
                    *(DWORD*)cx |= MB_PARTITION_I4x4;
                else
                    ret = FALSE;
                q = p + 1;
            }
        }
    }
    return ret;
}
static BOOL set_tff(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->interlaced = TRUE;
    ((CONF_ENC *)cx)->tff = TRUE;
    return TRUE;
}
static BOOL set_bff(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->interlaced = TRUE;
    ((CONF_ENC *)cx)->tff = FALSE;
    return TRUE;
}
static BOOL set_timebase(void *cx, const char *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->use_timebase = TRUE;
    return set_int2(&((CONF_ENC *)cx)->timebase, value, list);
}
static BOOL set_level(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    size_t len = strlen(value);
    const char *tmp = value + len - 1;
    if (*tmp != '0')
        ret = set_list(cx, value, list);
    if (!ret) {
        //"5.0"とかを"5"に整形
        char *copy = (char *)malloc((len + 1) * sizeof(copy[0]));
        memcpy(copy, value, (len + 1) * sizeof(copy[0]));
        char *p = copy + len - 1;
        while (*p == '0' && p >= copy)
            p--;
        if (*p == '.') p--; //最後に'.'が残ったら消す
        *(p + 1) = '\0';
        ret = set_list(cx, copy, list);
        free(copy);
    }
    //整数指定を解析
    if (!ret) {
        int i = 0;
        if (auo_strtol(&i, value, NULL)) {
            char buf[16];
            if (i == 9)
                strcpy_s(buf, _countof(buf), "1b");
            else {
                size_t buf_len = sprintf_s(buf, _countof(buf), "%.1f", i / 10.0);
                char *p = buf + buf_len - 1;
                while (*p == '0' && p >= buf)
                    p--;
                if (*p == '.') p--; //最後に'.'が残ったら消す
                *(p + 1) = '\0';
            }
            ret = set_list(cx, buf, list);
        }
    }
    return ret;
}
static BOOL set_analyse(void *cx, const char *value, const ENC_OPTION_STR *list) {
    INT2 i_val = { 0, 0 };
    BOOL ret = set_int2(&i_val, value, list);
    if (ret) {
        *(DWORD*)cx = MB_PARTITION_NONE;
        if ((DWORD)i_val.y & 0x0100) *(DWORD*)cx |= MB_PARTITION_B8x8;
        if ((DWORD)i_val.y & 0x0020) *(DWORD*)cx |= MB_PARTITION_P4x4;
        if ((DWORD)i_val.y & 0x0010) *(DWORD*)cx |= MB_PARTITION_P8x8;
        if ((DWORD)i_val.y & 0x0002) *(DWORD*)cx |= MB_PARTITION_I8x8;
        if ((DWORD)i_val.y & 0x0001) *(DWORD*)cx |= MB_PARTITION_I4x4;
    }
    return ret;
}
static BOOL set_rc(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (NULL == strncmp(value, "2pass", strlen("2pass"))) {
        ((CONF_ENC *)cx)->rc_mode = X265_RC_BITRATE;
        ((CONF_ENC *)cx)->use_auto_npass = TRUE;
        ((CONF_ENC *)cx)->auto_npass = 2;
    } else if (NULL == strncmp(value, "crf", strlen("crf"))) {
        ((CONF_ENC *)cx)->rc_mode = X265_RC_CRF;
    } else if (NULL == strncmp(value, "cbr", strlen("cbr"))
            || NULL == strncmp(value, "abr", strlen("abr"))) {
        ((CONF_ENC *)cx)->rc_mode = X265_RC_BITRATE;
        ((CONF_ENC *)cx)->use_auto_npass = FALSE;
    } else if (NULL == strncmp(value, "cqp", strlen("cqp"))) {
        ((CONF_ENC *)cx)->rc_mode = X265_RC_QP;
    } else {
        ret = FALSE;
    }
    return ret;
}
static BOOL set_aq(void *cx, const char *value, const ENC_OPTION_STR *list) {
    FLOAT2 f_val = { 0, 0 };
    BOOL ret = set_float2(&f_val, value, list);
    if (ret) {
        ((CONF_ENC *)cx)->aq_mode = ((int)(f_val.x + 0.5));
        ((CONF_ENC *)cx)->aq_strength = f_val.y;
    }
    return ret;
}
static BOOL set_interlaced(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (!value) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = TRUE;
    } else if (NULL == strncmp(value, "tff", strlen("tff"))) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = TRUE;
    } else if (NULL == strncmp(value, "bff", strlen("bff"))) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = FALSE;
    } else if (NULL == strncmp(value, "0", strlen("0"))) {
        ((CONF_ENC *)cx)->interlaced = FALSE;
        ((CONF_ENC *)cx)->tff = FALSE;
    } else {
        ret = FALSE;
    }
    return ret;
}
static BOOL set_psy(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (value) {
        if (NULL == strcmp(value, "0")) {
            ((FLOAT2 *)cx)->x = 0.0;
            ((FLOAT2 *)cx)->y = 0.0;
        } else if (NULL == strcmp(value, "1")) {
            ; //何もしない
        } else {
            ret = set_float2(cx, value, list);
        }
    }
    return ret;
}
static BOOL set_x265_sar(void *cx, const char *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    if (list) {
        for (int i = 0; list[i].name; i++) {
            if (NULL == _stricmp(value, list[i].name)) {
                int a = 0, b = 0;
                if (2 == sscanf_s(list[i].name, "%d:%d", &a, &b)) {
                    ((INT2 *)cx)->x = a;
                    ((INT2 *)cx)->y = b;
                    ret = TRUE;
                }
                break;
            }
        }
    } else {
        ret = set_int2(cx, value, list);
    }
    return ret;
}
static BOOL set_do_nothing(void *cx, const char *value, const ENC_OPTION_STR *list) {
    return FALSE;
}


static int write_bool(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    BOOL *bptr = (BOOL*)((BYTE*)cx + options->p_offset);
    BOOL *defptr = (BOOL*)((BYTE*)def + options->p_offset);
    if ((write_all || *bptr != *defptr) && *bptr)
        return sprintf_s(cmd, nSize, " --%s", options->long_name);
    return 0;
}

static int write_bool_reverse(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    BOOL *bptr = (BOOL*)((BYTE*)cx + options->p_offset);
    BOOL *defptr = (BOOL*)((BYTE*)def + options->p_offset);
    if ((write_all || *bptr != *defptr) && !(*bptr))
        return sprintf_s(cmd, nSize, " --%s", options->long_name);
    return 0;
}

static int write_bool2_reverse(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *bptr = (INT2*)((BYTE*)cx + options->p_offset);
    if (!(bptr->x | bptr->y))
        return sprintf_s(cmd, nSize, " --%s", options->long_name);
    return 0;
}

static int write_int(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr)
        return sprintf_s(cmd, nSize, " --%s %d", options->long_name, *iptr);
    return 0;
}
//小数表示の鬱陶しい0を消す
static inline int write_float_ex(char *cmd, size_t nSize, float f) {
    double d = (int)(f * 1000.0 + 0.5 - (f<0)) / 1000.0; //これを入れないと22.2が22.19999とかになる
    int len = sprintf_s(cmd, nSize, "%lf", d);
    char *p = cmd + len - 1;
    while (*p == '0' && p >= cmd)
        p--;
    if (*p == '.') p--; //最後に'.'が残ったら消す
    p++;
    *p = '\0';
    return p - cmd;
}

static int write_float(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    float *fptr = (float*)((BYTE*)cx + options->p_offset);
    float *defptr = (float*)((BYTE*)def + options->p_offset);
    if (write_all || abs(*fptr - *defptr) > EPS_FLOAT) {
        int len = sprintf_s(cmd, nSize, " --%s ", options->long_name);
        return len + write_float_ex(cmd + len, nSize - len, *fptr);
    }
    return 0;
}

static int write_int2(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *iptr = (INT2*)((BYTE*)cx + options->p_offset);
    INT2 *defptr = (INT2*)((BYTE*)def + options->p_offset);
    if (write_all || iptr->x != defptr->x || iptr->y != defptr->y)
        return sprintf_s(cmd, nSize, " --%s %d:%d", options->long_name, iptr->x, iptr->y);
    return 0;
}

static int write_float2(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    FLOAT2 *fptr = (FLOAT2*)((BYTE*)cx + options->p_offset);
    FLOAT2 *defptr = (FLOAT2*)((BYTE*)def + options->p_offset);
    if (write_all || fptr->x != defptr->x || fptr->y != defptr->y) {
        int len = sprintf_s(cmd, nSize, " --%s ", options->long_name);
        len += write_float_ex(cmd + len, nSize - len, fptr->x);
        strcpy_s(cmd + len, nSize - len, ":");
        len += strlen(":");
        return len + write_float_ex(cmd + len, nSize - len, fptr->y);
    }
    return 0;
}

static int write_list(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr)
        return sprintf_s(cmd, nSize, " --%s %s", options->long_name, options->list[*iptr].name);
    return 0;
}

static int write_crf(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == X265_RC_CRF) {
        int len = sprintf_s(cmd, nSize, " --%s ", options->long_name);
        return len + write_float_ex(cmd + len, nSize - len, cx->crf / 100.0f);
    }
    return 0;
}
static int write_bitrate(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == X265_RC_BITRATE) {
        int len = sprintf_s(cmd, nSize, " --%s %d", options->long_name, cx->bitrate);
        if (cx->pass) {
            len += sprintf_s(cmd + len, nSize - len, " --pass %d", cx->pass);
            len += sprintf_s(cmd + len, nSize - len, " --stats \"%s\"", vid->stats);
            if (cx->analysis_reuse) {
                len += sprintf_s(cmd + len, nSize - len, " --analysis-%s \"%s\"", (cx->pass == 1) ? "save" : "load", vid->analysis_file);
            }
        }
        return len;
    }
    return 0;
}
static int write_qp(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == X265_RC_QP) {
        if (cx->qp < 0) {
            return sprintf_s(cmd, nSize, " --lossless");
        } else {
            return sprintf_s(cmd, nSize, " --%s %d", options->long_name, cx->qp);
        }
    }
    return 0;
}
static int write_keyint_x264(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr) {
        return (*iptr != 0) ? sprintf_s(cmd, nSize, " --%s %d", options->long_name, *iptr)
                            : sprintf_s(cmd, nSize, " --%s %s", options->long_name, "infinite");
    }
    return 0;
}
static int write_deblock(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->use_deblock)
        if (write_all || cx->deblock.x != def->deblock.x || cx->deblock.y != def->deblock.y)
            return sprintf_s(cmd, nSize, " --%s %d:%d", options->long_name, cx->deblock.x, cx->deblock.y);
    return 0;
}
static int write_x265_sar(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *iptr = (INT2*)((BYTE*)cx + options->p_offset);
    INT2 *defptr = (INT2*)((BYTE*)def + options->p_offset);
    if (write_all || iptr->x != defptr->x || iptr->y != defptr->y) {
        return sprintf_s(cmd, nSize, " --sar %d:%d", iptr->x, iptr->y);
    }
    return 0;
}
static int write_input_depth(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->bit_depth > 8)
        return sprintf_s(cmd, nSize, " --input-depth 16 --output-depth %d", cx->bit_depth);
    return 0;
}
static int write_slow_first_pass(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == X265_RC_BITRATE && cx->pass) {
        return write_bool(cmd, nSize, options, cx, def, vid, write_all);
    }
    return 0;
}
static int write_slow_first_pass_r(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == X265_RC_BITRATE && cx->pass) {
        return write_bool_reverse(cmd, nSize, options, cx, def, vid, write_all);
    }
    return 0;
}
static int write_do_nothing(char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    return 0;
}

#pragma warning( pop ) //( disable: 4100 ) 終了

//この配列に従って各関数に飛ばされる
typedef BOOL (*SET_VALUE) (void *cx, const char *value, const ENC_OPTION_STR *list);
const SET_VALUE set_value[] = {
    NULL,
    set_bool,
    set_bool_reverse,
    set_int,
    set_float,
    set_int2,
    set_float2,
    set_bool2_reverse,
    set_list,
    set_crf,
    set_bitrate,
    set_qp,
    set_keyint,
    set_deblock,
    set_list,
    set_do_nothing,
    set_int,
    set_int,
    set_mb_partitions,
    set_tff,
    set_bff,
    set_timebase,
    set_level,
    set_analyse,
    set_rc,
    set_aq,
    set_interlaced,
    set_psy,
    set_x265_sar,
    set_lossless,
    set_bool,
    set_bool_reverse,
};

//この配列に従って各関数に飛ばされる
typedef int (*WRITE_CMD_x265) (char *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all);
//const WRITE_CMD_x265 write_cmd_x264[] = {
//    NULL,
//    write_bool,
//    write_bool_reverse,
//    write_int,
//    write_float,
//    write_int2,
//    write_float2,
//    write_bool2_reverse,
//    write_list,
//    write_crf,
//    write_bitrate,
//    write_qp,
//    write_keyint_x264,
//    write_deblock,
//    write_cqm,
//    write_tcfilein,
//    write_input_depth,
//    write_do_nothing,
//    write_mb_partitions,
//    write_tff,
//    write_do_nothing,
//    write_timebase,
//    write_list,
//    write_do_nothing,
//    write_do_nothing,
//    write_do_nothing,
//    write_do_nothing,
//    write_do_nothing,
//    write_do_nothing
//};
const WRITE_CMD_x265 write_cmd_x265[] = {
    NULL,
    write_bool,
    write_bool_reverse,
    write_int,
    write_float,
    write_int2,
    write_float2,
    write_bool2_reverse,
    write_list,
    write_crf,
    write_bitrate,
    write_qp,
    write_int,
    write_deblock,
    write_do_nothing,
    write_do_nothing,
    write_input_depth,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_do_nothing,
    write_x265_sar,
    write_do_nothing,
    write_slow_first_pass,
    write_slow_first_pass_r,
};

//MediaInfoからの情報で無視するもの
static BOOL is_arg_ignore_mediainfo(const char *arg) {
    static const char * const IGNORE_ARGS[] = { "threads", "lookahead_threads" };
    for (int i = 0; i < _countof(IGNORE_ARGS); i++)
        if (NULL == strcmp(arg, IGNORE_ARGS[i]))
            return TRUE;
    return FALSE;
}

static void parse_arg(char *cmd, size_t cmd_len, std::vector<CMD_ARG> &cmd_arg_list) {
    BOOL dQB = FALSE;
    BOOL space_flag = TRUE;
    BOOL next_option_flag = TRUE;
    CMD_ARG cmd_arg = { 0 };
    const char *cmd_fin = cmd + cmd_len;
    while (cmd < cmd_fin) {
        if (*cmd == '"') dQB = !dQB;
        if (!dQB) {
            if ((*cmd == ' ' || *cmd == '\r' || *cmd == '\n')) {
                *cmd = '\0';
                space_flag = TRUE;
            } else if (space_flag) {
                space_flag = FALSE;
                if (*cmd == '-' && !isdigit(*(cmd+1))) { //isdigitは負数を避けるため
                    if (cmd_arg.arg_type) {
                        cmd_arg_list.push_back(cmd_arg);
                        ZeroMemory(&cmd_arg, sizeof(CMD_ARG));
                    }
                    cmd_arg.value = NULL;
                    cmd++;
                    if (*cmd == '-') {
                        cmd++;
                        cmd_arg.arg_type = ARG_TYPE_LONG;
                    } else {
                        cmd_arg.arg_type = ARG_TYPE_SHORT;
                        if (cmd[1] != ' ')
                            cmd_arg.value = cmd + 1;
                    }
                    cmd_arg.option_name = cmd;
                    next_option_flag = FALSE;
                } else if (!next_option_flag) {
                    cmd_arg.value = cmd;
                    next_option_flag = TRUE;
                } else if (cmd[0] == '/' && cmd[1] == ' ') {
                    ;
                } else {
                    //オプション名でもなく、オプション値でもない
                    //MediaInfoのオプションかどうかをチェック
                    BOOL b_QB = FALSE;
                    for (char *ptr = cmd; ptr <= cmd_fin; ptr++) {
                        if (*ptr == '"') b_QB = !b_QB;
                        if (!b_QB) {
                            if ((*ptr == ' ' || *ptr == '\r' || *ptr == '\n' || ptr == cmd_fin)) {
                                char *equal_ptr = NULL;
                                if (NULL != (equal_ptr = strrchr(cmd, '=', ptr - cmd))) {
                                    //mediainfoの書式
                                    *equal_ptr = '\0';
                                    CMD_ARG cmd_arg_media_info = { 0 };
                                    char *r_space_ptr = strrchr(equal_ptr, ' ');
                                    cmd_arg_media_info.option_name = (r_space_ptr) ? r_space_ptr + 1 : cmd;
                                    if (!is_arg_ignore_mediainfo(cmd_arg_media_info.option_name)) {
                                        cmd_arg_media_info.arg_type = ARG_TYPE_LONG;
                                        cmd_arg_media_info.value = equal_ptr + 1;
                                        cmd_arg_media_info.type_mediainfo = TRUE;
                                        cmd_arg_list.push_back(cmd_arg_media_info);
                                    }
                                }
                                cmd = ptr;
                                *cmd = '\0';
                                space_flag = TRUE;
                                next_option_flag = TRUE;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (space_flag) {
            space_flag = FALSE;
            if (!next_option_flag) {
                cmd_arg.value = cmd;
                next_option_flag = TRUE;
            }
        }
        cmd++;
    }
    if (cmd_arg.arg_type)
        cmd_arg_list.push_back(cmd_arg);

    //最初と最後の'"'を落とす
    foreach (it_arg, cmd_arg_list) {
        if (it_arg->value == NULL)
            continue;
        size_t value_len = strlen(it_arg->value);
        if (it_arg->value[0] == '"' && it_arg->value[value_len-1] == '"') {
            it_arg->value[value_len-1] = '\0';
            it_arg->value++;
            it_arg->value_had_dQB = TRUE; //落としたことを記憶
        }
    }
}

static void set_setting_list() {
    if (ex_stg->get_reset_s_enc_referesh()) {
        for (DWORD i = 0, check = 0x00; check != (0x04|0x02|0x01) && x265_options_table[i].long_name; i++) {
            if        (!(check & 0x01) && strcmp(x265_options_table[i].long_name, "preset") == NULL) {
                check |= 0x01; x265_options_table[i].list = ex_stg->s_enc.preset.name;
            } else if (!(check & 0x02) && strcmp(x265_options_table[i].long_name, "tune") == NULL) {
                check |= 0x02; x265_options_table[i].list = ex_stg->s_enc.tune.name;
            } else if (!(check & 0x04) && strcmp(x265_options_table[i].long_name, "profile") == NULL) {
                check |= 0x04; x265_options_table[i].list = ex_stg->s_enc.profile.name;
            }
        }
    }
}

//MediaInfoからの情報の補正を行う
static void check_values_from_mediainfo(std::vector<CMD_ARG> &cmd_arg_list, CONF_ENC *conf_set) {
    BOOL keyint_from_mediainfo = FALSE;
    BOOL keyint_min_from_mediainfo = FALSE;
    BOOL chroma_qp_offset_from_mediainfo = FALSE;
    //2重に指定された場合などを考え、一度すべて見てみてから判定
    for (auto arg : cmd_arg_list) {
        if (arg.ret) {
            if        (NULL == strcmp(arg.option_name, "chroma_qp_offset")) {
                chroma_qp_offset_from_mediainfo = arg.type_mediainfo;
            } else if (NULL == strcmp(arg.option_name, "keyint")) {
                keyint_from_mediainfo = arg.type_mediainfo;
            } else if (NULL == strcmp(arg.option_name, "keyint_min")) {
                keyint_min_from_mediainfo = arg.type_mediainfo;
            }
        }
    }
    //x264で使われていたコード
    //if (chroma_qp_offset_from_mediainfo) {
    //    conf_set->chroma_qp_offset += ((conf_set->psy_rd.x > 0.0) + (conf_set->psy_rd.x > 0.25));
    //    conf_set->chroma_qp_offset += ((conf_set->psy_rd.y > 0.0) + (conf_set->psy_rd.y > 0.25));
    //}
    if (keyint_from_mediainfo && keyint_min_from_mediainfo)
        if (conf_set->keyint_max / 10 == conf_set->keyint_min)
            conf_set->keyint_min = 0;

    if (keyint_from_mediainfo)
        if (conf_set->keyint_max % 10 == 0)
            conf_set->keyint_max = AUO_KEYINT_MAX_AUTO;
}

static inline BOOL option_has_no_value(DWORD type) {
    for (int i = 0; OPTION_NO_VALUE[i]; i++)
        if (type == OPTION_NO_VALUE[i])
            return TRUE;
    return FALSE;
}

static void set_conf_x265(std::vector<CMD_ARG> &cmd_arg_list, CONF_ENC *conf_set) {
    foreach (it_arg, cmd_arg_list) {
        int i;
        for (i = 0; i < _countof(x265_options_table); i++) {
            if (NULL == x265_options_table[i].long_name) //書き出しの終了条件
                continue;
            if (NULL == ((it_arg->arg_type == ARG_TYPE_LONG) ? strcmp(it_arg->option_name, x265_options_table[i].long_name) :
                                                               strncmp(it_arg->option_name, x265_options_table[i].short_name, 1))) {
                it_arg->ret = (option_has_no_value(x265_options_table[i].type) == FALSE && it_arg->value == NULL) ? FALSE : TRUE;
                break;
            }
        }
        if (it_arg->ret)
            it_arg->ret = set_value[x265_options_table[i].type]((void *)((BYTE *)conf_set + x265_options_table[i].p_offset), it_arg->value, x265_options_table[i].list);
    }
    //check_values_from_mediainfo(cmd_arg_list, conf_set);
}

void set_cmd_to_conf(char *cmd, CONF_ENC *conf_set, size_t cmd_len, BOOL build_not_imported_cmd) {
    std::vector<CMD_ARG> cmd_arg_list;
    set_setting_list();
    parse_arg(cmd, cmd_len, cmd_arg_list);
    set_conf_x265(cmd_arg_list, (CONF_ENC *)conf_set);
    if (build_not_imported_cmd) {
        //コマンドラインの再構築
        //it_arg->option_name のポインタが指すのはcmd上の文字列なので、
        //直接cmdに書き込むとおかしなことになる。
        //そこで一時領域を確保する
        char * const tmp = (char *)calloc((cmd_len + 1) * sizeof(tmp[0]), 1);
        size_t new_len = 0;
        for (auto arg : cmd_arg_list) {
            //正常に読み込まれていない、かつMediaInfoの書式でないものを再構成する
            if (!arg.ret && !arg.type_mediainfo) {
                new_len += sprintf_s(tmp + new_len, cmd_len+1 - new_len, "%s%s%s",
                    (new_len) ? " " : "", (arg.arg_type == ARG_TYPE_LONG) ? "--" : "-", arg.option_name);
                if (arg.value) {
                    const char * const dqb = (arg.value_had_dQB) ? "\"" : "";
                    new_len += sprintf_s(tmp + new_len, cmd_len+1 - new_len, " %s%s%s", dqb, arg.value, dqb);
                }
            }
        }
        memcpy(cmd, tmp, (new_len + 1) * sizeof(cmd[0]));
        free(tmp);
    }
}

void set_cmd_to_conf(const char *cmd_src, CONF_ENC *conf_set) {
    //parse_argでコマンドラインは書き変えられるので、
    //一度コピーしておく
    size_t cmd_len = strlen(cmd_src) + 1;
    char * const cmd = (char *)malloc((cmd_len + 1) * sizeof(cmd[0]));
    memcpy(cmd, cmd_src, (cmd_len + 1) * sizeof(cmd[0]));
    set_cmd_to_conf(cmd, conf_set, cmd_len, FALSE); //parse_arg
    free(cmd);
}

void get_default_conf(CONF_ENC *conf_set, BOOL use_highbit) {
    ZeroMemory(conf_set, sizeof(CONF_ENC));
    set_cmd_to_conf(ex_stg->s_enc.default_cmd, conf_set);
    if (use_highbit)
        set_cmd_to_conf(ex_stg->s_enc.default_cmd_highbit, conf_set);

    apply_presets(conf_set);
}

void set_preset_to_conf(CONF_ENC *conf_set, int preset_index) {
    if (ex_stg->s_enc.preset.name[preset_index].name)
        set_cmd_to_conf(ex_stg->s_enc.preset.cmd[preset_index], conf_set);
}

void set_tune_to_conf(CONF_ENC *conf_set, int tune_index) {
    if (ex_stg->s_enc.tune.name[tune_index].name)
        set_cmd_to_conf(ex_stg->s_enc.tune.cmd[tune_index], conf_set);
}

void set_profile_to_conf(CONF_ENC *conf_set, int profile_index) {
    if (ex_stg->s_enc.profile.name[profile_index].name)
        set_cmd_to_conf(ex_stg->s_enc.profile.cmd[profile_index], conf_set);
}

void apply_presets(CONF_ENC *conf_set) {
    set_preset_to_conf(conf_set, conf_set->preset);
    set_tune_to_conf(conf_set, conf_set->tune);
    set_profile_to_conf(conf_set, conf_set->profile);
}

//int check_profile_x264(const CONF_X264 *conf_set) {
//    CONF_X264 check;
//    int profile_index;
//    for (profile_index = 0; ex_stg->s_x264.profile.cmd[profile_index]; profile_index++) {
//        memcpy(&check, conf_set, sizeof(CONF_X264));
//        set_cmd_to_conf(ex_stg->s_x264.profile.cmd[profile_index], (CONF_ENC *)&check, ENC_TYPE_X264);
//        if (memcmp(&check, conf_set, sizeof(CONF_X264)) == NULL)
//            return profile_index;
//    }
//    return profile_index - 1;
//}

void build_cmd_from_conf(char *cmd, size_t nSize, const CONF_ENC *conf, const void *_vid, BOOL write_all) {
    CONF_ENC x265def = { 0 };
    CONF_ENC *def = (CONF_ENC *)&x265def;
    CONF_VIDEO *vid = (CONF_VIDEO *)_vid;
    get_default_conf(   (CONF_ENC *)def, conf->bit_depth > 8);
    set_preset_to_conf( (CONF_ENC *)def, conf->preset);
    set_tune_to_conf(   (CONF_ENC *)def, conf->tune);
    set_profile_to_conf((CONF_ENC *)def, conf->profile);

    for (X265_OPTIONS *opt = x265_options_table; opt->long_name; opt++) {
        int len = write_cmd_x265[opt->type](cmd, nSize, opt, conf, def, vid, write_all);
        nSize -= len;
        cmd += len;
        if (opt->p_offset && (opt->p_offset == (opt+1)->p_offset) && len && !write_all)
            opt++;
    }
}

void set_guiEx_auto_sar(int *sar_x, int *sar_y, int width, int height) {
    if (width > 0 && height > 0 && *sar_x < 0 && *sar_y < 0) {
        int x = -1 * *sar_x * height;
        int y = -1 * *sar_y * width;
        if (abs(y - x) > -16 * *sar_y) {
            int gcd = get_gcd(x, y);
            *sar_x = x / gcd;
            *sar_y = y / gcd;
        } else {
            *sar_x = *sar_y = 1;
        }
    } else if (*sar_x * *sar_y < 0) {
        *sar_x = *sar_y = 0;
    }
}
static void set_guiEx_auto_sar(CONF_ENC *cx, int width, int height) {
    set_guiEx_auto_sar(&cx->sar.x, &cx->sar.y, width, height);
}

static void set_guiEx_auto_colormatrix(CONF_ENC *cx, int height) {
    int auto_matrix = (height >= COLOR_MATRIX_THRESHOLD) ? COLOR_MATRIX_HD : COLOR_MATRIX_SD;
    if (cx->colormatrix == COLOR_MATRIX_AUTO)
        cx->colormatrix = auto_matrix;
    if (cx->colorprim == COLOR_MATRIX_AUTO)
        cx->colorprim = auto_matrix;
    if (cx->transfer == COLOR_MATRIX_AUTO)
        cx->transfer = auto_matrix;
}

//static void set_guiEx_auto_vbv(CONF_ENC *cx, int width, int height, int fps_num, int fps_den, BOOL ref_limit_by_level) {
//    if (cx->vbv_bufsize < 0 || cx->vbv_maxrate < 0) {
//        int profile_index = check_profile_x264((CONF_ENC *)cx);
//        int level_index = cx->h26x_level;
//        if (!level_index)
//            level_index = calc_auto_level(width, height, (ref_limit_by_level) ? 0 : cx->ref_frames, cx->interlaced, fps_num, fps_den, cx->vbv_maxrate, cx->vbv_bufsize);
//        int *vbv_buf = (cx->vbv_bufsize < 0) ? &cx->vbv_bufsize : NULL;
//        int *vbv_max = (cx->vbv_maxrate < 0) ? &cx->vbv_maxrate : NULL;
//        get_vbv_value(vbv_max, vbv_buf, level_index, profile_index, 8 < cx->bit_depth, ex_stg);
//    }
//}

void set_guiEx_auto_keyint(int *keyint_max, int fps_num, int fps_den) {
    if (*keyint_max == AUO_KEYINT_MAX_AUTO) {
        *keyint_max = (int)((fps_num + (fps_den - 1)) / fps_den) * 10; // 60000/1001 fpsの時に 600になるように最後に10倍する (599とか嫌すぎる)
    }
}

//static void set_guiEx_auto_ref_limit_by_level(CONF_ENC *cx, int width, int height, int fps_num, int fps_den, BOOL ref_limit_by_level) {
//    x264用のコード
//    int level_index = cx->h26x_level;
//    if (!level_index)
//        level_index = calc_auto_level(width, height, 0, cx->interlaced, fps_num, fps_den, cx->vbv_maxrate, cx->vbv_bufsize);
//    cx->ref_frames = max(1, min(cx->ref_frames, get_ref_limit(level_index, width, height, cx->interlaced)));
//}

#pragma warning( push )
#pragma warning( disable: 4100 )
void apply_guiEx_auto_settings(CONF_ENC *cx, int width, int height, int fps_num, int fps_den, BOOL ref_limit_by_level) {
    set_guiEx_auto_sar(cx, width, height);
    set_guiEx_auto_colormatrix(cx, height);
    set_guiEx_auto_keyint(&cx->keyint_max, fps_num, fps_den);
    //set_guiEx_auto_vbv(cx, width, height, fps_num, fps_den, ref_limit_by_level);
    //set_guiEx_auto_ref_limit_by_level(cx, width, height, fps_num, fps_den, ref_limit_by_level);
}
#pragma warning( pop )

const ENC_OPTION_STR * get_option_list_x265(const char *option_name) {
    for (int i = 0; x265_options_table[i].long_name; i++)
        if (x265_options_table[i].type == OPTION_TYPE_LIST || x265_options_table[i].type == OPTION_TYPE_INT)
            if (strcmp(x265_options_table[i].long_name, option_name) == NULL)
                return x265_options_table[i].list;
    return NULL;
}

void set_ex_stg_ptr(guiEx_settings *_ex_stg) {
    ex_stg = _ex_stg;
}

int get_option_value(const char *cmd_src, const char *target_option_name, char *buffer, size_t nSize) {
    //初期化
    buffer[0] = '\0';
    //LONG_OPTIONかSHORT_OPTIONかの判定
    if (target_option_name[0] != '-')
        return -1;
    target_option_name++;
    BOOL target_arg_type = (target_option_name[0] == '-');
    target_option_name += target_arg_type;

    int ret = 1;
    std::vector<CMD_ARG> cmd_arg_list;
    //parse_argでコマンドラインは書き変えられるので、
    //一度コピーしておく
    size_t cmd_len = strlen(cmd_src);
    char * const cmd = (char *)malloc((cmd_len + 1) * sizeof(cmd[0]));
    memcpy(cmd, cmd_src, (cmd_len + 1) * sizeof(cmd[0]));
    set_setting_list();
    parse_arg(cmd, cmd_len, cmd_arg_list);
    for (auto arg : cmd_arg_list) {
        if (arg.arg_type == target_arg_type) {
            if (NULL == strcmp(arg.option_name, target_option_name)) {
                if (arg.value) {
                    strcpy_s(buffer, nSize, arg.value);
                    ret = 0; //正常に取得
                }
                break;
            }
        }
    }
    free(cmd);
    return ret;
}
