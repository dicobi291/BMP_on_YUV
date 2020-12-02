#include "yuv.h"

YUV::YUV()
{
    filename = "";
    y.resize(0);
    u.resize(0);
    v.resize(0);
    width = 0;
    height = 0;
    size = 0;
}

YUV::YUV(const std::string &fn, int w, int h)
{
    filename = fn;
    width = w;
    height = h;
    size = (width*height*3)/2;
}

YUV::YUV(const YUV &y_cpy)
{
    filename = y_cpy.filename;
    y.clear();
    y.resize(y_cpy.y.size());
    y = y_cpy.y;
    u.clear();
    u.resize(y_cpy.u.size());
    u = y_cpy.u;
    v.clear();
    v.resize(y_cpy.v.size());
    v = y_cpy.v;
    width = y_cpy.width;
    height = y_cpy.height;
    size = y_cpy.size;
}

YUV::~YUV()
{
    filename.clear();
    y.clear();
    u.clear();
    v.clear();
}

void YUV::setWidth(int w)
{
    width = w;
}

void YUV::setHeight(int h)
{
    height = h;
}

void YUV::setSize(int s)
{
    size = s;
}

void YUV::setFileName(const std::string &fn)
{
    filename = fn;
}

void YUV::setY(const std::vector<BYTE> &vec_y)
{
    y.resize(vec_y.size());
    y = vec_y;
}

void YUV::setU(const std::vector<BYTE> &vec_u)
{
    u.resize(vec_u.size());
    u = vec_u;
}

void YUV::setV(const std::vector<BYTE> &vec_v)
{
    v.resize(vec_v.size());
    v = vec_v;
}

int YUV::getWidth() const
{
    return width;
}

int YUV::getHeight() const
{
    return height;
}

int YUV::getSize() const
{
    return size;
}

std::string YUV::getFileName() const
{
    return filename;
}

std::vector<BYTE> YUV::getY() const
{
    return y;
}

std::vector<BYTE> YUV::getU() const
{
    return u;
}

std::vector<BYTE> YUV::getV() const
{
    return v;
}
