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
    { _T("output-depth"),     _T(""),   OPTION_TYPE_OUTPUT_DEPTH,  NULL,                 offsetof(CONF_ENC, bit_depth      ) },
    { _T("input-csp"),        _T(""),   OPTION_TYPE_LIST,          list_output_csp_x265, offsetof(CONF_ENC, output_csp     ) },
    { _T("pass"),             _T(""),   OPTION_TYPE_PASS,          NULL,                 offsetof(CONF_ENC, pass           ) },
    { _T("slow-firstpass"),   _T(""),   OPTION_TYPE_FIRST_PASS,    NULL,                 offsetof(CONF_ENC, slow_first_pass) },
    { _T("no-slow-firstpass"),_T(""),   OPTION_TYPE_FIRST_PASS_R,  NULL,                 offsetof(CONF_ENC, slow_first_pass) },
    //{ _T("stats"),            _T(""),   OPTION_TYPE_STATS,         NULL,                 NULL                                  },
    { _T("preset"),          _T("p"),   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, preset         ) },
    { _T("tune"),            _T("t"),   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, tune           ) },
    { _T("profile"),         _T("P"),   OPTION_TYPE_LIST,          NULL,                 offsetof(CONF_ENC, profile        ) },
    { _T("crf"),              _T(""),   OPTION_TYPE_CRF,           NULL,                 NULL                                 },
    { _T("bitrate"),           _T(""),  OPTION_TYPE_BITRATE,       NULL,                 NULL                                 },
    { _T("qp"),               _T("q"),  OPTION_TYPE_QP,            NULL,                 NULL                                 },
    { _T("lossless"),          _T(""),  OPTION_TYPE_LOSSLESS,      NULL,                 NULL                                 },
    //{ _T("ipratio"),          _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, ip_ratio       ) },
    //{ _T("pbratio"),          _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, pb_ratio       ) },
    //{ _T("qpmin"),            _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_min         ) },
    //{ _T("qpmax"),            _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_max         ) },
    //{ _T("qpstep"),           _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qp_step        ) },
    { _T("qcomp"),            _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, qp_compress    ) },
    { _T("cbqpoffs"),         _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, cbqpoffs       ) },
    { _T("crqpoffs"),         _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, crqpoffs       ) },
    //{ _T("chroma-qp-offset"), _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, chroma_qp_offset) },
    //{ _T("no-mbtree"),        _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, mbtree         ) },
    //{ _T("mbtree"),           _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, mbtree         ) },
    { _T("rc-lookahead"),     _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rc_lookahead   ) },
    { _T("vbv-bufsize"),      _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, vbv_bufsize    ) },
    { _T("vbv-maxrate"),      _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, vbv_maxrate    ) },
    { _T("aq-mode"),          _T(""),   OPTION_TYPE_INT,           list_aq,              offsetof(CONF_ENC, aq_mode        ) },
    { _T("aq-strength"),      _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, aq_strength    ) },
    { _T("psy-rd"),           _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { _T("no-psy-rd"),        _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { _T("psy-rdoq"),         _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, psy_rdoq       ) },
    { _T("no-psy-rdoq"),      _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, psy_rdoq       ) },
    { _T("rdoq-level"),       _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rdoq_level     ) },
    { _T("no-rdoq-level"),    _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, rdoq_level     ) },
    //{ _T("psy"),              _T(""),   OPTION_TYPE_PSY,           NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    //{ _T("no-psy"),           _T(""),   OPTION_TYPE_BOOL2_REVERSE, NULL,                 offsetof(CONF_ENC, psy_rd         ) },
    { _T("scenecut"),         _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, scenecut       ) },
    { _T("no-scenecut"),      _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, scenecut       ) },
    { _T("keyint"),           _T("I"),  OPTION_TYPE_KEYINT,        NULL,                 offsetof(CONF_ENC, keyint_max     ) },
    { _T("min-keyint"),       _T("i"),  OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, keyint_min     ) },
    { _T("no-open-gop"),      _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, open_gop       ) },
    { _T("open-gop"),         _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, open_gop       ) },
    //{ _T("no-cabac"),         _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, cabac          ) },
    //{ _T("cabac"),            _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, cabac          ) },
    { _T("bframes"),          _T("b"),  OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, bframes        ) },
    { _T("b-adapt"),          _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, b_adapt        ) },
    { _T("bframe-bias"),      _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, b_bias         ) },
    { _T("b-pyramid"),        _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, b_pyramid      ) },
    { _T("no-b-pyramid"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, b_pyramid      ) },
    //{ _T("slices"),           _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, slice_n        ) },
    //{ _T("no-interlace"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, interlaced     ) },
    { _T("deblock"),          _T("f"),  OPTION_TYPE_DEBLOCK,       NULL,                 NULL                                 },
    { _T("no-deblock"),       _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, use_deblock    ) },
    { _T("no-sao"),           _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, sao            ) },
    { _T("sao"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, sao            ) },
    { _T("no-interlace"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, interlaced     ) },
    { _T("interlace"),        _T(""),   OPTION_TYPE_LIST,          list_interlace_x265,  offsetof(CONF_ENC, interlaced     ) },
    //{ _T("bff"),              _T(""),   OPTION_TYPE_BFF,           NULL,                 NULL                                 },
    //{ _T("partitions"),       _T("A"),  OPTION_TYPE_MB_PARTITION,  NULL,                 offsetof(CONF_ENC, mb_partition   ) },
    //{ _T("no-8x8dct"),        _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, dct8x8         ) },
    //{ _T("8x8dct"),           _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, dct8x8         ) },
    { _T("no-wpp"),           _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, wpp            ) },
    { _T("wpp"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, wpp            ) },
    { _T("ctu"),             _T("s"),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, ctu            ) },
    { _T("tu-intra-depth"),   _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, tu_intra_depth ) },
    { _T("tu-inter-depth"),   _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, tu_inter_depth ) },
    { _T("me"),               _T(""),   OPTION_TYPE_LIST,          list_me_x265,         offsetof(CONF_ENC, me             ) },
    { _T("subme"),           _T("m"),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, subme          ) },
    { _T("merange"),          _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, me_range       ) },
    { _T("no-rect"),          _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, rect_mp        ) },
    { _T("rect"),             _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, rect_mp        ) },
    { _T("no-amp"),           _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, asymmnteric_mp ) },
    { _T("amp"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, asymmnteric_mp ) },
    { _T("limit-refs"),       _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, limit_refs     ) },
    { _T("no-limit-modes"),   _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, limit_modes    ) },
    { _T("limit-modes"),      _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, limit_modes    ) },
    //{ _T("no-early-skip"),    _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, early_skip     ) },
    //{ _T("early-skip"),       _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, early_skip     ) },
    //{ _T("no-chroma-me"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, chroma_me      ) },
    //{ _T("chroma-me"),        _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, chroma_me      ) },
    //{ _T("direct"),           _T(""),   OPTION_TYPE_LIST,          list_direct,          offsetof(CONF_ENC, direct_mv      ) },
    { _T("ref"),              _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, ref_frames     ) },
    //{ _T("no-mixed-ref"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, mixed_ref      ) },
    //{ _T("mixed-ref"),        _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, mixed_ref      ) },
    { _T("max-merge"),        _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, max_merge      ) },
    { _T("rskip"),            _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, recursion_skip ) },
    { _T("no-weightb"),       _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, weight_b       ) },
    { _T("weightb"),          _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, weight_b       ) },
    { _T("no-weightp"),       _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, weight_p       ) },
    { _T("weightp"),         _T("w"),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, weight_p       ) },
    //{ _T("rdpenalty"),        _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rdpenalty      ) },
    //{ _T("no-tskip"),         _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, tskip          ) },
    //{ _T("tskip"),            _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, tskip          ) },
    //{ _T("no-tskip-fast"),    _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, tskip_fast     ) },
    //{ _T("tskip-fast"),       _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, tskip_fast     ) },
    //{ _T("no-strong-intra-smoothing"),_T(""),OPTION_TYPE_BOOL_REVERSE,NULL,              offsetof(CONF_ENC, strong_intra_smooth ) },
    //{ _T("strong-intra-smoothing"),   _T(""),OPTION_TYPE_BOOL,        NULL,              offsetof(CONF_ENC, strong_intra_smooth ) },
    { _T("rd"),               _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, rd             ) },
    //{ _T("nr"),               _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, noise_reduction) },
    //{ _T("no-fast-pskip"),    _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, no_fast_pskip  ) },
    //{ _T("no-dct-decimate"),  _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, no_dct_decimate) },
    //{ _T("trellis"),          _T("t"),  OPTION_TYPE_INT,           list_trellis,         offsetof(CONF_ENC, trellis        ) },
    //{ _T("cqm"),              _T(""),   OPTION_TYPE_CQM,           list_cqm,             offsetof(CONF_ENC, cqm            ) },
    //{ _T("cqmfile"),          _T(""),   OPTION_TYPE_CQMFILE,       NULL,                 NULL                                 },
    { _T("colormatrix"),      _T(""),   OPTION_TYPE_LIST,          list_colormatrix,     offsetof(CONF_ENC, colormatrix    ) },
    { _T("colorprim"),        _T(""),   OPTION_TYPE_LIST,          list_colorprim,       offsetof(CONF_ENC, colorprim      ) },
    { _T("transfer"),         _T(""),   OPTION_TYPE_LIST,          list_transfer,        offsetof(CONF_ENC, transfer       ) },
    { _T("range"),            _T(""),   OPTION_TYPE_LIST,          list_range,           offsetof(CONF_ENC, input_range    ) },
    { _T("sar"),              _T(""),   OPTION_TYPE_SAR_X265,      list_sar_x265,        offsetof(CONF_ENC, sar            ) },
    { _T("extended-sar"),     _T(""),   OPTION_TYPE_SAR_X265,      NULL,                 offsetof(CONF_ENC, sar            ) },
    //{ _T("level"),            _T(""),   OPTION_TYPE_LEVEL,         list_x264guiEx_level, offsetof(CONF_ENC, h264_level     ) },
    { _T("videoformat"),      _T(""),   OPTION_TYPE_LIST,          list_videoformat,     offsetof(CONF_ENC, videoformat    ) },
    //{ _T("aud"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, aud            ) },
    //{ _T("pic-struct"),       _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pic_struct     ) },
    //{ _T("nal-hrd"),          _T(""),   OPTION_TYPE_LIST,          list_nal_hrd,         offsetof(CONF_ENC, nal_hrd        ) },
    //{ _T("bluray-compat"),    _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, bluray_compat  ) },
    { _T("qg-size"),          _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, qg_size          ) },
    { _T("pools"),            _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, pools          ) },
    { _T("frame-threads"),   _T("F"),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, frame_threads  ) },
    //{ _T("lookahead-threads"), _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, lookahead_threads) },
    //{ _T("sliced-threads"),   _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, sliced_threading) },
    //{ _T("log-level"),        _T(""),   OPTION_TYPE_LIST,          list_log_type,        offsetof(CONF_ENC, log_mode       ) },
    //{ _T("psnr"),             _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, psnr           ) },
    //{ _T("ssim"),             _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, ssim           ) },
    //{ _T("tcfile-in"),        _T(""),   OPTION_TYPE_TCFILE_IN,     NULL,                 NULL                                 },
    //{ _T("timebase"),         _T(""),   OPTION_TYPE_TIMEBASE,      NULL,                 NULL                                 },
    //{ _T("progress"),         _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, disable_progress) },
    //{ _T("no-progress"),      _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, disable_progress) },
    { _T("no-cutree"),        _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, cutree         ) },
    { _T("cutree"),           _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, cutree         ) },
    { _T("no-pmode"),         _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, pmode          ) },
    { _T("pmode"),            _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pmode          ) },
    { _T("no-pme"),           _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, pme            ) },
    { _T("pme"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, pme            ) },
    { _T("no-aq-motion"),     _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, aq_motion      ) },
    { _T("aq-motion"),        _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, aq_motion      ) },
    { _T("no-ssim-rd"),       _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, ssim_rd        ) },
    { _T("ssim-rd"),          _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, ssim_rd        ) },
    //{ _T("analysis-reuse-mode"), _T(""), OPTION_TYPE_BOOL,        NULL,                 offsetof(CONF_ENC, analysis_reuse) },
    { _T("analysis-reuse-level"), _T(""), OPTION_TYPE_INT,         NULL,                 offsetof(CONF_ENC, analysis_reuse_level) },
    //{ _T("refine-intra"),     _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, refine_intra)         },
    //{ _T("refine-inter"),     _T(""),   OPTION_TYPE_INT,           NULL,                 offsetof(CONF_ENC, refine_inter)         },
    { _T("no-hevc-aq"),       _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, hevc_aq) },
    { _T("hevc-aq"),          _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, hevc_aq) },
    { _T("no-svt"),           _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, svt) },
    { _T("svt"),              _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, svt) },
    { _T("fades"),            _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, fades) },
    { _T("no-fades"),         _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, fades) },
    { _T("hist-scenecut"),    _T(""),   OPTION_TYPE_BOOL,          NULL,                 offsetof(CONF_ENC, hist_scenecut) },
    { _T("no-hist-scenecut"), _T(""),   OPTION_TYPE_BOOL_REVERSE,  NULL,                 offsetof(CONF_ENC, hist_scenecut) },
    { _T("hist-threshold"),   _T(""),   OPTION_TYPE_FLOAT,         NULL,                 offsetof(CONF_ENC, hist_threshold) },
    { _T("rskip-edge-threshold"), _T(""), OPTION_TYPE_INT,         NULL,                 offsetof(CONF_ENC, rskip_edge_threshold) },
    { NULL,               NULL, NULL,                      NULL,                 NULL                                 },
};

static BOOL auo_strtol(int *i, const TCHAR *str, DWORD len) {
    TCHAR *eptr = NULL;
    int v;
    BOOL ret = TRUE;
    if (len == NULL) len = ULONG_MAX;
    if (*str != _T('{')) {
        v = _tcstol(str, &eptr, 0);
        if (*eptr == _T('\0') || (DWORD)(eptr - str) == len) { *i = v; } else { ret = FALSE; }
    } else {
        str++;
        BOOL multi = (*str == _T('*'));
        v = _tcstol(str + multi, &eptr, 0);
        if (*eptr == _T('}')) { (multi) ? *i *= v : *i += v; } else { ret = FALSE; }
    }
    return ret;
}

static BOOL auo_strtof(float *f, const TCHAR *str, DWORD len) {
    TCHAR *eptr = NULL;
    float v;
    BOOL ret = TRUE;
    if (len == NULL) len = ULONG_MAX;
    if (*str != _T('{')) {
        v = (float)_tcstod(str, &eptr);
        if (*eptr == _T('\0') || (DWORD)(eptr - str) == len) { *f = v; } else { ret = FALSE; }
    } else {
        str++;
        BOOL multi = (*str == _T('*'));
        v = (float)_tcstod(str + multi, &eptr);
        if (*eptr == _T('}')) { (multi) ? *f *= v : *f += v; } else { ret = FALSE; }
    }
    return ret;
}

static BOOL auo_parse_int(int *i, const TCHAR *value, DWORD len) {
    BOOL ret;
    if ((ret = auo_strtol(i, value, len)) == FALSE) {
        size_t str_len = _tcslen(value);
        if (*value == _T('[') && value[str_len -1] == _T(']')) {
            const TCHAR *a, *b, *c;
            if ((a = _tcsstr(value, _T("if>"))) != NULL && (b = _tcsstr(value, _T("else"))) != NULL) {
                int v;
                c = a + _tcslen(_T("if>"));
                ret |= auo_strtol(&v, c, b-c);
                b += _tcslen(_T("else"));
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

static BOOL auo_parse_float(float *f, const TCHAR *value, DWORD len) {
    BOOL ret;
    if ((ret = auo_strtof(f, value, len)) == FALSE) {
        size_t val_len = _tcslen(value);
        if (*value == _T('[') && value[val_len -1] == _T(']')) {
            const TCHAR *a, *b, *c;
            if ((a = _tcsstr(value, _T("if>"))) != NULL && (b = _tcsstr(value, _T("else"))) != NULL) {
                float v;
                c = a + _tcslen(_T("if>"));
                ret |= auo_strtof(&v, c, b-c);
                b += _tcslen(_T("else"));
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

static BOOL set_bool(void *b, const TCHAR *value, const ENC_OPTION_STR *list) {
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

static BOOL set_bool_reverse(void *b, const TCHAR *value, const ENC_OPTION_STR *list) {
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
static BOOL set_int(void *i, const TCHAR *value, const ENC_OPTION_STR *list) {
    return auo_parse_int((int *)i, value, NULL);
}

static BOOL set_float(void *f, const TCHAR *value, const ENC_OPTION_STR *list) {
    return auo_parse_float((float *)f, value, NULL);
}

static BOOL set_int2(void *i, const TCHAR *value, const ENC_OPTION_STR *list) {
    const size_t len = _tcslen(value);
    //一度値をコピーして分析
    BOOL ret = FALSE;
    for (size_t j = 0; j < len; j++) {
        if (value[j] == _T(':') || value[j] == _T('|') || value[j] == _T(',') || value[j] == _T('/')) {
            ret = TRUE;
            if (!(j == _tcslen(_T("<unset>")) && _tcsnicmp(value, _T("<unset>"), _tcslen(_T("<unset>"))) == NULL))
                ret &= auo_parse_int(&((INT2 *)i)->x, value, j);
            if (_tcsicmp(&value[j+1], _T("<unset>")) != NULL)
                ret &= auo_parse_int(&((INT2 *)i)->y, &value[j+1], 0);
            break;
        }
    }
    return ret;
}

static BOOL set_bool2_reverse(void *b, const TCHAR *value, const ENC_OPTION_STR *list) {
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

static BOOL set_float2(void *f, const TCHAR *value, const ENC_OPTION_STR *list) {
    const size_t len = _tcslen(value);
    BOOL ret = FALSE;
    for (size_t j = 0; j < len; j++) {
        if (value[j] == _T(':') || value[j] == _T('|') || value[j] == _T(',') || value[j] == _T('/')) {
            ret = TRUE;
            if (!(j == _tcslen(_T("<unset>")) && _tcsnicmp(value, _T("<unset>"), _tcslen(_T("<unset>"))) == NULL))
                ret &= auo_parse_float(&((FLOAT2 *)f)->x, value, j);
            if (_tcsicmp(&value[j+1], _T("<unset>")) != NULL)
                ret &= auo_parse_float(&((FLOAT2 *)f)->y, &value[j+1], 0);
            break;
        }
    }
    return ret;
}

static BOOL set_list(void *i, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    for (int j = 0; list[j].name; j++) {
        if (_tcsicmp(value, list[j].name) == NULL) {
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
static BOOL set_crf(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = ENC_RC_CRF;
    float f = 23.0f;
    auo_strtof(&f, value, NULL);
    ((CONF_ENC *)cx)->crf = (int)(f * 100 + 0.5);
    return TRUE;
}
static BOOL set_bitrate(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = ENC_RC_BITRATE;
    return auo_strtol(&((CONF_ENC *)cx)->bitrate, value, NULL);
}
static BOOL set_qp(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = ENC_RC_QP;
    return auo_strtol(&((CONF_ENC *)cx)->qp, value, NULL);
}
static BOOL set_lossless(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->rc_mode = ENC_RC_QP;
    ((CONF_ENC *)cx)->qp = -1;
    return TRUE;
}
static BOOL set_keyint(void *i, const TCHAR *value, const ENC_OPTION_STR *list) {
    if ((*(int*)i = _tcsicmp(value, _T("infinite"))) != NULL)
        return auo_parse_int((int *)i, value, NULL);
    return TRUE;
}
static BOOL set_deblock(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    if (NULL == value) {
        ((CONF_ENC *)cx)->use_deblock = TRUE;
        ret = TRUE;
    } else {
        const int BUF_LEN = 128;
        const TCHAR *a = NULL;
        size_t len = _tcslen(value);
        if (*value == _T('[') && value[len-1] == _T(']') && NULL != (a = _tcsstr(value, _T("if_on"))) && BUF_LEN >= a - value && ((CONF_ENC *)cx)->use_deblock) {
            TCHAR tmp[BUF_LEN] = { 0 };
            memcpy(tmp, value + 1, a - value - 1);
            ret = set_int2(&((CONF_ENC *)cx)->deblock, tmp, list);
        } else if (FALSE != (ret = set_int2(&((CONF_ENC *)cx)->deblock, value, list))) {
            ((CONF_ENC *)cx)->use_deblock = TRUE;
        }
    }
    return ret;
}
static BOOL set_mb_partitions(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    *(DWORD*)cx = MB_PARTITION_NONE;
    if (stristr(value, _T("all"))) {
        *(DWORD*)cx = MB_PARTITION_ALL;
    } else if (stristr(value, _T("none"))) {
        ;//なにもしない
    } else {
        BOOL fin = FALSE;
        for (const TCHAR *p = value, *q = value; !fin; p++) {
            if (*p == _T('\0')) fin = TRUE;
            if (fin || *p == _T(',') || *p == _T('/') || *p == _T(';') || *p == _T(':')) {
                int len = p - q;
                if (     !_tcsnicmp(q, _T("p8x8"), len))
                    *(DWORD*)cx |= MB_PARTITION_P8x8;
                else if (!_tcsnicmp(q, _T("b8x8"), len))
                    *(DWORD*)cx |= MB_PARTITION_B8x8;
                else if (!_tcsnicmp(q, _T("p4x4"), len))
                    *(DWORD*)cx |= MB_PARTITION_P4x4;
                else if (!_tcsnicmp(q, _T("i8x8"), len))
                    *(DWORD*)cx |= MB_PARTITION_I8x8;
                else if (!_tcsnicmp(q, _T("i4x4"), len))
                    *(DWORD*)cx |= MB_PARTITION_I4x4;
                else
                    ret = FALSE;
                q = p + 1;
            }
        }
    }
    return ret;
}
static BOOL set_tff(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->interlaced = TRUE;
    ((CONF_ENC *)cx)->tff = TRUE;
    return TRUE;
}
static BOOL set_bff(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->interlaced = TRUE;
    ((CONF_ENC *)cx)->tff = FALSE;
    return TRUE;
}
static BOOL set_timebase(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    ((CONF_ENC *)cx)->use_timebase = TRUE;
    return set_int2(&((CONF_ENC *)cx)->timebase, value, list);
}
static BOOL set_level(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    size_t len = _tcslen(value);
    const TCHAR *tmp = value + len - 1;
    if (*tmp != _T('0'))
        ret = set_list(cx, value, list);
    if (!ret) {
        //"5.0"とかを"5"に整形
        TCHAR *copy = (TCHAR *)malloc((len + 1) * sizeof(copy[0]));
        memcpy(copy, value, (len + 1) * sizeof(copy[0]));
        TCHAR *p = copy + len - 1;
        while (*p == _T('0') && p >= copy)
            p--;
        if (*p == _T('.')) p--; //最後に'.'が残ったら消す
        *(p + 1) = _T('\0');
        ret = set_list(cx, copy, list);
        free(copy);
    }
    //整数指定を解析
    if (!ret) {
        int i = 0;
        if (auo_strtol(&i, value, NULL)) {
            TCHAR buf[16];
            if (i == 9)
                _stprintf_s(buf, _countof(buf), _T("1b"));
            else {
                size_t buf_len = _stprintf_s(buf, _countof(buf), _T("%.1f"), i / 10.0);
                TCHAR *p = buf + buf_len - 1;
                while (*p == _T('0') && p >= buf)
                    p--;
                if (*p == _T('.')) p--; //最後に'.'が残ったら消す
                *(p + 1) = _T('\0');
            }
            ret = set_list(cx, buf, list);
        }
    }
    return ret;
}
static BOOL set_analyse(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
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
static BOOL set_rc(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (NULL == _tcsncmp(value, _T("2pass"), _tcslen(_T("2pass")))) {
        ((CONF_ENC *)cx)->rc_mode = ENC_RC_BITRATE;
        ((CONF_ENC *)cx)->use_auto_npass = TRUE;
        ((CONF_ENC *)cx)->auto_npass = 2;
    } else if (NULL == _tcsncmp(value, _T("crf"), _tcslen(_T("crf")))) {
        ((CONF_ENC *)cx)->rc_mode = ENC_RC_CRF;
    } else if (NULL == _tcsncmp(value, _T("cbr"), _tcslen(_T("cbr")))
            || NULL == _tcsncmp(value, _T("abr"), _tcslen(_T("abr")))) {
        ((CONF_ENC *)cx)->rc_mode = ENC_RC_BITRATE;
        ((CONF_ENC *)cx)->use_auto_npass = FALSE;
    } else if (NULL == _tcsncmp(value, _T("cqp"), _tcslen(_T("cqp")))) {
        ((CONF_ENC *)cx)->rc_mode = ENC_RC_QP;
    } else {
        ret = FALSE;
    }
    return ret;
}
static BOOL set_aq(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    FLOAT2 f_val = { 0, 0 };
    BOOL ret = set_float2(&f_val, value, list);
    if (ret) {
        ((CONF_ENC *)cx)->aq_mode = ((int)(f_val.x + 0.5));
        ((CONF_ENC *)cx)->aq_strength = f_val.y;
    }
    return ret;
}
static BOOL set_interlaced(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (!value) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = TRUE;
    } else if (NULL == _tcsncmp(value, _T("tff"), _tcslen(_T("tff")))) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = TRUE;
    } else if (NULL == _tcsncmp(value, _T("bff"), _tcslen(_T("bff")))) {
        ((CONF_ENC *)cx)->interlaced = TRUE;
        ((CONF_ENC *)cx)->tff = FALSE;
    } else if (NULL == _tcsncmp(value, _T("0"), _tcslen(_T("0")))) {
        ((CONF_ENC *)cx)->interlaced = FALSE;
        ((CONF_ENC *)cx)->tff = FALSE;
    } else {
        ret = FALSE;
    }
    return ret;
}
static BOOL set_psy(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = TRUE;
    if (value) {
        if (NULL == _tcscmp(value, _T("0"))) {
            ((FLOAT2 *)cx)->x = 0.0;
            ((FLOAT2 *)cx)->y = 0.0;
        } else if (NULL == _tcscmp(value, _T("1"))) {
            ; //何もしない
        } else {
            ret = set_float2(cx, value, list);
        }
    }
    return ret;
}
static BOOL set_x265_sar(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    BOOL ret = FALSE;
    if (list) {
        for (int i = 0; list[i].name; i++) {
            if (NULL == _tcsicmp(value, list[i].name)) {
                int a = 0, b = 0;
                if (2 == _stscanf_s(list[i].name, _T("%d:%d"), &a, &b)) {
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
static BOOL set_do_nothing(void *cx, const TCHAR *value, const ENC_OPTION_STR *list) {
    return FALSE;
}


static int write_bool(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    BOOL *bptr = (BOOL*)((BYTE*)cx + options->p_offset);
    BOOL *defptr = (BOOL*)((BYTE*)def + options->p_offset);
    if ((write_all || *bptr != *defptr) && *bptr)
        return _stprintf_s(cmd, nSize, _T(" --%s"), options->long_name);
    return 0;
}

static int write_bool_reverse(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    BOOL *bptr = (BOOL*)((BYTE*)cx + options->p_offset);
    BOOL *defptr = (BOOL*)((BYTE*)def + options->p_offset);
    if ((write_all || *bptr != *defptr) && !(*bptr))
        return _stprintf_s(cmd, nSize, _T(" --%s"), options->long_name);
    return 0;
}

static int write_bool2_reverse(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *bptr = (INT2*)((BYTE*)cx + options->p_offset);
    if (!(bptr->x | bptr->y))
        return _stprintf_s(cmd, nSize, _T(" --%s"), options->long_name);
    return 0;
}

static int write_int(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr)
        return _stprintf_s(cmd, nSize, _T(" --%s %d"), options->long_name, *iptr);
    return 0;
}
//小数表示の鬱陶しい0を消す
static inline int write_float_ex(TCHAR *cmd, size_t nSize, float f) {
    double d = (int)(f * 1000.0 + 0.5 - (f<0)) / 1000.0; //これを入れないと22.2が22.19999とかになる
    int len = _stprintf_s(cmd, nSize, _T("%lf"), d);
    TCHAR *p = cmd + len - 1;
    while (*p == _T('0') && p >= cmd)
        p--;
    if (*p == _T('.')) p--; //最後に'.'が残ったら消す
    p++;
    *p = _T('\0');
    return p - cmd;
}

static int write_float(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    float *fptr = (float*)((BYTE*)cx + options->p_offset);
    float *defptr = (float*)((BYTE*)def + options->p_offset);
    if (write_all || abs(*fptr - *defptr) > EPS_FLOAT) {
        int len = _stprintf_s(cmd, nSize, _T(" --%s "), options->long_name);
        return len + write_float_ex(cmd + len, nSize - len, *fptr);
    }
    return 0;
}

static int write_int2(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *iptr = (INT2*)((BYTE*)cx + options->p_offset);
    INT2 *defptr = (INT2*)((BYTE*)def + options->p_offset);
    if (write_all || iptr->x != defptr->x || iptr->y != defptr->y)
        return _stprintf_s(cmd, nSize, _T(" --%s %d:%d"), options->long_name, iptr->x, iptr->y);
    return 0;
}

static int write_float2(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    FLOAT2 *fptr = (FLOAT2*)((BYTE*)cx + options->p_offset);
    FLOAT2 *defptr = (FLOAT2*)((BYTE*)def + options->p_offset);
    if (write_all || fptr->x != defptr->x || fptr->y != defptr->y) {
        int len = _stprintf_s(cmd, nSize, _T(" --%s "), options->long_name);
        len += write_float_ex(cmd + len, nSize - len, fptr->x);
        _stprintf_s(cmd + len, nSize - len, _T(":"));
        len += _tcslen(_T(":"));
        return len + write_float_ex(cmd + len, nSize - len, fptr->y);
    }
    return 0;
}

static int write_list(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr)
        return _stprintf_s(cmd, nSize, _T(" --%s %s"), options->long_name, options->list[*iptr].name);
    return 0;
}

static int write_crf(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == ENC_RC_CRF) {
        int len = _stprintf_s(cmd, nSize, _T(" --%s "), options->long_name);
        return len + write_float_ex(cmd + len, nSize - len, cx->crf / 100.0f);
    }
    return 0;
}
static int write_bitrate(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == ENC_RC_BITRATE) {
        int len = _stprintf_s(cmd, nSize, _T(" --%s %d"), options->long_name, cx->bitrate);
        if (cx->pass) {
            len += _stprintf_s(cmd + len, nSize - len, _T(" --pass %d"), cx->pass);
            len += _stprintf_s(cmd + len, nSize - len, _T(" --stats \"%s\""), vid->stats);
            if (cx->analysis_reuse) {
                len += _stprintf_s(cmd + len, nSize - len, _T(" --analysis-%s \"%s\""), (cx->pass == 1) ? _T("save") : _T("load"), vid->analysis_file);
            }
        }
        return len;
    }
    return 0;
}
static int write_qp(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == ENC_RC_QP) {
        if (cx->qp < 0) {
            return _stprintf_s(cmd, nSize, _T(" --lossless"));
        } else {
            return _stprintf_s(cmd, nSize, _T(" --%s %d"), options->long_name, cx->qp);
        }
    }
    return 0;
}
static int write_keyint_x264(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    int *iptr = (int*)((BYTE*)cx + options->p_offset);
    int *defptr = (int*)((BYTE*)def + options->p_offset);
    if (write_all || *iptr != *defptr) {
        return (*iptr != 0) ? _stprintf_s(cmd, nSize, _T(" --%s %d"), options->long_name, *iptr)
                            : _stprintf_s(cmd, nSize, _T(" --%s %s"), options->long_name, _T("infinite"));
    }
    return 0;
}
static int write_deblock(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->use_deblock)
        if (write_all || cx->deblock.x != def->deblock.x || cx->deblock.y != def->deblock.y)
            return _stprintf_s(cmd, nSize, _T(" --%s %d:%d"), options->long_name, cx->deblock.x, cx->deblock.y);
    return 0;
}
static int write_x265_sar(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    INT2 *iptr = (INT2*)((BYTE*)cx + options->p_offset);
    INT2 *defptr = (INT2*)((BYTE*)def + options->p_offset);
    if (write_all || iptr->x != defptr->x || iptr->y != defptr->y) {
        return _stprintf_s(cmd, nSize, _T(" --sar %d:%d"), iptr->x, iptr->y);
    }
    return 0;
}
static int write_input_depth(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->bit_depth > 8)
        return _stprintf_s(cmd, nSize, _T(" --input-depth 16 --output-depth %d"), cx->bit_depth);
    return 0;
}
static int write_slow_first_pass(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == ENC_RC_BITRATE && cx->pass) {
        return write_bool(cmd, nSize, options, cx, def, vid, write_all);
    }
    return 0;
}
static int write_slow_first_pass_r(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    if (cx->rc_mode == ENC_RC_BITRATE && cx->pass) {
        return write_bool_reverse(cmd, nSize, options, cx, def, vid, write_all);
    }
    return 0;
}
static int write_do_nothing(TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all) {
    return 0;
}

#pragma warning( pop ) //( disable: 4100 ) 終了

//この配列に従って各関数に飛ばされる
typedef BOOL (*SET_VALUE) (void *cx, const TCHAR *value, const ENC_OPTION_STR *list);
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
typedef int (*WRITE_CMD_x265) (TCHAR *cmd, size_t nSize, const X265_OPTIONS *options, const CONF_ENC *cx, const CONF_ENC *def, const CONF_VIDEO *vid, BOOL write_all);
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
static BOOL is_arg_ignore_mediainfo(const TCHAR *arg) {
    static const TCHAR * const IGNORE_ARGS[] = { _T("threads"), _T("lookahead_threads") };
    for (int i = 0; i < _countof(IGNORE_ARGS); i++)
        if (NULL == _tcscmp(arg, IGNORE_ARGS[i]))
            return TRUE;
    return FALSE;
}

static void parse_arg(TCHAR *cmd, size_t cmd_len, std::vector<CMD_ARG> &cmd_arg_list) {
    BOOL dQB = FALSE;
    BOOL space_flag = TRUE;
    BOOL next_option_flag = TRUE;
    CMD_ARG cmd_arg = { 0 };
    const TCHAR *cmd_fin = cmd + cmd_len;
    while (cmd < cmd_fin) {
        if (*cmd == _T('"')) dQB = !dQB;
        if (!dQB) {
            if ((*cmd == _T(' ') || *cmd == _T('\r') || *cmd == _T('\n'))) {
                *cmd = _T('\0');
                space_flag = TRUE;
            } else if (space_flag) {
                space_flag = FALSE;
                if (*cmd == _T('-') && !_istdigit(*(cmd+1))) { //isdigitは負数を避けるため
                    if (cmd_arg.arg_type) {
                        cmd_arg_list.push_back(cmd_arg);
                        ZeroMemory(&cmd_arg, sizeof(CMD_ARG));
                    }
                    cmd_arg.value = NULL;
                    cmd++;
                    if (*cmd == _T('-')) {
                        cmd++;
                        cmd_arg.arg_type = ARG_TYPE_LONG;
                    } else {
                        cmd_arg.arg_type = ARG_TYPE_SHORT;
                        if (cmd[1] != _T(' '))
                            cmd_arg.value = cmd + 1;
                    }
                    cmd_arg.option_name = cmd;
                    next_option_flag = FALSE;
                } else if (!next_option_flag) {
                    cmd_arg.value = cmd;
                    next_option_flag = TRUE;
                } else if (cmd[0] == _T('/') && cmd[1] == _T(' ')) {
                    ;
                } else {
                    //オプション名でもなく、オプション値でもない
                    //MediaInfoのオプションかどうかをチェック
                    BOOL b_QB = FALSE;
                    for (TCHAR *ptr = cmd; ptr <= cmd_fin; ptr++) {
                        if (*ptr == _T('"')) b_QB = !b_QB;
                        if (!b_QB) {
                            if ((*ptr == _T(' ') || *ptr == _T('\r') || *ptr == _T('\n') || ptr == cmd_fin)) {
                                TCHAR *equal_ptr = NULL;
                                // cmd から ptr の範囲で最後の '=' を探す
                                if (ptr > cmd) {
                                    for (TCHAR *p2 = ptr - 1; p2 >= cmd; p2--) {
                                        if (*p2 == _T('=')) { equal_ptr = p2; break; }
                                        if (p2 == cmd) break; // avoid underflow
                                    }
                                }
                                if (NULL != equal_ptr) {
                                    //mediainfoの書式
                                    *equal_ptr = _T('\0');
                                    CMD_ARG cmd_arg_media_info = { 0 };
                                    // equal_ptr より前で最後の空白を探す
                                    TCHAR *r_space_ptr = NULL;
                                    for (TCHAR *p3 = equal_ptr - 1; p3 >= cmd; p3--) {
                                        if (*p3 == _T(' ')) { r_space_ptr = p3; break; }
                                        if (p3 == cmd) break; // avoid underflow
                                    }
                                    cmd_arg_media_info.option_name = (r_space_ptr) ? r_space_ptr + 1 : cmd;
                                    if (!is_arg_ignore_mediainfo(cmd_arg_media_info.option_name)) {
                                        cmd_arg_media_info.arg_type = ARG_TYPE_LONG;
                                        cmd_arg_media_info.value = equal_ptr + 1;
                                        cmd_arg_media_info.type_mediainfo = TRUE;
                                        cmd_arg_list.push_back(cmd_arg_media_info);
                                    }
                                }
                                cmd = ptr;
                                *cmd = _T('\0');
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
        size_t value_len = _tcslen(it_arg->value);
        if (it_arg->value[0] == _T('"') && it_arg->value[value_len-1] == _T('"')) {
            it_arg->value[value_len-1] = _T('\0');
            it_arg->value++;
            it_arg->value_had_dQB = TRUE; //落としたことを記憶
        }
    }
}

static void set_setting_list() {
    if (ex_stg->get_reset_s_enc_referesh()) {
        for (DWORD i = 0, check = 0x00; check != (0x04|0x02|0x01) && x265_options_table[i].long_name; i++) {
            if        (!(check & 0x01) && _tcscmp(x265_options_table[i].long_name, _T("preset")) == NULL) {
                check |= 0x01; x265_options_table[i].list = ex_stg->s_enc.preset.name;
            } else if (!(check & 0x02) && _tcscmp(x265_options_table[i].long_name, _T("tune")) == NULL) {
                check |= 0x02; x265_options_table[i].list = ex_stg->s_enc.tune.name;
            } else if (!(check & 0x04) && _tcscmp(x265_options_table[i].long_name, _T("profile")) == NULL) {
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
            if        (NULL == _tcscmp(arg.option_name, _T("chroma_qp_offset"))) {
                chroma_qp_offset_from_mediainfo = arg.type_mediainfo;
            } else if (NULL == _tcscmp(arg.option_name, _T("keyint"))) {
                keyint_from_mediainfo = arg.type_mediainfo;
            } else if (NULL == _tcscmp(arg.option_name, _T("keyint_min"))) {
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
            if (NULL == ((it_arg->arg_type == ARG_TYPE_LONG) ? _tcscmp(it_arg->option_name, x265_options_table[i].long_name) :
                                                               _tcsncmp(it_arg->option_name, x265_options_table[i].short_name, 1))) {
                it_arg->ret = (option_has_no_value(x265_options_table[i].type) == FALSE && it_arg->value == NULL) ? FALSE : TRUE;
                break;
            }
        }
        if (it_arg->ret)
            it_arg->ret = set_value[x265_options_table[i].type]((void *)((BYTE *)conf_set + x265_options_table[i].p_offset), it_arg->value, x265_options_table[i].list);
    }
    //check_values_from_mediainfo(cmd_arg_list, conf_set);
}

void set_cmd_to_conf(TCHAR *cmd, CONF_ENC *conf_set, size_t cmd_len, BOOL build_not_imported_cmd) {
    std::vector<CMD_ARG> cmd_arg_list;
    set_setting_list();
    parse_arg(cmd, cmd_len, cmd_arg_list);
    set_conf_x265(cmd_arg_list, (CONF_ENC *)conf_set);
    if (build_not_imported_cmd) {
        //コマンドラインの再構築
        //it_arg->option_name のポインタが指すのはcmd上の文字列なので、
        //直接cmdに書き込むとおかしなことになる。
        //そこで一時領域を確保する
        TCHAR * const tmp = (TCHAR *)calloc((cmd_len + 1) * sizeof(tmp[0]), 1);
        size_t new_len = 0;
        for (auto arg : cmd_arg_list) {
            //正常に読み込まれていない、かつMediaInfoの書式でないものを再構成する
            if (!arg.ret && !arg.type_mediainfo) {
                new_len += _stprintf_s(tmp + new_len, cmd_len+1 - new_len, _T("%s%s%s"),
                    (new_len) ? _T(" ") : _T(""), (arg.arg_type == ARG_TYPE_LONG) ? _T("--") : _T("-"), arg.option_name);
                if (arg.value) {
                    const TCHAR * const dqb = (arg.value_had_dQB) ? _T("\"") : _T("");
                    new_len += _stprintf_s(tmp + new_len, cmd_len+1 - new_len, _T(" %s%s%s"), dqb, arg.value, dqb);
                }
            }
        }
        memcpy(cmd, tmp, (new_len + 1) * sizeof(cmd[0]));
        free(tmp);
    }
}

void set_cmd_to_conf(const TCHAR *cmd_src, CONF_ENC *conf_set) {
    //parse_argでコマンドラインは書き変えられるので、
    //一度コピーしておく
    size_t cmd_len = _tcslen(cmd_src) + 1;
    TCHAR * const cmd = (TCHAR *)malloc((cmd_len + 1) * sizeof(cmd[0]));
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

void set_cmd_to_conf_full(const TCHAR *cmd_src, CONF_ENC *conf_set) {
    // 一度ロードし、プロファイル等を再設定する
    CONF_ENC preload;
    get_default_conf(&preload, FALSE); //デフォルトを呼んでおく
    set_cmd_to_conf(cmd_src, &preload);
    memcpy(conf_set, &preload, sizeof(CONF_ENC));
    get_default_conf(conf_set, preload.bit_depth > 8);
    set_preset_to_conf(conf_set, preload.preset);
    set_tune_to_conf(conf_set, preload.tune);
    set_profile_to_conf(conf_set, preload.profile);
    set_cmd_to_conf(cmd_src, conf_set);
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

void build_cmd_from_conf(TCHAR *cmd, size_t nSize, const CONF_ENC *conf, const void *_vid, BOOL write_all) {
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

void set_guiEx_auto_keyint(CONF_ENC *cx, int fps_num, int fps_den) {
    if (cx->keyint_max == AUO_KEYINT_MAX_AUTO) {
        cx->keyint_max = (int)((fps_num + (fps_den - 1)) / fps_den) * 10; // 60000/1001 fpsの時に 600になるように最後に10倍する (599とか嫌すぎる)
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
    set_guiEx_auto_keyint(cx, fps_num, fps_den);
    //set_guiEx_auto_vbv(cx, width, height, fps_num, fps_den, ref_limit_by_level);
    //set_guiEx_auto_ref_limit_by_level(cx, width, height, fps_num, fps_den, ref_limit_by_level);
}
#pragma warning( pop )

const ENC_OPTION_STR * get_option_list_x265(const TCHAR *option_name) {
    for (int i = 0; x265_options_table[i].long_name; i++)
        if (x265_options_table[i].type == OPTION_TYPE_LIST || x265_options_table[i].type == OPTION_TYPE_INT)
            if (_tcscmp(x265_options_table[i].long_name, option_name) == NULL)
                return x265_options_table[i].list;
    return NULL;
}

void set_ex_stg_ptr(guiEx_settings *_ex_stg) {
    ex_stg = _ex_stg;
}

int get_option_value(const TCHAR *cmd_src, const TCHAR *target_option_name, TCHAR *buffer, size_t nSize) {
    //初期化
    buffer[0] = '\0';
    //LONG_OPTIONかSHORT_OPTIONかの判定
    if (target_option_name[0] != '-')
        return -1;
    target_option_name++;
    BOOL target_arg_type = (target_option_name[0] == _T('-'));
    target_option_name += target_arg_type;

    int ret = 1;
    std::vector<CMD_ARG> cmd_arg_list;
    //parse_argでコマンドラインは書き変えられるので、
    //一度コピーしておく
    size_t cmd_len = _tcslen(cmd_src);
    TCHAR * const cmd = (TCHAR *)malloc((cmd_len + 1) * sizeof(cmd[0]));
    memcpy(cmd, cmd_src, (cmd_len + 1) * sizeof(cmd[0]));
    set_setting_list();
    parse_arg(cmd, cmd_len, cmd_arg_list);
    for (auto arg : cmd_arg_list) {
        if (arg.arg_type == target_arg_type) {
            if (NULL == _tcscmp(arg.option_name, target_option_name)) {
                if (arg.value) {
                    _stprintf_s(buffer, nSize, arg.value);
                    ret = 0; //正常に取得
                }
                break;
            }
        }
    }
    free(cmd);
    return ret;
}
