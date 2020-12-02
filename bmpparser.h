#ifndef BMPPARSER_H
#define BMPPARSER_H

#include "stdafx.h"
#include "bmp.h"

class BmpParser
{
public:
    BmpParser();
    BmpParser(const std::string &fn);

    ~BmpParser();

    void parse();
    BMP getBMP() const;

private:
    BMP bmp;
    std::string filename;
    std::fstream fin;
    BITMAPFILEHEADER bif;
    BITMAPINFOHEADER bih;
    std::vector<std::vector<float> > bitmap;
};

#endif // BMPPARSER_H
