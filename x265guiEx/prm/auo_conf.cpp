// -----------------------------------------------------------------------------------------
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

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include "auo_util.h"
#include "auo_conf.h"
#include "auo_options.h"

const int guiEx_config::conf_block_data[CONF_BLOCK_COUNT] = {
    sizeof(CONF_VIDEO),
    sizeof(CONF_ENC),
    sizeof(CONF_AUDIO),
    sizeof(CONF_MUX),
    sizeof(CONF_OTHER),
};

const size_t guiEx_config::conf_block_pointer[CONF_BLOCK_COUNT] = {
    offsetof(CONF_GUIEX, vid),
    offsetof(CONF_GUIEX, enc),
    offsetof(CONF_GUIEX, aud),
    offsetof(CONF_GUIEX, mux),
    offsetof(CONF_GUIEX, oth),
};

guiEx_config::guiEx_config() { }

void guiEx_config::write_conf_header(CONF_GUIEX *save_conf) {
    sprintf_s(save_conf->conf_name, sizeof(save_conf->conf_name), CONF_NAME);
    save_conf->size_all = sizeof(CONF_GUIEX);
    save_conf->head_size = CONF_HEAD_SIZE;
    save_conf->block_count = CONF_BLOCK_COUNT;
    for (int i = 0; i < CONF_BLOCK_COUNT; ++i) {
        save_conf->block_size[i] = conf_block_data[i];
        save_conf->block_head_p[i] = conf_block_pointer[i];
    }
}

//設定ファイルサイズを自動拡張する
//拡張できない場合 FALSEを返す
BOOL guiEx_config::adjust_conf_size(CONF_GUIEX *conf_buf, void *old_data, int old_size) {
    BOOL ret = FALSE;
    init_CONF_GUIEX(conf_buf, FALSE);
    if (((CONF_GUIEX *)old_data)->size_all != CONF_INITIALIZED)
        return ret;
    if (old_size == sizeof(CONF_GUIEX)) {
        memcpy(conf_buf, old_data, old_size);
        if (0 == strcmp(conf_buf->conf_name, CONF_NAME_OLD3))
            convert_x265stgv2_to_x265stgv4(conf_buf);
        
        if (0 == strcmp(conf_buf->conf_name, CONF_NAME_OLD4))
            convert_x265stgv3_to_x265stgv4(conf_buf);
        ret = TRUE;
    } else {
        const void *data_table = NULL;
        if (((CONF_GUIEX *)old_data)->block_count) {
            //新しい形式からの調整
            //ブロックサイズは保存されている
            data_table = old_data;
        }
        if (data_table == NULL)
            return ret;
        if (6 == ((CONF_GUIEX *)data_table)->block_count) {
            //x264/x265guiEx 3.xx系の設定ファイルから変換する
            //x26xguiEx 3.xxβ系の設定ファイルから変換する
            convert_x26xstg_to_x265stgv4(conf_buf, old_data);
        } else {
            if (0 == strcmp(((CONF_GUIEX *)old_data)->conf_name, CONF_NAME_OLD3))
                convert_x265stgv2_to_x265stgv4((CONF_GUIEX *)old_data);
            
            if (0 == strcmp(((CONF_GUIEX *)old_data)->conf_name, CONF_NAME_OLD4))
                convert_x265stgv3_to_x265stgv4((CONF_GUIEX *)old_data);

            BYTE *dst = (BYTE *)conf_buf;
            BYTE *block = NULL;
            dst += CONF_HEAD_SIZE;
            //ブロック部分のコピー
            for (int i = 0; i < ((CONF_GUIEX *)data_table)->block_count; ++i) {
                block = (BYTE *)old_data + ((CONF_GUIEX *)data_table)->block_head_p[i];
                dst = (BYTE *)conf_buf + conf_block_pointer[i];
                memcpy(dst, block, std::min(((CONF_GUIEX *)data_table)->block_size[i], conf_block_data[i]));
            }
        }
        ret = TRUE;
    }
    return ret;
}

int guiEx_config::load_guiEx_conf(CONF_GUIEX *conf, const char *stg_file) {
    size_t conf_size = 0;
    BYTE *dst, *filedat;
    //初期化
    ZeroMemory(conf, sizeof(CONF_GUIEX));
    //ファイルからロード
    FILE *fp = NULL;
    if (fopen_s(&fp, stg_file, "rb") != NULL)
        return CONF_ERROR_FILE_OPEN;
    //設定ファイルチェック
    char conf_name[CONF_NAME_BLOCK_LEN + 32];
    fread(&conf_name, sizeof(char), CONF_NAME_BLOCK_LEN, fp);
    if (   strcmp(CONF_NAME,      conf_name)
        && strcmp(CONF_NAME_OLD1, conf_name)
        && strcmp(CONF_NAME_OLD2, conf_name)
        && strcmp(CONF_NAME_OLD3, conf_name)
        && strcmp(CONF_NAME_OLD4, conf_name)) {
        fclose(fp);
        return CONF_ERROR_FILE_OPEN;
    }
    fread(&conf_size, sizeof(int), 1, fp);
    BYTE *dat = (BYTE*)calloc(conf_size, 1);
    init_CONF_GUIEX(conf, FALSE);
    fseek(fp, 0, SEEK_SET);
    fread(dat, conf_size, 1, fp);
    fclose(fp);
    
    convert_x265stgv2_to_x265stgv4(conf);
    write_conf_header(conf);
    
    //旧設定ファイルから変換
    if (   0 == strcmp(CONF_NAME_OLD1, conf_name)
        || 0 == strcmp(CONF_NAME_OLD2, conf_name)) {
        convert_x26xstg_to_x265stgv4(conf, dat);
        return 0;
    }

    //ブロックサイズチェック
    if (((CONF_GUIEX *)dat)->block_count > CONF_BLOCK_COUNT)
        return CONF_ERROR_BLOCK_SIZE;

    dst = (BYTE *)conf;
    //filedat = (BYTE *)data;
    //memcpy(dst, filedat, data->head_size);
    dst += CONF_HEAD_SIZE;

    //ブロック部分のコピー
    for (int i = 0; i < ((CONF_GUIEX *)dat)->block_count; ++i) {
        filedat = dat + ((CONF_GUIEX *)dat)->block_head_p[i];
        dst = (BYTE *)conf + conf_block_pointer[i];
        memcpy(dst, filedat, std::min(((CONF_GUIEX *)dat)->block_size[i], conf_block_data[i]));
    }

    if (0 == strcmp(CONF_NAME_OLD3, conf_name))
        convert_x265stgv2_to_x265stgv4(conf);
    
    if (0 == strcmp(CONF_NAME_OLD4, conf_name))
        convert_x265stgv3_to_x265stgv4(conf);

    //x264/x265のオプション群はコマンドから設定を作る
    char *all_cmd = ((char *)dat) + ((CONF_GUIEX *)dat)->block_head_p[CONF_BLOCK_COUNT];
    set_cmd_to_conf(all_cmd, &conf->enc);

    //初期化するかどうかで使うので。
    conf->size_all = CONF_INITIALIZED;
    free(dat);
    return 0;
}

int guiEx_config::save_guiEx_conf(const CONF_GUIEX *conf, const char *stg_file) {
    CONF_GUIEX save_conf;
    memcpy(&save_conf, conf, sizeof(CONF_GUIEX));
    ZeroMemory(&save_conf.block_count, sizeof(save_conf.block_count));

    //展開したコマンドライン
    char cmd_all[MAX_CMD_LEN] = { 0 };
    build_cmd_from_conf(cmd_all, sizeof(cmd_all), &conf->enc, &conf->vid, TRUE);
    const DWORD cmd_all_len = strlen(cmd_all) + 1;

    //設定ファイルのブロックごとの大きさを書き込み
    sprintf_s(save_conf.conf_name, sizeof(save_conf.conf_name), CONF_NAME);
    save_conf.size_all = sizeof(CONF_GUIEX) + cmd_all_len;
    save_conf.head_size = CONF_HEAD_SIZE;
    save_conf.block_count = CONF_BLOCK_COUNT;
    for (int i = 0; i < CONF_BLOCK_COUNT; ++i) {
        save_conf.block_size[i] = conf_block_data[i];
        save_conf.block_head_p[i] = conf_block_pointer[i];
    }
    //最後に展開したコマンドラインを追加する
    save_conf.block_size[CONF_BLOCK_COUNT]   = cmd_all_len;
    save_conf.block_head_p[CONF_BLOCK_COUNT] = sizeof(CONF_GUIEX);

    //ファイルへ書きこみ
    FILE *fp = NULL;
    if (fopen_s(&fp, stg_file, "wb") != NULL)
        return CONF_ERROR_FILE_OPEN;
    fwrite(&save_conf, sizeof(CONF_GUIEX), 1, fp);
    fwrite(cmd_all, 1, cmd_all_len, fp);
    fclose(fp);
    return 0;
}