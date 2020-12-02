#ifndef BMP_H
#define BMP_H

#include "stdafx.h"

class BMP
{
public:
    BMP(const std::string &fn = "");
    BMP(const BMP &b);

    ~BMP();

    void setBitMapFileHeader(const BITMAPFILEHEADER &bf);
    void setBitMapInfoHeader(const BITMAPINFOHEADER &bh);
    void setFileName(const std::string &f);
    void setBitmap(const std::vector<std::vector<float> > &v_bmp);

    int width() const;
    int height() const;
    unsigned int imageSize() const;     //размер картинки в байтах
    std::vector<std::vector<float> > getBitmap() const;

    BMP & operator=(const BMP &b);

private:
    BITMAPFILEHEADER bif;
    BITMAPINFOHEADER bih;
    std::string filename;
    std::vector<std::vector<float> > bitmap;
};

#endif // BMP_H
