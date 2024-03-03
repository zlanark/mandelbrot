#ifndef IMGWRITER_H
#define IMGWRITER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "matrix.hpp"
#include <png++/png.hpp>

class imgWriter {
    public:
        imgWriter(){};
        bool write(std::string format, std::string directory, matrix<RGBpixel> image);
    private:
        bool writePPM(std::string directory, matrix<RGBpixel> image);
        bool writePGM(std::string directory, matrix<u_int8_t> image);
        bool writePBM(std::string directory, matrix<bool> image);
        bool writePNG(std::string directory, matrix<RGBpixel> image);
};

/*
Attemps to write a matrix of RGBpixels to `directory` in the specified image format.
Returns `true` on success and `false` on failure.
*/
bool imgWriter::write(std::string format, std::string directory, matrix<RGBpixel> image) {
    if(format == "ppm") {
        return this->writePPM(directory, image);
    }
    else if(format == "png") {
        return this->writePNG(directory, image);
    }
    else {
        //unrecognised format
        return false;
    }
}

bool imgWriter::writePNG(std::string directory, matrix<RGBpixel> image) {
    
    png::image<png::rgb_pixel> pngImage(image.width, image.height);

    for (png::uint_32 row = 0; row < pngImage.get_height(); ++row) {
        for (png::uint_32 col = 0; col < pngImage.get_width(); ++col) {

            RGBpixel * pixel = &image[row][col];
            pngImage[row][col] = png::rgb_pixel(pixel->R, pixel->G, pixel->B);
        }
    }

    try {
        pngImage.write(directory);
    }
    catch (png::std_error) {
        return false;
    }
    return true;
}

bool imgWriter::writePPM(std::string directory, matrix<RGBpixel> image) {
    std::ofstream output;
    output.open(directory);
    if(!output) {
        // error opening file
        return false;
    }

    // header
    output << "P3" << '\n';
    output << image.width << ' ' << image.height << '\n';
    output << 255 << '\n';

    // data
    std::vector<RGBpixel> data = image.serialize();
    for(RGBpixel p : data) {
        output << p << '\n';
    }

    output.close();
    return true;
}

#endif