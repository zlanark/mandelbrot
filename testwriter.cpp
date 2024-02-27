#include "imgWriter.h"
#include "matrix.h"
#include <iostream>


/* */
int main() {
    RGBpixel pixel(0,0,0);
    matrix<RGBpixel> testimg(1920, 1080, pixel);

    std::cout << "width: " << testimg.width << ", height: " << testimg.height << std::endl;

    int checker_size = 20;

    int NUM_COLORS = 4;

    for(int row=0; row<testimg.height; ++row){
        for(int col=0; col<testimg.width; ++col){
            int horizontal = (col/checker_size)%NUM_COLORS;
            int vertical = (row/checker_size)%NUM_COLORS;

            RGBpixel * pix = &testimg[row][col];
            int value = (horizontal + (NUM_COLORS - vertical)) % NUM_COLORS;

            u_int8_t colors[12] =  {255,0,0, \
                                    0,255,0, \
                                    0,0,255, \
                                    0,0,0};
            pix->R = colors[3*value] * float(row)/testimg.height;
            pix->G = colors[3*value + 1] * float(testimg.height-row)/testimg.height;
            pix->B = colors[3*value + 2] * float(col)/testimg.width;
        }
    }

    imgWriter writer{}; 
    writer.write("png", "./testpng.png", testimg);
    return 0;
}