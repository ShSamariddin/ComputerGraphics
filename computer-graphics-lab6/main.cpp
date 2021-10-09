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

    if (argc != 5) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    const char *outFile;
    const char *inFile1;
    const char *inFile2;
    inFile1 = argv[1];
    inFile2 = argv[2];
    outFile = argv[3];
    int class_type = std::stoi(argv[4]);
    if(class_type != 0 && class_type != 1){
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    Pgm picture;


    if (exception_case(picture.Read(inFile1,0 )) || exception_case((picture.Read(inFile2, 1)))) {
        return 1;
    }

    picture.correlation(class_type);
    if(exception_case(picture.writer(outFile))){
        return 1;
    }

}
