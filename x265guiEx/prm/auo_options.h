﻿// -----------------------------------------------------------------------------------------
// x264guiEx/x265guiEx/svtAV1guiEx/ffmpegOut/QSVEnc/NVEnc/VCEEnc by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2010-2022 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#ifndef _AUO_OPTIONS_H_
#define _AUO_OPTIONS_H_

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <vector>

#include "auo.h"
#include "auo_settings.h"

//エンコードモード
enum {
    X265_RC_CRF = 0,
    X265_RC_BITRATE,
    X265_RC_QP,
};

//QPの最大値
const int X264_QP_MAX_8BIT  = 69;
const int X264_QP_MAX_10BIT = 81;

//差がこのくらいなら等しいとみなす(オプション用なのでこのくらいで十分)
const float EPS_FLOAT = 1.0e-4f;

static const int AUO_KEYINT_MAX_AUTO = 0;

//マクロブロックタイプの一般的なオプション
enum {
    MB_PARTITION_NONE = 0x00000000,
    MB_PARTITION_P8x8 = 0x00000001,
    MB_PARTITION_B8x8 = 0x00000002,
    MB_PARTITION_P4x4 = 0x00000004,
    MB_PARTITION_I8x8 = 0x00000008,
    MB_PARTITION_I4x4 = 0x00000010,
    MB_PARTITION_ALL  = 0x0000001F,
};

enum {
    OUT_CSP_NV12,
    OUT_CSP_NV16,
    OUT_CSP_YUV444,
    OUT_CSP_RGB,
    OUT_CSP_YUY2,
    OUT_CSP_YV12,
    OUT_CSP_YUV422,
    OUT_CSP_YUV400,
};

enum {
    YC48_COLMAT_CONV_AUTO,
    YC48_COLMAT_CONV_NONE,
    YC48_COLMAT_CONV_BT709,
};

enum {
    AUTO_NPASS_NONE = 0,
    AUTO_NPASS_AMP,
    AUTO_NPASS_CRF,
};

//x264のinput-cspとして使用するもの
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
static const char * const specify_csp[] = {
    "nv12", //OUT_CSP_NV12
    "nv16", //OUT_CSP_NV16
    "i444", //OUT_CSP_YUV444
    "rgb",  //OUT_CSP_RGB
    "yuy2", //OUT_CSP_YUY2
    "i420", //OUT_CSP_YV12
    "i422", //OUT_CSP_YUV422
};
//文字列を引数にとるオプションの引数リスト
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
const ENC_OPTION_STR list_output_csp_x264[] = {
    { "i420", AUO_MES_UNKNOWN, L"i420" },
    { "i422", AUO_MES_UNKNOWN, L"i422" },
    { "i444", AUO_MES_UNKNOWN, L"i444" },
    { "rgb",  AUO_MES_UNKNOWN, L"rgb"  },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_output_csp_x265[] = {
    { "i420", AUO_MES_UNKNOWN, L"i420" },
    { "i422", AUO_MES_UNKNOWN, L"i422" },
    { "i444", AUO_MES_UNKNOWN, L"i444" },
    { NULL, AUO_MES_UNKNOWN, NULL },
    { NULL, AUO_MES_UNKNOWN, NULL },
    { "i420", AUO_MES_UNKNOWN, L"i420" },
    { "i422", AUO_MES_UNKNOWN, L"i422" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_aq[] = {
    { NULL, AUO_OPTION_AQ_NONE,         L"none"              },
    { NULL, AUO_OPTION_AQ_VAQ,          L"VAQ"               },
    { NULL, AUO_OPTION_AQ_AUTOVAQ,      L"AutoVAQ"           },
    { NULL, AUO_OPTION_AQ_AUTOVAQ_DARK, L"AutoVAQ(暗部保護)" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_weightp[] = {
    { NULL, AUO_MES_UNKNOWN, L"Disabled" },
    { NULL, AUO_MES_UNKNOWN, L"Simple"   },
    { NULL, AUO_MES_UNKNOWN, L"Smart"    },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_b_adpat[] = {
    { NULL, AUO_OPTION_BADAPT_DISABLED, L"無効" },
    { NULL, AUO_OPTION_BADAPT_SIMPLE,   L"簡易" },
    { NULL, AUO_OPTION_BADAPT_FULL,     L"完全" },
    { NULL, AUO_MES_UNKNOWN,            NULL }
};
const ENC_OPTION_STR list_trellis[] = {
    { NULL, AUO_MES_UNKNOWN, L"Disabled"        },
    { NULL, AUO_MES_UNKNOWN, L"Final MB encode" },
    { NULL, AUO_MES_UNKNOWN, L"All"             },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_direct[] = {
    { "none",     AUO_MES_UNKNOWN, L"none"     },
    { "spatial",  AUO_MES_UNKNOWN, L"spatial"  },
    { "temporal", AUO_MES_UNKNOWN, L"temporal" },
    { "auto",     AUO_MES_UNKNOWN, L"auto"     },
    { NULL,       AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_cqm[] = {
    { "flat", AUO_MES_UNKNOWN, NULL },
    { "jvt",  AUO_MES_UNKNOWN, NULL },
    { NULL,   AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_input_range[] = {
    { "auto", AUO_MES_UNKNOWN, L"auto" },
    { "pc",   AUO_MES_UNKNOWN, L"pc" },
    { NULL,   AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_interlace_x265[] = {
    { "prog", AUO_MES_UNKNOWN, L"progressive" },
    { "tff",  AUO_MES_UNKNOWN, L"tff" },
    { "bff",  AUO_MES_UNKNOWN, L"bff" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};

const ENC_OPTION_STR list_colorprim[] = {
    { "undef",     AUO_OPTION_VUI_UNDEF, L"指定なし"  },
    { "auto",      AUO_OPTION_VUI_AUTO, L"自動"      },
    { "bt709",     AUO_MES_UNKNOWN, L"bt709"     },
    { "smpte170m", AUO_MES_UNKNOWN, L"smpte170m" },
    { "bt470m",    AUO_MES_UNKNOWN, L"bt470m"    },
    { "bt470bg",   AUO_MES_UNKNOWN, L"bt470bg"   },
    { "smpte240m", AUO_MES_UNKNOWN, L"smpte240m" },
    { "film",      AUO_MES_UNKNOWN, L"film"      },
    { "bt2020",    AUO_MES_UNKNOWN, L"bt2020"    },
    { "smpte428",  AUO_MES_UNKNOWN, L"smpte428"  },
    { "smpte431",  AUO_MES_UNKNOWN, L"smpte431"  },
    { "smpte432",  AUO_MES_UNKNOWN, L"smpte432"  },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_transfer[] = {
    { "undef",        AUO_OPTION_VUI_UNDEF, L"指定なし"     },
    { "auto",         AUO_OPTION_VUI_AUTO, L"自動"         },
    { "bt709",        AUO_MES_UNKNOWN, L"bt709"        },
    { "smpte170m",    AUO_MES_UNKNOWN, L"smpte170m"    },
    { "bt470m",       AUO_MES_UNKNOWN, L"bt470m"       },
    { "bt470bg",      AUO_MES_UNKNOWN, L"bt470bg"      },
    { "smpte240m",    AUO_MES_UNKNOWN, L"smpte240m"    },
    { "linear",       AUO_MES_UNKNOWN, L"linear"       },
    { "log100",       AUO_MES_UNKNOWN, L"log100"       },
    { "log316",       AUO_MES_UNKNOWN, L"log316"       },
    { "iec61966-2-4", AUO_MES_UNKNOWN, L"iec61966-2-4" },
    { "bt1361e",      AUO_MES_UNKNOWN, L"bt1361e"      },
    { "iec61966-2-1", AUO_MES_UNKNOWN, L"iec61966-2-1" },
    { "bt2020-10",    AUO_MES_UNKNOWN, L"bt2020-10"    },
    { "bt2020-12",    AUO_MES_UNKNOWN, L"bt2020-12"    },
    { "smpte2084",    AUO_MES_UNKNOWN, L"smpte2084"},
    { "smpte428",     AUO_MES_UNKNOWN, L"smpte428" },
    { "arib-std-b67", AUO_MES_UNKNOWN, L"arib-std-b67" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_range[] = {
    { "limited",  AUO_MES_UNKNOWN,    L"limited"     },
    { "full",     AUO_MES_UNKNOWN,    L"full"        },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_colormatrix[] = {
    { "undef",     AUO_OPTION_VUI_UNDEF, L"指定なし"  },
    { "auto",      AUO_OPTION_VUI_AUTO, L"自動"      },
    { "bt709",     AUO_MES_UNKNOWN, L"bt709"     },
    { "smpte170m", AUO_MES_UNKNOWN, L"smpte170m" },
    { "bt470bg",   AUO_MES_UNKNOWN, L"bt470bg"   },
    { "smpte240m", AUO_MES_UNKNOWN, L"smpte240m" },
    { "ycgco",     AUO_MES_UNKNOWN, L"ycgco"     },
    { "fcc",       AUO_MES_UNKNOWN, L"fcc"       },
    { "gbr",       AUO_MES_UNKNOWN, L"gbr"       },
    { "bt2020nc",  AUO_MES_UNKNOWN, L"bt2020nc"  },
    { "bt2020c",   AUO_MES_UNKNOWN, L"bt2020c"   },
    { "smpte2085", AUO_MES_UNKNOWN, L"smpte2085" },
    { "chroma-derived-nc", AUO_MES_UNKNOWN, L"chroma-derived-nc" },
    { "chroma-derived-c",  AUO_MES_UNKNOWN, L"chroma-derived-c"  },
    { "ictcp",     AUO_MES_UNKNOWN, L"ictcp" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_videoformat[] = {
    { "undef",     AUO_OPTION_VUI_UNDEF, L"指定なし"  },
    { "ntsc",      AUO_MES_UNKNOWN, L"ntsc"      },
    { "component", AUO_MES_UNKNOWN, L"component" },
    { "pal",       AUO_MES_UNKNOWN, L"pal"       },
    { "secam",     AUO_MES_UNKNOWN, L"secam"     },
    { "mac",       AUO_MES_UNKNOWN, L"mac"       },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_sar_x265[] = {
    { "undef",     AUO_OPTION_VUI_UNDEF, L"undef"     },
    { "1:1",       AUO_MES_UNKNOWN, L"1:1"       },
    { "12:11",     AUO_MES_UNKNOWN, L"12:11"     },
    { "10:11",     AUO_MES_UNKNOWN, L"10:11"     },
    { "16:11",     AUO_MES_UNKNOWN, L"16:11"     },
    { "40:33",     AUO_MES_UNKNOWN, L"40:33"     },
    { "24:11",     AUO_MES_UNKNOWN, L"24:11"     },
    { "20:11",     AUO_MES_UNKNOWN, L"20:11"     },
    { "32:11",     AUO_MES_UNKNOWN, L"32:11"     },
    { "80:33",     AUO_MES_UNKNOWN, L"80:33"     },
    { "18:11",     AUO_MES_UNKNOWN, L"18:11"     },
    { "15:11",     AUO_MES_UNKNOWN, L"15:11"     },
    { "64:33",     AUO_MES_UNKNOWN, L"64:33"     },
    { "160:99",    AUO_MES_UNKNOWN, L"160:99"    },
    { "4:3",       AUO_MES_UNKNOWN, L"4:3"       },
    { "3:2",       AUO_MES_UNKNOWN, L"3:2"       },
    { "2:1",       AUO_MES_UNKNOWN, L"2:1"       },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_log_type[] = {
    { "none",    AUO_MES_UNKNOWN, L"none"    },
    { "error",   AUO_MES_UNKNOWN, L"error"   },
    { "warning", AUO_MES_UNKNOWN, L"warning" },
    { "info",    AUO_MES_UNKNOWN, L"info"    },
    { "debug",   AUO_MES_UNKNOWN, L"debug"   },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_b_pyramid[] = {
    { "none",   AUO_MES_UNKNOWN, L"none"   },
    { "strict", AUO_MES_UNKNOWN, L"strict" },
    { "normal", AUO_MES_UNKNOWN, L"normal" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_nal_hrd[] = {
    { "none",   AUO_MES_UNKNOWN, L"none" },
    { "vbr",    AUO_MES_UNKNOWN, L"vbr"  },
    { "cbr",    AUO_MES_UNKNOWN, L"cbr"  },
    { NULL, AUO_MES_UNKNOWN, NULL }
};

const ENC_OPTION_STR list_rd[] = {
    { NULL, AUO_OPTION_RD_0, L" 0 - least" },
    { NULL, AUO_OPTION_RD_1, L" 1" },
    { NULL, AUO_OPTION_RD_2, L" 2 - full"  },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_me_x265[] = {
    { "dia",  AUO_OPTION_ME_DIA,  L"Diamond Search (高速)" },
    { "hex",  AUO_OPTION_ME_HEX,  L"Hexagonal Search"      },
    { "umh",  AUO_OPTION_ME_UMH,  L"Uneven Multi-Hexagon"  },
    { "star", AUO_OPTION_ME_STAR, L"Star Search"           },
    { "sea",  AUO_OPTION_ME_SEA,  L"SEA Search"            },
    { "full", AUO_OPTION_ME_FULL, L"Full Search (低速)"    },
    { NULL, AUO_MES_UNKNOWN, NULL }
};
const ENC_OPTION_STR list_subme_x265[] = {
    { NULL, AUO_OPTION_SUBME_00, L" 0 (高速)"   },
    { NULL, AUO_OPTION_SUBME_01, L" 1"          },
    { NULL, AUO_OPTION_SUBME_02, L" 2"          },
    { NULL, AUO_OPTION_SUBME_03, L" 3"          },
    { NULL, AUO_OPTION_SUBME_04, L" 4"          },
    { NULL, AUO_OPTION_SUBME_05, L" 5"          },
    { NULL, AUO_OPTION_SUBME_06, L" 6"          },
    { NULL, AUO_OPTION_SUBME_07, L" 7 (高品質)" },
    { NULL, AUO_MES_UNKNOWN, NULL }
};


//色についてのオプション設定(0がデフォルトとなるよう、x264とは並び順が異なる)
//まあGUIで順番なんてどうでも…
enum {
    COLOR_MATRIX_DEFAULT   = 0,
    COLOR_MATRIX_AUTO      = 1,
    COLOR_MATRIX_HD        = 2,
    COLOR_MATRIX_SD        = 3,
};
const int COLOR_MATRIX_THRESHOLD = 720;

typedef struct INT2 {
    int x;
    int y;
} INT2;

typedef struct FLOAT2 {
    float x;
    float y;
} FLOAT2;

#pragma pack(push,4)
typedef struct CONF_ENC {
    int     bit_depth;
    int     output_csp;
    int     pass;
    BOOL    slow_first_pass;
    BOOL    use_auto_npass;
    int     auto_npass;
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
    int     interlaced;
    BOOL    tff;
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
    int     bframes;
    int     b_adapt;
    int     b_bias;
    int     b_pyramid;
    BOOL    weight_b;
    int     weight_p;
    int     me;
    int     subme;
    int     me_range;
    int     ref_frames;
    int     colormatrix;
    int     colorprim;
    int     transfer;
    int     input_range;
    int     videoformat;
    int     h26x_level;
    INT2    sar;
    int     log_mode;
    BOOL    psnr;
    BOOL    ssim;
    BOOL    use_tcfilein;
    BOOL    use_timebase;
    INT2    timebase;
    int     pools;
    int     frame_threads;
    BOOL    wpp;
    int     ctu;
    int     tu_intra_depth;
    int     tu_inter_depth;
    int     cbqpoffs;
    int     crqpoffs;
    BOOL    rect_mp;
    BOOL    asymmnteric_mp;
    int     max_merge;
    BOOL    early_skip;
    BOOL    fast_cbf;
    int     rdpenalty;
    BOOL    tskip;
    BOOL    tskip_fast;
    BOOL    strong_intra_smooth;
    int     rd;
    BOOL    use_deblock;
    BOOL    sao;
    BOOL    cutree;
    float   psy_rdoq;
    BOOL    pmode;
    BOOL    pme;
    INT2    deblock;
    int     rdoq_level;
    int     qg_size;
    int     recursion_skip;
    int     limit_refs;
    BOOL    limit_modes;
    BOOL    aq_motion;
    BOOL    ssim_rd;
    BOOL    analysis_reuse;
    int     analysis_reuse_level;
    int     refine_intra;
    int     refine_inter;
    int     hevc_aq;
    BOOL    svt;
    BOOL    fades;
    BOOL    hist_scenecut;
    float   hist_threshold;
    int     rskip_edge_threshold;
} CONF_ENC;
#pragma pack(pop)

typedef struct X265_OPTIONS {
    char *long_name;
    char *short_name;
    DWORD type;
    const ENC_OPTION_STR *list;
    size_t p_offset;
} X265_OPTIONS;

enum {
    ARG_TYPE_UNKNOWN = 0,
    ARG_TYPE_LONG    = 1,
    ARG_TYPE_SHORT   = 2,
};

typedef struct CMD_ARG {
    int arg_type;       //LONGかSHORTか
    char *option_name;  //オプション名(最初の"--"なし)
    char *value;        //オプションの値
    BOOL value_had_dQB; //前後に'"'があったか
    BOOL ret;           //構造体に読み込まれたかどうか
    BOOL type_mediainfo; //MediaInfoの書式だったかどうか
} CMD_ARG;

//コマンドラインの解析・生成
void set_cmd_to_conf(const char *cmd_src, CONF_ENC *conf_set);
void set_cmd_to_conf(char *cmd, CONF_ENC *conf_set, size_t cmd_len, BOOL build_not_imported_cmd);
void get_default_conf(CONF_ENC *conf_set, BOOL use_highbit);
//void set_preset_to_conf(CONF_ENC *conf_set, int preset_indexe);
//void set_tune_to_conf(CONF_ENC *conf_set, int tune_index);
void set_profile_to_conf(CONF_ENC *conf_set, int profile_index);
void apply_presets(CONF_ENC *conf_set);
void build_cmd_from_conf(char *cmd, size_t nSize, const CONF_ENC *conf, const void *_vid, BOOL write_all);
void build_cmd_from_conf_x264(char *cmd, size_t nSize, const CONF_ENC *conf, const void *_vid, BOOL write_all);
void build_cmd_from_conf_x265(char *cmd, size_t nSize, const CONF_ENC *conf, const void *_vid, BOOL write_all);
void set_guiEx_auto_sar(int *sar_x, int *sar_y, int width, int height);
void set_guiEx_auto_keyint(int *keyint_max, int fps_num, int fps_den);
void apply_guiEx_auto_settings(CONF_ENC *cx, int width, int height, int fps_num, int fps_den, BOOL ref_limit_by_level);
const ENC_OPTION_STR * get_option_list_x265(const char *option_name);
void set_ex_stg_ptr(guiEx_settings *_ex_stg);
int get_option_value(const char *cmd_src, const char *target_option_name, char *buffer, size_t nSize);

#endif //_AUO_OPTIONS_H_
