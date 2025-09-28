#include "classes.h"
#include <stdlib.h>

void Pixel::to_greyscale(Pixel** greyscale, uint8_t greyscaleVal) {
  this->r = greyscaleVal;
  this->g = greyscaleVal;
  this->b = greyscaleVal;
}