#ifndef RTW_IMAGE_H
#define RTW_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <iostream>
#include "color.h"

class Rtw_image {
   private:
    const int byte_per_pixel = 3;
    int byte_per_scanline;
    unsigned char *data;
    int image_width;
    int image_height;

   private:
    void load(const char *image_filename) {
        int tmp_output = byte_per_pixel;
        data = stbi_load(image_filename, &image_width, &image_height, &tmp_output, byte_per_pixel);
        if (data == nullptr)
            std::exit(EXIT_FAILURE);
        byte_per_scanline = image_width * byte_per_pixel;
    }

   public:
    Rtw_image() : data(nullptr) {}
    Rtw_image(const char *image_filename) : data(nullptr) {
        load(image_filename);
        std::clog << "load the picture successfully!";
    }
    ~Rtw_image() {
        stbi_image_free(data);
    }
    int get_width() const {
        return data == nullptr ? 0 : image_width;
    }
    int get_height() const {
        return data == nullptr ? 0 : image_height;
    }
    color get_pixel_color(int x, int y) const {
        if (data == nullptr)
            return color(0.0, 0.0, 0.0);

        x = std::min(x, image_width);
        x = std::max(x, 0);
        y = std::min(y, image_height);
        y = std::max(y, 0);

        unsigned char *pixel_address = data + y * byte_per_scanline + x * byte_per_pixel;
        double R = int(pixel_address[0]) / 255.99;
        double G = int(pixel_address[1]) / 255.99;
        double B = int(pixel_address[2]) / 255.99;

        return color(R, G, B);
    }
};

#endif
