#ifndef CLASSES_H
#define CLASSES_H

#include <stdlib.h>

// Pixel
class Pixel {
 public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t greyscale;

  // Assign the greyscale value to R, G, and B
  void to_greyscale(Pixel** greyscale, uint8_t greyscaleVal);

  // Apply gaussian blur to the pixel (update greyscale value)
  void to_gaussian(Pixel** gaussian, uint8_t gaussianVal);
};

#endif