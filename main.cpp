#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <new>

#include "classes.h"
#include "functions.h"
#include "stb_image.h"

int main(int argc, char** argv) {
  // GPT5 --------------------------------------------------------------------|
  if (argc != 2) {
    std::cout << stderr << "Usage: " << argv[0] << "input.jpg" << std::endl;
    return 1;
  }

  int w = 0, h = 0, n = 0;
  // Force 3 channels (RGB)
  unsigned char* data = stbi_load(argv[1], &w, &h, &n, 3);
  if (!data) {
    std::cout << stderr << "stbi_load failed: " << stbi_failure_reason()
              << std::endl;
    return 2;
  }

  // Example: copy into your Pixel[] here if you want
  // size_t N = (size_t)w * (size_t)h;
  // Pixel* img = new Pixel[N];
  // const unsigned char* p = data;
  // for (size_t i = 0; i < N; ++i) { img[i] = Pixel(p[0], p[1], p[2]); p += 3;
  // } delete[] img;

  std::cout << "Loaded " << w << "x" << h << "RGB" << std::endl;
  stbi_image_free(data);
  // GPT5 END ---------------------------------------------------------------|
}
