#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "Png.h"

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
    } else if (ex_case == ZLIB_FAILED){
        std::cerr<<"zlib failed\n";
    }
    return false;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    const char *outFile;
    const char *inFile;
    inFile = argv[1];
    outFile = argv[2];
    Pgm picture;


    if (exception_case(picture.Read(inFile))) {
        return 1;
    }
    picture.filter();
//    picture.otsu(class_type);
    if(exception_case(picture.writer(outFile))){
        return 1;
    }

}
