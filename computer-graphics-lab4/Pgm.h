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

enum ScaleType {
    NEAREST_POINT = 0,
    BILINEAR = 1,
    LANCZOS3 = 2,
    BC_SPLINE = 3
};

enum ErrorType {
    DONE = 0,
    OPEN_FILE = 1,
    FORMAT_EXCEPTION = 2,
    MEMORY_ALLOCATION = 3
};

class Pixel {
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class Pgm {
public:
    Pgm(int res_height, int res_width, double BC_b, double BC_c, double dx, double dy, double gamma) {
        this->gamma = gamma;
        this->dx = dx;
        this->dy = dy;
        this->res_height = res_height;
        this->res_width = res_width;
        this->BC_b = BC_b;
        this->BC_c = BC_c;
    }

    double Rev_Gamma(double value) const {
//        value /= (color_num * 1.0);
        if (gamma == 0) {
            if (value <= 0.0031308) {
                return (value * 12.92) * color_num;
            } else {
                return (1.055 * std::pow(value, 1 / 2.4) - 0.055) * color_num;
            }
        } else {
            return std::pow(value, 1.0 / gamma) * color_num;
        }
    }

    double Gamma(double value) const {
        value /= (color_num * 1.0);
        if (gamma == 0) {
            if (value <= 0.040449936) {
                return value / 12.92;

            } else {
                return pow((value + 0.055) / 1.055, 2.4);
            }
        } else {
            return std::pow(value, gamma);
        }
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
        if (result != 3 || buf[0] != 'P' || (buf[1] != '5' && buf[1] != '6') || buf[2] != '\n') {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }
        image_type = buf[1];
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
        if (image_type == '5') {
            result = fread(data_p5, 1, width * height, pFile);
            if (result != width * height) {
                ex_case = FORMAT_EXCEPTION;
                goto FEND;
            }
        } else {
            result = fread(data_p6, 1, 3 * width * height, pFile);
            if (result != 3 * width * height) {
                ex_case = FORMAT_EXCEPTION;
                goto FEND;
            }
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
        if (image_type == '5') {
            output << "P5\n" << res_width << '\n' << res_height << '\n' << color_num << '\n';
            output.write((char *) res_p5, res_height * res_width);
        } else {
            output << "P6\n" << res_width << '\n' << res_height << '\n' << color_num << '\n';
            output.write((char *) res_p6, sizeof(Pixel) * res_height * res_width);
        }
        output.close();
        return DONE;
    }

    static double bilinear(double x) {
        return fmax(1 - fabs(x), 0);
    }

    static double lanczos_filter(double x) {
        if (x == 0) {
            return 1;
        }
        if (-3 <= x && x <= 3) {
            return ((3.0 * sin(M_PI * x) * sin((M_PI * x) / 3.0)) / (M_PI * M_PI * x * x));
        }
        return 0;
    }

    double Mitchell_Netravali(double x) {
        if (x < 0)
            x = -x;
        if (x < 1.) {
            return ((6.0 - 2.0 * BC_b) / 6.0)
                   + ((-18. + 12.0 * BC_b + 6.0 * BC_c) / 6.0) * x * x
                   + ((12.0 - 9.0 * BC_b - 6.0 * BC_c) / 6.0) * x * x * x;
        } else if (x < 2.) {
            return ((8.0 * BC_b + 24.0 * BC_c) / 6.0)
                   + ((-12.0 * BC_b - 48.0 * BC_c) / 6.0) * x
                   + ((6.0 * BC_b + 30.0 * BC_c) / 6.0) * x * x
                   + ((-1.0 * BC_b - 6.0 * BC_c) / 6.0) * x * x * x;
        } else {
            return 0.;
        }
    }

    uint8_t close_ne(double total_col) {
        double bor = std::max(std::min(total_col, 1.0), 0.0);
        uint8_t left = Rev_Gamma(bor);
        uint8_t right = border(left + 1, 255);
        if (fabs(Gamma(left) - bor) < fabs(Gamma(right) - bor)) {
            return border(left, 255);
        } else {
            return border(right, 255);
        }
    }

    static double border(double val, double mx) {
        return val < 0.0 ? 0.0 : val > mx ? mx : val;
    }

    static int iborder(double val, double mx) {
        return val < 0.0 ? 0.0 : val > mx ? mx : val;
    }

    void Scale(ScaleType scaleType) {
        double old_cen_i = (height / 2.0);
        double old_cen_j = (width / 2.0);
        double cen_i = (res_height / 2.0);
        double cen_j = (res_width / 2.0);
        double mov_cen_i = (dx + cen_i);
        double mov_cen_j = (dy + cen_j);
        double dif_si_i = std::max(1.0, ((height * 1.0) / (res_height * 1.0)));
        double dif_si_j = std::max(1.0, ((width * 1.0) / (res_width * 1.0)));

        if (scaleType == NEAREST_POINT) {
            for (int i = 0; i < res_height; i++) {
                for (int j = 0; j < res_width; j++) {
                    double val_i = ((i * 1.0) + 0.5 - mov_cen_i) / cen_i;
                    double val_j = ((j * 1.0) + 0.5 - mov_cen_j) / cen_j;
                    double pos_i = val_i * old_cen_i + old_cen_i;
                    double pos_j = val_j * old_cen_j + old_cen_j;
                    pos_i = std::round(pos_i - 0.5) + 0.5;
                    pos_i = border(pos_i, height - 1);
                    pos_j = std::round(pos_j - 0.5) + 0.5;
                    pos_j = border(pos_j, width - 1);
                    size_t ipos_i = pos_i;
                    size_t ipos_j = pos_j;
                    if (image_type == '5') {
                        res_p5[i * res_width + j] = data_p5[ipos_i * width + ipos_j];
                    } else {
                        res_p6[i * res_width + j] = data_p6[ipos_i * width + ipos_j];
                    }
                }
            }
            return;
        }
        for (int i = 0; i < res_height; i++) {
            for (int j = 0; j < width; j++) {
                double val_i = (i + 0.5 - mov_cen_i) / cen_i;
                double pos_i = val_i * old_cen_i + old_cen_i;
                double ipos_i = std::round(pos_i - 0.5) + 0.5;
                double total_col0 = 0;
                double total_col1 = 0;
                double total_col2 = 0;
                double total_col3 = 0;
                switch (scaleType) {
                    case BILINEAR: {

                        for (double c = -dif_si_i; c <= dif_si_i; c += 1.0) {
                            double idx = (ipos_i + c);
                            if (image_type == '5') {
                                total_col0 += (bilinear((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p5[iborder(idx, height - 1) * width + j] * 1.0);
                            } else {
                                total_col1 += (bilinear((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].r * 1.0);
                                total_col2 += (bilinear((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].g * 1.0);
                                total_col3 += (bilinear((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].b * 1.0);
                            }

                        }
                        break;
                    }
                    case LANCZOS3: {
                        for (double c = -(3.0 * dif_si_i); c <= (3.0 * dif_si_i); c += 1.0) {
                            double idx = (ipos_i + c);
                            if (image_type == '5') {
                                total_col0 += (lanczos_filter((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p5[iborder(idx, height - 1) * width + j] * 1.0);
                            } else {
                                total_col1 += (lanczos_filter((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].r * 1.0);
                                total_col2 += (lanczos_filter((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].g * 1.0);
                                total_col3 += (lanczos_filter((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].b * 1.0);

                            }
                        }
                        break;
                    }
                    case BC_SPLINE: {
                        for (double c = -(2.0 * dif_si_i); c <= (2.0 * dif_si_i); c += 1.0) {
                            double idx = (ipos_i + c);
                            if (image_type == '5') {
                                total_col0 += (Mitchell_Netravali((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p5[iborder(idx, height - 1) * width + j] * 1.0);
                            } else {
                                total_col1 += (Mitchell_Netravali((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].r * 1.0);
                                total_col2 += (Mitchell_Netravali((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].g * 1.0);
                                total_col3 += (Mitchell_Netravali((pos_i - idx) / dif_si_i) / dif_si_i)
                                              * Gamma(data_p6[iborder(idx, height - 1) * width + j].b * 1.0);

                            }
                        }
                        break;

                    }

                }
                if (image_type == '5') {
                    inter_res_p5[i * width + j] = close_ne(total_col0);
                } else {
                    inter_res_p6[i * width + j].r = close_ne(total_col1);
                    inter_res_p6[i * width + j].g = close_ne(total_col2);
                    inter_res_p6[i * width + j].b = close_ne(total_col3);

                }
            }
        }
        for (int i = 0; i < res_height; ++i) {
            for (int j = 0; j < res_width; j++) {
                double val_j = (j + 0.5 - mov_cen_j) / cen_j;
                double pos_j = val_j * old_cen_j + old_cen_j;
                double ipos_j = std::round(pos_j - 0.5) + 0.5;
                double total_col0 = 0;
                double total_col1 = 0;
                double total_col2 = 0;
                double total_col3 = 0;
                switch (scaleType) {
                    case BILINEAR: {
                        for (double c = -dif_si_j; c <= dif_si_j; c += 1.0) {
                            double idx = (ipos_j + c);
                            if (image_type == '5') {
                                total_col0 += (bilinear((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p5[i * width + iborder(idx, width - 1)]);
                            } else {
                                total_col1 += (bilinear((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].r);
                                total_col2 += (bilinear((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].g);
                                total_col3 += (bilinear((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].b);
                            }
                        }
                        break;
                    }
                    case LANCZOS3: {
                        for (double c = -(3.0 * dif_si_j); c <= (3 * dif_si_j); c += 1.0) {
                            double idx = (ipos_j + c);
                            if (image_type == '5') {
                                total_col0 += (lanczos_filter((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p5[i * width + iborder(idx, width - 1)]);
                            } else {
                                total_col1 += (lanczos_filter((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].r);
                                total_col2 += (lanczos_filter((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].g);
                                total_col3 += (lanczos_filter((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].b);
                            }
                        }

                        break;
                    }
                    case BC_SPLINE: {
                        for (double c = -(2.0 * dif_si_j); c <= (2 * dif_si_j); c += 1.0) {
                            double idx = (ipos_j + c);
                            if (image_type == '5') {
                                total_col0 += (Mitchell_Netravali((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p5[i * width + iborder(idx, width - 1)]);
                            } else {
                                total_col1 += (Mitchell_Netravali((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].r);
                                total_col2 += (Mitchell_Netravali((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].g);
                                total_col3 += (Mitchell_Netravali((pos_j - idx) / dif_si_j) / dif_si_j)
                                              * Gamma(inter_res_p6[i * width + iborder(idx, width - 1)].b);
                            }
                        }
                        break;
                    }
                }
                if (image_type == '5') {
                    res_p5[i * res_width + j] = close_ne(total_col0);
                } else {
                    res_p6[i * res_width + j].r = close_ne(total_col1);
                    res_p6[i * res_width + j].g = close_ne(total_col2);
                    res_p6[i * res_width + j].b = close_ne(total_col3);
                }


            }

        }

    }

    bool ImageMalloc() {
        free(data_p5);
        free(res_p5);
        free(inter_res_p5);
        free(data_p6);
        free(res_p6);
        free(inter_res_p6);
        data_p5 = (uint8_t *) (malloc(width * height));
        inter_res_p5 = (uint8_t *) (malloc(width * res_height));
        res_p5 = (uint8_t *) (malloc(res_width * res_height));

        data_p6 = (Pixel *) (malloc(width * height * sizeof(Pixel)));
        inter_res_p6 = (Pixel *) (malloc(width * res_height * sizeof(Pixel)));
        res_p6 = (Pixel *) (malloc(res_width * res_height * sizeof(Pixel)));
        if (data_p5 == nullptr || data_p6 == nullptr) {
            return false;
        }
        if (inter_res_p5 == nullptr || inter_res_p6 == nullptr) {
            return false;
        }
        if (res_p5 == nullptr || res_p6 == nullptr) {
            return false;
        }
        return true;
    }

    ~Pgm() {
        free(data_p5);
        free(inter_res_p5);
        free(res_p5);
        free(data_p6);
        free(inter_res_p6);
        free(res_p6);
    }

private:
    double p[4] = {0};
    int width = 0;
    double dx, dy;
    double gamma;
    double BC_b, BC_c;
    int height = 0;
    int color_num = 0;
    uint8_t *data_p5 = nullptr;
    uint8_t *res_p5 = nullptr;
    uint8_t *inter_res_p5 = nullptr;
    Pixel *data_p6 = nullptr;
    Pixel *res_p6 = nullptr;
    Pixel *inter_res_p6 = nullptr;
    int res_width;
    char image_type;
    int res_height;

};

#endif //COMPUTER_GRAPHICS_LAB4_PGM_H
