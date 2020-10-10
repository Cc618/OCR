#include <math.h>
#include <stdio.h>
#include "matrix.h"
#include "error.h"

// Unsafe get
#define MAT_GET(m, i, j) (m->data[i * m->cols + j])

#define MAT_PRINT_FORMAT "%5.2f"

Matrix *matrixNew(size_t rows, size_t cols) {
    Matrix *mat = malloc(sizeof(Matrix));

    mat->rows = rows;
    mat->cols = cols;
    mat->data = malloc(sizeof(float) * rows * cols);

    return mat;
}

Matrix *matrixCreate(size_t rows, size_t cols, const float *data) {
    Matrix *m = matrixNew(rows, cols);

    memcpy(m->data, data, rows * cols * sizeof(float));

    return m;
}

Matrix *matrixLike(const Matrix *other) {
    return matrixNew(other->rows, other->cols);
}

Matrix *matrixCopy(const Matrix *other) {
    Matrix *m = matrixLike(other);

    memcpy(m->data, other->data, sizeof(float) * m->rows * m->cols);

    return m;
}

void matrixFree(Matrix *m) {
    free(m->data);
    free(m);
}

void matrixPrint(const Matrix *m) {
    for (size_t i = 0; i < m->rows; ++i) {
        printf(MAT_PRINT_FORMAT, MAT_GET(m, i, 0));
        for (size_t j = 1; j < m->cols; ++j)
            printf(" " MAT_PRINT_FORMAT, MAT_GET(m, i, j));

        putchar('\n');
    }
}

void matrixSet(Matrix *m, size_t i, size_t j, float val) {
    ASSERT(i < m->rows, "matrixSet : i out of bounds, i >= rows");
    ASSERT(j < m->cols, "matrixSet : j out of bounds, j >= cols");

    MAT_GET(m, i, j) = val;
}

