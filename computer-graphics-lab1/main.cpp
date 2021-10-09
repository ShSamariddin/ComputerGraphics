#include <iostream>
#include <string>
#include <cstring>

class Pixel {
public:
    char r;
    char g;
    char b;
};

class ImageData {
public:
    ImageData() = default;

    bool image_malloc() {
        free(data_p6);
        free(data_p5);
        if (image_type == '5') {
            data_p5 = (char *) (malloc(width * height));
            if (data_p5 == nullptr) {
                return false;
            }
        } else {
            data_p6 = (Pixel *) malloc(width * height * sizeof(Pixel));
            if (data_p6 == nullptr) {
                return false;
            }
        }
        return true;
    }

    ~ImageData() {
        free(data_p6);
        free(data_p5);
    }

    Pixel *data_p6;
    char *data_p5;
    char image_type;
    size_t width = 0;
    size_t height = 0;
};

ImageData imageData;


int read_pnm(const char *in) {
    FILE *pFile;
    char buf[12];
    size_t ex_case = 0;
    size_t result;

    pFile = fopen(in, "rb");
    if (pFile == nullptr) {
        return 1;
    }
    fseek(pFile, 0, SEEK_END);
    rewind(pFile);

    /*reading picture type  */
    result = fread(buf, 1, 3, pFile);
    if (result != 3 || buf[0] != 'P' || (buf[1] != '5' && buf[1] != '6') || buf[2] != '\n') {
        ex_case = 2;
        goto FEND;
    }
    imageData.image_type = buf[1];
    /*reading picture size*/
    while (true) {
        result = fread(buf, 1, 1, pFile);
        if (result != 1) {
            ex_case = 2;
            goto FEND;
        }
        if (buf[0] == '\n') {
            break;
        }
        if (isdigit(buf[0])) {
            imageData.height = imageData.height * 10 + (buf[0] - '0');
        } else if (buf[0] == ' ') {
            imageData.width = imageData.height;
            imageData.height = 0;
        } else {
            ex_case = 2;
            goto FEND;
        }
    }

    /*reading pixel size*/
    result = fread(buf, 1, 4, pFile);
    if (result != 4 || buf[0] != '2' || buf[1] != '5' || buf[2] != '5' || buf[3] != '\n') {
        ex_case = 2;
        goto FEND;
    }
    /*memory allocation*/
    if (!imageData.image_malloc()) {
        ex_case = 3;
        goto FEND;
    }
    /*reading data*/
    if (imageData.image_type == '6') {
        result = fread(imageData.data_p6, 1, 3 * imageData.width * imageData.height, pFile);
        if (result != 3 * imageData.width * imageData.height) {
            ex_case = 2;
            goto FEND;
        }
    } else {
        result = fread(imageData.data_p5, 1, imageData.width * imageData.height, pFile);
        if (result != imageData.width * imageData.height) {
            ex_case = 2;
            goto FEND;
        }
    }

    FEND:
    fclose(pFile);
    return ex_case;

}

int writer(const char *out) {
    FILE *outFile;
    char buf[12];
    std::string number_to_str;
    buf[0] = 'P';
    buf[1] = imageData.image_type;
    buf[2] = '\n';
    outFile = fopen(out, "wb");
    if (outFile == nullptr) {
        return 1;
    }
    fwrite(buf, 1, 3, outFile);
    number_to_str = std::to_string(imageData.width) + " " + std::to_string(imageData.height) + '\n';
    strcpy(buf, number_to_str.c_str());
    fwrite(buf, 1, number_to_str.size(), outFile);
    buf[0] = '2';
    buf[1] = '5';
    buf[2] = '5';
    buf[3] = '\n';
    fwrite(buf, 1, 4, outFile);
    if (imageData.image_type == '6') {
        fwrite(imageData.data_p6, 1, 3 * imageData.width * imageData.height, outFile);
    } else {
        fwrite(imageData.data_p5, 1, imageData.width * imageData.height, outFile);
    }
    fclose(outFile);
    return 0;
}

void my_swap(Pixel &x, Pixel &y) {
    std::swap(x.r, y.r);
    std::swap(x.g, y.g);
    std::swap(x.b, y.b);
}

void horizontal_mirror() {
    for (int i = 0; i < imageData.height; i++) {
        for (int j = 0; j < imageData.width / 2; j++) {
            if (imageData.image_type == '6') {
                my_swap(imageData.data_p6[i * imageData.width + j],
                        imageData.data_p6[i * imageData.width + (imageData.width - j - 1)]);
            } else {
                std::swap(imageData.data_p5[i * imageData.width + j],
                          imageData.data_p5[i * imageData.width + (imageData.width - j - 1)]);
            }
        }
    }
}

void vertical_mirror() {
    for (int i = 0; i < imageData.height / 2; i++) {
        for (int j = 0; j < imageData.width; j++) {
            if (imageData.image_type == '6') {
                my_swap(imageData.data_p6[i * imageData.width + j],
                        imageData.data_p6[(imageData.height - i - 1) * imageData.width + j]);
            } else {
                std::swap(imageData.data_p5[i * imageData.width + j],
                          imageData.data_p5[(imageData.height - i - 1) * imageData.width + j]);
            }
        }
    }
}

void invert() {
    for (int i = 0; i < imageData.height; i++) {
        for (int j = 0; j < imageData.width; j++) {
            if (imageData.image_type == '6') {
                imageData.data_p6[i * imageData.width + j].r = 255 - imageData.data_p6[i * imageData.width + j].r;
                imageData.data_p6[i * imageData.width + j].g = 255 - imageData.data_p6[i * imageData.width + j].g;
                imageData.data_p6[i * imageData.width + j].b = 255 - imageData.data_p6[i * imageData.width + j].b;
            } else {
                imageData.data_p5[i * imageData.width + j] = 255 - imageData.data_p5[i * imageData.width + j];
            }
        }
    }

}

void turn_right() {
    Pixel *new_data_p6;
    char *new_data_p5;
    if (imageData.image_type == '6') {
        new_data_p6 = (Pixel *) malloc(imageData.height * imageData.width * sizeof(Pixel));
    } else {
        new_data_p5 = (char *) malloc(imageData.height * imageData.width);
    }
    for (int i = 0; i < imageData.width; i++) {
        for (int j = 0; j < imageData.height; j++) {
            if (imageData.image_type == '6') {
                new_data_p6[i * imageData.height + j].r = imageData.data_p6[
                        (imageData.height - j - 1) * imageData.width +
                        i].r;
                new_data_p6[i * imageData.height + j].g = imageData.data_p6[
                        (imageData.height - j - 1) * imageData.width +
                        i].g;
                new_data_p6[i * imageData.height + j].b = imageData.data_p6[
                        (imageData.height - j - 1) * imageData.width +
                        i].b;
            } else {
                new_data_p5[i * imageData.height + j] = imageData.data_p5[
                        (imageData.height - j - 1) * imageData.width +
                        i];
            }
        }
    }
    std::swap(imageData.width, imageData.height);
    for (int i = 0; i < imageData.height; i++) {
        for (int j = 0; j < imageData.width; j++) {
            if (imageData.image_type == '6') {
                imageData.data_p6[i * imageData.width + j] = new_data_p6[i * imageData.width + j];
            } else {
                imageData.data_p5[i * imageData.width + j] = new_data_p5[i * imageData.width + j];
            }
        }
    }
    if (imageData.image_type == '6') {
        free(new_data_p6);
    } else {
        free(new_data_p5);
    }
}

void turn_left() {
    Pixel *new_data_p6;
    char *new_data_p5;
    if (imageData.image_type == '6') {
        new_data_p6 = (Pixel *) malloc(imageData.height * imageData.width * sizeof(Pixel));
    } else {
        new_data_p5 = (char *) malloc(imageData.height * imageData.width);
    }
    for (int i = 0; i < imageData.width; i++) {
        for (int j = 0; j < imageData.height; j++) {
            if (imageData.image_type == '6') {
                new_data_p6[i * imageData.height + j].r = imageData.data_p6[j * imageData.width +
                                                                            (imageData.width - i - 1)].r;
                new_data_p6[i * imageData.height + j].g = imageData.data_p6[j * imageData.width +
                                                                            (imageData.width - i - 1)].g;
                new_data_p6[i * imageData.height + j].b = imageData.data_p6[j * imageData.width +
                                                                            (imageData.width - i - 1)].b;
            } else {
                new_data_p5[i * imageData.height + j] = imageData.data_p5[j * imageData.width +
                                                                          (imageData.width - i - 1)];
            }
        }
    }
    std::swap(imageData.width, imageData.height);
    for (int i = 0; i < imageData.height; i++) {
        for (int j = 0; j < imageData.width; j++) {
            if (imageData.image_type == '6') {
                imageData.data_p6[i * imageData.width + j] = new_data_p6[i * imageData.width + j];
            } else {
                imageData.data_p5[i * imageData.width + j] = new_data_p5[i * imageData.width + j];
            }
        }
    }
    if (imageData.image_type == '6') {
        free(new_data_p6);
    } else {
        free(new_data_p5);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "arguments exception";
        return 1;
    }
    int ex_case = read_pnm(argv[1]);
    if (ex_case == 1) {
        std::cerr << "could not open file\n";
        return 1;
    } else if (ex_case == 2) {
        std::cerr << "format exception\n";
        return 1;
    } else if (ex_case == 3) {
        std::cerr << "failed to allocate memory\n";
        return 1;
    }
    switch (argv[3][0]) {
        case '0':
            invert();
            break;
        case '1':
            horizontal_mirror();
            break;
        case '2':
            vertical_mirror();
            break;
        case '3':
            turn_right();
            break;
        case '4':
            turn_left();
            break;
        default:
            std::cerr << "arguments exception";
            return 1;
    }
    ex_case = writer(argv[2]);
    if (ex_case == 1) {
        std::cerr << "could not open file\n";
        return 1;
    }
}