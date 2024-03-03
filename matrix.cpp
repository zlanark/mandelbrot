#include "matrix.hpp"

std::ostream& operator<<(std::ostream& os, const RGBpixel& pixel){
    os << std::to_string(pixel.R) << ' ';
    os << std::to_string(pixel.G) << ' ';
    os << std::to_string(pixel.B);
    return os;
}