#include "classes.h"

void Pixel::to_greyscale(Pixel** greyscale, uint8_t greyscaleVal) {
  this->r = greyscaleVal;
  this->g = greyscaleVal;
  this->b = greyscaleVal;
  this->greyscale = greyscaleVal;
}

void Pixel::to_gaussian(Pixel** gaussian, uint8_t gaussianVal){
  this->r = gaussianVal;
  this->g = gaussianVal;
  this->b = gaussianVal;
  this->gaussian = gaussianVal;
}
