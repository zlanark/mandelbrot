#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

constexpr double pi = 3.14159265358979323846;

const std::string helpText{\
"Usage: mandelbrot [OPTION]...\n\
\n\
Render an image of the mandelbrot set.\n\
  -x, --width                  The width of the image in pixels.\n\
  -y, --height                 The height of the image in pixels.\n\
  -r, --real                   The real component of the coordinate\n\
                               at the centre of the image.\n\
\n\
  -r, --imaginary              The imaginary component of the\n\
                               coordinate at the centre of the image.\n\
\n\
  -m, --max_iterations         The maximum number of iterations to test\n\
                               a pixel for.\n\
\n\
  -e, --escape_radius          The modulus/magnitude/norm beyond which\n\
                               iteration will stop.\n\
\n\
  -s, --scale                  The reciprocal of the width of the\n\
                               image on the complex plane.\n\
\n\
  -a, --angle                  The angle of the image in degrees.\n\
  -o, --ouput                  The output directory.\n\
  -h, --help                   Display this help text.\n\
\n\
All numerical arguments take floating point numbers except for\n\
--width, --height, and --max_iterations, which only take integers.\n\0"};

#endif