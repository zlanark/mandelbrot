#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <complex>
#include <array>

#include "view.hpp"
#include "matrix.hpp"

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
        const double render_pixel(int x, int y) {
            std::array<double, 2> c = camera.pixel2world(x,y);
            // std::array<double, 2> c{x*0.02,y*0.02};
            return getNormalisedIterations(this->maxIterations, this->escapeRadius, complex<double>{c[0],c[1]});
        }

        double getNormalisedIterations(const unsigned int& max_depth, const double& escape_radius, const std::complex<double>& c);
        unsigned int getIterations(const unsigned int & max_depth, const double& escape_radius, const std::complex<double>& c);
        unsigned int getIterationsRecursive(unsigned int depth, const double& escape_radius, std::complex<double>& z, const std::complex<double>& c);

        view camera;
        const int maxIterations;
        const double escapeRadius;
};

#endif