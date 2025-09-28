#ifndef CLASSES_H
#define CLASSES_H

#include <stdlib.h>

// Pixel
class Pixel {
    public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    
    void to_greyscale(Pixel** greyscale, uint8_t greyscaleVal);
};



#endif