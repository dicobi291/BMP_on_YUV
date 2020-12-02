/*
 * Программа состоит из трех частей:
 *  - Чтение файла bmp;
 *  - Преобразование rgb в yuv420;
 *  - Наложение получившейся картинки поверх кадров видеоряда;
 *
 * Чтение выполняется классом BmpParser, методом parse(). Для этого в конструктор класса
 * передается путь к картинке в формате bmp. Затем из файла считываются заголовочные структуры
 * BITMAPFILEHEADER и BITMAPINFOHEADER, после чего все содержимое файла помещается в массив bitmap.
 * На основе этой информации создается объект BMP, который можно получить из парсера методом
 * grtBMP(). Так инициализируется объект в main.
 *
 * После чего выполняется преобразование из rgb в yuv в функции RGBtoYUV, которой в качестве аргумента передается
 * объект BMP.
 *
 * Затем получившася картинка в формате yuv накладывается поверх кадров видео в формате yuv в функции
 * insertYUV, на вход которой подается массив объектов YUV каждый из которых представляет собой кадр видео
 * и объект YUV содержащий преобразованну картинку.
 *
 * Результат сохраняется в папке под названием new_video.yuv.
 *
 * */
#include "stdafx.h"     //содержит все необходимые объявления структур, констант и заголовочных файлов
#include "bmpparser.h"
#include "bmp.h"
#include "yuv.h"

using namespace std;

/*
 * readYUV и writeYUV вспомогательные функции для чтения и записи yuv файлов, если нужно считать
 * видеоряд, то необходимо передать в функцию количество кадров.
 * */
vector<YUV> readYUV(const string &filename, int width, int height, int frame_num);
void writeYUV(const vector<YUV> &yuv, const string &fn);

YUV RGBtoYUV(const BMP &bmp);
void insertYUV(vector<YUV> &video, const YUV &yuv);

/*
 * Функции потоков, используются для параллельной записи в несколько массивов.
 * */
void thr_write_component(vector<float> src, vector<vector<float> > &comp, int start, int n);
void thr_write_y(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h);
void thr_write_uv(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h);
//void thr_write_v(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h);

int main()
{
    BmpParser parser("image/p.bmp");
    parser.parse();

    BMP bmp_image;
    bmp_image = parser.getBMP();

    YUV frame = RGBtoYUV(bmp_image);

    vector<YUV> yuv_video = readYUV("video/BUS_176x144_15_orig_01.yuv", 176, 144, 75);

    insertYUV(yuv_video, frame);

    cout << "Done!\n";

    return 0;
}

void thr_write_component(vector<float> src, vector<vector<float> > &comp, int start, int n)
{
    for(int i = start; i < src.size(); i += 3){
        comp[n].push_back(src[i]);
    }
}

void thr_write_y(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h)
{
    int p = 0;
    int n = 0;
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if(((i >= of.y_top_offset_height) && (i < of.y_bottom_offset_height)) && ((j >= of.y_left_offset_width) && (j < of.y_right_offset_width))){
                res.push_back(src_1.at(p));
                p++;
            } else
                res.push_back(src.at(n));
            n++;
        }
    }
}

void thr_write_uv(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h)
{
    int n = 0;
    int p = 0;
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if(((i >= of.uv_top_offset_height) && (i < of.uv_bottom_offset_height)) && ((j >= of.uv_left_offset_width) && (j < of.uv_right_offset_width))){
                res.push_back(src_1.at(p));
                p++;
            } else
                res.push_back(src.at(n));
            n++;
        }
    }
}

/*void thr_write_v(vector<BYTE> src, vector<BYTE> src_1, vector<BYTE> &res, Offset &of, int w, int h)
{
    int n = 0;
    int p = 0;
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if(((i >= of.uv_top_offset_height) && (i < of.uv_bottom_offset_height)) && ((j >= of.uv_left_offset_width) && (j < of.uv_right_offset_width))){
                res.push_back(src_1.at(p));
                p++;
            } else
                res.push_back(src.at(n));
            n++;
        }
    }
}*/

YUV RGBtoYUV(const BMP &bmp)
{
    cout << "Converting rgb to yuv...\n";

    YUV yuv;
    /*
     * Результат преобразования сохраняется в массивы для компонент yuv, которые потом
     * записываются в объект yuv.
     * */
    vector<BYTE> y_r;
    vector<BYTE> u_r;
    vector<BYTE> v_r;

    /*
     * Компоненты rgb хранятся в двумерных массивах, где первый индекс означает позицию
     * по вертикали, а второй по горизонтали.
     * */
    vector<vector<float> > r;
    vector<vector<float> > g;
    vector<vector<float> > b;
    r.resize(bmp.getBitmap().size());
    g.resize(bmp.getBitmap().size());
    b.resize(bmp.getBitmap().size());

    /*
     * В цикле построчно считываются компоненты rgb из массива байт объекта BMP.
     * Каждый компонент записывается в отдельном потоке. В функцию thr_write_component
     * передается строка байт, ссылка на массив в который будет записываться компонента
     * и смещение от начала строки, т.к. в файле компоненты записаны побайтно в порядке
     * BGR.
     * */
    for(unsigned int i = 0, h = 0; i < bmp.getBitmap().size(); i++, h++){
        thread thr_blue(thr_write_component, bmp.getBitmap().at(i), ref(b), 0, h);
        thread thr_green(thr_write_component, bmp.getBitmap().at(i), ref(g), 1, h);
        thread thr_red(thr_write_component, bmp.getBitmap().at(i), ref(r), 2, h);
        thr_blue.join();
        thr_green.join();
        thr_red.join();
    }

    /*
     * Переменные __m128 хранят по 4 значения, для удобства доступа к отдельным элементам
     * было объявлено объединение simd_element(см. stdafx.h).
     *
     * В rgb записывается значение компонент для текущего пикселя.
     * uv это переменная в которой хранятся компоненты u и v после преобразования.
     * rgb_const, yuv_const, alig_const это переменные с константами, которые используются
     * для преобразования значений из rgb в yuv.
     * y_tmp вспомогательная переменная.
     * */
    simd_element rgb;
    simd_element uv;
    simd_element rgb_const;
    simd_element yuv_const;
    simd_element alig_const;
    simd_element y_tmp;

    rgb_const.var = _mm_set_ps(0, Kb, Kg, Kr);
    yuv_const.var = _mm_set_ps(0, Ku, 0, Kv);
    alig_const.var = _mm_set_ps(0, alig, 0, alig);

    float y_temp, u_temp, v_temp;


     /*for(int i = bmp.height() - 1; i >=0; i--){
        for(int j = 0; j < bmp.width(); j++){
            y_temp =( r[i][j]*Kr) + (g[i][j]*Kg) + (b[i][j]*Kb);
            cout << y_temp << endl;
            y_r.push_back((BYTE) y_temp);
            if(!((i + 1)%2) && !((j + 1)%2)){
                u_temp = ((b[i][j] - y_temp) * 0.565) + 128;
                v_temp = ((r[i][j] - y_temp) * 0.713) + 128;
                u_r.push_back((BYTE) u_temp);
                v_r.push_back((BYTE) v_temp);
            }
        }
    }*/

    /*
     * Преобразования выполняются по следующим формулам:
     * Y = 0.299*R + 0.587*G + 0.114*B;
     * U = (B - Y)*0.564 + 128;
     * V = (R - Y)*0.713 + 128;
     * */


    for(int i = bmp.height() - 1; i >=0; i--){
        for(int j = 0; j < bmp.width(); j++){
            rgb.var = _mm_set_ps(0, b[i][j], g[i][j], r[i][j]);         //rgb компоненты помещаются в регистр.
            y_tmp.var = _mm_mul_ps(rgb.var, rgb_const.var);             //Выполняется умножение на константы.
            y_temp = y_tmp.n_var[0] + y_tmp.n_var[1] + y_tmp.n_var[2];  //Результат складывается, используя возможность обращения к элементам через union.
            y_r.push_back((BYTE) y_temp);                               //Получившееся значение помещается в массив, с преобразованием в unsigned char(BYTE).
            if(!((i + 1)%2) && !((j + 1)%2)){                           //Компоненты u и v в yuv420 приходятся по 1 штуке на 4 пикселя, поэтому
                y_tmp.var = _mm_set_ps(0, y_temp, 0, y_temp);           //вычисляются только при выполнении условия, т.е. для каждого второго пикселя
                y_tmp.var = _mm_sub_ps(rgb.var, y_tmp.var);             //по горизонтали и по вертикали, начиная с нулевого.
                y_tmp.var = _mm_mul_ps(y_tmp.var, yuv_const.var);       //Из blue и red вычитается y и умножается на соответствующие константы,
                uv.var = _mm_add_ps(y_tmp.var, alig_const.var);         //затем прибавляется 128, чтобы не было отрицательных значений и значения помещаются в массив
                u_temp = uv.n_var[2];                                   //с преобразованием в unsigned char(BYTE).
                v_temp = uv.n_var[0];
                u_r.push_back((BYTE) u_temp);
                v_r.push_back((BYTE) v_temp);
            }
            rgb.var = _mm_setzero_ps();
            y_tmp.var = _mm_setzero_ps();
            uv.var = _mm_setzero_ps();
        }
    }

    /*
     * Поля объекта yuv инициализируются и объект возвращается.
     * */
    yuv.setFileName("frame.yuv");
    yuv.setWidth(bmp.width());
    yuv.setHeight(bmp.height());
    yuv.setSize(y_r.size() + u_r.size() + v_r.size());
    yuv.setY(y_r);
    yuv.setU(u_r);
    yuv.setV(v_r);

    return  yuv;
}

void insertYUV(vector<YUV> &video, const YUV &yuv)
{
    cout << "Inserting image...\n";
    vector<BYTE> y;
    vector<BYTE> u;
    vector<BYTE> v;
    vector<YUV> result;
    int width = video[0].getWidth();
    int height = video[0].getHeight();

    Offset offset;

    offset.y_top_offset_height      = ((height - yuv.getHeight())/2);
    offset.y_bottom_offset_height 	= yuv.getHeight() + offset.y_top_offset_height;
    offset.y_left_offset_width      = ((width - yuv.getWidth())/2);
    offset.y_right_offset_width 	= yuv.getWidth() + offset.y_left_offset_width;

    offset.uv_top_offset_height     = ((height/2) - (yuv.getHeight()/2))/2;
    offset.uv_bottom_offset_height	= (yuv.getHeight()/2) + offset.uv_top_offset_height;
    offset.uv_left_offset_width 	= ((width/2) - (yuv.getWidth()/2))/2;
    offset.uv_right_offset_width	= (yuv.getWidth()/2) + offset.uv_left_offset_width;

    for(int f = 0; f < video.size(); f++){
        thread thr_y(thr_write_y, video[f].getY(), yuv.getY(), ref(y), ref(offset), width, height);
        thread thr_u(thr_write_uv, video[f].getU(), yuv.getU(), ref(u), ref(offset), width/2, height/2);
        thread thr_v(thr_write_uv, video[f].getV(), yuv.getV(), ref(v), ref(offset), width/2, height/2);
        thr_y.join();
        thr_u.join();
        thr_v.join();

        YUV new_frame("New frame", width, height);
        new_frame.setY(y);
        new_frame.setU(u);
        new_frame.setV(v);
        result.push_back(new_frame);

        y.clear();
        u.clear();
        v.clear();
    }

    writeYUV(result, "new_video.yuv");
}

vector<YUV> readYUV(const string &filename, int width, int height, int frame_num = 1)
{
    fstream fin(filename, ios_base::in | ios_base::binary);
    if(!fin.is_open()){
        cout << filename << " is not open\n";
        fin.close();
    } else {
        vector<BYTE> y;
        vector<BYTE> u;
        vector<BYTE> v;
        vector<YUV> yuv;
        BYTE temp;
        for(int f = 0; f < frame_num; f++){
            for(int i = 0; i < (width * height); i++){
                fin.read((char *) &temp, sizeof(BYTE));
                y.push_back(temp);
            }
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    if(!(i%2) && !(j%2) && (((i + 1) != height) && ((j + 1) != width))){
                        fin.read((char *) &temp, sizeof(BYTE));
                        u.push_back(temp);
                    }
                }
            }
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    if(!(i%2) && !(j%2) && (((i + 1) != height) && ((j + 1) != width))){
                        fin.read((char *) &temp, sizeof(BYTE));
                        v.push_back(temp);
                    }
                }
            }
            YUV yuv_frame(filename, width, height);
            yuv_frame.setY(y);
            yuv_frame.setU(u);
            yuv_frame.setV(v);
            yuv.push_back(yuv_frame);
            y.clear();
            u.clear();
            v.clear();
        }

        fin.close();
        return yuv;
    }
}

void writeYUV(const vector<YUV> &yuv, const std::string &fn)
{
    fstream fout(fn, ios_base::out | ios_base::binary);
    if(!fout.is_open()){
        cout << fn << " is not open\n";
        fout.close();
        return;
    }

    for(int f = 0; f < yuv.size(); f++){
        for(int i = 0; i < yuv[f].getY().size(); i++){
            fout.write((char *) &yuv[f].getY().at(i), sizeof(BYTE));
        }
        for(int i = 0; i < yuv[f].getU().size(); i++){
            fout.write((char *) &yuv[f].getU().at(i), sizeof(BYTE));
        }
        for(int i =0; i < yuv[f].getV().size(); i++){
            fout.write((char *) &yuv[f].getV().at(i), sizeof(BYTE));
        }
    }
    fout.close();
}

