#include "view.hpp"
#include <array>
#include <cmath>

std::array<double, 2> view::getRightOffset() {
    std::array<double, 2> offset{0,0};
    offset[0] = this->mapping.xScale * cos(this->mapping.rotation);
    offset[1] = this->mapping.xScale * sin(this->mapping.rotation);
    return offset;
}

std::array<double, 2> view::getDownOffset() {
    std::array<double, 2> offset{0,0};
    double angle = (this->mapping.rotation) - (90*(pi/180));
    offset[0] = this->mapping.yScale * cos(angle);
    offset[1] = this->mapping.yScale * sin(angle);
    return offset;        
}

view::view(unsigned int resX, unsigned int resY, transform2d mapping) :
resX(resX), resY(resY), totalPixels(resX*resY), mapping(mapping) {
    std::array<double, 2> offsetRight = getRightOffset();
    std::array<double, 2> offsetDown = getDownOffset();
    rightOffsetX = offsetRight[0];
    rightOffsetY = offsetRight[1];
    downOffsetX = offsetDown[0];
    downOffsetY = offsetDown[1];
}
