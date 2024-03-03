#ifndef VIEW_H
#define VIEW_H

#include <array>

#include "constants.hpp"

struct transform2d {
    transform2d(double rotation, double xScale, double yScale, double xLoc, double yLoc) : 
    rotation(rotation), xScale(xScale), yScale(yScale), xLoc(xLoc), yLoc(yLoc) {}

    double rotation; // radians

    double xScale; // pixel width in worldspace
    double yScale; // pixel height in worldspace

    double xLoc; // top right pixel location
    double yLoc; // 

};

struct view {
    view(unsigned int resX, unsigned int resY, transform2d mapping);

    unsigned int resX;
    unsigned int resY;
    unsigned int totalPixels;
    transform2d mapping;

    /* converts screen space coordinate to world space (defined by this->mapping) coordinate */
    std::array<double, 2> pixel2world(int x, int y) {
        x = x - (resX-1)/2;
        y = y - (resY-1)/2;
        double worldX = mapping.xLoc + x*rightOffsetX + y*downOffsetX;
        double worldY = mapping.yLoc + x*rightOffsetY + y*downOffsetY;
        return {worldX, worldY};
    }

    private:
        std::array<double, 2> getRightOffset();
        std::array<double, 2> getDownOffset();
        // worldspace X displacement of one pixel to the right 
        double rightOffsetX;
        // worldspace Y displacement of one pixel to the right
        double rightOffsetY;
        // worldspace X displacement of one pixel down 
        double downOffsetX;
        // worldspace Y displacement of one pixel down
        double downOffsetY;
};


#endif