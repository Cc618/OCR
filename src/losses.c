#include "losses.h"
#include <math.h>
#include "layers.h"
#include "error.h"

// To avoid out of domains errors
#define EPSILON (1e-6f)

void lossFree(Loss *l) {
    matrixFree(l->grad);
    free(l);
}

Loss *mseLoss(const Matrix *y, unsigned int target) {
    ASSERT(target < y->rows && y->cols == 1,
            "mseLoss : Target label outside of bounds");

    Loss *l = malloc(sizeof(Loss));
    l->loss = 0;
    l->grad = matrixLike(y);

    // Loss
    size_t n = y->rows;
    for (size_t i = 0; i < n; ++i) {
        float dist = i == target ? y->data[i] - 1 : y->data[i];
        l->loss += dist * dist;
    }
    l->loss /= 2 * n;

    // Grad
    float factor = 1.f / (float)n;
    for (size_t i = 0; i < n; ++i) {
        float dist = i == target ? y->data[i] - 1 : y->data[i];
        l->grad->data[i] = factor * dist;
    }

    return l;
}

Loss *nllLoss(const Matrix *y, unsigned int target) {
    ASSERT(target < y->rows && y->cols == 1,
            "nllLoss : Target label outside of bounds");

    Loss *l = malloc(sizeof(Loss));

    // Probability for the target class
    float prob = EPSILON + matrixGet(y, target, 0);

    l->loss = -log(prob) / (float)y->rows;

    // d(-log Y_x ) / dY = -1 / Y_x
    l->grad = matrixZero(y->rows, y->cols);
    matrixSet(l->grad, target, 0, -1.f / (prob * y->rows));

    return l;
}
