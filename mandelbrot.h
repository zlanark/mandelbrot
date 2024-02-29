#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <complex>
#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <future>

#include "view.h"
#include "matrix.h"
#include "render.h"

using namespace std::complex_literals;
using std::complex;
using std::string;

class mandelbrot {
    public:
        mandelbrot(view camera, unsigned int maxIterations, double escapeRadius) 
            : camera(camera), maxIterations(maxIterations), escapeRadius(escapeRadius)
        {}

        matrix<double> capture();
        matrix<double> parallel_capture();
        matrix<double> parallel_capture(const int batchSize);

    private:
        // void pixel_worker(std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<double>& img);
        const double render_pixel(int x, int y) {
            std::array<double, 2> c = camera.pixel2world(x,y);
            return getNormalisedIterations(this->maxIterations, this->escapeRadius, complex<double>{c[0],c[1]});
        }

        double getNormalisedIterations(const unsigned int& max_depth, const double& escape_radius, const std::complex<double>& c);
        unsigned int getIterations(const unsigned int & max_depth, const double& escape_radius, const std::complex<double>& c);
        unsigned int getIterationsRecursive(unsigned int depth, const double& escape_radius, std::complex<double>& z, const std::complex<double>& c);

        view camera;
        const int maxIterations;
        const double escapeRadius;
};

/* Recursive version of getIterations() */
unsigned int mandelbrot::getIterationsRecursive(unsigned int depth, const double& escape_radius, std::complex<double>& z, const std::complex<double>& c) {
    if (std::abs(z) > escape_radius){
        // Not in set
        return depth;
    }
    if(depth <= 0){
        // Might be in set
        return -1;
    }
    z = z*z + c;
    return getIterationsRecursive(depth-1, escape_radius, z, c);
}

/* Will return -1 if the pixel diverges, otherwise # iterations */
unsigned int mandelbrot::getIterations(const unsigned int & max_depth, const double& escape_radius, const std::complex<double>& c){
    std::complex<double> z = 0;
    int iters_left = getIterationsRecursive(max_depth, escape_radius, z, c);
    return (iters_left < 0 ? -1 : max_depth - iters_left);
}

double mandelbrot::getNormalisedIterations(const unsigned int& max_depth, const double& escape_radius, const std::complex<double>& c) {
    std::complex<double> z = 0;
    u_int iterations = 0;
    while(abs(z) <= escape_radius && iterations < max_depth) {
        z = z*z + c;
        iterations += 1;
    }
    if(iterations == max_depth){
        return -1;
    }
    return iterations - log(log2(abs(z)));
}

matrix<double> mandelbrot::capture() {
    matrix<double> img (camera.resX, camera.resY, 0);
    // if(real(TL) >= real(BR) || imag(TL) <= imag(BR)) {
    //     // image has no area (negative dimensions not allowed). Return empty image.
    //     return img;
    // }

    for(int y=0; y<camera.resY; y++) {
        for(int x=0; x<camera.resX; x++) {
            std::array<double, 2> c = camera.pixel2world(x,y);
            img[y][x] = getNormalisedIterations(maxIterations, escapeRadius, complex<double>{c[0],c[1]});
        }
    }
    return img;
}

matrix<double> mandelbrot::parallel_capture() {
    unsigned int numThreads = std::thread::hardware_concurrency();
    numThreads = (numThreads == 0 ? 1 : numThreads);
    const int batchSize = (numThreads > 1 ? camera.totalPixels / (4*numThreads) : camera.totalPixels);

    return mandelbrot::parallel_capture(batchSize);
}

matrix<double> mandelbrot::parallel_capture(const int batchSize) {
    matrix<double> img (camera.resX, camera.resY, 0);
    // if(real(TL) >= real(BR) || imag(TL) <= imag(BR)) {
    //     // image has no area (negative dimensions not allowed). Return empty image.
    //     return img;
    // }

    unsigned int numThreads = std::thread::hardware_concurrency();
    numThreads = (numThreads == 0 ? 1 : numThreads);
    
    std::vector<std::thread> threads;
    std::mutex batchCounterMutex;
    std::atomic<int> batchCounter = 0;

    std::function<double(int, int)> pixel_operation = std::bind(&mandelbrot::render_pixel, this, std::placeholders::_1, std::placeholders::_2);
    for(int x=0; x < numThreads; x++){
        threads.push_back(std::thread(&pixel_worker<double>, pixel_operation, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(img)));
    }
    for(std::thread& t : threads) t.join();
    return img;
}

// template<typename I, typename O>
// matrix<O> parallel_render(const int batchSize, const int numThreads, std::function<I(int, int)> pixel_operation, matrix<I>& inputImg, matrix<O>& outputImg) {
//     matrix<T> img (camera.resX, camera.resY, 0);

//     unsigned int numThreads = std::thread::hardware_concurrency();
//     numThreads = (numThreads == 0 ? 1 : numThreads);
    
//     std::vector<std::thread> threads;
//     std::mutex batchCounterMutex;
//     std::atomic<int> batchCounter = 0;

//     std::function<double(int, int)> pixel_operation = std::bind(&mandelbrot::render_pixel, this, std::placeholders::_1, std::placeholders::_2);
//     for(int x=0; x < numThreads; x++){
//         threads.push_back(std::thread(&pixel_worker<double, double>, pixel_operation, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(img), std::ref(img)));
//     }
//     for(std::thread& t : threads) t.join();
//     return img;
// }

// void mandelbrot::pixel_worker(std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<double>& img) {
//     while(true) {
//         // Get the next available batch number and increment
//         std::unique_lock lock(batchCounterMutex);
//         int batch = batchCounter;
//         batchCounter++;
//         lock.unlock();

//         unsigned int numPixels = batchSize;
//         // Would the last pixel in this batch be outside image bounds?
//         if((batchCounter+1)*batchSize > camera.totalPixels) {
//             // By how much?
//             unsigned int overshoot = (batchCounter+1)*batchSize - camera.totalPixels;
//             // If we haven't overshot by at least 1 batchSize, then there are still `batchSize - overshoot` pixels unrendered
//             if(overshoot < batchSize) numPixels = batchSize - overshoot;
//             else break;
//         };
        
//         unsigned int lowerBound = batch * batchSize;
//         unsigned int upperBound = lowerBound + numPixels;
//         // Render each pixel in batch
//         for(unsigned int pixelCounter=lowerBound; pixelCounter < upperBound; pixelCounter++) {
//             int x = pixelCounter % camera.resX;
//             int y = pixelCounter / camera.resX;
//             std::array<double, 2> c = camera.pixel2world(x,y);
//             img[y][x] = getNormalisedIterations(this->maxIterations, this->escapeRadius, complex<double>{c[0],c[1]});
//         }
//     }
//     std::this_thread::yield();
// }
#endif