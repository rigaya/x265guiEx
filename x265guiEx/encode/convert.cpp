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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <emmintrin.h> //イントリンシック命令 SSE2

#include "convert.h"
#include "convert_const.h"

#ifndef clamp
#define clamp(x, low, high) (((x) <= (high)) ? (((x) >= (low)) ? (x) : (low)) : (high))
#endif

//直前の16byteアライメント
static inline void * get_aligned_next(void *p) {
    return (void *)(((size_t)p + 15) & ~15);
}
//直後の16byteアライメント
static inline void * get_aligned_prev(void *p) {
    return (void *)(((size_t)p) & ~15);
}
//16bit音声 -> 8bit音声
void convert_audio_16to8(BYTE *dst, short *src, int n) {
    BYTE *byte = dst;
    const BYTE *fin = byte + n;
    short *sh = src;
    while (byte < fin) {
        *byte = (*sh >> 8) + 128;
        byte++;
        sh++;
    }
}
//上のSSE2版
void convert_audio_16to8_sse2(BYTE *dst, short *src, int n) {
    BYTE *byte = dst;
    short *sh = src;
    BYTE * const loop_start = (BYTE *)get_aligned_next(dst);
    BYTE * const loop_fin   = (BYTE *)get_aligned_prev(dst + n);
    BYTE * const fin = dst + n;
    __m128i xSA, xSB;
    static const __m128i xConst = _mm_set1_epi16(128);
    //アライメント調整
    while (byte < loop_start) {
        *byte = (*sh >> 8) + 128;
        byte++;
        sh++;
    }
    //メインループ
    while (byte < loop_fin) {
        xSA = _mm_loadu_si128((const __m128i *)sh);
        sh += 8;
        xSA = _mm_srai_epi16(xSA, 8);
        xSA = _mm_add_epi16(xSA, xConst);
        xSB = _mm_loadu_si128((const __m128i *)sh);
        sh += 8;
        xSB = _mm_srai_epi16(xSB, 8);
        xSB = _mm_add_epi16(xSB, xConst);
        xSA = _mm_packus_epi16(xSA, xSB);
        _mm_stream_si128((__m128i *)byte, xSA);
        byte += 16;
    }
    //残り
    while (byte < fin) {
        *byte = (*sh >> 8) + 128;
        byte++;
        sh++;
    }
}

void split_audio_16to8x2(BYTE *dst, short *src, int n) {
    BYTE *byte0 = dst;
    BYTE *byte1 = dst + n;
    short *sh = src;
    short *sh_fin = src + n;
    for ( ; sh < sh_fin; sh++, byte0++, byte1++) {
        *byte0 = (*sh >> 8)   + 128;
        *byte1 = (*sh & 0xff) + 128;
    }
}

void split_audio_16to8x2_sse2(BYTE *dst, short *src, int n) {
    BYTE *byte0 = dst;
    BYTE *byte1 = dst + n;
    short *sh = src;
    short *sh_fin = src + (n & ~15);
    __m128i x0, x1, x2, x3;
    __m128i xMask = _mm_srli_epi16(_mm_cmpeq_epi8(_mm_setzero_si128(), _mm_setzero_si128()), 8);
    __m128i xConst = _mm_set1_epi8(-128);
    for ( ; sh < sh_fin; sh += 16, byte0 += 16, byte1 += 16) {
        x0 = _mm_loadu_si128((__m128i*)(sh + 0));
        x1 = _mm_loadu_si128((__m128i*)(sh + 8));
        x2 = _mm_and_si128(x0, xMask); //Lower8bit
        x3 = _mm_and_si128(x1, xMask); //Lower8bit
        x0 = _mm_srli_epi16(x0, 8);    //Upper8bit
        x1 = _mm_srli_epi16(x1, 8);    //Upper8bit
        x2 = _mm_packus_epi16(x2, x3);
        x0 = _mm_packus_epi16(x0, x1);
        x2 = _mm_add_epi8(x2, xConst);
        x0 = _mm_add_epi8(x0, xConst);
        _mm_storeu_si128((__m128i*)byte0, x0);
        _mm_storeu_si128((__m128i*)byte1, x2);
    }
    sh_fin = sh + (n & 15);
    for ( ; sh < sh_fin; sh++, byte0++, byte1++) {
        *byte0 = (*sh >> 8)   + 128;
        *byte1 = (*sh & 0xff) + 128;
    }
}

void copy_yuy2(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    memcpy(pixel_data->data[0], frame, width * height * 2);
}

void copy_yuy2_sse2(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *src = (BYTE *)frame;
    BYTE *dst = pixel_data->data[0];
    BYTE *dst_fin = pixel_data->data[0] + ((width * height * 2) & ~63);
    __m128i x0, x1, x2, x3;
    for (; dst < dst_fin; src += 64, dst += 64) {
        x0    = _mm_loadu_si128((const __m128i *)(src +  0));
        x1    = _mm_loadu_si128((const __m128i *)(src + 16));
        x2    = _mm_loadu_si128((const __m128i *)(src + 32));
        x3    = _mm_loadu_si128((const __m128i *)(src + 48));
        _mm_stream_si128((__m128i *)(dst +  0), x0);
        _mm_stream_si128((__m128i *)(dst + 16), x1);
        _mm_stream_si128((__m128i *)(dst + 32), x2);
        _mm_stream_si128((__m128i *)(dst + 48), x3);
    }
    dst_fin = dst + ((width * height * 2) & 63);
    for (; dst < dst_fin; src += 4, dst += 4)
        *(int*)dst = *(int*)src;
}

template<typename TypeOut, int out_bit_depth>
void convert_rgb_to_yuv444_base(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *ptrY = pixel_data->data[0];
    BYTE *ptrU = pixel_data->data[1];
    BYTE *ptrV = pixel_data->data[2];
    const float *coeff_table = COEFF_RGB2YUV[pixel_data->colormatrix ? 1 : 0];
    int y0 = 0, y1 = height - 1;
    const int srcstep = (width*3 + 3) & ~3;
    for (; y0 < height; y0++, y1--) {
        TypeOut *dstY = (TypeOut *)(ptrY + y1*width*sizeof(TypeOut));
        TypeOut *dstU = (TypeOut *)(ptrU + y1*width*sizeof(TypeOut));
        TypeOut *dstV = (TypeOut *)(ptrV + y1*width*sizeof(TypeOut));
        BYTE *src = (BYTE*)frame + y0*srcstep;
        for (int x = 0; x < width; x++) {
            const float b = (float)src[x*3 + 0];
            const float g = (float)src[x*3 + 1];
            const float r = (float)src[x*3 + 2];
            const float y = (coeff_table[0] * r + coeff_table[1] * g + coeff_table[2] * b +  16.0f) * (1 << (out_bit_depth - 8));
            const float u = (coeff_table[3] * r + coeff_table[4] * g + coeff_table[5] * b + 128.0f) * (1 << (out_bit_depth - 8));
            const float v = (coeff_table[6] * r + coeff_table[7] * g + coeff_table[8] * b + 128.0f) * (1 << (out_bit_depth - 8));
            dstY[x] = (TypeOut)clamp((int)(y + 0.5f), 0, (1 << out_bit_depth) - 1);
            dstU[x] = (TypeOut)clamp((int)(u + 0.5f), 0, (1 << out_bit_depth) - 1);
            dstV[x] = (TypeOut)clamp((int)(v + 0.5f), 0, (1 << out_bit_depth) - 1);
        }
    }
}

void convert_rgb_to_yuv444(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    convert_rgb_to_yuv444_base<BYTE, 8>(frame, pixel_data, width, height);
}
void convert_rgb_to_yuv444_16bit(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    convert_rgb_to_yuv444_base<USHORT, 16>(frame, pixel_data, width, height);
}

void copy_rgb(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *ptr = pixel_data->data[0];
    BYTE *dst, *src;
    int y0 = 0, y1 = height - 1;
    const int step = (width*3 + 3) & ~3;
    for (; y0 < height; y0++, y1--) {
        dst = ptr          + y1*width*3;
        src = (BYTE*)frame + y0*step;
        for (int x = 0; x < width * 3; x++)
            dst[x] = src[x];
    }
}
void copy_rgb_sse2(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *ptr = pixel_data->data[0];
    BYTE *dst, *src, *src_fin;
    __m128i x0, x1, x2, x3;
    int y0 = 0, y1 = height - 1;
    const int step = (width*3 + 3) & ~3;
    const int y_fin = height - 1;
    for (; y0 < y_fin; y0++, y1--) {
        dst = ptr          + y0*width*3;
        src = (BYTE*)frame + y1*step;
        src_fin = src + width*3;
        for (; src < src_fin; src += 64, dst += 64) {
            x0    = _mm_loadu_si128((const __m128i *)(src +  0));
            x1    = _mm_loadu_si128((const __m128i *)(src + 16));
            x2    = _mm_loadu_si128((const __m128i *)(src + 32));
            x3    = _mm_loadu_si128((const __m128i *)(src + 48));
            _mm_storeu_si128((__m128i *)(dst +  0), x0);
            _mm_storeu_si128((__m128i *)(dst + 16), x1);
            _mm_storeu_si128((__m128i *)(dst + 32), x2);
            _mm_storeu_si128((__m128i *)(dst + 48), x3);
        }
    }
    dst = ptr          + y0*width*3;
    src = (BYTE*)frame + y1*step;
    src_fin = src + ((width*3) & ~63);
    for (; src < src_fin; src += 64, dst += 64) {
        x0    = _mm_loadu_si128((const __m128i *)(src +  0));
        x1    = _mm_loadu_si128((const __m128i *)(src + 16));
        x2    = _mm_loadu_si128((const __m128i *)(src + 32));
        x3    = _mm_loadu_si128((const __m128i *)(src + 48));
        _mm_storeu_si128((__m128i *)(dst +  0), x0);
        _mm_storeu_si128((__m128i *)(dst + 16), x1);
        _mm_storeu_si128((__m128i *)(dst + 32), x2);
        _mm_storeu_si128((__m128i *)(dst + 48), x3);
    }
    src_fin = src + ((width*3) & 63);
    for (; src < src_fin; src++, dst++)
        *dst = *src;
}
void sort_to_rgb(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *ptr = pixel_data->data[0];
    BYTE *dst, *src;
    int y0 = 0, y1 = height - 1;
    const int step = (width*3 + 3) & ~3;
    for (; y0 < height; y0++, y1--) {
        dst = ptr          + y1*width*3;
        src = (BYTE*)frame + y0*step;
        for (int x = 0; x < width; x++) {
            dst[x*3 + 2] = src[x*3 + 0];
            dst[x*3 + 1] = src[x*3 + 1];
            dst[x*3 + 0] = src[x*3 + 2];
        }
    }
}


void copy_rgba(void* frame, CONVERT_CF_DATA* pixel_data, const int width, const int height) {
    BYTE* ptr = pixel_data->data[0];
    BYTE* dst, * src;
    int y0 = 0, y1 = height - 1;
    const int step = width * 4;
    for (; y0 < height; y0++, y1--) {
        dst = ptr + y1 * width * 4;
        src = (BYTE*)frame + y0 * step;
        for (int x = 0; x < width * 4; x++)
            dst[x] = src[x];
    }
}
void copy_rgba_sse2(void* frame, CONVERT_CF_DATA* pixel_data, const int width, const int height) {
    BYTE* ptr = pixel_data->data[0];
    BYTE* dst, * src, * src_fin;
    __m128i x0, x1, x2, x3;
    int y0 = 0, y1 = height - 1;
    const int step = width * 4;
    const int y_fin = height - 1;
    for (; y0 < y_fin; y0++, y1--) {
        dst = ptr + y0 * width * 4;
        src = (BYTE*)frame + y1 * step;
        src_fin = src + width * 4;
        for (; src < src_fin; src += 64, dst += 64) {
            x0 = _mm_loadu_si128((const __m128i*)(src + 0));
            x1 = _mm_loadu_si128((const __m128i*)(src + 16));
            x2 = _mm_loadu_si128((const __m128i*)(src + 32));
            x3 = _mm_loadu_si128((const __m128i*)(src + 48));
            _mm_storeu_si128((__m128i*)(dst + 0), x0);
            _mm_storeu_si128((__m128i*)(dst + 16), x1);
            _mm_storeu_si128((__m128i*)(dst + 32), x2);
            _mm_storeu_si128((__m128i*)(dst + 48), x3);
        }
    }
    dst = ptr + y0 * width * 4;
    src = (BYTE*)frame + y1 * step;
    src_fin = src + ((width * 4) & ~63);
    for (; src < src_fin; src += 64, dst += 64) {
        x0 = _mm_loadu_si128((const __m128i*)(src + 0));
        x1 = _mm_loadu_si128((const __m128i*)(src + 16));
        x2 = _mm_loadu_si128((const __m128i*)(src + 32));
        x3 = _mm_loadu_si128((const __m128i*)(src + 48));
        _mm_storeu_si128((__m128i*)(dst + 0), x0);
        _mm_storeu_si128((__m128i*)(dst + 16), x1);
        _mm_storeu_si128((__m128i*)(dst + 32), x2);
        _mm_storeu_si128((__m128i*)(dst + 48), x3);
    }
    src_fin = src + ((width * 4) & 63);
    for (; src < src_fin; src++, dst++)
        *dst = *src;
}

void convert_yuy2_to_yv12(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x, y;
    BYTE *Y  = pixel_data->data[0];
    BYTE *Cb = pixel_data->data[1];
    BYTE *Cr = pixel_data->data[2];
    BYTE *p = (BYTE *)frame;
    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x += 2) {
            Y[ y   *width  + x    ] = p[( y   *width + x)*2    ];
            Y[ y   *width  + x + 1] = p[( y   *width + x)*2 + 2];
            Y[(y+1)*width  + x    ] = p[((y+1)*width + x)*2    ];
            Y[(y+1)*width  + x + 1] = p[((y+1)*width + x)*2 + 2];
            Cb[y*width/4   + x/2  ] =(p[( y   *width + x)*2 + 1] + p[((y+1)*width + x)*2 + 1] + 1)/2;
            Cr[y*width/4   + x/2  ] =(p[( y   *width + x)*2 + 3] + p[((y+1)*width + x)*2 + 3] + 1)/2;
        }
    }
}
//適当。
void convert_yuy2_to_nv12(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x, y;
    BYTE *Y = pixel_data->data[0];
    BYTE *C = pixel_data->data[1];
    BYTE *p = (BYTE *)frame;
    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x += 2) {
            Y[ y   *width + x    ] = p[( y   *width + x)*2    ];
            Y[ y   *width + x + 1] = p[( y   *width + x)*2 + 2];
            Y[(y+1)*width + x    ] = p[((y+1)*width + x)*2    ];
            Y[(y+1)*width + x + 1] = p[((y+1)*width + x)*2 + 2];
            C[y*width/2   + x    ] =(p[( y   *width + x)*2 + 1] + p[((y+1)*width + x)*2 + 1] + 1)/2;
            C[y*width/2   + x + 1] =(p[( y   *width + x)*2 + 3] + p[((y+1)*width + x)*2 + 3] + 1)/2;
        }
    }
}

//これも適当。
void convert_yuy2_to_nv12_i(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x, y;
    BYTE *Y = pixel_data->data[0];
    BYTE *C = pixel_data->data[1];
    BYTE *p = (BYTE *)frame;
    for (y = 0; y < height; y += 4) {
        for (x = 0; x < width; x += 2) {
            Y[ y   *width + x    ] = p[( y   *width + x)*2    ];
            Y[ y   *width + x + 1] = p[( y   *width + x)*2 + 2];
            Y[(y+1)*width + x    ] = p[((y+1)*width + x)*2    ];
            Y[(y+1)*width + x + 1] = p[((y+1)*width + x)*2 + 2];
            Y[(y+2)*width + x    ] = p[((y+2)*width + x)*2    ];
            Y[(y+2)*width + x + 1] = p[((y+2)*width + x)*2 + 2];
            Y[(y+3)*width + x    ] = p[((y+3)*width + x)*2    ];
            Y[(y+3)*width + x + 1] = p[((y+3)*width + x)*2 + 2];
            C[y/2*width   + x    ] =(p[( y   *width + x)*2 + 1] * 3 + p[((y+2)*width + x)*2 + 1] * 1 + 2)>>2;
            C[y/2*width   + x + 1] =(p[( y   *width + x)*2 + 3] * 3 + p[((y+2)*width + x)*2 + 3] * 1 + 2)>>2;
            C[(y/2+1)*width + x  ] =(p[((y+1)*width + x)*2 + 1] * 1 + p[((y+3)*width + x)*2 + 1] * 3 + 2)>>2;
            C[(y/2+1)*width + x+1] =(p[((y+1)*width + x)*2 + 3] * 1 + p[((y+3)*width + x)*2 + 3] * 3 + 2)>>2;
        }
    }
}

//これも適当。
void convert_yuy2_to_yv12_i(void *frame, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x, y;
    BYTE *Y  = pixel_data->data[0];
    BYTE *Cb = pixel_data->data[1];
    BYTE *Cr = pixel_data->data[2];
    BYTE *p = (BYTE *)frame;
    for (y = 0; y < height; y += 4) {
        for (x = 0; x < width; x += 2) {
            Y[ y   *width + x    ] = p[( y   *width + x)*2    ];
            Y[ y   *width + x + 1] = p[( y   *width + x)*2 + 2];
            Y[(y+1)*width + x    ] = p[((y+1)*width + x)*2    ];
            Y[(y+1)*width + x + 1] = p[((y+1)*width + x)*2 + 2];
            Y[(y+2)*width + x    ] = p[((y+2)*width + x)*2    ];
            Y[(y+2)*width + x + 1] = p[((y+2)*width + x)*2 + 2];
            Y[(y+3)*width + x    ] = p[((y+3)*width + x)*2    ];
            Y[(y+3)*width + x + 1] = p[((y+3)*width + x)*2 + 2];
            Cb[y/2*(width/2)   + x/2   ] =(p[( y   *width + x)*2 + 1] * 3 + p[((y+2)*width + x)*2 + 1] * 1 + 2)>>2;
            Cr[y/2*(width/2)   + x/2   ] =(p[( y   *width + x)*2 + 3] * 3 + p[((y+2)*width + x)*2 + 3] * 1 + 2)>>2;
            Cb[(y/2+1)*(width/2) + x/2 ] =(p[((y+1)*width + x)*2 + 1] * 1 + p[((y+3)*width + x)*2 + 1] * 3 + 2)>>2;
            Cr[(y/2+1)*(width/2) + x/2 ] =(p[((y+1)*width + x)*2 + 3] * 1 + p[((y+3)*width + x)*2 + 3] * 3 + 2)>>2;
        }
    }
}
static int inline pixel_YC48_to_YUV(int y, int mul, int add, int rshift, int ycc, int min, int max) {
    return clamp(((y * mul + add) >> rshift) + ycc, min, max);
}

void convert_yc48_to_nv12_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_C = (short *)pixel_data->data[1];
    short *Y = NULL, *C = (short *)dst_C;
    for (y = 0; y < height; y += 2) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        for (x = 0; x < width; x += 2) {
            Y[x        ] = (short)pixel_YC48_to_YUV(ycp[x        ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1      ] = (short)pixel_YC48_to_YUV(ycp[x+1      ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width] = (short)pixel_YC48_to_YUV(ycp[x  +width].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width] = (short)pixel_YC48_to_YUV(ycp[x+1+width].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            *C = (short)pixel_YC48_to_YUV(((int)ycp[x].cb + (int)ycp[x+width].cb) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420P, Y_L_YCC_16, 0, LIMIT_16);
            C++;
            *C = (short)pixel_YC48_to_YUV(((int)ycp[x].cr + (int)ycp[x+width].cr) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420P, Y_L_YCC_16, 0, LIMIT_16);
            C++;
        }
    }
}
void convert_yc48_to_nv12_i_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp = NULL;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_C = (short *)pixel_data->data[1];
    short *Y = NULL, *C = NULL;
    for (y = 0; y < height; y += 4) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        C = (short *)dst_C + (i>>1);
        for (x = 0; x < width; x += 2) {
            Y[x          ] = (short)pixel_YC48_to_YUV(ycp[x          ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1        ] = (short)pixel_YC48_to_YUV(ycp[x+1        ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width  ] = (short)pixel_YC48_to_YUV(ycp[x  +width  ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width  ] = (short)pixel_YC48_to_YUV(ycp[x+1+width  ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width*2] = (short)pixel_YC48_to_YUV(ycp[x  +width*2].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width*2] = (short)pixel_YC48_to_YUV(ycp[x+1+width*2].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width*3] = (short)pixel_YC48_to_YUV(ycp[x  +width*3].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width*3] = (short)pixel_YC48_to_YUV(ycp[x+1+width*3].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            C[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cb * 3 + (int)ycp[x+width*2].cb * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            C[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cb * 1 + (int)ycp[x+width*3].cb * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            C++;
            C[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cr * 3 + (int)ycp[x+width*2].cr * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            C[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cr * 1 + (int)ycp[x+width*3].cr * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            C++;
        }
    }
}

void convert_yc48_to_yv12_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_U = (short *)pixel_data->data[1];
    short *dst_V = (short *)pixel_data->data[2];
    short *Y = NULL, *U = NULL, *V = NULL;
    for (y = 0; y < height; y += 2) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        U = (short *)dst_U + (i>>2);
        V = (short *)dst_V + (i>>2);
        for (x = 0; x < width; x += 2) {
            Y[x        ] = (short)pixel_YC48_to_YUV(ycp[x        ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1      ] = (short)pixel_YC48_to_YUV(ycp[x+1      ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width] = (short)pixel_YC48_to_YUV(ycp[x  +width].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width] = (short)pixel_YC48_to_YUV(ycp[x+1+width].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            *U = (short)pixel_YC48_to_YUV(((int)ycp[x].cb + (int)ycp[x+width].cb) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420P, Y_L_YCC_16, 0, LIMIT_16);
            U++;
            *V = (short)pixel_YC48_to_YUV(((int)ycp[x].cr + (int)ycp[x+width].cr) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420P, Y_L_YCC_16, 0, LIMIT_16);
            V++;
        }
    }
}
void convert_yc48_to_yv12_i_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp = NULL;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_U = (short *)pixel_data->data[1];
    short *dst_V = (short *)pixel_data->data[2];
    short *Y = NULL, *U = NULL, *V = NULL;
    for (y = 0; y < height; y += 4) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        U = (short *)dst_U + (i>>2);
        V = (short *)dst_V + (i>>2);
        for (x = 0; x < width; x += 2) {
            Y[x          ] = (short)pixel_YC48_to_YUV(ycp[x          ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1        ] = (short)pixel_YC48_to_YUV(ycp[x+1        ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width  ] = (short)pixel_YC48_to_YUV(ycp[x  +width  ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width  ] = (short)pixel_YC48_to_YUV(ycp[x+1+width  ].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width*2] = (short)pixel_YC48_to_YUV(ycp[x  +width*2].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width*2] = (short)pixel_YC48_to_YUV(ycp[x+1+width*2].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x  +width*3] = (short)pixel_YC48_to_YUV(ycp[x  +width*3].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            Y[x+1+width*3] = (short)pixel_YC48_to_YUV(ycp[x+1+width*3].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
            U[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cb * 3 + (int)ycp[x+width*2].cb * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            U[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cb * 1 + (int)ycp[x+width*3].cb * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            U++;
            V[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cr * 3 + (int)ycp[x+width*2].cr * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            V[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cr * 1 + (int)ycp[x+width*3].cr * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_16, UV_L_RSH_16_420I, UV_L_YCC_16, 0, LIMIT_16);
            V++;
        }
    }
}

void convert_yc48_to_yv12_10bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_U = (short *)pixel_data->data[1];
    short *dst_V = (short *)pixel_data->data[2];
    short *Y = NULL, *U = NULL, *V = NULL;
    for (y = 0; y < height; y += 2) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        U = (short *)dst_U + (i>>2);
        V = (short *)dst_V + (i>>2);
        for (x = 0; x < width; x += 2) {
            Y[x        ] = (short)pixel_YC48_to_YUV(ycp[x        ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1      ] = (short)pixel_YC48_to_YUV(ycp[x+1      ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x  +width] = (short)pixel_YC48_to_YUV(ycp[x  +width].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1+width] = (short)pixel_YC48_to_YUV(ycp[x+1+width].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            *U = (short)pixel_YC48_to_YUV(((int)ycp[x].cb + (int)ycp[x+width].cb) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420P, Y_L_YCC_10, 0, LIMIT_10);
            U++;
            *V = (short)pixel_YC48_to_YUV(((int)ycp[x].cr + (int)ycp[x+width].cr) + UV_OFFSET_x2, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420P, Y_L_YCC_10, 0, LIMIT_10);
            V++;
        }
    }
}
void convert_yc48_to_yv12_i_10bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    int x = 0, y = 0, i = 0;
    PIXEL_YC *ycp = NULL;
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_U = (short *)pixel_data->data[1];
    short *dst_V = (short *)pixel_data->data[2];
    short *Y = NULL, *U = NULL, *V = NULL;
    for (y = 0; y < height; y += 4) {
        i = width * y;
        ycp = (PIXEL_YC *)pixel + i;
        Y = (short *)dst_Y + i;
        U = (short *)dst_U + (i>>2);
        V = (short *)dst_V + (i>>2);
        for (x = 0; x < width; x += 2) {
            Y[x          ] = (short)pixel_YC48_to_YUV(ycp[x          ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1        ] = (short)pixel_YC48_to_YUV(ycp[x+1        ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x  +width  ] = (short)pixel_YC48_to_YUV(ycp[x  +width  ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1+width  ] = (short)pixel_YC48_to_YUV(ycp[x+1+width  ].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x  +width*2] = (short)pixel_YC48_to_YUV(ycp[x  +width*2].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1+width*2] = (short)pixel_YC48_to_YUV(ycp[x+1+width*2].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x  +width*3] = (short)pixel_YC48_to_YUV(ycp[x  +width*3].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            Y[x+1+width*3] = (short)pixel_YC48_to_YUV(ycp[x+1+width*3].y, Y_L_MUL, Y_L_ADD_10, Y_L_RSH_10, Y_L_YCC_10, 0, LIMIT_10);
            U[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cb * 3 + (int)ycp[x+width*2].cb * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420I, UV_L_YCC_10, 0, LIMIT_10);
            U[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cb * 1 + (int)ycp[x+width*3].cb * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420I, UV_L_YCC_10, 0, LIMIT_10);
            U++;
            V[0          ] = (short)pixel_YC48_to_YUV(((int)ycp[x      ].cr * 3 + (int)ycp[x+width*2].cr * 1) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420I, UV_L_YCC_10, 0, LIMIT_10);
            V[0  +width  ] = (short)pixel_YC48_to_YUV(((int)ycp[x+width].cr * 1 + (int)ycp[x+width*3].cr * 3) + UV_OFFSET_x4, UV_L_MUL, Y_L_ADD_10, UV_L_RSH_10_420I, UV_L_YCC_10, 0, LIMIT_10);
            V++;
        }
    }
}

void convert_yc48_to_yuv444(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    PIXEL_YC *ycp_fin = (PIXEL_YC *)pixel + width * height;
    BYTE *Y = pixel_data->data[0];
    BYTE *U = pixel_data->data[1];
    BYTE *V = pixel_data->data[2];
    for (PIXEL_YC *ycp = (PIXEL_YC *)pixel; ycp < ycp_fin; ycp++, Y++, U++, V++) {
        *Y = (BYTE)pixel_YC48_to_YUV(ycp->y,                  Y_L_MUL,  Y_L_ADD_8,      Y_L_RSH_8,      Y_L_YCC_8, 0, LIMIT_8);
        *U = (BYTE)pixel_YC48_to_YUV(ycp->cb + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_8_444, UV_L_RSH_8_444, UV_L_YCC_8, 0, LIMIT_8);
        *V = (BYTE)pixel_YC48_to_YUV(ycp->cr + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_8_444, UV_L_RSH_8_444, UV_L_YCC_8, 0, LIMIT_8);
    }
}

void convert_yc48_to_yuv444_10bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    PIXEL_YC *ycp_fin = (PIXEL_YC *)pixel + width * height;
    short *Y = (short *)pixel_data->data[0];
    short *U = (short *)pixel_data->data[1];
    short *V = (short *)pixel_data->data[2];
    for (PIXEL_YC *ycp = (PIXEL_YC *)pixel; ycp < ycp_fin; ycp++, Y++, U++, V++) {
        *Y = (short)pixel_YC48_to_YUV(ycp->y,                  Y_L_MUL,  Y_L_ADD_10,      Y_L_RSH_10,      Y_L_YCC_10, 0, LIMIT_10);
        *U = (short)pixel_YC48_to_YUV(ycp->cb + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_10_444, UV_L_RSH_10_444, UV_L_YCC_10, 0, LIMIT_10);
        *V = (short)pixel_YC48_to_YUV(ycp->cr + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_10_444, UV_L_RSH_10_444, UV_L_YCC_10, 0, LIMIT_10);
    }
}

void convert_yc48_to_yuv444_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    PIXEL_YC *ycp_fin = (PIXEL_YC *)pixel + width * height;
    short *Y = (short *)pixel_data->data[0];
    short *U = (short *)pixel_data->data[1];
    short *V = (short *)pixel_data->data[2];
    for (PIXEL_YC *ycp = (PIXEL_YC *)pixel; ycp < ycp_fin; ycp++, Y++, U++, V++) {
        *Y = (short)pixel_YC48_to_YUV(ycp->y,                  Y_L_MUL,  Y_L_ADD_16,      Y_L_RSH_16,      Y_L_YCC_16, 0, LIMIT_16);
        *U = (short)pixel_YC48_to_YUV(ycp->cb + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
        *V = (short)pixel_YC48_to_YUV(ycp->cr + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
    }
}

void convert_yuy2_to_nv16(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = pixel_data->data[0];
    BYTE *dst_C = pixel_data->data[1];
    BYTE *p = (BYTE *)pixel;
    const int n = width * height;
    for (int i = 0; i < n; i += 2) {
        dst_Y[i]   = p[i*2 + 0];
        dst_C[i]   = p[i*2 + 1];
        dst_Y[i+1] = p[i*2 + 2];
        dst_C[i+1] = p[i*2 + 3];
    }
}

void convert_yuy2_to_yuv422(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = pixel_data->data[0];
    BYTE *dst_U = pixel_data->data[1];
    BYTE *dst_V = pixel_data->data[2];
    BYTE *p = (BYTE *)pixel;
    const int n = width * height;
    for (int i = 0; i < n; i += 2) {
        dst_Y[i +0] = p[i*2 + 0];
        dst_U[i>>1] = p[i*2 + 1];
        dst_Y[i +1] = p[i*2 + 2];
        dst_V[i>>1] = p[i*2 + 3];
    }
}

void convert_yc48_to_yuv422_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_U = (short *)pixel_data->data[1];
    short *dst_V = (short *)pixel_data->data[2];
    PIXEL_YC *ycp = (PIXEL_YC *)pixel;
    const int n = width * height;
    for (int i = 0; i < n; i += 2) {
        dst_Y[i + 0] = (short)pixel_YC48_to_YUV(ycp[i + 0].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
        dst_U[i >> 1] = (short)pixel_YC48_to_YUV(ycp[i + 0].cb + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
        dst_V[i >> 1] = (short)pixel_YC48_to_YUV(ycp[i + 0].cr + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
        dst_Y[i + 1] = (short)pixel_YC48_to_YUV(ycp[i + 1].y, Y_L_MUL, Y_L_ADD_16, Y_L_RSH_16, Y_L_YCC_16, 0, LIMIT_16);
    }
}

void convert_yc48_to_nv16_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    short *dst_Y = (short *)pixel_data->data[0];
    short *dst_C = (short *)pixel_data->data[1];
    PIXEL_YC *ycp = (PIXEL_YC *)pixel;
    const int n = width * height;
    for (int i = 0; i < n; i += 2) {
        dst_Y[i+0] = (short)pixel_YC48_to_YUV(ycp[i+0].y,                  Y_L_MUL,  Y_L_ADD_16,     Y_L_RSH_16,       Y_L_YCC_16, 0, LIMIT_16);
        dst_C[i+0] = (short)pixel_YC48_to_YUV(ycp[i+0].cb + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
        dst_C[i+1] = (short)pixel_YC48_to_YUV(ycp[i+0].cr + UV_OFFSET_x1, UV_L_MUL, UV_L_ADD_16_444, UV_L_RSH_16_444, UV_L_YCC_16, 0, LIMIT_16);
        dst_Y[i+1] = (short)pixel_YC48_to_YUV(ycp[i+1].y,                  Y_L_MUL,  Y_L_ADD_16,     Y_L_RSH_16,       Y_L_YCC_16, 0, LIMIT_16);
    }
}

void convert_lw48_to_nv12(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = (BYTE *)pixel_data->data[0];
    BYTE *dst_C = (BYTE *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            dst_Y[(y+0) * width + x + 0] = (BYTE)(ycp[(y+0) * width + x + 0].y >> 8);
            dst_Y[(y+0) * width + x + 1] = (BYTE)(ycp[(y+0) * width + x + 1].y >> 8);
            dst_Y[(y+1) * width + x + 0] = (BYTE)(ycp[(y+1) * width + x + 0].y >> 8);
            dst_Y[(y+1) * width + x + 1] = (BYTE)(ycp[(y+1) * width + x + 1].y >> 8);
            dst_C[y * width / 2 + x + 0] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cb + (DWORD)ycp[(y+1) * width + x + 0].cb + (1<<8)) >> 9);
            dst_C[y * width / 2 + x + 1] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cr + (DWORD)ycp[(y+1) * width + x + 0].cr + (1<<8)) >> 9);
        }
    }
}
void convert_lw48_to_nv12_i(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = (BYTE *)pixel_data->data[0];
    BYTE *dst_C = (BYTE *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 2) {
            dst_Y[(y+0) * width + x + 0] = (BYTE)(ycp[(y+0) * width + x + 0].y >> 8);
            dst_Y[(y+0) * width + x + 1] = (BYTE)(ycp[(y+0) * width + x + 1].y >> 8);
            dst_Y[(y+1) * width + x + 0] = (BYTE)(ycp[(y+1) * width + x + 0].y >> 8);
            dst_Y[(y+1) * width + x + 1] = (BYTE)(ycp[(y+1) * width + x + 1].y >> 8);
            dst_Y[(y+2) * width + x + 0] = (BYTE)(ycp[(y+2) * width + x + 0].y >> 8);
            dst_Y[(y+2) * width + x + 1] = (BYTE)(ycp[(y+2) * width + x + 1].y >> 8);
            dst_Y[(y+3) * width + x + 0] = (BYTE)(ycp[(y+3) * width + x + 0].y >> 8);
            dst_Y[(y+3) * width + x + 1] = (BYTE)(ycp[(y+3) * width + x + 1].y >> 8);
            dst_C[(y/2+0)*width + x + 0] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cb * 3 + (DWORD)ycp[(y+1) * width + x + 0].cb * 1 + (1<<9)) >> 10);
            dst_C[(y/2+1)*width + x + 1] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cr * 3 + (DWORD)ycp[(y+1) * width + x + 0].cr * 1 + (1<<9)) >> 10);
            dst_C[(y/2+0)*width + x + 0] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cb * 1 + (DWORD)ycp[(y+1) * width + x + 0].cb * 3 + (1<<9)) >> 10);
            dst_C[(y/2+1)*width + x + 1] = (BYTE)(((DWORD)ycp[(y+0) * width + x + 0].cr * 1 + (DWORD)ycp[(y+1) * width + x + 0].cr * 3 + (1<<9)) >> 10);
        }
    }
}
void convert_lw48_to_nv12_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    USHORT *dst_Y = (USHORT *)pixel_data->data[0];
    USHORT *dst_C = (USHORT *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            dst_Y[(y+0) * width + x + 0] = (USHORT)ycp[(y+0) * width + x + 0].y;
            dst_Y[(y+0) * width + x + 1] = (USHORT)ycp[(y+0) * width + x + 1].y;
            dst_Y[(y+1) * width + x + 0] = (USHORT)ycp[(y+1) * width + x + 0].y;
            dst_Y[(y+1) * width + x + 1] = (USHORT)ycp[(y+1) * width + x + 1].y;
            dst_C[y * width / 2 + x + 0] = (USHORT)(((DWORD)ycp[(y+0) * width + x + 0].cb + (DWORD)ycp[(y+1) * width + x + 0].cb) >> 1);
            dst_C[y * width / 2 + x + 1] = (USHORT)(((DWORD)ycp[(y+0) * width + x + 0].cr + (DWORD)ycp[(y+1) * width + x + 0].cr) >> 1);
        }
    }
}
void convert_lw48_to_nv12_i_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    USHORT *dst_Y = (USHORT *)pixel_data->data[0];
    USHORT *dst_C = (USHORT *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 2) {
            dst_Y[(y+0) * width + x + 0] = (USHORT)ycp[(y+0) * width + x + 0].y;
            dst_Y[(y+0) * width + x + 1] = (USHORT)ycp[(y+0) * width + x + 1].y;
            dst_Y[(y+1) * width + x + 0] = (USHORT)ycp[(y+1) * width + x + 0].y;
            dst_Y[(y+1) * width + x + 1] = (USHORT)ycp[(y+1) * width + x + 1].y;
            dst_Y[(y+2) * width + x + 0] = (USHORT)ycp[(y+2) * width + x + 0].y;
            dst_Y[(y+2) * width + x + 1] = (USHORT)ycp[(y+2) * width + x + 1].y;
            dst_Y[(y+3) * width + x + 0] = (USHORT)ycp[(y+3) * width + x + 0].y;
            dst_Y[(y+3) * width + x + 1] = (USHORT)ycp[(y+3) * width + x + 1].y;
            dst_C[(y/2+0)*width + x + 0] = (USHORT)(((DWORD)ycp[(y+0) * width + x + 0].cb * 3 + (DWORD)ycp[(y+1) * width + x + 0].cb * 1 + 2) >> 2);
            dst_C[(y/2+0)*width + x + 1] = (USHORT)(((DWORD)ycp[(y+0) * width + x + 0].cr * 3 + (DWORD)ycp[(y+1) * width + x + 0].cr * 1 + 2) >> 2);
            dst_C[(y/2+1)*width + x + 0] = (USHORT)(((DWORD)ycp[(y+1) * width + x + 0].cb * 1 + (DWORD)ycp[(y+3) * width + x + 0].cb * 3 + 2) >> 2);
            dst_C[(y/2+1)*width + x + 1] = (USHORT)(((DWORD)ycp[(y+1) * width + x + 0].cr * 1 + (DWORD)ycp[(y+3) * width + x + 0].cr * 3 + 2) >> 2);
        }
    }
}
void convert_lw48_to_nv16(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = (BYTE *)pixel_data->data[0];
    BYTE *dst_C = (BYTE *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    const int pixel_n = width * height;
    for (int i = 0; i < pixel_n; i += 2) {
        dst_Y[i + 0] = (BYTE)(ycp[i + 0].y >> 8);
        dst_Y[i + 1] = (BYTE)(ycp[i + 1].y >> 8);
        dst_C[i + 0] = (BYTE)(ycp[i+0].cb >> 8);
        dst_C[i + 1] = (BYTE)(ycp[i+0].cr >> 8);
    }
}
void convert_lw48_to_yuv444(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    BYTE *dst_Y = (BYTE *)pixel_data->data[0];
    BYTE *dst_U = (BYTE *)pixel_data->data[1];
    BYTE *dst_V = (BYTE *)pixel_data->data[2];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    const int pixel_n = width * height;
    for (int i = 0; i < pixel_n; i++) {
        dst_Y[i + 0] = (BYTE)(ycp[i+0].y >> 8);
        dst_U[i + 0] = (BYTE)(ycp[i+0].cb >> 8);
        dst_V[i + 1] = (BYTE)(ycp[i+0].cr >> 8);
    }
}
void convert_lw48_to_nv16_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    USHORT *dst_Y = (USHORT *)pixel_data->data[0];
    USHORT *dst_C = (USHORT *)pixel_data->data[1];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    const int pixel_n = width * height;
    for (int i = 0; i < pixel_n; i += 2) {
        dst_Y[i + 0] = ycp[i+0].y;
        dst_Y[i + 1] = ycp[i+1].y;
        dst_C[i + 0] = ycp[i+0].cb;
        dst_C[i + 1] = ycp[i+0].cr;
    }
}
void convert_lw48_to_yuv444_16bit(void *pixel, CONVERT_CF_DATA *pixel_data, const int width, const int height) {
    USHORT *dst_Y = (USHORT *)pixel_data->data[0];
    USHORT *dst_U = (USHORT *)pixel_data->data[1];
    USHORT *dst_V = (USHORT *)pixel_data->data[2];
    PIXEL_LW48 *ycp = (PIXEL_LW48 *)pixel;
    const int pixel_n = width * height;
    for (int i = 0; i < pixel_n; i++) {
        dst_Y[i] = ycp[i].y;
        dst_U[i] = ycp[i].cb;
        dst_V[i] = ycp[i].cr;
    }
}
