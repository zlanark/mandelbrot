#include "mandelbrot.h"
#include "imgWriter.h"
#include "render.h"

RGBpixel color(int x, int y, matrix<double>& img){
    double iterations = img[y][x];
    double factor = std::max(1.0, iterations/256);
    double blue = sqrt(factor);
    return (iterations < 0 ? RGBpixel{255,255,255} : RGBpixel{u_int8_t(255*sqrt(pow(factor,1.5))),u_int8_t(255*factor),u_int8_t(255*blue)});
}

int main(int argc, char *argv[]) {
    int resX = 4096;
    int resY = 4096;
    unsigned int maxIterations = 256;
    double escapeRadius = 3.1;

    transform2d t(0, 0.001953125, 0.001953125, -1, 1);
    view v(resX, resY, t);

    mandelbrot m(v, maxIterations, escapeRadius);
    // complex<double> topLeft{-1.3,0.34};
    // complex<double> bottomRight{-1.1,0.14};
    // complex<double> topLeft {-2, ((double) 4/dimX)*(dimY/2)};
    // complex<double> bottomRight {2, -((double) 4/dimX)*(dimY/2)};
    // unsigned int maxIterations = 256;
    // double escapeRadius = 3.1;



    // matrix<double> mandelbrot_result = mandelbrot(dimX, dimY, topLeft, bottomRight, maxIterations, escapeRadius);
    matrix<double> mandelbrot_result = m.parallel_capture(resX);

    matrix<RGBpixel> image{resX, resY, RGBpixel{0,0,0}};
    {
        unsigned int numThreads = std::thread::hardware_concurrency();
        numThreads = (numThreads == 0 ? 1 : numThreads);
        
        std::vector<std::thread> threads;
        std::mutex batchCounterMutex;
        std::atomic<int> batchCounter = 0;
        int batchSize = resX;

        std::function<RGBpixel(int, int, matrix<double>&)> pixel_operation = color;
        for(int x=0; x < numThreads; x++){
            threads.push_back(std::thread(&pixel_worker<double, RGBpixel>, color, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(mandelbrot_result), std::ref(image)));
        }
        for(std::thread& t : threads) t.join();
    }
    

    // for(int y=0; y<resY; y++){
    //     for(int x=0; x<resX; x++){
    //         double iterations = mandelbrot_result[y][x];
    //         double factor = iterations/maxIterations; //(double)2/3
    //         // double blue = (double) 1 / double(1 + pow(( factor / double(1-factor)), -1) );
    //         double blue = sqrt(factor);
    //         image[y][x] = (iterations < 0 ? RGBpixel{255,255,255} : RGBpixel{u_int8_t(255*sqrt(pow(factor,1.5))),u_int8_t(255*factor),u_int8_t(255*blue)});
    //     }
    // }

    imgWriter writer{};
    writer.write("png", "./mandelbrot.png", image);
    return 0;
}