#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "classes.h"
#include <string.h>
#include <iostream>

const double* getKernel();

Pixel** makeImg(int w, int h, int n, unsigned char* data, Pixel* block);

Pixel** applyGreyscale(int w, int h, int n, Pixel** img,  Pixel* block2);

Pixel** applyGaussianBlur(int w, int h, int n, Pixel** greyscale,  Pixel* block3);

std::string now_string();

#endif