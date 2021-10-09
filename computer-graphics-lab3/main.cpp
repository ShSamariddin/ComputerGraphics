#include <iostream>
#include <cstring>
#include <cstdio>
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

    if (argc != 7) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    const char *outFile;
    const char *inFile;
    inFile = argv[1];
    outFile = argv[2];
    if (strlen(argv[3]) != 1 || strlen(argv[4]) != 1 || strlen(argv[5]) != 1) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    bool grad = (argv[3][0] == '1');
    if (argv[3][0] != '1' && argv[3][0] != '0') {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    DithType dith_type;
    if (argv[4][0] >= '0' && argv[4][0] <= '7') {
        dith_type = (DithType) (argv[4][0] - '0');
    } else {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    int bits_size;
    if (argv[5][0] >= '1' && argv[5][0] <= '8') {
        bits_size = argv[5][0] - '0';
    } else {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    double gama = std::stod(argv[6]);
    Pgm picture(gama, dith_type, bits_size, grad);


    if (exception_case(picture.Read(inFile))) {
        return 1;
    }

    picture.Dither();

    if(exception_case(picture.writer(outFile))){
        return 1;
    }

}
