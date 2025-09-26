#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG

#pragma clang diagnostic push
#include "stb_image.h"
#pragma clang diagnostic ignored "-Wunused-parameter"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <new>

#include "classes.h"
#include "functions.h"

int main(int argc, char** argv) {
  // GPT5 --------------------------------------------------------------------|

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << "input.jpg" << std::endl;
    return 1;
  }

  int w = 0, h = 0, n = 0;
  // Force 3 channels (RGB)
  unsigned char* data = stbi_load(argv[1], &w, &h, &n, 3);
  if (!data) {
    std::cerr << "stbi_load failed: " << stbi_failure_reason() << std::endl;
    return 2;
  }

  // Example: copy into your Pixel[] here if you want
  // size_t N = (size_t)w * (size_t)h;
  // Pixel* img = new Pixel[N];
  // const unsigned char* p = data;
  // for (size_t i = 0; i < N; ++i) { img[i] = Pixel(p[0], p[1], p[2]); p += 3;
  // } delete[] img;

  std::cout << "Loaded " << w << "x" << h << "RGB" << std::endl;

  Pixel* block = new Pixel[(size_t)w * h];  // contiguous pixels
  Pixel** img = new Pixel*[h];              // row pointers
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

  // ACCESS img to access img[x][y] pixel's RGB values 

  stbi_image_free(data);

  // GPT5 END ---------------------------------------------------------------|



  delete[] img;
  delete[] block;
  
  return 0;
}
