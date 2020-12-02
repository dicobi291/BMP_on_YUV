#ifndef YUV_H
#define YUV_H

#include "stdafx.h"

class YUV
{
public:
    YUV();
    YUV(const std::string &fn, int w, int h);
    YUV(const YUV &y_cpy);

    ~YUV();

    void setWidth(int w);
    void setHeight(int h);
    void setSize(int s);
    void setFileName(const std::string &fn);
    void setY(const std::vector<BYTE> &vec_y);
    void setU(const std::vector<BYTE> &vec_u);
    void setV(const std::vector<BYTE> &vec_v);

    int getWidth() const;
    int getHeight() const;
    int getSize() const;
    std::string getFileName() const;
    std::vector<BYTE> getY() const;
    std::vector<BYTE> getU() const;
    std::vector<BYTE> getV() const;

private:
    std::string filename;
    std::vector<BYTE> y;
    std::vector<BYTE> u;
    std::vector<BYTE> v;

    int width;
    int height;
    int size;
};

#endif // YUV_H
