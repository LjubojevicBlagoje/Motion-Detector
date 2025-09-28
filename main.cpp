#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Silence warnings from stb_image_write.h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "stb_image_write.h"
#pragma clang diagnostic pop

// Silence warnings from stb_image.h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "stb_image.h"
#pragma clang diagnostic pop

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <new>

#include "classes.h"
#include "functions.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " frame.jpg mask.jpg" << std::endl;
    return 1;
  }

  // GPT5 --------------------------------------------------------------------|

  int w = 0, h = 0, n = 0;
  // Force 3 channels (RGB)
  unsigned char* data = stbi_load(argv[1], &w, &h, &n, 3);
  if (!data) {
    std::cerr << "stbi_load failed: " << stbi_failure_reason() << std::endl;
    return 2;
  }
  std::cout << "Loaded " << w << "x" << h << "RGB" << std::endl;

  unsigned char* maskData = stbi_load(argv[2], &w, &h, &n, 3);
  if (!maskData) {
    std::cerr << "stbi_load failed: " << stbi_failure_reason() << std::endl;
    return 2;
  }
  std::cout << "Loaded " << w << "x" << h << "RGB" << std::endl;

  Pixel* block = new Pixel[(size_t)w * h];   // contiguous pixels
  Pixel* mblock = new Pixel[(size_t)w * h];  // contiguous pixels

  Pixel** img = makeImg(w, h, n, data, block);
  Pixel** mask = makeImg(w, h, n, maskData, mblock);
  // Use as img[y][x] to access pixel's RGB values

  stbi_image_free(data);
  stbi_image_free(maskData);

  // GPT5 END ---------------------------------------------------------------|

  // CONVERT TO GREYSCALE ---------------------------------------------------|
  Pixel* block2 = new Pixel[(size_t)w * h];   // contiguous pixels
  Pixel* mblock2 = new Pixel[(size_t)w * h];  // contiguous pixels

  // Apply greyscale to frame
  Pixel** greyscale = applyGreyscale(w, h, n, img, block2);
  // Apply greyscale to mask
  Pixel** greyscaleMask = applyGreyscale(w, h, n, mask, mblock2);

  // GAUSSIAN BLUR ---------------------------------------------------------|
  Pixel* block3 = new Pixel[(size_t)w * h];  // contiguous pixels
  Pixel* mblock3 = new Pixel[(size_t)w * h];

  // Apply gaussian blur to frame
  Pixel** gaussian = applyGaussianBlur(w, h, n, greyscale, block3);
  // Apply gaussian blur to mask
  Pixel** gaussianMask = applyGaussianBlur(w, h, n, greyscaleMask, mblock3);

  // -----------------------------------------------------------------------|

  delete[] img;
  delete[] block;
  delete[] mask;
  delete[] mblock;
  delete[] greyscale;
  delete[] block2;
  delete[] greyscaleMask;
  delete[] mblock2;
  delete[] gaussian;
  delete[] block3;
  delete[] gaussianMask;
  delete[] mblock3;

  return 0;
}
