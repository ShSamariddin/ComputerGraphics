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


enum ErrorType {
    DONE = 0,
    OPEN_FILE = 1,
    FORMAT_EXCEPTION = 2,
    MEMORY_ALLOCATION = 3
};


class Pgm {
public:


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
        output << "P5\n" << width  << '\n' << height << '\n' << color_num << '\n';
        output.write((char *) res_p5, height * width);
        output.close();
        return DONE;
    }
    void calc_histogram(){
        for(int i = 0; i < height; i ++){
            for(int j = 0; j < width; j ++){
                his[data_p5[i * width + j]]++;
            }
        }
    }
    void calc_prob(){
        auto  total_num = static_cast<double> (width * height);
        for(int i = 0; i <= 255; i ++){
            prob[i] = static_cast<double> (his[i]) / total_num;
        }
    }
    long double calc_for_seg(int l, int r){
        long double q = 0;
        for(int i = l; i <= r; i ++){
            q += prob[i];
        }
        long double m = 0;
        for(int f = l; f <= r; f ++){
            m += (f * prob[f]) / q;
        }
        return m * m * q;
    }
    void check_case(const std::vector<int>& border){
        long double ans = 0;
        for(int i =0 ; i < border.size() - 1; i ++) {
            ans += calc_for_seg(border[i], border[i + 1] - 1);
        }
        if(ans > mx){
            mx = ans;
            ans_bord.clear();
            for(int val:border){
                ans_bord.push_back(val);
            }
        }

    }
    void change_seg_pixel(int l ,int r){
        uint8_t val = (r + l) / 2;
        for(int i = l; i <= r; i ++){
            upg_pixel[i] = val;
        }
    }
    void change_picture_pixel(){
        for(int i = 0; i < height; i ++){
            for(int j = 0; j < width; j ++){
                res_p5[i * width + j] = upg_pixel[data_p5[i * width + j]];
            }
        }
    }
    void find_bord(int start_pos, int left_bor, std::vector<int> border){
        if(start_pos == 256 && left_bor != 0){
            return ;
        }
        if(left_bor == 0){
            border.push_back(256);
            check_case(border);
            border.pop_back();
            return ;
        }
        for(int i = start_pos; i <= 255; i ++){
            border.push_back(i);
            find_bord(i + 1, left_bor - 1, border);
            border.pop_back();
        }
    }


    void otsu(int class_type){
        calc_histogram();
        calc_prob();
        std::vector<int> v;
        v.push_back(0);
        find_bord(1, class_type - 1, v);
        for(int i = 0; i < ans_bord.size() - 1; i ++){
            change_seg_pixel(ans_bord[i], ans_bord[i + 1] - 1);
        }
        change_picture_pixel();
      /*  std::cout << mx<<'\n';
        for(int val:ans_bord){
            std::cout << val<<' ';
        }
        std::cout << '\n';*/
    }
    bool ImageMalloc() {
        free(data_p5);
        free(res_p5);
        data_p5 = (uint8_t *) (malloc(width * height));
        res_p5 = (uint8_t *) (malloc(width * height));

        if (data_p5 == nullptr) {
            return false;
        }
        if (res_p5 == nullptr) {
            return false;
        }
        return true;
    }

    ~Pgm() {
        free(data_p5);
        free(res_p5);
    }

private:
    int width = 0;
    int height = 0;
    int color_num = 0;
    uint8_t *data_p5 = nullptr;
    uint8_t *res_p5 = nullptr;
    int his[300]{0};
    double prob[300];
    uint8_t upg_pixel[300];
    long double mx = -1.0;
    std::vector<int> ans_bord;

};


#endif //COMPUTER_GRAPHICS_LAB4_PGM_H
