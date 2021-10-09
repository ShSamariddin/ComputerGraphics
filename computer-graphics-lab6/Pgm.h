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
#include <complex>
#include <complex.h>

#define _USE_MATH_DEFINES

#include <cmath>


enum ErrorType {
    DONE = 0,
    OPEN_FILE = 1,
    FORMAT_EXCEPTION = 2,
    MEMORY_ALLOCATION = 3
};


class Pgm {
public:


    int Read(const char *const fileName, const int id) {
        FILE *pFile;
        int cur_width = 0;
        int cur_height = 0;
        int cur_color_number = 0;
        char buf[12] = {0};
        ErrorType ex_case = DONE;
        size_t result = 0;
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
                cur_height = cur_height * 10 + (buf[0] - '0');
            } else if (buf[0] == ' ') {
                cur_width = cur_height;
                cur_height = 0;
            } else {
                ex_case = FORMAT_EXCEPTION;
                goto FEND;
            }
        }


        if (id == 1 && (cur_height != height || cur_width != width)) {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }
        height = cur_height;
        width = cur_width;

        if (id == 0 && !ImageMalloc()) {
            ex_case = MEMORY_ALLOCATION;
            goto FEND;
        }
        /*reading pixel size*/
        result = fread(buf, 1, 4, pFile);
        cur_color_number = std::stoi(buf);

        if (cur_color_number < 0 || cur_color_number > 255) {
            ex_case = FORMAT_EXCEPTION;
            goto FEND;
        }
        color_num = cur_color_number;
        /*reading data*/
        if (id == 0) {
            result = fread(data_p5_0, 1, width * height, pFile);
        } else {
            result = fread(data_p5_1, 1, width * height, pFile);
        }
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
        output << "P5\n" << width << ' ' << height << '\n' << color_num << '\n';
        output.write((char *) res_p5, height * width);
        output.close();
        return DONE;
    }

    void trans(std::vector<std::complex<double>> &in) {
        if(in.size() <= 1){
            return;
        }
        std::vector<std::complex<double>> ev;
        std::vector<std::complex<double>> od;
        for (int i = 0; i < in.size(); i++) {
            if (i % 2 == 0) {
                ev.push_back(in[i]);
            } else {
                od.push_back(in[i]);
            }
        }
        trans(ev);
        trans(od);
        double thetaT = (2.0 * M_PI) / (1.0 * in.size());
        std::complex<double> po(1);
        for (int i = 0; i < in.size() / 2; i ++){
            in[i] = ev[i] + po * od[i];
            in[i + in.size() / 2] = ev[i] - po * od[i];
            po *= std::complex<double>(cos(thetaT), sin(thetaT));
        }

    }

    void fast_fourier(std::vector<std::vector<std::complex<double>>>& compl_ans, int idx) {
        int new_width = 1;
        int new_height = 1;
        while (new_width < width) {
            new_width *= 2;
        }
        while (new_height < height) {
            new_height *= 2;
        }
        for (int i = 0; i < new_height; i++) {
            std::vector<std::complex<double>> inter_ans;
            for (int j = 0; j < new_width; j++) {
                if (i < height && j < width) {
                    if (idx == 0) {
                        inter_ans.push_back(std::complex<double>(data_p5_0[i * width + j], 0));
                    } else {
                        inter_ans.push_back(std::complex<double>(data_p5_1[i * width + j], 0));
                    }
                } else {
                    inter_ans.push_back(0);
                }
            }
            trans(inter_ans);
            compl_ans.push_back(inter_ans);
        }
        for(int j = 0; j < new_width; j ++){
            std::vector<std::complex<double>> inter_ans;
            for(int i = 0; i < new_height; i ++) {
                inter_ans.push_back(compl_ans[i][j]);
            }
            trans(inter_ans);
            for(int i = 0; i < new_height; i ++){
                compl_ans[i][j] = inter_ans[i];
            }
        }
        for(int i = height; i < new_height; i ++){
            compl_ans.pop_back();
        }
        for(int i = 0; i < height; i ++){
            for(int j = width; j < new_width; j ++){
                compl_ans[i].pop_back();
            }
        }
    }

    void correlation(int type) {
        double mn_val = -1;
        switch (type) {
            case 0: {
                for (int mx = -(height / 2); mx < (height / 2 + height % 2); mx++) {
                    for (int my = (-width / 2); my < (width / 2 + width % 2); my++) {
                        double res = 0;
                        for (int i = 0; i < height; i++) {
                            for (int j = 0; j < width; j++) {
                                int i1 = (mx + i + height) % height;
                                int j1 = (my + j + width) % width;
                                res += (data_p5_0[i * width + j] * data_p5_1[i1 * width + j1] * 1.0) /
                                       (width * height * 1.0);
                            }
                        }
                        if (mn_val > res || mn_val == -1) {
                            mn_val = res;
                        }
                        inter_res_p5[(mx + (height / 2)) * width + (my + (width / 2))] = res;
                    }
                }
                break;
            }
            case 1: {
                std::vector<std::vector<std::complex<double>>> compl_f0;
                std::vector<std::vector<std::complex<double>>> compl_f1;
                std::vector<std::vector<std::complex<double>>> pre_calc_f;

                fast_fourier(compl_f0, 0);
                fast_fourier(compl_f1, 1);
                for (int i = 0; i < height; i++) {
                    std::vector<std::complex<double>> res_calc;
                    for (int j = 0; j < width; j++) {
                        std::complex<double> f0 = compl_f0[i][j] / std::abs(compl_f0[i][j]);
                        std::complex<double> f1 = conj(compl_f1[i][j]) / std::abs(conj(compl_f1[i][j]));
                        res_calc.push_back(f0 * f1);
                    }
                    pre_calc_f.push_back(res_calc);
                }
                for (int mx = -(height / 2); mx < (height / 2 + height % 2); mx++) {
                    for (int my = (-width / 2); my < (width / 2 + width % 2); my++) {
                        double res = 0;
                        for (int i = 0; i < height; i++) {
                            for (int j = 0; j < width; j++) {
                                double x = (i * mx * 1.0) / (height * 1.0);
                                double y = (j * my * 1.0) / (width * 1.0);
                                std::complex<double> ex = std::exp( (x + y) * (2 * M_PI) * std::complex<double>(0,1));
                                std::complex<double> inter_res  = pre_calc_f[i][j]  * ex;
                                res += (inter_res.real() * 1.0)/(width * height * 1.0) ;
                            }
                        }
                        if (mn_val > res || mn_val == -1) {
                            mn_val = res;
                        }
                        inter_res_p5[(mx + (height / 2)) * width + (my + (width / 2))] = res;
                    }
                }

                break;
            }
        }
        double mx_val = 0;
        int ans_pos_x;
        int ans_pos_y;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                inter_res_p5[i * width + j] -= mn_val;
                if (mx_val < inter_res_p5[i * width + j]) {
                    ans_pos_x = i;
                    ans_pos_y = j;
                    mx_val = inter_res_p5[i * width + j];
                }
            }
        }
        std::cout << "position of max result: h:" << ans_pos_x << "  w: " << ans_pos_y << '\n';
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                double val = ((255 * inter_res_p5[i * width + j]) / mx_val);
                res_p5[i * width + j] = (uint8_t) val;
            }
        }
    }

    bool ImageMalloc() {
        free(inter_res_p5);
        free(data_p5_0);
        free(data_p5_1);
        free(res_p5);
        data_p5_0 = (uint8_t *) (malloc(width * height));
        data_p5_1 = (uint8_t *) (malloc(width * height));
        res_p5 = (uint8_t *) (malloc(width * height));
        inter_res_p5 = (double *) (malloc(width * height * sizeof(double)));
        if (data_p5_0 == nullptr || data_p5_1 == nullptr) {
            return false;
        }
        if (res_p5 == nullptr || inter_res_p5 == nullptr) {
            return false;
        }
        return true;
    }

    ~Pgm() {
        free(inter_res_p5);
        free(data_p5_0);
        free(data_p5_1);
        free(res_p5);
    }

private:
    int width = 0;
    int height = 0;
    int color_num = 0;
    uint8_t *data_p5_0 = nullptr;
    uint8_t *data_p5_1 = nullptr;
    uint8_t *res_p5 = nullptr;
    double *inter_res_p5 = nullptr;

};


#endif //COMPUTER_GRAPHICS_LAB4_PGM_H
