//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#ifndef _AUO_OPTIONS_H_
#define _AUO_OPTIONS_H_

#include <Windows.h>
#include <vector>

#include "auo.h"
#include "auo_settings.h"

//エンコードモード
enum {
	X26X_RC_CRF = 0,
	X26X_RC_BITRATE,
	X26X_RC_QP,
};

//QPの最大値
const int X264_QP_MAX_8BIT  = 69;
const int X264_QP_MAX_10BIT = 81;

//差がこのくらいなら等しいとみなす(オプション用なのでこのくらいで十分)
const float EPS_FLOAT = 1.0e-4f;

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
	"yv12", //OUT_CSP_YV12
};
//文字列を引数にとるオプションの引数リスト
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
const X26X_OPTION_STR list_output_csp[] = {
	{ "i420", L"i420" },
	{ "i422", L"i422" },
	{ "i444", L"i444" },
	{ "rgb",  L"rgb"  },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_aq[] = { 
	{ NULL, L"none"    }, 
	{ NULL, L"VAQ"     }, 
	{ NULL, L"AutoVAQ" }, 
	{ NULL, NULL } 
};
const X26X_OPTION_STR list_weightp[] = { 
	{ NULL, L"Disabled" }, 
	{ NULL, L"Simple"   }, 
	{ NULL, L"Smart"    }, 
	{ NULL, NULL } 
};
const X26X_OPTION_STR list_b_adpat[] = {
	{ NULL, L"無効" },
	{ NULL, L"簡易" },
	{ NULL, L"完全" },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_trellis[] = {
	{ NULL, L"Disabled"        },
	{ NULL, L"Final MB encode" },
	{ NULL, L"All"             },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_me[] = {
	{ "dia",  L"Diamond Search (高速)" },
	{ "hex",  L"Hexagonal Search"      },
	{ "umh",  L"Uneven Multi-Hexagon"  },
	{ "esa",  L"Exaustive Search"      },
	{ "tesa", L"Hadamard ESA (低速)"   },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_subme[] = {
	{ NULL, L" 0 (fullpell only)"                },
	{ NULL, L" 1 (高速)"                         },
	{ NULL, L" 2"                                },
	{ NULL, L" 3"                                },
	{ NULL, L" 4"                                },
	{ NULL, L" 5"                                },
	{ NULL, L" 6 (RDO for I/P frames)"           },
	{ NULL, L" 7 (RDO for all frames)"           },
	{ NULL, L" 8 (RD refinement for I/P frames)" },
	{ NULL, L" 9 (RD refinement for all frames)" },
	{ NULL, L"10 (QP-RD)"                        },
	{ NULL, L"11 (Full RD)"                      },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_direct[] = {
	{ "none",     L"none"     },
	{ "spatial",  L"spatial"  },
	{ "temporal", L"temporal" },
	{ "auto",     L"auto"     },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_cqm[] = {
	{ "flat", NULL },
	{ "jvt",  NULL },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_input_range[] = {
	{ "auto", L"auto" },
	{ "pc",   L"pc" },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_colorprim[] = {
	{ "undef",     L"指定なし"  },
	{ "auto",      L"自動"      },
	{ "bt709",     L"bt709"     },
	{ "smpte170m", L"smpte170m" },
	{ "bt470m",    L"bt470m"    },
	{ "bt470bg",   L"bt470bg"   },
	{ "smpte240m", L"smpte240m" },
	{ "film",      L"film"      },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_transfer[] = {
	{ "undef",     L"指定なし"  },
	{ "auto",      L"自動"      },
	{ "bt709",     L"bt709"     },
	{ "smpte170m", L"smpte170m" },
	{ "bt470m",    L"bt470m"    },
	{ "bt470bg",   L"bt470bg"   },
	{ "smpte240m", L"smpte240m" },
	{ "linear",    L"linear"    },
	{ "log100",    L"log100"    },
	{ "log316",    L"log316"    },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_colormatrix[] = {
	{ "undef",     L"指定なし"  },
	{ "auto",      L"自動"      },
	{ "bt709",     L"bt709"     },
	{ "smpte170m", L"smpte170m" },
	{ "bt470bg",   L"bt470bg"   },
	{ "smpte240m", L"smpte240m" },
	{ "YCgCo",     L"YCgCo"     },
	{ "fcc",       L"fcc"       },
	{ "GBR",       L"GBR"       },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_videoformat[] = {
	{ "undef",     L"指定なし"  },
	{ "ntsc",      L"ntsc"      },
	{ "component", L"component" },
	{ "pal",       L"pal"       },
	{ "secam",     L"secam"     },
	{ "mac",       L"mac"       },
	{ NULL, NULL } 
};
const X26X_OPTION_STR list_log_type[] = {
	{ "none",    L"none"    },
	{ "error",   L"error"   },
	{ "warning", L"warning" },
	{ "info",    L"info"    },
	{ "debug",   L"debug"   },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_b_pyramid[] = {
	{ "none",   L"none"   },
	{ "strict", L"strict" },
	{ "normal", L"normal" },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_nal_hrd[] = {
	{ "none",   L"使用しない" },
	{ "vbr",    L"vbr"        },
	{ "cbr",    L"cbr"        },
	{ NULL, NULL }
};

const X26X_OPTION_STR list_rd[] = {
	{ NULL, L" 0 - least" },
	{ NULL, L" 1" },
	{ NULL, L" 2 - full"  },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_me_x265[] = {
	{ "dia",  L"Diamond Search (高速)" },
	{ "hex",  L"Hexagonal Search"      },
	{ "umh",  L"Uneven Multi-Hexagon"  },
	{ "star", L"Star Search"           },
	{ "full", L"Full Search (低速)"    },
	{ NULL, NULL }
};
const X26X_OPTION_STR list_subme_x265[] = {
	{ NULL, L" 0 (高速)"   },
	{ NULL, L" 1"          },
	{ NULL, L" 2"          },
	{ NULL, L" 3"          },
	{ NULL, L" 4"          },
	{ NULL, L" 5"          },
	{ NULL, L" 6"          },
	{ NULL, L" 7 (高品質)" },
	{ NULL, NULL }
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

//コード共通化のためいろいろ小細工をする
// CONF_X264 と CONF_X265 の共通部分を構造体の先頭に設置し、
// CONF_X26Xによりアクセスできるようにする
// 最終的にパラメータがx265で実装されなかったとしても気にしない
// この際、階層を深く掘らずにアクセスできるようにする
// また、面倒なことを避けるため、
// sizeof(CONF_X26X) == sizeof(CONF_X264) == sizeof(CONF_X265) になるようにする
// これも、コンパイラの自動計算によりそうなるよう細工する (手動でのパディングはしない)
#define VALUE_OF_STRUCT_COMMON \
	BOOL    use_highbit_depth; \
	int     output_csp; \
	int     pass; \
	BOOL    slow_first_pass; \
	BOOL    use_auto_npass; \
	int		auto_npass; \
	BOOL    nul_out; \
	int     preset; \
	int     tune; \
	int     profile; \
	int     rc_mode; \
	int     bitrate; \
	int     crf; \
	int     qp; \
	float   ip_ratio; \
	float   pb_ratio; \
	int     qp_min; \
	int     qp_max; \
	int     qp_step; \
	float   qp_compress; \
	BOOL    interlaced; \
	BOOL    tff; \
	BOOL    mbtree; \
	int     rc_lookahead; \
	int     vbv_bufsize; \
	int     vbv_maxrate; \
	int     aq_mode; \
	float   aq_strength; \
	FLOAT2  psy_rd; \
	int     scenecut; \
	int     keyint_min; \
	int     keyint_max; \
	BOOL    open_gop; \
	int     bframes; \
	int     b_adapt; \
	int     b_bias; \
	int     b_pyramid; \
	BOOL    weight_b; \
	int     weight_p; \
	int     me; \
	int     subme; \
	int     me_range; \
	int     ref_frames; \
	int     colormatrix; \
	int     colorprim; \
	int     transfer; \
	int     input_range; \
	int     videoformat; \
	int     h26x_level; \
	INT2    sar; \
	int     log_mode; \
	BOOL    psnr; \
	BOOL    ssim; \
	BOOL    use_tcfilein; \
	BOOL    use_timebase; \
	INT2    timebase; \
	int     threads;

//x264専用のオプション
#define VALUE_OF_STRUCT_X264 \
	int     chroma_qp_offset; \
	BOOL    cabac; \
	int     slice_n; \
	BOOL    use_deblock; \
	INT2    deblock; \
	DWORD   mb_partition; \
	BOOL	dct8x8; \
	BOOL    chroma_me; \
	int     direct_mv; \
	BOOL    mixed_ref; \
	int     noise_reduction; \
	BOOL    no_fast_pskip; \
	BOOL    no_dct_decimate; \
	int     trellis; \
	int     cqm; \
	BOOL    aud; \
	BOOL    pic_struct; \
	int     nal_hrd; \
	BOOL    bluray_compat; \
	int     sliced_threading; \
	BOOL    disable_progress; \
	int     lookahead_threads;

//x265専用のオプション
#define VALUE_OF_STRUCT_X265 \
	int     frame_threads; \
	BOOL    wpp; \
	int     ctu; \
	int     tu_intra_depth; \
	int     tu_inter_depth; \
	int     cbqpoffs; \
	int     crqpoffs; \
	BOOL    rect_mp; \
	BOOL    asymmnteric_mp; \
	int     max_merge; \
	BOOL    early_skip; \
	BOOL    fast_cbf; \
	int     rdpenalty; \
	BOOL    tskip; \
	BOOL    tskip_fast; \
	BOOL    strong_intra_smooth; \
	int     rd; \
	BOOL    loop_filter; \
	BOOL    sao;

//サイズ計算用
typedef struct CONF_X26x_dummy {
	VALUE_OF_STRUCT_COMMON;
} CONF_X26x_dummy;

//サイズ計算用
typedef struct CONF_X264_dummy {
	VALUE_OF_STRUCT_COMMON;
	VALUE_OF_STRUCT_X264;
} CONF_X264_dummy;

//サイズ計算用
typedef struct CONF_X265_dummy {
	VALUE_OF_STRUCT_COMMON;
	VALUE_OF_STRUCT_X265;
} CONF_X265_dummy;

typedef struct CONF_X264 {
	VALUE_OF_STRUCT_COMMON;
	VALUE_OF_STRUCT_X264;
	BYTE dummy[4 + max(sizeof(CONF_X264_dummy), sizeof(CONF_X265_dummy)) - sizeof(CONF_X264_dummy)]; // sizeof(CONF_X264) == sizeof(CONF_X265) とするため / 4足すのはサイズ"0"の配列となるのを避けるため
} CONF_X264;

typedef struct CONF_X265 {
	VALUE_OF_STRUCT_COMMON;
	VALUE_OF_STRUCT_X265;
	BYTE dummy[4 + max(sizeof(CONF_X264_dummy), sizeof(CONF_X265_dummy)) - sizeof(CONF_X265_dummy)]; // sizeof(CONF_X264) == sizeof(CONF_X265) とするため / 4足すのはサイズ"0"の配列となるのを避けるため
} CONF_X265;

typedef struct CONF_X26X {
	VALUE_OF_STRUCT_COMMON;
	BYTE dummy[sizeof(CONF_X264) - sizeof(CONF_X26x_dummy)]; //これにより最終的に sizeof(CONF_X26X) == sizeof(CONF_X264) == sizeof(CONF_X265) とする
} CONF_X26X;

#undef VALUE_OF_STRUCT_COMMON
#undef VALUE_OF_STRUCT_X264
#undef VALUE_OF_STRUCT_X265

typedef struct X26X_OPTIONS {
	char *long_name;
	char *short_name;
	DWORD type;
	const X26X_OPTION_STR *list;
	size_t p_offset;
} X26X_OPTIONS;

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
void set_cmd_to_conf(const char *cmd_src, CONF_X26X *conf_set, int enc_mode);
void set_cmd_to_conf(char *cmd, CONF_X26X *conf_set, size_t cmd_len, BOOL build_not_imported_cmd, int enc_mode);
void get_default_conf(CONF_X26X *conf_set, BOOL use_highbit, int enc_type);
//void set_preset_to_conf(CONF_X26X *conf_set, int preset_index, int enc_type);
//void set_tune_to_conf(CONF_X26X *conf_set, int tune_index, int enc_type);
void set_profile_to_conf(CONF_X26X *conf_set, int profile_index, int enc_type);
void apply_presets(CONF_X26X *conf_set, int enc_type);
int check_profile_x264(const CONF_X264 *conf_set);
void build_cmd_from_conf(char *cmd, size_t nSize, const CONF_X26X *conf, const void *_vid, BOOL write_all, int enc_type);
void build_cmd_from_conf_x264(char *cmd, size_t nSize, const CONF_X265 *conf, const void *_vid, BOOL write_all);
void build_cmd_from_conf_x265(char *cmd, size_t nSize, const CONF_X265 *conf, const void *_vid, BOOL write_all);
void set_guiEx_auto_sar(int *sar_x, int *sar_y, int width, int height);
void apply_guiEx_auto_settings(CONF_X26X *cx, int width, int height, int fps_num, int fps_den, int enc_type);
const X26X_OPTION_STR * get_option_list_x264(const char *option_name);
const X26X_OPTION_STR * get_option_list_x265(const char *option_name);
void set_ex_stg_ptr(guiEx_settings *_ex_stg);
int get_option_value(const char *cmd_src, const char *target_option_name, char *buffer, size_t nSize);

#endif //_AUO_OPTIONS_H_
