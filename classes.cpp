#include "classes.h"

// Pixel implementation
Pixel::Pixel() : r(0), g(0), b(0) {};
Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b) {
  this->r = r;
  this->g = g;
  this->b = b;
}