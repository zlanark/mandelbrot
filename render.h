#ifndef PIXELWORKER_H
#define PIXELWORKER_H

#include <functional>
#include <atomic>
#include <future>

#include "matrix.h"

// For rendering a matrix from only coordinates
template <typename T>
void pixel_worker(std::function<T(int, int)> operation, std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<T>& img) {
    unsigned int totalPixels = img.width * img.height;
    while(true) {
        // Get the next available batch number and increment
        std::unique_lock lock(batchCounterMutex);
        int currentBatch = batchCounter;
        batchCounter++;
        lock.unlock();

        unsigned int numPixels = batchSize;
        // Would the last pixel in this batch be outside image bounds?
        if((currentBatch+1)*batchSize > totalPixels) {
            // By how much?
            unsigned int overshoot = (batchCounter+1)*batchSize - totalPixels;
            // If we haven't overshot by at least 1 batchSize, then there are still `batchSize - overshoot` pixels unrendered
            if(overshoot < batchSize) numPixels = batchSize - overshoot;
            else break;
        };
        
        unsigned int lowerBound = currentBatch * batchSize;
        unsigned int upperBound = lowerBound + numPixels;
        // Render each pixel in batch
        for(unsigned int pixelCounter=lowerBound; pixelCounter < upperBound; pixelCounter++) {
            int x = pixelCounter % img.width;
            int y = pixelCounter / img.width;

            img[y][x] = operation(x,y);
        }
    }
    std::this_thread::yield();
}

// For performing transformations on an existing matrix
template <typename I, typename O>
void pixel_worker(std::function<O(int, int, matrix<I>&)> operation, std::atomic<int>& batchCounter, std::mutex& batchCounterMutex, const int batchSize, matrix<I>& inputImg, matrix<O>& outputImg) {
    if(inputImg.width != outputImg.width || inputImg.height != outputImg.height) throw std::runtime_error("The input matrix and output matrix have different dimensions.");
    unsigned int totalPixels = inputImg.width * inputImg.height;
    while(true) {
        // Get the next available batch number and increment
        std::unique_lock lock(batchCounterMutex);
        int currentBatch = batchCounter;
        batchCounter++;
        lock.unlock();

        unsigned int numPixels = batchSize;
        // Would the last pixel in this batch be outside image bounds?
        /* !!! This if-statement should only be true once, but it is being entered multiple times 
        I think I just fixed it: changed batchCounter -> currentBatch in conditional. gotta go */
        if((currentBatch+1)*batchSize > totalPixels) {
            // By how much?
            unsigned int overshoot = (batchCounter+1)*batchSize - totalPixels;
            // If we haven't overshot by at least 1 batchSize, then there are still `batchSize - overshoot` pixels unrendered
            if(overshoot < batchSize) numPixels = batchSize - overshoot;
            else break;
        };
        
        unsigned int lowerBound = currentBatch * batchSize;
        unsigned int upperBound = lowerBound + numPixels;
        // Render each pixel in batch
        for(unsigned int pixelCounter=lowerBound; pixelCounter < upperBound; pixelCounter++) {
            int x = pixelCounter % inputImg.width;
            int y = pixelCounter / inputImg.width;

            outputImg[y][x] = operation(x, y, inputImg);
        }
    }
    std::this_thread::yield();
}

template<typename I, typename O>
matrix<O> parallel_render(const int batchSize, const int numThreads, std::function<I(int, int)> pixel_operation, matrix<I>& inputImg) {
    matrix<O> outputImg (inputImg.x, inputImg.y, O());
    
    std::vector<std::thread> threads;
    std::mutex batchCounterMutex;
    std::atomic<int> batchCounter = 0;

    for(int x=0; x < numThreads; x++){
        threads.push_back(std::thread(&pixel_worker<I, O>, pixel_operation, std::ref(batchCounter), std::ref(batchCounterMutex), batchSize, std::ref(inputImg), std::ref(outputImg)));
    }
    for(std::thread& t : threads) t.join(); 
    return outputImg;
}

#endif