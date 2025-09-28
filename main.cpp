// GPT5 --------------------------------------------------------------------|
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
// GPT5 END ---------------------------------------------------------------|

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

  // -----------------------------------------------------------------------|

  // TEST EXPORT GREYSCALE AS JPG ---------------------------------|
  // Allocate raw buffer for tightly packed RGB bytes
  uint8_t* buffer2 = new uint8_t[(size_t)w * h * 3];

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Pixel& p = greyscale[y][x];  // or gaussian[y][x]
      size_t idx = ((size_t)y * w + x) * 3;
      buffer2[idx + 0] = p.r;
      buffer2[idx + 1] = p.g;
      buffer2[idx + 2] = p.b;
    }
  }

  if (!stbi_write_jpg("greyscaleFRAME.jpg", w, h, 3, buffer2, 90)) {
    std::cerr << "Failed to write greyscaleFRAME.jpg\n";
  }

  // Free buffer after writing
  delete[] buffer2;
  // ---------------------------------------------------
  uint8_t* buffer2mask = new uint8_t[(size_t)w * h * 3];

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Pixel& p = greyscaleMask[y][x];  // or gaussian[y][x]
      size_t idx = ((size_t)y * w + x) * 3;
      buffer2mask[idx + 0] = p.r;
      buffer2mask[idx + 1] = p.g;
      buffer2mask[idx + 2] = p.b;
    }
  }

  if (!stbi_write_jpg("greyscaleMASK.jpg", w, h, 3, buffer2mask, 90)) {
    std::cerr << "Failed to write greyscaleMASK.jpg\n";
  }

  // Free buffer after writing
  delete[] buffer2mask;

  // --------------------------------------------------------------|

  // GAUSSIAN BLUR ---------------------------------------------------------|
  Pixel* block3 = new Pixel[(size_t)w * h];  // contiguous pixels
  Pixel* mblock3 = new Pixel[(size_t)w * h];

  // Apply gaussian blur to frame
  Pixel** gaussian = applyGaussianBlur(w, h, n, greyscale, block3);
  // Apply gaussian blur to mask
  Pixel** gaussianMask = applyGaussianBlur(w, h, n, greyscaleMask, mblock3);

  // -----------------------------------------------------------------------|

  // TEST EXPORT GAUSSIAN AS JPG ---------------------------------|
  // Allocate raw buffer for tightly packed RGB bytes
  uint8_t* buffer3 = new uint8_t[(size_t)w * h * 3];

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Pixel& p = gaussian[y][x];  // or gaussian[y][x]
      size_t idx = ((size_t)y * w + x) * 3;
      buffer3[idx + 0] = p.r;
      buffer3[idx + 1] = p.g;
      buffer3[idx + 2] = p.b;
    }
  }

  if (!stbi_write_jpg("gaussianFRAME.jpg", w, h, 3, buffer3, 90)) {
    std::cerr << "Failed to write gaussianFRAME.jpg\n";
  }

  // Free buffer after writing
  delete[] buffer3;
  // -----------------------------------------------------------------------|

  // TEST EXPORT GAUSSIAN AS JPG ---------------------------------|
  // Allocate raw buffer for tightly packed RGB bytes
  uint8_t* buffer3mask = new uint8_t[(size_t)w * h * 3];

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Pixel& p = gaussianMask[y][x];  // or gaussian[y][x]
      size_t idx = ((size_t)y * w + x) * 3;
      buffer3mask[idx + 0] = p.r;
      buffer3mask[idx + 1] = p.g;
      buffer3mask[idx + 2] = p.b;
    }
  }

  if (!stbi_write_jpg("gaussianMASK.jpg", w, h, 3, buffer3mask, 90)) {
    std::cerr << "Failed to write gaussianMASK.jpg\n";
  }

  // Free buffer after writing
  delete[] buffer3mask;
  // --------------------------------------------------------------|

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
