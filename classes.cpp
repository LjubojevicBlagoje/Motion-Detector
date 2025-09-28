#include "classes.h"

#include <stdlib.h>

void Pixel::to_greyscale(uint8_t greyscaleVal) {
  this->r = greyscaleVal;
  this->g = greyscaleVal;
  this->b = greyscaleVal;
  this->greyscale = greyscaleVal;
}

void Pixel::to_gaussian(uint8_t gaussianVal){
  this->r = gaussianVal;
  this->g = gaussianVal;
  this->b = gaussianVal;
  this->gaussian = gaussianVal;
}
