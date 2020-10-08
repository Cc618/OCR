#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct Matrix_t {
    // Height, width
    size_t rows, cols;

    // Value at position [i, j] = data[i * cols + j]
    float *data;
} Matrix;

Matrix *matrixNew(size_t rows, size_t cols);

// matrixNew with an initializer
Matrix *matrixCreate(size_t rows, size_t cols, const float *data);

// Matrix new and the memory is zeroed
// * Matrix *matrixZero(size_t rows, size_t cols);

// matrixNew(other->rows, other->cols)
// * Matrix *matrixLike(const Matrix *other);

// * Matrix *matrixCopy(const Matrix *other);

void matrixFree(Matrix *m);

void matrixPrint(const Matrix *m);

// Safe method to get an item from a matrix
// * float matrixGet(const Matrix *m, size_t i, size_t j);

// Safely set value
void matrixSet(Matrix *m, size_t i, size_t j, float val);

// Matrix product
// * Matrix *matrixDot(const Matrix *a, const Matrix *b);

// Transposed product a^T * b
// * Matrix *matrixDotT(const Matrix *a, const Matrix *b);

// Inplace element wise operations
// * void matrixAdd(Matrix *a, float b);
// * void matrixSub(Matrix *a, float b);
// * void matrixMul(Matrix *a, float b);
// * void matrixDiv(Matrix *a, float b);
// * void matrixAddMat(Matrix *a, const Matrix *b);
// * void matrixSubMat(Matrix *a, const Matrix *b);
// * void matrixMulMat(Matrix *a, const Matrix *b);
// * void matrixDivMat(Matrix *a, const Matrix *b);

// Inplace map element wise function to matrix
// * void matrixMap(Matrix *a, float (*func)(float value));

#endif // MATRIX_H

