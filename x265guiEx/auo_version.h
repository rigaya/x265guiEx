//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#ifndef _AUO_VERSION_H_
#define _AUO_VERSION_H_

#ifdef AUO_MAIN
#define EXTERN
#else
#define EXTERN extern
#endif //DEFINE_GLOBAL

#define AUO_VERSION           0,3,17,0
#define AUO_VERSION_STR       "3.17β"
#define AUO_NAME_X264         "x264guiEx.auo"
#define AUO_NAME_X265         "x265guiEx.auo"
#define AUO_NAME_X264_W      L"x264guiEx.auo"
#define AUO_NAME_X265_W      L"x265guiEx.auo"
#define AUO_FULL_NAME_X264    "拡張 x264 出力(GUI) Ex"
#define AUO_FULL_NAME_X265    "拡張 x265 出力(GUI) Ex べ～た"
#define AUO_VERSION_NAME_X264 "拡張 x264 出力(GUI) Ex "AUO_VERSION_STR
#define AUO_VERSION_NAME_X265 "拡張 x265 出力(GUI) Ex べ～た "AUO_VERSION_STR
#define AUO_VERSION_INFO_X264 "拡張 x264 出力(GUI) Ex (x264guiEx) "AUO_VERSION_STR" by rigaya"
#define AUO_VERSION_INFO_X265 "拡張 x265 出力(GUI) Ex べ～た (x265guiEx) "AUO_VERSION_STR" by rigaya"
#define AUO_EXT_FILTER        "All Support Formats (*.*)\0*.mp4;*.mkv;*.264;*.mp4\0mp4 file (*.mp4)\0*.mp4\0mkv file (*.mkv)\0*.mkv\0raw file (*.264)\0*.264\0"

EXTERN const char    *auo_name;
EXTERN const wchar_t *auo_name_w;
EXTERN const char    *auo_full_name;
EXTERN const char    *auo_version_name;
EXTERN const char    *auo_version_info;

#ifdef DEBUG
#define VER_DEBUG   VS_FF_DEBUG
#define VER_PRIVATE VS_FF_PRIVATEBUILD
#else
#define VER_DEBUG   0
#define VER_PRIVATE 0
#endif

#define VER_STR_COMMENTS         "拡張 x264/x265 出力(GUI) Ex"
#define VER_STR_COMPANYNAME      ""
#define VER_STR_FILEDESCRIPTION  "拡張 x264/x265 出力(GUI) Ex"
#define VER_FILEVERSION          AUO_VERSION
#define VER_STR_FILEVERSION      AUO_VERSION_STR
#define VER_STR_INTERNALNAME     "拡張 x264/x265 出力(GUI) Ex"
#define VER_STR_ORIGINALFILENAME "x264guiEx / x265guiEx"
#define VER_STR_LEGALCOPYRIGHT   "x264guiEx / x265guiEx by rigaya"
#define VER_STR_PRODUCTNAME      "x264guiEx / x265guiEx"
#define VER_PRODUCTVERSION       VER_FILEVERSION
#define VER_STR_PRODUCTVERSION   VER_STR_FILEVERSION

#endif //_AUO_VERSION_H_
