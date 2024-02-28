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

    private:
        void pixel_worker(std::atomic<int>& pixel_counter, std::mutex& pixel_counter_mutex, matrix<double>& img);

        view camera;
        const int maxIterations;
        const double escapeRadius;
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

#endif