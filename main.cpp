#include <iostream>
#include <strstream>
#include <getopt.h>

#include "mandelbrot.h"
#include "imgWriter.h"
#include "render.h"

constexpr double pi = 3.14159265358979323846;

RGBpixel color(int const x, const int y, matrix<double>& img, unsigned int maxIterations){
    double iterations = img[y][x];
    if (y==0 && x==0) std::cout << x << ' ' << y << ": " << img[y][x];
    double factor = std::min(1.0, iterations/maxIterations);
    double blue = sqrt(factor);
    return (iterations < 0 ? RGBpixel{255,255,255} : RGBpixel{u_int8_t(255*sqrt(pow(factor,1.5))),u_int8_t(255*factor),u_int8_t(255*blue)});
}

int main(int argc, char *argv[]) {
    double coordX = -0.7;
    double coordY = 0;
    double angle = 0;
    double scale = 3;

    int resX = 1024;
    int resY = 1024;
    unsigned int maxIterations = 256;
    double escapeRadius = 3.1;

    // Parse command line arguments
    int c;
    while (1) {
        static struct option long_options[] =
        {
            {"width", required_argument, 0, 'x'},
            {"height", required_argument, 0, 'y'},
            {"real", required_argument, 0, 'r'},
            {"imaginary", required_argument, 0, 'i'},
            {"max_iterations", required_argument, 0, 'm'},
            {"escape_radius", required_argument, 0, 'e'},
            {"scale", required_argument, 0, 's'},
            {"angle", required_argument, 0, 'a'},
            {0, 0, 0, 0}
        }; 
        int option_index = 0;

        c = getopt_long(argc, argv, "x:y:r:i:m:e:s:a:", long_options, &option_index);

        // break at end of options
        if(c == -1) break;

        // Detect the end of the options
        switch (c) {
            case 'x':
                resX = std::stoi(optarg);
                break;

            case 'y':
                resY = std::stoi(optarg);
                break;

            case 'r':
                coordX = std::stod(optarg);
                break;

            case 'i':
                coordY = std::stod(optarg);
                break;

            case 'm':
                maxIterations = std::stoi(optarg);
                break;

            case 'e':
                escapeRadius = std::stod(optarg);
                break;

            case 's':
                scale = 1 / std::stod(optarg);
                break;

            case 'a':
                angle = std::stod(optarg) * (pi/180);
                break;

            case '?':
                // error
                std::cout << "Error parsing arguments\n";
                abort();

            default:
                abort();
        }
    }

    // Print leftover command line arguments
    if (optind < argc) {
        std::cout << "Warning: Unrecognised options: ";
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    double pixelScaleX = scale/resX;
    double pixelScaleY = pixelScaleX; // 1:1

    transform2d t(angle, pixelScaleX, pixelScaleY, coordX-(pixelScaleX*(resX/2)), coordY+(pixelScaleY*(resY/2)));
    view v(resX, resY, t);

    mandelbrot m(v, maxIterations, escapeRadius);
    matrix<double> mandelbrot_result = m.parallel_capture(resX);

    // Colorize
    matrix<RGBpixel> image{resX, resY, RGBpixel{0,0,0}};
    {
        unsigned int numThreads = std::thread::hardware_concurrency();
        numThreads = (numThreads == 0 ? 1 : numThreads);
        
        std::vector<std::thread> threads;
        std::mutex batchCounterMutex;
        std::atomic<int> batchCounter = 0;
        int batchSize = resX;

        std::function<RGBpixel(int, int, matrix<double>&)> pixel_operation = std::bind(color, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, maxIterations);
        for(int x=0; x < numThreads; x++){
            threads.push_back(std::thread(&pixel_worker<double, RGBpixel>, pixel_operation, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(mandelbrot_result), std::ref(image)));
        }
        for(std::thread& t : threads) t.join();
    }
    
    // Save image
    imgWriter writer{};
    writer.write("png", "./mandelbrot.png", image);
    return 0;
}