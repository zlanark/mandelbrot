#include <complex>
#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <future>

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
        view(int resX, int resY, double rotation, complex<double> topLeft, complex<double> bottomRight, int maxIterations, double escapeRadius) 
            : resX(resX), resY(resY), rotation(std::fmod(rotation, 360)), BR(bottomRight), TL(topLeft), maxIterations(maxIterations), escapeRadius(escapeRadius), \
            xDiv(double(std::real(bottomRight) - std::real(topLeft)) / double(resX)), \
            yDiv(double(std::imag(topLeft) - std::imag(bottomRight)) / double(resY)),
            totalPixels(resX*resY)
        {
            // how far to the right (real) and down (imag) you need to go to get from TL to BR
            complex<double> H = bottomRight - topLeft;
            double tau = 90 - rotation;
            
        }

        matrix<double> capture();
        matrix<double> parallel_capture();

    private:
        void pixel_worker(std::atomic<int>& pixel_counter, std::mutex& pixel_counter_mutex, matrix<double>& img);

        complex<double> xDiv;
        complex<double> yDiv;
        const double rotation;
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
    matrix<double> img (resX, resY, 0);
    if(real(TL) >= real(BR) || imag(TL) <= imag(BR)) {
        // image has no area (negative dimensions not allowed). Return empty image.
        return img;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    num_threads = (num_threads == 0 ? 1 : num_threads);

    std::vector<std::thread> threads;
    std::mutex pixel_counter_mutex;
    std::atomic<int> pixel_counter = 0;

    for(int x=0; x < num_threads; x++){
        threads.push_back(std::thread(&view::pixel_worker, this, std::ref(pixel_counter), std::ref(pixel_counter_mutex), std::ref(img)));
    }
    for(std::thread& t : threads) t.join();
    return img;
}

void view::pixel_worker(std::atomic<int>& pixel_counter, std::mutex& pixel_counter_mutex, matrix<double>& img) {
    while(true) {
        std::unique_lock lock(pixel_counter_mutex);
        if(pixel_counter >= this->totalPixels) break;
        int pixel_num = pixel_counter;
        pixel_counter++;
        lock.unlock();

        int x = pixel_num % this->resX;
        int y = pixel_num / this->resX;
        complex<double> c {(real(this->TL) + x*this->xDiv), (imag(this->TL) - y*this->yDiv)};

        img[y][x] = getNormalisedIterations(this->maxIterations, this->escapeRadius, c);
    }
    std::this_thread::yield();
}

int main(int argc, char *argv[]) {
    int resX = 2048;
    int resY = 2048;
    complex<double> topLeft{-1.3,0.34};
    complex<double> bottomRight{-1.1,0.14};
    // complex<double> topLeft {-2, ((double) 4/dimX)*(dimY/2)};
    // complex<double> bottomRight {2, -((double) 4/dimX)*(dimY/2)};
    u_int maxIterations = 256;
    double escapeRadius = 3.1;

    // matrix<double> mandelbrot_result = mandelbrot(dimX, dimY, topLeft, bottomRight, maxIterations, escapeRadius);
    matrix<double> mandelbrot_result = view(resX, resY, 0, topLeft, bottomRight, maxIterations, escapeRadius).parallel_capture();
    matrix<RGBpixel> image{resX, resY, RGBpixel{0,0,0}};

    for(int y=0; y<resY; y++){
        for(int x=0; x<resX; x++){
            double iterations = mandelbrot_result[y][x];
            double factor = iterations/maxIterations; //(double)2/3
            // double blue = (double) 1 / double(1 + pow(( factor / double(1-factor)), -1) );
            double blue = sqrt(factor);
            image[y][x] = (iterations < 0 ? RGBpixel{255,255,255} : RGBpixel{int(255*sqrt(pow(factor,1.5))),int(255*factor),int(255*blue)});
        }
    }

    imgWriter writer{};
    writer.write("png", "./mandelbrot.png", image);
    return 0;
}