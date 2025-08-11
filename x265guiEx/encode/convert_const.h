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

#ifndef _CONVERT_CONST_H_
#define _CONVERT_CONST_H_

//色変換で用いる定数
//convert.cppとconvert_avx.cppで使うので分離

///---------------------------
///
///    定数の名前
///      Y: 輝度,       UV: 色差
///      L: 圧縮レンジ, F:  フルレンジ
///      8: 8bit,       16: 16bit
///      MUL: 積        ADD: 加算
///      MA : 積和
///      RSH: 右シフト
///      YCC: 圧縮レンジのゲタ
///
///-------------------------

///
///   計算式
///   ※clamp(x, low, high) = (((x) <= (high)) ? (((x) >= (low)) ? (x) : (low)) : (high))
///
///   YC48(y,cb,cr) -> YUV(8bit)
///
///            Y   = clamp((y * 219 + 383)>>12) + 16, 0, 255)
///   (YUV444) U,V = clamp(((cb,cr                     + 2048) * 14 + 132*1)>> 8) + 16, 0, 255)
///   (YUV420p)U,V = clamp(((cb0,cr0     + cb1,cr1     + 4096) * 14 + 132*2)>> 9) + 16, 0, 255)
///   (YUV420p)U,V = clamp(((cb0,cr0 * 3 + cb1,cr1 * 1 + 8192) * 14 + 132*4)>>10) + 16, 0, 255)
///
///   YC48 -> YUV(16bit)
///
///            Y   = clamp(((y * 219 + 383)>>6) + 4096, 0, 65535)
///   (YUV444) U,V = clamp(((cb,cr                     + 2048) * 14 + (132>>8)*1)>>0) + 4096, 0, 65535)
///   (YUV420p)U,V = clamp(((cb0,cr0     + cb1,cr1     + 4096) * 14 + (132>>8)*2)>>1) + 4096, 0, 65535)
///   (YUV420i)U,V = clamp(((cb0,cr0 * 3 + cb1,cr1 * 1 + 8192) * 14 + (132>>8)*4)>>2) + 4096, 0, 65535)

static const int LSFT_UV_OFFSET = 11;
static const int UV_OFFSET_x1 = (1<<(LSFT_UV_OFFSET  )); //4096
static const int UV_OFFSET_x2 = (1<<(LSFT_UV_OFFSET+1)); //8192
static const int UV_OFFSET_x4 = (1<<(LSFT_UV_OFFSET+2)); //16384

static const int RSFT_ONE      = 15; //0xffff>>15=0x0001
static const int LSFT_YCC_8    = 4; //1<<4 = 16
static const int LSFT_YCC_10   = 6; //1<<6 = 64
static const int LSFT_YCC_16   = 12; //1<<12 = 4096

//各ビットの最大値
static const int LIMIT_8    = (1<< 8) - 1;
static const int LIMIT_10   = (1<<10) - 1;
static const int LIMIT_16   = (1<<16) - 1;

//YC圧縮レンジ用定数
static const int Y_L_MUL    = 219;
static const int Y_L_ADD_8  = 383;
static const int Y_L_ADD_10 = Y_L_ADD_8>>2;
static const int Y_L_ADD_16 = Y_L_ADD_8>>8;
static const int Y_L_RSH_8  = 12;
static const int Y_L_RSH_10 = Y_L_RSH_8-2;
static const int Y_L_RSH_16 = Y_L_RSH_8-8;
static const int Y_L_YCC_8  = 16;
static const int Y_L_YCC_10 = Y_L_YCC_8<<2;
static const int Y_L_YCC_16 = Y_L_YCC_8<<8;

static const int UV_L_MUL         = 14;
static const int UV_L_ADD_8_444   = 132;
static const int UV_L_ADD_8_420P  = UV_L_ADD_8_444<<1;
static const int UV_L_ADD_8_420I  = UV_L_ADD_8_444<<2;
static const int UV_L_ADD_10_444  = UV_L_ADD_8_444>>2;
static const int UV_L_ADD_10_420P = UV_L_ADD_10_444<<1;
static const int UV_L_ADD_10_420I = UV_L_ADD_10_444<<2;
static const int UV_L_ADD_16_444  = UV_L_ADD_8_444>>8;
static const int UV_L_ADD_16_420P = UV_L_ADD_16_444<<1;
static const int UV_L_ADD_16_420I = UV_L_ADD_16_444<<2;
static const int UV_L_RSH_8_444   =  8;
static const int UV_L_RSH_8_420P  =  UV_L_RSH_8_444 + 1;
static const int UV_L_RSH_8_420I  =  UV_L_RSH_8_444 + 2;
static const int UV_L_RSH_10_444  =  UV_L_RSH_8_444 + 0 - 2;
static const int UV_L_RSH_10_420P =  UV_L_RSH_8_444 + 1 - 2;
static const int UV_L_RSH_10_420I =  UV_L_RSH_8_444 + 2 - 2;
static const int UV_L_RSH_16_444  =  UV_L_RSH_8_444 + 0 - 8;
static const int UV_L_RSH_16_420P =  UV_L_RSH_8_444 + 1 - 8;
static const int UV_L_RSH_16_420I =  UV_L_RSH_8_444 + 2 - 8;
static const int UV_L_YCC_8       = 16;
static const int UV_L_YCC_10      = UV_L_YCC_8<<2;
static const int UV_L_YCC_16      = UV_L_YCC_8<<8;

#define ALIGN32_CONST_ARRAY static const _declspec(align(32))

ALIGN32_CONST_ARRAY short Array_Y_L_MA_8[16]        = { Y_L_MUL,  Y_L_ADD_8,       Y_L_MUL,   Y_L_ADD_8,        Y_L_MUL,  Y_L_ADD_8,        Y_L_MUL,  Y_L_ADD_8,       Y_L_MUL,  Y_L_ADD_8,       Y_L_MUL,   Y_L_ADD_8,        Y_L_MUL,  Y_L_ADD_8,        Y_L_MUL,  Y_L_ADD_8       };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_8_420P[16]  = {UV_L_MUL, UV_L_ADD_8_420P, UV_L_MUL,  UV_L_ADD_8_420P,  UV_L_MUL, UV_L_ADD_8_420P,  UV_L_MUL, UV_L_ADD_8_420P, UV_L_MUL, UV_L_ADD_8_420P, UV_L_MUL,  UV_L_ADD_8_420P,  UV_L_MUL, UV_L_ADD_8_420P,  UV_L_MUL, UV_L_ADD_8_420P  };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_8_420I[2][16]  = {
    {UV_L_MUL * 3, UV_L_ADD_8_444 * 3, UV_L_MUL * 3,  UV_L_ADD_8_444 * 3,  UV_L_MUL * 3, UV_L_ADD_8_444 * 3,  UV_L_MUL * 3, UV_L_ADD_8_444 * 3, UV_L_MUL * 3, UV_L_ADD_8_444 * 3, UV_L_MUL * 3,  UV_L_ADD_8_444 * 3,  UV_L_MUL * 3, UV_L_ADD_8_444 * 3,  UV_L_MUL * 3, UV_L_ADD_8_444 * 3  },
    {UV_L_MUL,     UV_L_ADD_8_444,     UV_L_MUL,      UV_L_ADD_8_444,      UV_L_MUL,     UV_L_ADD_8_444,      UV_L_MUL,     UV_L_ADD_8_444,     UV_L_MUL,     UV_L_ADD_8_444,     UV_L_MUL,      UV_L_ADD_8_444,      UV_L_MUL,     UV_L_ADD_8_444,      UV_L_MUL,     UV_L_ADD_8_444      } };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_8_444[16]   = {UV_L_MUL, UV_L_ADD_8_444,  UV_L_MUL,  UV_L_ADD_8_444,   UV_L_MUL, UV_L_ADD_8_444,   UV_L_MUL, UV_L_ADD_8_444,   UV_L_MUL, UV_L_ADD_8_444,  UV_L_MUL,  UV_L_ADD_8_444,   UV_L_MUL, UV_L_ADD_8_444,   UV_L_MUL, UV_L_ADD_8_444};

ALIGN32_CONST_ARRAY short Array_Y_L_MA_10[16]       = { Y_L_MUL,  Y_L_ADD_10,      Y_L_MUL,   Y_L_ADD_10,       Y_L_MUL,  Y_L_ADD_10,       Y_L_MUL,  Y_L_ADD_10,       Y_L_MUL,  Y_L_ADD_10,      Y_L_MUL,   Y_L_ADD_10,       Y_L_MUL,  Y_L_ADD_10,       Y_L_MUL,  Y_L_ADD_10 };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_10_420P[16] = {UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P, UV_L_MUL, UV_L_ADD_10_420P };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_10_420I[2][16] =  {
    {UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3, UV_L_MUL * 3, UV_L_ADD_10_444 * 3 },
    {UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444,     UV_L_MUL,     UV_L_ADD_10_444     } };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_10_444[16]  = {UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444,  UV_L_MUL, UV_L_ADD_10_444  };
ALIGN32_CONST_ARRAY short Array_Y_L_MA_16[16]       = { Y_L_MUL,  Y_L_ADD_16,      Y_L_MUL,   Y_L_ADD_16,       Y_L_MUL,  Y_L_ADD_16,       Y_L_MUL,  Y_L_ADD_16,       Y_L_MUL,  Y_L_ADD_16,      Y_L_MUL,   Y_L_ADD_16,       Y_L_MUL,  Y_L_ADD_16,       Y_L_MUL,  Y_L_ADD_16 };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_16_420P[16] = {UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P, UV_L_MUL, UV_L_ADD_16_420P };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_16_420I[2][16] =  {
    {UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3, UV_L_MUL * 3, UV_L_ADD_16_444 * 3 },
    {UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444,     UV_L_MUL,     UV_L_ADD_16_444     } };
ALIGN32_CONST_ARRAY short Array_UV_L_MA_16_444[16]  = {UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444,  UV_L_MUL, UV_L_ADD_16_444  };

ALIGN32_CONST_ARRAY short Array_MASK_YCP2Y[2][16] = {
    0, -1, 0, 0, -1, 0, 0, -1,
    0, -1, 0, 0, -1, 0, 0, -1,
    0, 0, -1, 0, 0, -1, 0,  0,
    0, 0, -1, 0, 0, -1, 0,  0,
};
ALIGN32_CONST_ARRAY short Array_MASK_YCP2UV[2][16] = {
    -1, 0, 0,  0,  0, -1, -1, 0,
    -1, 0, 0,  0,  0, -1, -1, 0,
     0, 0, 0, -1, -1,  0,  0, 0,
     0, 0, 0, -1, -1,  0,  0, 0,
};
ALIGN32_CONST_ARRAY BYTE  Array_SUFFLE_YCP_Y[32] = {
    0, 1, 6, 7, 12, 13, 2, 3, 8, 9, 14, 15, 4, 5, 10, 11,
    0, 1, 6, 7, 12, 13, 2, 3, 8, 9, 14, 15, 4, 5, 10, 11
};
ALIGN32_CONST_ARRAY BYTE  Array_SUFFLE_YCP_C[16] = {
    2, 3, 4, 5, 14, 15, 1, 2, 10, 11, 12, 13, 6, 7, 8, 9
};

ALIGN32_CONST_ARRAY BYTE  Array_INTERLACE_WEIGHT[2][32] = {
    {1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3},
    {3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1}
};

static const float COEFF_RGB2YUV[2][9] = {
    { 0.299f,     0.587f,   0.114f,
     -0.168736f, -0.331264f,  0.5f,
      0.5f,      -0.418688f, -0.081312f },
    { 0.2126f,    0.7152f,    0.0722f,
     -0.114572f, -0.385427f,  0.5f,
      0.5f,      -0.453596f, -0.045977f }
};

#ifdef __AVX512BW__

#define INT_SHORT2(i) (((unsigned int)(i) << 16) | (unsigned int)(i))
#define INT_CHAR4(i)  (((unsigned int)(i) << 24) |((unsigned int)(i) << 16) |((unsigned int)(i) << 8) | (unsigned int)(i))

#define _mm512_broadcast_int(x) (_mm512_broadcastd_epi32(_mm_cvtsi32_si128(x)))
#define _mm512_broadcast_short(x) (_mm512_broadcastd_epi32(_mm_cvtsi32_si128(((((unsigned int)(x) << 16) | (unsigned int)(x))))))
#define _mm512_broadcast_char(x) (_mm512_broadcastd_epi32(_mm_cvtsi32_si128((((unsigned int)(x) << 24) | ((unsigned int)(x) << 16) | ((unsigned int)(x) << 8) | (unsigned int)(x)))))

#define zC_Y_L_MA_8           _mm512_broadcast_int(*(const int*) Array_Y_L_MA_8)
#define zC_UV_L_MA_8_420P     _mm512_broadcast_int(*(const int*)Array_UV_L_MA_8_420P)
#define zC_UV_L_MA_8_420I(i)  _mm512_broadcast_int(*(const int*)Array_UV_L_MA_8_420I[i])
#define zC_UV_L_MA_8_444      _mm512_broadcast_int(*(const int*)Array_UV_L_MA_8_444)
#define  zC_Y_L_MA_10         _mm512_broadcast_int(*(const int*) Array_Y_L_MA_10)
#define zC_UV_L_MA_10_420P    _mm512_broadcast_int(*(const int*)Array_UV_L_MA_10_420P)
#define zC_UV_L_MA_10_420I(i) _mm512_broadcast_int(*(const int*)Array_UV_L_MA_10_420I[i])
#define zC_UV_L_MA_10_444     _mm512_broadcast_int(*(const int*)Array_UV_L_MA_10_444)
#define  zC_Y_F_MA_10         _mm512_broadcast_int(*(const int*) Array_Y_F_MA_10)
#define zC_UV_F_MA_10_420P    _mm512_broadcast_int(*(const int*)Array_UV_F_MA_10_420P)
#define zC_UV_F_MA_10_420I(i) _mm512_broadcast_int(*(const int*)Array_UV_F_MA_10_420I[i])
#define zC_UV_F_MA_10_444     _mm512_broadcast_int(*(const int*)Array_UV_F_MA_10_444)
#define  zC_Y_L_MA_16         _mm512_broadcast_int(*(const int*) Array_Y_L_MA_16)
#define zC_UV_L_MA_16_420P    _mm512_broadcast_int(*(const int*)Array_UV_L_MA_16_420P)
#define zC_UV_L_MA_16_420I(i) _mm512_broadcast_int(*(const int*)Array_UV_L_MA_16_420I[i])
#define zC_UV_L_MA_16_444     _mm512_broadcast_int(*(const int*)Array_UV_L_MA_16_444)
#define  zC_Y_F_MA_16         _mm512_broadcast_int(*(const int*) Array_Y_F_MA_16)
#define zC_UV_F_MA_16_420P    _mm512_broadcast_int(*(const int*)Array_UV_F_MA_16_420P)
#define zC_UV_F_MA_16_420I(i) _mm512_broadcast_int(*(const int*)Array_UV_F_MA_16_420I[i])
#define zC_UV_F_MA_16_444     _mm512_broadcast_int(*(const int*)Array_UV_F_MA_16_444)

#elif defined(__AVX2__) //AVX2

#define yC_Y_L_MA_8           _mm256_load_si256((__m256i*) Array_Y_L_MA_8)
#define yC_UV_L_MA_8_420P     _mm256_load_si256((__m256i*)Array_UV_L_MA_8_420P)
#define yC_UV_L_MA_8_420I(i)  _mm256_load_si256((__m256i*)Array_UV_L_MA_8_420I[i])
#define yC_UV_L_MA_8_444      _mm256_load_si256((__m256i*)Array_UV_L_MA_8_444)
#define  yC_Y_L_MA_10         _mm256_load_si256((__m256i*) Array_Y_L_MA_10)
#define yC_UV_L_MA_10_420P    _mm256_load_si256((__m256i*)Array_UV_L_MA_10_420P)
#define yC_UV_L_MA_10_420I(i) _mm256_load_si256((__m256i*)Array_UV_L_MA_10_420I[i])
#define yC_UV_L_MA_10_444     _mm256_load_si256((__m256i*)Array_UV_L_MA_10_444)
#define  yC_Y_F_MA_10         _mm256_load_si256((__m256i*) Array_Y_F_MA_10)
#define yC_UV_F_MA_10_420P    _mm256_load_si256((__m256i*)Array_UV_F_MA_10_420P)
#define yC_UV_F_MA_10_420I(i) _mm256_load_si256((__m256i*)Array_UV_F_MA_10_420I[i])
#define yC_UV_F_MA_10_444     _mm256_load_si256((__m256i*)Array_UV_F_MA_10_444)
#define  yC_Y_L_MA_16         _mm256_load_si256((__m256i*) Array_Y_L_MA_16)
#define yC_UV_L_MA_16_420P    _mm256_load_si256((__m256i*)Array_UV_L_MA_16_420P)
#define yC_UV_L_MA_16_420I(i) _mm256_load_si256((__m256i*)Array_UV_L_MA_16_420I[i])
#define yC_UV_L_MA_16_444     _mm256_load_si256((__m256i*)Array_UV_L_MA_16_444)
#define  yC_Y_F_MA_16         _mm256_load_si256((__m256i*) Array_Y_F_MA_16)
#define yC_UV_F_MA_16_420P    _mm256_load_si256((__m256i*)Array_UV_F_MA_16_420P)
#define yC_UV_F_MA_16_420I(i) _mm256_load_si256((__m256i*)Array_UV_F_MA_16_420I[i])
#define yC_UV_F_MA_16_444     _mm256_load_si256((__m256i*)Array_UV_F_MA_16_444)

#define yC_INTERLACE_WEIGHT(i) _mm256_load_si256((__m256i*)Array_INTERLACE_WEIGHT[i])

#define yC_MASK_YCP2Y(i)       _mm256_load_si256((__m256i*)Array_MASK_YCP2Y[i])
#define yC_MASK_YCP2UV(i)      _mm256_load_si256((__m256i*)Array_MASK_YCP2UV[i])
#define yC_SUFFLE_YCP_Y        _mm256_load_si256((__m256i*)Array_SUFFLE_YCP_Y)

#else

#define xC_Y_L_MA_8           _mm_load_si128((__m128i*) Array_Y_L_MA_8)
#define xC_UV_L_MA_8_420P     _mm_load_si128((__m128i*)Array_UV_L_MA_8_420P)
#define xC_UV_L_MA_8_420I(i)  _mm_load_si128((__m128i*)Array_UV_L_MA_8_420I[i])
#define xC_UV_L_MA_8_444      _mm_load_si128((__m128i*)Array_UV_L_MA_8_444)
#define  xC_Y_L_MA_10         _mm_load_si128((__m128i*) Array_Y_L_MA_10)
#define xC_UV_L_MA_10_420P    _mm_load_si128((__m128i*)Array_UV_L_MA_10_420P)
#define xC_UV_L_MA_10_420I(i) _mm_load_si128((__m128i*)Array_UV_L_MA_10_420I[i])
#define xC_UV_L_MA_10_444     _mm_load_si128((__m128i*)Array_UV_L_MA_10_444)
#define  xC_Y_L_MA_16         _mm_load_si128((__m128i*) Array_Y_L_MA_16)
#define xC_UV_L_MA_16_420P    _mm_load_si128((__m128i*)Array_UV_L_MA_16_420P)
#define xC_UV_L_MA_16_420I(i) _mm_load_si128((__m128i*)Array_UV_L_MA_16_420I[i])
#define xC_UV_L_MA_16_444     _mm_load_si128((__m128i*)Array_UV_L_MA_16_444)

#define xC_INTERLACE_WEIGHT(i) _mm_load_si128((__m128i*)Array_INTERLACE_WEIGHT[i])

#define xC_MASK_YCP2Y(i)       _mm_load_si128((__m128i*)Array_MASK_YCP2Y[i])
#define xC_MASK_YCP2UV(i)      _mm_load_si128((__m128i*)Array_MASK_YCP2UV[i])
#define xC_SUFFLE_YCP_Y        _mm_load_si128((__m128i*)Array_SUFFLE_YCP_Y)

#endif //_INCLUDED_IMM

#endif //_CONVERT_CONST_H_
