#include <stdio.h>
#include <time.h>
#include <math.h>
#include "initializer.h"

#define PI 3.141592654

float randFloatUnif(float min, float max) {
    return (((float)rand()) / ((float)RAND_MAX)) * (max-min) + min;
}

float randFloatNorm() {
    // Method described by Abramowitz and Stegun to generate random numbers
    // with a gaussian distribution

	static float U, V;
	static int phase = 0;
	float Z;

	if(phase == 0) {
		U = (rand() + 1.) / (RAND_MAX + 2.);
		V = rand() / (RAND_MAX + 1.);
		Z = sqrt(-2 * log(U)) * sin(2 * PI * V);
	} else
		Z = sqrt(-2 * log(U)) * cos(2 * PI * V);

	phase = 1 - phase;

	return Z;
}

Matrix *randUniform(size_t rows, size_t cols, float min, float max) {
    Matrix *m = matrixNew(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            MAT_GET(m, i, j) = randFloatUnif(min, max);
        }
    }

    return m;
}

Matrix *randNormal(size_t rows, size_t cols) {
    Matrix *m = matrixNew(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            MAT_GET(m, i, j) = randFloatNorm();
        }
    }

    return m;
}
