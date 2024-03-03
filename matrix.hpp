#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>

template<typename T>
class matrix {
    public:
        int width;
        int height;

        matrix(int width, int height, T init);
        std::vector<T>& operator[](const int& x);
        std::vector<T> serialize();
    private:
        std::vector<std::vector<T>> elements;
};

template <typename T>
matrix<T>::matrix(int width, int height, T init) : width{width}, height{height}
    , elements {std::vector<std::vector<T>>(height, std::vector<T>(width, init))} {}

template <typename T>
std::vector<T>& matrix<T>::operator[] (const int& x) {
    if(x < 0 || x >= this->elements.size()) {
        throw std::out_of_range("Matrix row index out of bounds. Index " + std::to_string(x) 
        + " was requested, but this matrix only has indexes 0 to " + std::to_string(this->elements.size()-1)
        + " (size: " + std::to_string(this->elements.size()) + ").");
    }
    return this->elements[x];
}

template <typename T>
std::vector<T> matrix<T>::serialize() {
    std::vector<T> serialized;
    for(std::vector<T> row : this->elements) {
        for(T elem : row){
            serialized.push_back(elem);
        }
    }
    return serialized;
}

struct RGBpixel {
    RGBpixel(u_int8_t r, u_int8_t g, u_int8_t b) : R{r}, G{g}, B{b} {}
    // friend std::ostream& operator<<(std::ostream& os, const RGBpixel& pixel);
    u_int8_t R = 0;
    u_int8_t G = 0;
    u_int8_t B = 0;
};

std::ostream& operator<<(std::ostream& os, const RGBpixel& pixel);

#endif