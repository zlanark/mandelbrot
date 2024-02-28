#include <complex>
#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <bits/stdc++.h>

#include "matrix.h"
#include "imgWriter.h"

using namespace std::complex_literals;
using std::complex;
using std::string;


/* Recursive version of getIterations() */
int f(unsigned int depth, const double& escape_radius, std::complex<double>& z, const std::complex<double>& c) {
    if (std::abs(z) > escape_radius){
        // Not in set
        return depth;
    }
    if(depth <= 0){
        // Might be in set
        return -1;
    }
    z = z*z + c;
    return f(depth-1, escape_radius, z, c);
}

/* Will return -1 if the pixel diverges, otherwise # iterations */
int getIterations(const unsigned int & max_depth, const double& escape_radius, const std::complex<double>& c){
    std::complex<double> z = 0;
    int iters_left = f(max_depth, escape_radius, z, c);
    return (iters_left < 0 ? -1 : max_depth - iters_left);
}

double getNormalisedIterations(const u_int & max_depth, const double& escape_radius, const std::complex<double>& c){
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

class view {
    public:
        view(int resX, int resY, complex<double> topLeft, complex<double> bottomRight, int maxIterations, double escapeRadius) 
            : resX(resX), resY(resY), BR(bottomRight), TL(topLeft), maxIterations(maxIterations), escapeRadius(escapeRadius), \
            xDiv(double(std::real(bottomRight) - std::real(topLeft)) / double(resX)), \
            yDiv(double(std::imag(topLeft) - std::imag(bottomRight)) / double(resY)), \
            totalPixels(resX*resY)
        {}

        matrix<double> capture();

        matrix<double> parallel_capture();
        matrix<double> parallel_capture(const int batchSize);

    private:
        void pixel_worker(std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<double>& img);

        const double xDiv;
        const double yDiv;
        const int resX;
        const int resY;
        const complex<double> TL;
        const complex<double> BR;
        const int maxIterations;
        const double escapeRadius;
        const int totalPixels;
};

matrix<double> view::capture() {
    matrix<double> img (resX, resY, 0);
    if(real(TL) >= real(BR) || imag(TL) <= imag(BR)) {
        // image has no area (negative dimensions not allowed). Return empty image.
        return img;
    }

    for(int y=0; y<resY; y++) {
        for(int x=0; x<resX; x++) {
            complex<double> c {(real(TL) + x*xDiv), (imag(TL) - y*yDiv)};
            img[y][x] = getNormalisedIterations(maxIterations, escapeRadius, c);
        }
    }
    return img;
}

matrix<double> view::parallel_capture() {
    unsigned int numThreads = std::thread::hardware_concurrency();
    numThreads = (numThreads == 0 ? 1 : numThreads);
    const int batchSize = (numThreads > 1 ? this->totalPixels / (4*numThreads) : totalPixels);

    return view::parallel_capture(batchSize);
}

matrix<double> view::parallel_capture(const int batchSize) {
    matrix<double> img (resX, resY, 0);
    if(real(TL) >= real(BR) || imag(TL) <= imag(BR)) {
        // image has no area (negative dimensions not allowed). Return empty image.
        return img;
    }

    unsigned int numThreads = std::thread::hardware_concurrency();
    numThreads = (numThreads == 0 ? 1 : numThreads);
    
    std::vector<std::thread> threads;
    std::mutex batchCounterMutex;
    std::atomic<int> batchCounter = 0;

    for(int x=0; x < numThreads; x++){
        threads.push_back(std::thread(&view::pixel_worker, this, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(img)));
    }
    for(std::thread& t : threads) t.join();
    return img;
}

void view::pixel_worker(std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<double>& img) {
    while(true) {
        // Get the next available batch number and increment
        std::unique_lock lock(batchCounterMutex);
        int batch = batchCounter;
        batchCounter++;
        lock.unlock();

        unsigned int numPixels = batchSize;
        // Would the last pixel in this batch be outside image bounds?
        if((batchCounter+1)*batchSize > this->totalPixels) {
            // By how much?
            unsigned int overshoot = (batchCounter+1)*batchSize - totalPixels;
            // If we haven't overshot by at least 1 batchSize, then there are still `batchSize - overshoot` pixels unrendered
            if(overshoot < batchSize) numPixels = batchSize - overshoot;
            else break;
        };
        
        unsigned int lowerBound = batch * batchSize;
        unsigned int upperBound = lowerBound + numPixels;
        // Render each pixel in batch
        for(unsigned int pixelCounter=lowerBound; pixelCounter < upperBound; pixelCounter++) {
            int x = pixelCounter % this->resX;
            int y = pixelCounter / this->resX;
            complex<double> c {(real(this->TL) + x*this->xDiv), (imag(this->TL) - y*this->yDiv)};
            img[y][x] = getNormalisedIterations(this->maxIterations, this->escapeRadius, c);
        }
    }
    std::this_thread::yield();
}

int main(int argc, char *argv[]) {
    int resX = 4096;
    int resY = 4096;
    complex<double> topLeft{-1.3,0.34};
    complex<double> bottomRight{-1.1,0.14};
    // complex<double> topLeft {-2, ((double) 4/dimX)*(dimY/2)};
    // complex<double> bottomRight {2, -((double) 4/dimX)*(dimY/2)};
    u_int maxIterations = 256;
    double escapeRadius = 3.1;

    // matrix<double> mandelbrot_result = mandelbrot(dimX, dimY, topLeft, bottomRight, maxIterations, escapeRadius);
    matrix<double> mandelbrot_result = view(resX, resY, topLeft, bottomRight, maxIterations, escapeRadius).parallel_capture(4096);
    matrix<RGBpixel> image{resX, resY, RGBpixel{0,0,0}};

    for(int y=0; y<resY; y++){
        for(int x=0; x<resX; x++){
            double iterations = mandelbrot_result[y][x];
            double factor = iterations/maxIterations; //(double)2/3
            // double blue = (double) 1 / double(1 + pow(( factor / double(1-factor)), -1) );
            double blue = sqrt(factor);
            image[y][x] = (iterations < 0 ? RGBpixel{255,255,255} : RGBpixel{u_int8_t(255*sqrt(pow(factor,1.5))),u_int8_t(255*factor),u_int8_t(255*blue)});
        }
    }

    imgWriter writer{};
    writer.write("png", "./mandelbrot.png", image);
    return 0;
}