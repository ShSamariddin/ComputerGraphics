#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "Pgm.h"

bool exception_case(int ex_case) {
    if (ex_case == OPEN_FILE) {
        std::cerr << "could not open file\n";
        return true;
    } else if (ex_case == FORMAT_EXCEPTION) {
        std::cerr << "format exception\n";
        return true;
    } else if (ex_case == MEMORY_ALLOCATION) {
        std::cerr << "failed to allocate memory\n";
        return true;
    }
    return false;
}

int main(int argc, char **argv) {

    if (argc < 9) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    const char *outFile;
    const char *inFile;
    inFile = argv[1];
    outFile = argv[2];
    int res_width = std::stoi(argv[3]);
    int res_height = std::stoi(argv[4]);
    double dx = std::stod(argv[5]);
    double dy = std::stod(argv[6]);
    double gamma = std::stod(argv[7]);
    ScaleType scal_type = (ScaleType) std::stoi(argv[8]);
    double BC_b = 0,BC_c = 0.5;
    if(scal_type == BC_SPLINE && argc >= 11) {
        BC_b = std::stod(argv[9]);
        BC_c = std::stod(argv[10]);
    }
    Pgm picture(res_height, res_width, BC_b, BC_c, dx, dy, gamma);


    if (exception_case(picture.Read(inFile))) {
        return 1;
    }
    picture.Scale(scal_type);


    if(exception_case(picture.writer(outFile))){
        return 1;
    }

}
