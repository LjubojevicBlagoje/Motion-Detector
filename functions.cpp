#include "functions.h"
#include "classes.h"

#include <cmath>

const double* getKernel() {
  static double kernel[81] = {
      1.0 / 1003,  1.0 / 1003,  2.0 / 1003,  2.0 / 1003,  2.0 / 1003,
      2.0 / 1003,  2.0 / 1003,  1.0 / 1003,  1.0 / 1003,  1.0 / 1003,
      3.0 / 1003,  4.0 / 1003,  5.0 / 1003,  5.0 / 1003,  5.0 / 1003,
      4.0 / 1003,  3.0 / 1003,  1.0 / 1003,  2.0 / 1003,  4.0 / 1003,
      10.0 / 1003, 12.0 / 1003, 15.0 / 1003, 12.0 / 1003, 10.0 / 1003,
      4.0 / 1003,  2.0 / 1003,  2.0 / 1003,  5.0 / 1003,  12.0 / 1003,
      20.0 / 1003, 26.0 / 1003, 20.0 / 1003, 12.0 / 1003, 5.0 / 1003,
      2.0 / 1003,  2.0 / 1003,  5.0 / 1003,  15.0 / 1003, 26.0 / 1003,
      41.0 / 1003, 26.0 / 1003, 15.0 / 1003, 5.0 / 1003,  2.0 / 1003,
      2.0 / 1003,  5.0 / 1003,  12.0 / 1003, 20.0 / 1003, 26.0 / 1003,
      20.0 / 1003, 12.0 / 1003, 5.0 / 1003,  2.0 / 1003,  2.0 / 1003,
      4.0 / 1003,  10.0 / 1003, 12.0 / 1003, 15.0 / 1003, 12.0 / 1003,
      10.0 / 1003, 4.0 / 1003,  2.0 / 1003,  1.0 / 1003,  3.0 / 1003,
      4.0 / 1003,  5.0 / 1003,  5.0 / 1003,  5.0 / 1003,  4.0 / 1003,
      3.0 / 1003,  1.0 / 1003,  1.0 / 1003,  1.0 / 1003,  2.0 / 1003,
      2.0 / 1003,  2.0 / 1003,  2.0 / 1003,  2.0 / 1003,  1.0 / 1003,
      1.0 / 1003};
  return kernel;
}

// Turn data into an editable format
Pixel** makeImg(int w, int h, int n, unsigned char* data, Pixel* block) {
  Pixel** img = new Pixel*[h];  // row pointers
  for (int y = 0; y < h; ++y) img[y] = block + (size_t)y * w;

  // fill from stb_image's RGB bytes
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      size_t l = ((size_t)y * w + x) * 3;  // byte index into data
      img[y][x].r = data[l + 0];
      img[y][x].g = data[l + 1];
      img[y][x].b = data[l + 2];
    }
  }
  return img;
}

// Perform greyscale conversion
Pixel** applyGreyscale(int w, int h, int n, Pixel** img, Pixel* block2) {
  Pixel** greyscale = new Pixel*[h];  // row pointers
  for (int y = 0; y < h; ++y) greyscale[y] = block2 + (size_t)y * w;

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      uint8_t greyscaleVal = (img[y][x].r + img[y][x].g + img[y][x].b) / 3;

      // Make R, G, B, and dedicated greyscale value for each pixel in block2
      // equal greyscaleVal
      greyscale[y][x].to_greyscale(greyscale, greyscaleVal);
    }
  }
  return greyscale;
}

// Apply gaussian blur
Pixel** applyGaussianBlur(int w, int h, int n, Pixel** greyscale, Pixel* block3) {
  Pixel** gaussian = new Pixel*[h];  // row pointers
  for (int y = 0; y < h; ++y) gaussian[y] = block3 + (size_t)y * w;

  // Gaussian kernel
  const double* gaussianKernel = getKernel();

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      double sum = 0;

      int kernelIndex = 0;

      for (int gy = y - 4; gy < y + 5; gy++) {
        for (int gx = x - 4; gx < x + 5; gx++) {
          if (gx >= 0 && gx < w && gy >= 0 && gy < h) {
            sum += greyscale[gy][gx].greyscale * gaussianKernel[kernelIndex];
          }
          kernelIndex++;
        }
      }
      // Make R, G, B, and dedicated gaussian value for each pixel in block2
      // equal gaussianVal;
      uint8_t gaussianVal = static_cast<uint8_t>(std::round(sum));

      gaussian[y][x].to_gaussian(gaussian, gaussianVal);
    }
  }
  return gaussian;
}