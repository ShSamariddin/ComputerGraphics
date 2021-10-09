#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

class Pixel {
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class ImageData {
public:
    ImageData() = default;

    void CMY_to_RGB() {
        for (int i = 0; i < height * width; i++) {
            data_p6[i].r = 255 - data_p6[i].r;
            data_p6[i].g = 255 - data_p6[i].g;
            data_p6[i].b = 255 - data_p6[i].b;
        }
    }

    void RGB_to_CMY() {
        for (int i = 0; i < height * width; i++) {
            data_p6[i].r = 255 - data_p6[i].r;
            data_p6[i].g = 255 - data_p6[i].g;
            data_p6[i].b = 255 - data_p6[i].b;
        }
    }

    void YCbCr_601_to_RGB() {
        for (int i = 0; i < height * width; i++) {
            double Y1 = data_p6[i].r;
            double Cb = data_p6[i].g;
            double Cr = data_p6[i].b;
            int r = Y1 + 1.402 * (Cr - 255.0 / 2.0);
            int g = Y1 - 1.772 * (0.114 / 0.587) * (Cb - 255.0 / 2.0) -
                     1.402 * (0.299 / 0.587) * (Cr - 255.0 / 2.0);
            int b = Y1 + 1.772 * (Cb - 255.0 / 2.0);
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
//            data_p6[i].r = (uint8_t) ((298.082 / 256) * Y1 + (408.583 / 256) * Cr - 222.921);
//            data_p6[i].g = (uint8_t) ((298.082 * Y1) / 256 - (100.291 * Cb) / 256 - (208.120 * Cr) / 256 + 135.576);
//            data_p6[i].b = (uint8_t) ((298.082 * Y1) / 256 + (516.412 * Cb) / 256 - 276.836);
        }
    }

    void RGB_to_YCbCr_601() {
        for (int i = 0; i < height * width; i++) {

            double r1 = data_p6[i].r / 255.0;
            double g1 = data_p6[i].g / 255.0;
            double b1 = data_p6[i].b / 255.0;
            int r = 255 * (0.299 * r1 + 0.587 * g1 + 0.114 * b1);
            int g = 255.0 / 2.0 + 255 * (-0.1687 * r1 - 0.33126 * g1 + 0.5 * b1);
            int b = 255.0 / 2.0 + 255 * (0.5 * r1 - 0.4186 * g1 - 0.0813 * b1);
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
//            data_p6[i].r = (16 + (65.481 * r1 + 128.553 * g1 + 24.966 * b1));
//            data_p6[i].g = (128 + (-37.797 * r1 - 74.203 * g1 + 112.0 * b1));
//            data_p6[i].b =  (128 + (112.0 * r1 - 93.786 * g1 - 18.214 * b1));
        }
    }

    void YCbCr_709_to_RGB() {
        for (int i = 0; i < height * width; i++) {
            double Y1 = data_p6[i].r;
            double Cb = data_p6[i].g;
            double Cr = data_p6[i].b;
            int r = Y1 + 1.5748 * (Cr - 255.0 / 2.0);
            int g = Y1 - 0.1873 * (Cb - 255.0 / 2.0) - 0.4681 * (Cr - 255.0 / 2.0);
            int b = Y1 + 1.8556 * (Cb - 255.0 / 2.0);
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
        }
    }

    void RGB_to_YCbCr_709() {
        for (int i = 0; i < height * width; i++) {

            double r1 = data_p6[i].r / 255.0;
            double g1 = data_p6[i].g / 255.0;
            double b1 = data_p6[i].b / 255.0;
            int r = 255 * (0.2126 * r1 + 0.7152 * g1 + 0.0722 * b1);
            int g = 255.0 / 2.0 + 255 * (-0.1146 * r1 - 0.3853 * g1 + 0.5 * b1);
            int b = 255.0 / 2.0 + 255 * (0.5 * r1 - 0.4542 * g1 - 0.0458 * b1);
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
        }
    }

    void RGB_to_YCoCg() {
        for (int i = 0; i < height * width; i++) {
            double r1 = data_p6[i].r;
            double g1 = data_p6[i].g;
            double b1 = data_p6[i].b;
            int r = r1 / 4.0 + g1 / 2.0 + b1 / 4.0;
            int g = ((255.0 / 2.0) + r1 / 2.0 - b1 / 2.0);
            int b = ((255.0 / 2.0) - r1 / 4.0 + g1 / 2.0 - b1 / 4.0);
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
        }
    }

    void YCoCg_to_RGB() {
        for (int i = 0; i < height * width; i++) {
            double Y1 = data_p6[i].r;
            double Co = data_p6[i].g;
            double Cg = data_p6[i].b;
            Co -= (255.0 / 2.0);
            Cg -= (255.0 / 2.0);
            int r = Y1 + Co - Cg;
            int g = Y1 + Cg;
            int b = Y1 - Co - Cg;
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            data_p6[i].r = r;
            data_p6[i].g = g;
            data_p6[i].b = b;
        }
    }

    void RGB_to_HSL() {
        for (int i = 0; i < height * width; i++) {
            double r = data_p6[i].r, g = data_p6[i].g, b = data_p6[i].b;
            double rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
            double rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);
            rgbMax /= 255.0;
            rgbMin /= 255.0;
            r /= 255.0, g /= 255.0, b /= 255.0;
            double h = 0, s = 0, l = (rgbMax + rgbMin) / 2.0;
            double c = rgbMax - rgbMin;
            if (rgbMax == rgbMin) {
                h  = 0;
            } else if(rgbMax == r) {
                h = ((g - b) / c);
                if (h < 0.0){
                    h += 6.0;
                }
            } else if(rgbMax == g){
                h = (2 + ((b - r) / c));
            } else if(rgbMax == b){
                h = (4 + ((r - g) / c));
            }
            h /= 6;
            if(l == 0.0 || l == 1.0){
                s = 0;
            } else {
                s = (rgbMax - l) / (std::min(l, 1-l));
            }
            int r1 = (h * 255);
            int g1 = (s * 255);
            int b1 = (l * 255);
            r1 = std::max(0, std::min(255, r1));
            g1 = std::max(0, std::min(255, g1));
            b1 = std::max(0, std::min(255, b1));
            data_p6[i].r = r1;
            data_p6[i].g = g1;
            data_p6[i].b = b1;
        }
    }

    void HSL_to_RGB() {
        for (int i = 0; i < height * width; i++) {
            double r = data_p6[i].r, g = data_p6[i].g, b = data_p6[i].b;
            double s = (g / 255.0);
            double l = (b / 255.0);
            double c = (1.0 - std::abs(2.0 * l - 1.0)) * s;
            double h = (r / 255.0) * 6.0;
            double x = c * (1.0 - std::abs((std::fmod(h,2.0) - 1.0)));
            r = g = b = 0;
            if (h >= 0 && h < 1) {
                r = c, g = x, b = 0;
            } else if (h >= 1 && h < 2) {
                r = x, g = c, b = 0;
            } else if (h >= 2 && h < 3) {
                r = 0, g = c, b = x;
            } else if (h >= 3 && h < 4) {
                r = 0, g = x, b = c;
            } else if (h >= 4 && h < 5) {
                r = x, g = 0, b = c;
            } else if (h >= 5 && h < 6) {
                r = c, g = 0, b = x;
            }
            double m = l - c / 2;
            r = (r + m) * 255.0;
            g = (g + m) * 255.0;
            b = (b + m) * 255.0;
            int r1 = std::max(0.0, std::min(255.0, r));
            int g1 = std::max(0.0, std::min(255.0, g));
            int b1 = std::max(0.0, std::min(255.0, b));
            data_p6[i].r = r1;
            data_p6[i].b = b1;
            data_p6[i].g = g1;
        }
    }

    void RGB_to_HSV() {
        for (int i = 0; i < width * height; i++) {
            int r = data_p6[i].r, g = data_p6[i].g, b = data_p6[i].b;
            int rgbMin, rgbMax;

            rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
            rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);
            int h, s, v;
            v = rgbMax;
            if (v == 0) {
                h = 0;
                s = 0;
            } else {
                s = 255 * long(rgbMax - rgbMin) / v;
                if (s == 0) {
                    h = 0;
                } else {
                    if (rgbMax == r)
                        h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
                    else if (rgbMax == g)
                        h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
                    else
                        h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

                }
            }
            h = std::max(0, std::min(255, h));
            s = std::max(0, std::min(255, s));
            v = std::max(0, std::min(255, v));
            data_p6[i].r = h;
            data_p6[i].g = s;
            data_p6[i].b = v;
        }
    }

    void HSV_to_RGB() {
        for (int i = 0; i < width * height; i++) {
            int h = data_p6[i].r, s = data_p6[i].g, v = data_p6[i].b;
            int r, g, b;
            int region, remainder, p, q, t;
            if (s == 0) {
                r = v;
                g = v;
                b = v;
            } else {

                region = h / 43;
                remainder = (h - (region * 43)) * 6;

                p = (v * (255 - s)) >> 8;
                q = (v * (255 - ((s * remainder) >> 8))) >> 8;
                t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

                switch (region) {
                    case 0:
                        r = v;
                        g = t;
                        b = p;
                        break;
                    case 1:
                        r = q;
                        g = v;
                        b = p;
                        break;
                    case 2:
                        r = p;
                        g = v;
                        b = t;
                        break;
                    case 3:
                        r = p;
                        g = q;
                        b = v;
                        break;
                    case 4:
                        r = t;
                        g = p;
                        b = v;
                        break;
                    default:
                        r = v;
                        g = p;
                        b = q;
                        break;
                }
            }
            data_p6[i].r = std::max(0, std::min(255, r));
            data_p6[i].g = std::max(0, std::min(255, g));
            data_p6[i].b = std::max(0, std::min(255, b));
        }
    }

    bool image_malloc() {
        free(data_p6);
        free(data_p5);
        data_p5 = (uint8_t *) (malloc(width * height));
        if (data_p5 == nullptr) {
            return false;
        }
        data_p6 = (Pixel *) malloc(width * height * sizeof(Pixel));
        if (data_p6 == nullptr) {
            return false;
        }
        return true;
    }

    ~ImageData() {
        free(data_p6);
        free(data_p5);
    }

    Pixel *data_p6;
    uint8_t *data_p5;
    char image_type;
    size_t width = 0;
    size_t height = 0;
    size_t space_id = 0;
};

std::string init_color;
std::string final_color;
char *cnt_in_file;
char *cnt_out_file;
char *input_file;
char *output_file;
ImageData imageData;


int read_image(const char *in, char exp_type) {
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
    if (result != 3 || buf[0] != 'P' || buf[1] != exp_type || buf[2] != '\n') {
        ex_case = 2;
        goto FEND;
    }
    imageData.image_type = buf[1];
    imageData.width = 0;
    imageData.height = 0;
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
    if (imageData.space_id == 0) {
        if (!imageData.image_malloc()) {
            ex_case = 3;
            goto FEND;
        }
    }
    /*reading data*/
    if (imageData.image_type == '6') {

        result = fread(imageData.data_p6, 1, 3 * imageData.width * imageData.height, pFile);
        if (result != 3 * imageData.width * imageData.height) {
            ex_case = 2;
            goto FEND;
        }

//        std::cout << "RGBa:"<<int(imageData.data_p6[0].r)<<' '<<int(imageData.data_p6[0].g)<<' '<<int(imageData.data_p6[0].b)<<'\n';
    } else {
        result = fread(imageData.data_p5, 1, imageData.width * imageData.height, pFile);
        if (result != imageData.width * imageData.height) {
            ex_case = 2;
            goto FEND;
        }
        for (size_t i = 0; i < imageData.width * imageData.height; i++) {
            if (imageData.space_id == 0) {
                imageData.data_p6[i].r = imageData.data_p5[i];
            } else if (imageData.space_id == 1) {
                imageData.data_p6[i].g = imageData.data_p5[i];
            } else if (imageData.space_id == 2) {
                imageData.data_p6[i].b = imageData.data_p5[i];
            }
        }
//        std::cout << "CYM: "<<(int)imageData.data_p6[0].r<<' '<<(int)imageData.data_p6[0].g<<' '<<(int)imageData.data_p6[0].b<<'\n';
        imageData.space_id++;
    }

    FEND:
    fclose(pFile);
    return ex_case;

}

int writer(const char *out, char exp_type) {
    FILE *outFile;
    char buf[12];
    std::string number_to_str;
    buf[0] = 'P';
    buf[1] = exp_type;
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
    if (exp_type == '6') {
        fwrite(imageData.data_p6, 1, 3 * imageData.width * imageData.height, outFile);
    } else {
        for (size_t i = 0; i < imageData.width * imageData.height; i++) {
            if (imageData.space_id == 0) {
                imageData.data_p5[i] = imageData.data_p6[i].r;
            } else if (imageData.space_id == 1) {
                imageData.data_p5[i] = imageData.data_p6[i].g;
            } else if (imageData.space_id == 2) {
                imageData.data_p5[i] = imageData.data_p6[i].b;
            }
        }
        imageData.space_id++;
        fwrite(imageData.data_p5, 1, imageData.width * imageData.height, outFile);
    }
    fclose(outFile);
    return 0;
}


bool exception_case(int ex_case) {
    if (ex_case == 1) {
        std::cerr << "could not open file\n";
        return true;
    } else if (ex_case == 2) {
        std::cerr << "format exception\n";
        return true;
    } else if (ex_case == 3) {
        std::cerr << "failed to allocate memory\n";
        return true;
    }
    return false;
}

int main(int argc, char **argv) {
    bool arg_exc = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f")) {
            if (i == argc - 1) {
                arg_exc = true;
                break;
            } else {
                init_color = argv[i + 1];
                i++;
            }
        } else if (!strcmp(argv[i], "-t")) {
            if (i == argc - 1) {
                arg_exc = true;
                break;
            } else {
                final_color = argv[i + 1];
                i++;
            }
        } else if (!strcmp(argv[i], "-i")) {
            if (i > argc - 3) {
                arg_exc = true;
                break;
            } else {
                cnt_in_file = argv[i + 1];
                input_file = argv[i + 2];
                i += 2;
            }
        } else if (!strcmp(argv[i], "-o")) {
            if (i > argc - 3) {
                arg_exc = true;
                break;
            } else {
                cnt_out_file = argv[i + 1];
                output_file = argv[i + 2];
                i += 2;
            }
        }
    }
    if (arg_exc) {
        std::cerr << "arguments exception";
        return 1;
    }

    if (!strcmp(cnt_in_file, "1")) {
        if (exception_case(read_image(input_file, '6'))) {
            return 1;
        }
    } else if (!strcmp(cnt_in_file, "3")) {
        std::string in_file_name = input_file;
        std::string base_name;
        std::string in_file_type;
        bool dot_flag = false;
        for (int i = in_file_name.size() - 1; i >= 0; i--) {
            if (dot_flag) {
                base_name = in_file_name[i] + base_name;
            } else {
                in_file_type = in_file_name[i] + in_file_type;
            }
            if (in_file_name[i] == '.') {
                dot_flag = true;
            }
        }
        if (exception_case(read_image((base_name + "_1" + in_file_type).c_str(), '5'))
            || exception_case(read_image((base_name + "_2" + in_file_type).c_str(), '5'))
            || exception_case(read_image((base_name + "_3" + in_file_type).c_str(), '5'))) {
            return 1;
        }
    } else {
        std::cerr << "arguments exception";
        return 1;
    }
    imageData.space_id = 0;
    if (init_color == "CMY") {
        imageData.CMY_to_RGB();
    } else if (init_color == "YCbCr.601") {
        imageData.YCbCr_601_to_RGB();
    } else if (init_color == "YCbCr.709") {
        imageData.YCbCr_709_to_RGB();
    } else if (init_color == "YCoCg") {
        imageData.YCoCg_to_RGB();
    } else if (init_color == "HSL") {
        imageData.HSL_to_RGB();
    } else if (init_color == "HSV") {
        imageData.HSV_to_RGB();
    } else if (init_color != "RGB") {
        std::cerr << "arguments exception";
        return 1;
    }

    if (final_color == "CMY") {
        imageData.RGB_to_CMY();
    } else if (final_color == "YCbCr.601") {
        imageData.RGB_to_YCbCr_601();
    } else if (final_color == "YCbCr.709") {
        imageData.RGB_to_YCbCr_709();
    } else if (final_color == "YCoCg") {
        imageData.RGB_to_YCoCg();
    } else if (final_color == "HSL") {
        imageData.RGB_to_HSL();
    } else if (final_color == "HSV") {
        imageData.RGB_to_HSV();
    } else if (final_color != "RGB") {
        std::cerr << "arguments exception";
        return 1;
    }
    if (!strcmp(cnt_out_file, "1")) {
        if (exception_case(writer(output_file, '6'))) {
            return 1;
        }
    } else if (!strcmp(cnt_out_file, "3")) {
        std::string in_file_name = output_file;
        std::string base_name;
        std::string in_file_type;
        bool dot_flag = false;
        for (int i = in_file_name.size() - 1; i >= 0; i--) {
            if (dot_flag) {
                base_name = in_file_name[i] + base_name;
            } else {
                in_file_type = in_file_name[i] + in_file_type;
            }
            if (in_file_name[i] == '.') {
                dot_flag = true;
            }
        }

        if (exception_case(writer((base_name + "_1" + in_file_type).c_str(), '5'))
            || exception_case(writer((base_name + "_2" + in_file_type).c_str(), '5'))
            || exception_case(writer((base_name + "_3" + in_file_type).c_str(), '5'))) {
            return 1;
        }
    } else {
        std::cerr << "arguments exception";
        return 1;
    }
}
