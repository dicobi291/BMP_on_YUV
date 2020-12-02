#include "bmpparser.h"

BmpParser::BmpParser()
{
    filename = "";
}

BmpParser::BmpParser(const std::string &fn)
{
    filename = fn;
    fin.open(filename, std::ios_base::in | std::ios_base::binary);
    if(!fin.is_open()){
        std::cout << filename << " is not open\n";
    }
}

BmpParser::~BmpParser()
{
    filename.clear();
    fin.close();
}

void BmpParser::parse()
{
    if(!fin.is_open()){
        std::cout << "file is not open\n";
        return;
    }

    std::cout << "Parsing bmp file...\n";

    fin.read((char *) &bif, sizeof(BITMAPFILEHEADER));
    fin.read((char *) &bih, sizeof(BITMAPINFOHEADER));

    BYTE temp;
    std::vector<float> vec_temp;

    for(int i = 0; i < bih.biHeight; i++){
        for(int j = 0; j < bih.biWidth * 3; j++){
            fin.read((char *) &temp, sizeof(BYTE));
            vec_temp.push_back(temp);
        }
        if(vec_temp.size() < (bih.biWidth*3 + bih.biWidth%4)){
            for(int k = vec_temp.size(); k < (bih.biWidth*3 + bih.biWidth%4); k++){
                fin.read((char *) &temp, sizeof(BYTE));
                vec_temp.push_back(temp);
            }
        }
        bitmap.push_back(vec_temp);
        vec_temp.clear();
    }

    bmp.setBitMapFileHeader(bif);
    bmp.setBitMapInfoHeader(bih);
    bmp.setFileName(filename);
    bmp.setBitmap(bitmap);
}

BMP BmpParser::getBMP() const
{
    return bmp;
}
