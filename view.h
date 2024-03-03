#ifndef VIEW_H
#define VIEW_H

#include <array>
#include <cmath>

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
        // double worldX = 1*mapping.xScale;
        // double worldY = 1*mapping.yScale;


        // rightOffsetX = worldX * std::cos(mapping.rotation) + worldY * std::sin(mapping.rotation);
    }

    unsigned int resX;
    unsigned int resY;
    unsigned int totalPixels;
    transform2d mapping;

    /* converts screen space coordinate to world space (defined by this->mapping) coordinate */
    std::array<double, 2> pixel2world(unsigned int x, unsigned int y) {
        double worldX = mapping.xLoc + x*mapping.xScale;
        double worldY = mapping.yLoc - y*mapping.yScale;
        // if(mapping.rotation != 0) {
        //     worldX = worldX * std::cos(mapping.rotation) + worldY * std::sin(mapping.rotation);
        //     worldY = (-worldX) * std::sin(mapping.rotation) + worldY * std::cos(mapping.rotation);
        // }
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

};

#endif