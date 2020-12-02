#include "bmp.h"

BMP::BMP(const std::string &fn)
{
    filename = fn;
}

BMP::BMP(const BMP &b)
{
    filename = b.filename;
    bitmap = b.bitmap;
    bif = b.bif;
    bih = b.bih;
}

BMP::~BMP()
{
    filename.clear();
}

BMP & BMP::operator=(const BMP &b)
{
    if(&b == this)
        return *this;
    filename = b.filename;
    bitmap = b.bitmap;
    bif = b.bif;
    bih = b.bih;

    return *this;
}

void BMP::setBitMapFileHeader(const BITMAPFILEHEADER &bf)
{
    bif = bf;
}

void BMP::setBitMapInfoHeader(const BITMAPINFOHEADER &bh)
{
    bih = bh;
}

void BMP::setFileName(const std::string &f)
{
    filename = f;
}

void BMP::setBitmap(const std::vector<std::vector<float> > &v_bmp)
{
    bitmap.resize(v_bmp.size());
    for(unsigned int i = 0; i < v_bmp.size(); i++){
        bitmap[i].resize(v_bmp[i].size());
    }
    bitmap = v_bmp;
}

int BMP::width() const
{
    return bih.biWidth;
}

int BMP::height() const
{
    return bih.biHeight;
}

unsigned int BMP::imageSize() const
{
    return bih.biSizeImage;
}

std::vector<std::vector<float> > BMP::getBitmap() const
{
    return bitmap;
}

