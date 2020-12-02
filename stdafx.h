#ifndef STDAFX_H
#define STDAFX_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <xmmintrin.h>
#include <thread>

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    DWORD;
typedef int             LONG;

/*
 * #pragma pack(push, 1) - директива препроцессора
 * устанавливающая выравнивание положение полей структуры
 * по 1 байту.
 *
 * Заголовочные структуры файла bmp.
 * */
#pragma pack(push, 1)
struct BITMAPFILEHEADER{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffbits;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BITMAPINFOHEADER
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
};
#pragma pack(pop)

/*
 * Структура Offsetспользуется во время наложения картинки поверх
 * кадров видео. Поля структуры содержат расстояния от краев кадра
 * до накладываемой картинки. Структура передается в потоки, в которых
 * каждый компонент отдельно помещается в массив.
 * */
struct Offset{
    int y_top_offset_height;
    int y_bottom_offset_height;
    int y_left_offset_width;
    int y_right_offset_width;

    int uv_top_offset_height;
    int uv_bottom_offset_height;
    int uv_left_offset_width;
    int uv_right_offset_width;
};

/*
 * Объекдинение simd_element используется для удобства доступа к элементам
 * регистра __m128.
 * */
union simd_element{
    __m128 var;
    float  n_var[4];
};

/*
 * Константы для преобразование rgb в yuv.
 * */
const float Kr = 0.299f;
const float Kg = 0.587f;
const float Kb = 0.114f;
const float Ku = 0.565f;
const float Kv = 0.713f;
const float alig = 128.0f;

#endif // STDAFX_H
