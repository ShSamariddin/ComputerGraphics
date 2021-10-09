//
// Created by sWX952464 on 4/12/2021.
//

#ifndef COMPUTER_GRAPHICS_LAB3_PGM_H
#define COMPUTER_GRAPHICS_LAB3_PGM_H

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <fstream>
#include <vector>
#include <algorithm>

enum DithType {
    NONE = 0,
    ORDERED8x8 = 1,
    RANDOM = 2,
    FLOYD_STEINBERG = 3,
    JJJ = 4,
    SIERA_3 = 5,
    ATKINSON = 6,
    HALFTON4x4 = 7
};

enum ErrorType {
    DONE = 0,
    OPEN_FILE = 1,
    FORMAT_EXCEPTION = 2,
    MEMORY_ALLOCATION = 3
};

const int jjj_val[]{0, 0, 0, 7, 5, 3, 5, 7, 5, 3, 1, 3, 5, 3, 1};
const int sierra3[]{0, 0, 0, 5, 3, 2, 4, 5, 4, 2, 0, 2, 3, 2, 0};
const int aktinson[]{0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0};

const double Ordered[8][8] = {
        {0,  32, 8,  40, 2,  34, 10, 42},
        {48, 16, 56, 24, 50, 18, 58, 26},
        {12, 44, 4,  36, 14, 46, 6,  38},
        {60, 28, 52, 20, 62, 30, 54, 22},
        {3,  35, 11, 43, 1,  33, 9,  41},
        {51, 19, 59, 27, 49, 17, 57, 25},
        {15, 47, 7,  39, 13, 45, 5,  37},
        {63, 31, 55, 23, 61, 29, 53, 21},
};

const double Halftone[4][4] = {
        {7,  13, 11, 4},
        {12, 16, 14, 8},
        {10, 15, 6,  2},
        {5,  9,  3,  1}
};


class Pgm {
public:

    Pgm(double gamma, DithType dith_type, int bits_size, bool grad) {
        this->gamma = gamma;
        this->dith_type = dith_type;
        this->bits_size = bits_size;
        this->grad = grad;
    }

    int Read(const char *const fileName) {
        FILE *pFile;
        char buf[12];
        ErrorType ex_case = DONE;
        size_t result;
        bool com = false;

        pFile = fopen(fileName, "rb");
        if (pFile == nullptr) {
            return 1;
        }
        fseek(pFile, 0, SEEK_END);
        rewind(pFile);

        /*reading picture type  */
        result = fread(buf, 1, 3, pFile);
        if (result != 3 || buf[0] != 'P' || buf[1] != '5' || buf[2] != '\n') {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }
        /*reading picture size*/
        while (true) {
            result = fread(buf, 1, 1, pFile);
            if (result != 1) {
                ex_case = FORMAT_EXCEPTION;
                goto FEND;
            }
            if (buf[0] == '#') {
                com = true;
            }
            if (buf[0] == '\n') {
                if (com) {
                    com = false;
                    continue;
                }
                break;
            }
            if (com) {
                continue;
            }
            if (isdigit(buf[0])) {
                height = height * 10 + (buf[0] - '0');
            } else if (buf[0] == ' ') {
                width = height;
                height = 0;
            } else {
                ex_case = FORMAT_EXCEPTION;
                goto FEND;
            }
        }

        if (!ImageMalloc()) {
            ex_case = MEMORY_ALLOCATION;
            goto FEND;
        }

        /*reading pixel size*/
        result = fread(buf, 1, 4, pFile);
        color_num = std::stoi(buf);

        if (color_num < 0 || color_num > 255) {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }
        /*reading data*/
        result = fread(data_p5, 1, width * height, pFile);
        if (result != width * height) {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }


        FEND:
        fclose(pFile);
        return ex_case;

    }

    int writer(const char *out) {

        std::ofstream output;
        output.open(out, std::ios_base::out | std::ios_base::binary);
        if (!output.is_open()) {
            return OPEN_FILE;
        }
        output << "P5\n" << width << '\n' << height << '\n' << color_num << '\n';
        output.write((char *) data_p5, height * width);
        output.close();
        return DONE;
    }

    double Gradient(int h, int w) {
        if (grad) {
            return (w * 255.0) / ((width - 1) * 1.0);
        } else {
            return data_p5[h * width + w];
        }

    }

    double Gamma(double value) const {
        value /= (color_num * 1.0);
        if (gamma == 0) {
            if (value <= 0.040449936) {
                return color_num * value / 12.92;
            } else {
                return color_num * pow((value + 0.055) / 1.055, 2.4);
            }


        } else {
            return color_num * std::pow(value, gamma);
        }
    }

    void Dither() {
        int brightness = (1 << bits_size) - 1;
        switch (dith_type) {
            case NONE: {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = (Gradient(i, j) / color_num);
                        col *= brightness;
                        double lower_bo = (int) col;
                        double upper_bo = lower_bo + 1 > brightness ? brightness : lower_bo + 1;
                        lower_bo *= ((color_num * 1.0) / (brightness * 1.0));
                        upper_bo *= ((color_num * 1.0) / (brightness * 1.0));
                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);
                        col = Gradient(i, j);
                        double cur_gamma = (Gamma(col));
                        if ((std::abs(lower_gamma - cur_gamma)) <= (std::abs(upper_gamma - cur_gamma))) {
                            col = lower_gamma;
                        } else {
                            col = upper_gamma;
                        }
                        data_p5[i * width + j] = col;
                    }
                }
                break;
            }
            case ORDERED8x8: {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);
                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);
                        double diff = (upper_gamma - lower_gamma);

                        col = Gamma(col) + ((Ordered[i % 8][j % 8] + 0.5) / 64.0 - 0.5) * diff;
                        if (fabs(lower_gamma - col) < fabs(upper_gamma - col)) {
                            col = lower_gamma;
                        } else {
                            col = upper_gamma;
                        }
                        data_p5[i * width + j] = col;
                    }
                }
                break;
            }
            case RANDOM: {
                std::srand(1);
                double mx = 0;
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = (Gradient(i, j) / color_num);
                        col *= brightness;
                        double lower_bo = (int) col;
                        double upper_bo = lower_bo + 1 > brightness ? brightness : lower_bo + 1;
                        lower_bo *= ((color_num * 1.0) / (brightness * 1.0));
                        upper_bo *= ((color_num * 1.0) / (brightness * 1.0));
                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);
                        double diff = (upper_gamma - lower_gamma);
                        col = Gradient(i, j);
                        double cur_gamma = (Gamma(col)) + ((double) rand() / 32767.0) * diff;
                        if (cur_gamma >= upper_gamma) {
                            col = upper_gamma;
                        } else {
                            col = lower_gamma;
                        }
                        data_p5[i * width + j] = col;
                    }
                }
                break;
            }
            case FLOYD_STEINBERG: {
                std::vector<double> error(height *width,
                0);
                double ans = 0;
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);

                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);

                        double val = Gamma(col) + error[i * width + j];
                        double new_val;
                        if(std::abs(upper_gamma - val) < std::abs(lower_gamma - val)){
                            new_val = upper_gamma;
                        } else{
                            new_val = lower_gamma;
                        }
                        double CurrentErrorValue = (val - new_val);
                        data_p5[i * width + j] = (uint8_t) new_val;
                        if (j + 1 < width) {
                            error[i * width + j + 1] += ((7.0 * CurrentErrorValue) / 16.0);

                        }
                        if (i + 1 < height) {
                            if (j + 1 < width) {//(i + 1, j + 1)
                                error[(i + 1) * width + j + 1] += ((1.0 * CurrentErrorValue) / 16.0);
                            }
                            error[(i + 1) * width + j] += ((5.0 * CurrentErrorValue) / 16.0);//(i + 1, j)
                            if ((j - 1 >= 0)) {
                                error[(i + 1) * width + j - 1] += ((3.0 * CurrentErrorValue) / 16.0);//(i + 1, j - 1)
                            }
                        }

                    }
                }
                break;
            }
            case JJJ: {
                std::vector<double> error(height *width,
                0);
                double ans;
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);

                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);

                        double val = Gamma(col) + error[i * width + j];
                        double new_val;
                        if(std::abs(upper_gamma - val) < std::abs(lower_gamma - val)){
                            new_val = upper_gamma;
                        } else{
                            new_val = lower_gamma;
                        }
                        double CurrentErrorValue = (val - new_val);
                        data_p5[i * width + j] = (uint8_t) new_val;

                        int ord = 0;
                        for (int x = 0; x <= 2; x++) {
                            for (int y = -2; y <= 2; y++) {
                                if (i + x < height && j + y >= 0 && j + y < width) {
                                    error[(i + x) * width + (j + y)] += ((jjj_val[ord] * 1.0 * CurrentErrorValue) /
                                                                         48.0);
                                }
                                ord++;
                            }
                        }
                    }
                }
                break;
            }
            case SIERA_3: {
                std::vector<double> error(height *width,
                0);
                double ans;
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);

                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);

                        double val = Gamma(col) + error[i * width + j];
                        double new_val;
                        if(std::abs(upper_gamma - val) < std::abs(lower_gamma - val)){
                            new_val = upper_gamma;
                        } else{
                            new_val = lower_gamma;
                        }
                        double CurrentErrorValue = (val - new_val);
                        data_p5[i * width + j] = (uint8_t) new_val;
                        int ord = 0;
                        for (int x = 0; x <= 2; x++) {
                            for (int y = -2; y <= 2; y++) {
                                if (i + x < height && j + y >= 0 && j + y < width) {
                                    error[(i + x) * width + (j + y)] += ((sierra3[ord] * 1.0 * CurrentErrorValue) /
                                                                         32.0);
                                }
                                ord++;
                            }
                        }
                    }
                }
                break;
            }

            case ATKINSON: {
                std::vector<double> error(height *width,
                0);
                double ans;
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);

                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);

                        double val = Gamma(col) + error[i * width + j];
                        double new_val;
                        if(std::abs(upper_gamma - val) < std::abs(lower_gamma - val)){
                            new_val = upper_gamma;
                        } else{
                            new_val = lower_gamma;
                        }
                        double CurrentErrorValue = (val - new_val);
                        data_p5[i * width + j] = (uint8_t) new_val;
                        int ord = 0;
                        for (int x = 0; x <= 2; x++) {
                            for (int y = -2; y <= 2; y++) {
                                if (i + x < height && j + y >= 0 && j + y < width) {
                                    error[(i + x) * width + (j + y)] += ((aktinson[ord] * 1.0 * CurrentErrorValue) /
                                                                         8.0);
                                }
                                ord++;
                            }
                        }
                    }
                }
                break;
            }

            case HALFTON4x4: {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        double col = Gradient(i, j);
                        double seg = std::round(color_num / brightness);
                        double lower_bo = (int) ((col) / seg);
                        double upper_bo = lower_bo + 1;
                        lower_bo *= seg;
                        upper_bo *= seg;
                        lower_bo = (int) lower_bo;
                        upper_bo = std::min(255, (int) upper_bo);

                        double lower_gamma = Gamma(lower_bo);
                        double upper_gamma = Gamma(upper_bo);
                        double diff = (upper_gamma - lower_gamma);
                        col = Gamma(col) + ((Halftone[i % 4][j % 4] - 0.5) / 16.0 - 0.5) * diff;
                        if (fabs(lower_gamma - col) < fabs(upper_gamma - col)) {
                            col = lower_gamma;
                        } else {
                            col = upper_gamma;
                        }
                        data_p5[i * width + j] = col;
                    }
                }
                break;
            }


        }
    }


    bool ImageMalloc() {
        free(data_p5);
        data_p5 = (uint8_t *) (malloc(width * height));
        if (data_p5 == nullptr) {
            return false;
        }
        return true;
    }

    ~Pgm() {
        free(data_p5);
    }

private:
    int width = 0;
    int height = 0;
    int color_num = 0;
    int bits_size;
    double gamma;
    uint8_t *data_p5 = nullptr;
    DithType dith_type;
    bool grad;

};


#endif //COMPUTER_GRAPHICS_LAB3_PGM_H