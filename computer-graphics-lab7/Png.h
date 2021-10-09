//
// Created by sWX952464 on 4/18/2021.
//

#ifndef COMPUTER_GRAPHICS_LAB4_PGM_H
#define COMPUTER_GRAPHICS_LAB4_PGM_H


#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <algorithm>

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include "zlib/zlib.h"

class Pixel {

public:
    void sum_mod(Pixel val) {
        this->r = (this->r + val.r) % 256;
        this->g = (this->g + val.g) % 256;
        this->b = (this->b + val.b) % 256;
    }

    Pixel(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    Pixel() {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    uint8_t &get_i(int idx) {
        if (idx == 0) {
            return this->r;
        } else if (idx == 1) {
            return this->g;
        } else {
            return this->b;
        }
    }

    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum ErrorType {
    DONE = 0,
    OPEN_FILE = 1,
    FORMAT_EXCEPTION = 2,
    MEMORY_ALLOCATION = 3,
    ZLIB_FAILED = 4,
};

enum FilterType {
    NONE = 0,
    SUB = 1,
    UP = 2,
    AVERAGE = 3,
    PAETH = 4,
};


class Pgm {
public:
    int Read(const char *const fileName) {
        size_t length;
        size_t st_pos = 0;
        std::vector<uint8_t> idat_d;
        std::ifstream ifile(fileName, std::ios::in | std::ios::binary);
        if (!ifile.is_open()) {
            return OPEN_FILE;
        }
        ifile.seekg(0, std::ios::end);
        this->size_d = ifile.tellg();
        ifile.seekg(0, std::ios::beg);
        if (!DataMalloc()) {
            ifile.close();
            return MEMORY_ALLOCATION;
        }
        ifile.seekg(8); // skip the header
        ifile.read(reinterpret_cast<char *>(data), size_d - 8);
        if (!ifile) {
            ifile.close();
            return FORMAT_EXCEPTION;
        }
        ifile.close();
        while (st_pos < size_d) {
            size_t this_chunk_st_pos = st_pos;
            length = 0;
            for (int i = 0; i < 4; i++) {
                length = (length << 8) | (unsigned char) (data[st_pos]);
                st_pos++;
            }
            std::string str;
            for (size_t i = 0; i < 4; i++) {
                str += (char) data[st_pos];
                st_pos++;
            }
            if (str == "IDAT") {
                for (size_t i = 0; i < length; i++) {
                    idat_d.push_back(data[st_pos]);
                    st_pos++;
                }

            } else if (str == "IHDR") {
                for (size_t i = 0; i < 4; i++) {
                    width = (width << 8) | (unsigned char) (data[st_pos]);
                    st_pos++;
                }
                for (size_t i = 0; i < 4; i++) {
                    height = (height << 8) | (unsigned char) (data[st_pos]);
                    st_pos++;
                }
                if (data[st_pos] != 8 /*bit depth */
                    || (data[st_pos + 1] != 0 && data[st_pos + 1] != 2) /*colour type*/
                    || data[st_pos + 2] != 0/*compression method*/
                    || data[st_pos + 3] != 0/*filter method*/
                    || data[st_pos + 4] != 0/*interlace method*/) {
                    return FORMAT_EXCEPTION;
                }
                color_type = (unsigned char) (data[st_pos + 1]);

            } else if (str == "IEND") {
                break;
            }
            st_pos = this_chunk_st_pos + (12 + length);//move to the next chunk
        }
        if (!ImageMalloc()) {
            return MEMORY_ALLOCATION;
        }
        color_type += 1;
        size_t size_of_out_data = (color_type * width + 1) * height;
        std::vector<uint8_t> data_out(size_of_out_data);
        z_stream stream;
        stream.avail_in = idat_d.size();
        stream.next_in = &idat_d[0];
        stream.avail_out = size_of_out_data;
        stream.next_out = &data_out[0];
        stream.zfree = Z_NULL;
        stream.zalloc = Z_NULL;
        stream.opaque = Z_NULL;
        auto err = inflateInit(&stream);
        if (err != Z_OK) {
            return ZLIB_FAILED;
        }
        auto er = inflate(&stream, Z_FULL_FLUSH);
        if (er == Z_MEM_ERROR) {
            return MEMORY_ALLOCATION;
        } else if (er != Z_STREAM_END) {
            return ZLIB_FAILED;
        }
        inflateEnd(&stream);
        add_pixel(data_out);
        idat_d.clear();
        data_out.clear();
        return DONE;
    }

    void add_pixel(std::vector<uint8_t> &data_out) {
        for (int i = 0; i < height; i++) {
            size_t st_pos = (color_type * width + 1) * i;
            int typeFilter = data_out[st_pos];
            st_pos++;
            filter_type.push_back(typeFilter);
            for (int j = 0; j < width; j++) {
                for (int g = 0; g < color_type; g++) {
                    int val = data_out[st_pos];
                    if (g == 0) {
                        res_p5[i * width + j].r = val;
                    } else if (g == 1) {
                        res_p5[i * width + j].g = val;
                    } else {
                        res_p5[i * width + j].b = val;
                    }
                    st_pos++;
                }
            }
        }
    }

    void sub_filter(int r, int c) {
        if (c != 0) {
            res_p5[r * width + c].sum_mod(res_p5[r * width + c - 1]);
        }
    }

    void up_filter(int r, int c) {
        if (r != 0) {
            res_p5[r * width + c].sum_mod(res_p5[(r - 1) * width + c]);
        }
    }

    void average_filter(int r, int c) {
        Pixel av;
        Pixel left;
        Pixel up;
        if (r > 0)
            up = res_p5[(r - 1) * width + c];
        if (c > 0)
            left = res_p5[r * width + c - 1];
        av.r = (up.r + left.r) / 2;
        av.g = (up.g + left.g) / 2;
        av.b = (up.b + left.b) / 2;
        res_p5[r * width + c].sum_mod(av);
    }

    void paeth_filter(int r, int c) {
        Pixel left;
        Pixel up;
        Pixel up_left;
        if (c > 0) {
            left = res_p5[r * width + c - 1];
        }
        if (r > 0) {
            up = res_p5[(r - 1) * width + c];
        }
        if (r > 0 && c > 0) {
            up_left = res_p5[(r - 1) * width + (c - 1)];
        }
        for (int i = 0; i < 3; i++) {
            int val = left.get_i(i) + up.get_i(i) - up_left.get_i(i);
            int t1 = std::abs(val - left.get_i(i));
            int t2 = std::abs(val - up.get_i(i));
            int t3 = std::abs(val - up_left.get_i(i));
            if (t1 <= t2 && t1 <= t3) {
                res_p5[r * width + c].get_i(i) = (res_p5[r * width + c].get_i(i) + left.get_i(i)) % 256;
            } else if (t2 <= t3) {
                res_p5[r * width + c].get_i(i) = (res_p5[r * width + c].get_i(i) + up.get_i(i)) % 256;
            } else {
                res_p5[r * width + c].get_i(i) = (res_p5[r * width + c].get_i(i) + up_left.get_i(i)) % 256;
            }
        }
    }

    void filter() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                switch (filter_type[i]) {
                    case SUB: {
                        sub_filter(i, j);
                        break;
                    }
                    case UP: {
                        up_filter(i, j);
                        break;
                    }
                    case AVERAGE: {
                        average_filter(i, j);
                        break;
                    }
                    case PAETH: {
                        paeth_filter(i, j);
                        break;
                    }
                }
            }
        }
    }

    int writer(const char *out) {
        std::ofstream output;
        output.open(out, std::ios_base::out | std::ios_base::binary);
        if (!output.is_open()) {
            return OPEN_FILE;
        }
        if (color_type == 1) {
            output << "P5\n" << width << '\n' << height << '\n' << 255 << '\n';
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    output << res_p5[i * width + j].r;
                }
            }
        } else {
            output << "P6\n" << width << '\n' << height << '\n' << 255 << '\n';
            output.write((char *) res_p5, sizeof(Pixel) * height * width);
        }
        output.close();

        return DONE;
    }
    bool DataMalloc() {
        free(data);
        data = (uint8_t *) (malloc(size_d));
        if (data == nullptr) {
            return false;
        }
        return true;
    }
    bool ImageMalloc() {
        free(res_p5);
        res_p5 = (Pixel *) (malloc(width * height * sizeof(Pixel)));
        if (res_p5 == nullptr) {
            return false;
        }
        return true;
    }

    ~Pgm() {
        free(data);
        free(res_p5);
        filter_type.clear();
    }

private:
    uint8_t *data = nullptr;
    size_t size_d;
    Pixel *res_p5 = nullptr;
    size_t width = 0;
    size_t height = 0;
    size_t color_type = 0;
    std::vector<int> filter_type;
};


#endif //COMPUTER_GRAPHICS_LAB4_PGM_H
