#ifndef VIEW_H
#define VIEW_H

#include <array>
#include <cmath>

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
    view(unsigned int resX, unsigned int resY, transform2d mapping) :
    resX(resX), resY(resY), totalPixels(resX*resY), mapping(mapping) {
        std::array<double, 2> offsetRight = getRightOffset();
        std::array<double, 2> offsetDown = getDownOffset();
        rightOffsetX = offsetRight[0];
        rightOffsetY = offsetRight[1];
        downOffsetX = offsetDown[0];
        downOffsetY = offsetDown[1];
        // double worldX = 1*mapping.xScale;
        // double worldY = 1*mapping.yScale;


        // rightOffsetX = worldX * std::cos(mapping.rotation) + worldY * std::sin(mapping.rotation);
    }

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
        // worldspace X displacement of one pixel to the right 
        double rightOffsetX;
        // worldspace Y displacement of one pixel to the right
        double rightOffsetY;
        // worldspace X displacement of one pixel down 
        double downOffsetX;
        // worldspace Y displacement of one pixel down
        double downOffsetY;

    std::array<double, 2> getRightOffset() {
        std::array<double, 2> offset{0,0};
        offset[0] = mapping.xScale * cos(mapping.rotation);
        offset[1] = mapping.xScale * sin(mapping.rotation);
        return offset;
    }
    std::array<double, 2> getDownOffset() {
        std::array<double, 2> offset{0,0};
        double angle = (mapping.rotation) - (90*(pi/180));
        offset[0] = mapping.yScale * cos(angle);
        offset[1] = mapping.yScale * sin(angle);
        return offset;        
    }
};

#endif