#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "matrix.h"

float randFloatUnif(float min, float max);

float randFloatNorm();

Matrix *randUniform(size_t rows, size_t cols, float min, float max);

Matrix *randNormal(size_t rows, size_t cols);

#endif
