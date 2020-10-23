#include <math.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "error.h"

#define MAT_PRINT_FORMAT "%5.2f"

static float sigmoid(float x) {
    return 1.f / (1.f + exp(-x));
}

static float sigmoidPrime(float x) {
    float expMinusX = exp(-x);
    float div = 1 + expMinusX;
    div *= div;

    return expMinusX / div;
}

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

Matrix *matrixZero(size_t rows, size_t cols) {
    Matrix *m = matrixNew(rows, cols);

    memset(m->data, 0, rows * cols * sizeof(float));

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
    printf("[ ");

    for (size_t i = 0; i < m->rows; ++i) {
        printf(i != 0 ? "  " MAT_PRINT_FORMAT : MAT_PRINT_FORMAT,
                MAT_GET(m, i, 0));

        for (size_t j = 1; j < m->cols; ++j)
            printf(" " MAT_PRINT_FORMAT, MAT_GET(m, i, j));

        if (i != m->rows - 1)
            putchar('\n');
    }
    puts(" ]");
}

float matrixGet(const Matrix *m, size_t i, size_t j) {
    ASSERT(i < m->rows && j < m->cols, "matrixGet : i, j outside of matrix");

    return MAT_GET(m, i, j);
}

void matrixSet(Matrix *m, size_t i, size_t j, float val) {
    ASSERT(i < m->rows, "matrixSet : i out of bounds, i >= rows");
    ASSERT(j < m->cols, "matrixSet : j out of bounds, j >= cols");

    MAT_GET(m, i, j) = val;
}

Matrix *matrixDot(const Matrix *a, const Matrix *b) {
    ASSERT(a->cols == b->rows,
            "matrixDot : Incompatible shapes between a and b");

    Matrix *m = matrixNew(a->rows, b->cols);

    for (size_t i = 0; i < m->rows; ++i)
        for (size_t j = 0; j < m->cols; ++j) {
            float sum = 0.f;

            for (size_t k = 0; k < a->cols; ++k)
                sum += MAT_GET(a, i, k) * MAT_GET(b, k, j);

            MAT_GET(m, i, j) = sum;
        }

    return m;
}

Matrix *matrixDotT(const Matrix *a, const Matrix *b) {
    ASSERT(a->rows == b->rows,
            "matrixDot : Incompatible shapes between a and b");

    Matrix *m = matrixNew(a->cols, b->cols);

    for (size_t i = 0; i < m->rows; ++i)
        for (size_t j = 0; j < m->cols; ++j) {
            float sum = 0.f;

            for (size_t k = 0; k < b->rows; ++k)
                sum += MAT_GET(a, k, i) * MAT_GET(b, k, j);

            MAT_GET(m, i, j) = sum;
        }

    return m;
}

Matrix *matrixOuter(const Matrix *a, const Matrix *b) {
    ASSERT(a->cols == 1 && b->cols == 1,
            "matrixOuter : Not a vector as input");

    Matrix *m = matrixNew(a->rows, b->rows);

    for (size_t i = 0; i < a->rows; ++i)
        for (size_t j = 0; j < b->rows; ++j)
            // a and b are 1 dimensional so we can access items
            // within data directly
            MAT_GET(m, i, j) = a->data[i] * b->data[j];

    return m;
}

void matrixAdd(Matrix *a, float b) {
    for (size_t i = 0; i < a->rows; ++i)
        for (size_t j = 0; j < a->cols; ++j)
            MAT_GET(a, i, j) += b;
}

void matrixSub(Matrix *a, float b) {
    for (size_t i = 0; i < a->rows; ++i)
        for (size_t j = 0; j < a->cols; ++j)
            MAT_GET(a, i, j) -= b;
}

void matrixMul(Matrix *a, float b) {
    for (size_t i = 0; i < a->rows; ++i)
        for (size_t j = 0; j < a->cols; ++j)
            MAT_GET(a, i, j) *= b;
}

void matrixDiv(Matrix *a, float b) {
    for (size_t i = 0; i < a->rows; ++i)
        for (size_t j = 0; j < a->cols; ++j)
            MAT_GET(a, i, j) /= b;
}

void matrixAddMat(Matrix *a, const Matrix *b) {
    size_t n1 = a->rows;
    size_t p1 = a->cols;
    size_t n2 = b->rows;
    size_t p2 = b->cols;

    ASSERT(n1 == n2 && p1 == p2,
            "matrixAddMat : Incompatible shapes between a and b");

    for (size_t i = 0; i < n1; ++i) {
        for (size_t j = 0; j < p1; ++j) {
            MAT_GET(a, i, j) += MAT_GET(b, i, j);
        }
    }
}

void matrixSubMat(Matrix *a, const Matrix *b) {
    size_t n1 = a->rows;
    size_t p1 = a->cols;
    size_t n2 = b->rows;
    size_t p2 = b->cols;

    ASSERT(n1 == n2 && p1 == p2,
            "matrixSubMat : Incompatible shapes between a and b");

    for (size_t i = 0; i < n1; ++i) {
        for (size_t j = 0; j < p1; ++j) {
            MAT_GET(a, i, j) -= MAT_GET(b, i, j);
        }
    }
}

void matrixMulMat(Matrix *a, const Matrix *b) {
    size_t n1 = a->rows;
    size_t p1 = a->cols;
    size_t n2 = b->rows;
    size_t p2 = b->cols;

    ASSERT(n1 == n2 && p1 == p2,
            "matrixMulMat : Incompatible shapes between a and b");

    for (size_t i = 0; i < n1; ++i) {
        for (size_t j = 0; j < p1; ++j) {
            MAT_GET(a, i, j) *= MAT_GET(b, i, j);
        }
    }
}

void matrixDivMat(Matrix *a, const Matrix *b) {
    size_t n1 = a->rows;
    size_t p1 = a->cols;
    size_t n2 = b->rows;
    size_t p2 = b->cols;

    ASSERT(n1 == n2 && p1 == p2,
            "matrixDivMat : Incompatible shapes between a and b");

    for (size_t i = 0; i < n1; ++i) {
        for (size_t j = 0; j < p1; ++j) {
            MAT_GET(a, i, j) /= MAT_GET(b, i, j);
        }
    }
}

void matrixMap(Matrix *m, float (*func)(float value)) {
    for (size_t i = 0; i < m->rows; ++i)
        for (size_t j = 0; j < m->cols; ++j)
            MAT_GET(m, i, j) = func(MAT_GET(m, i, j));
}

void matrixSigmoid(Matrix *m) {
    matrixMap(m, sigmoid);
}

void matrixSigmoidPrime(Matrix *m) {
    matrixMap(m, sigmoidPrime);
}

