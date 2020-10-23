#include "losses.h"
#include "layers.h"
#include "error.h"

void lossFree(Loss *l) {
    matrixFree(l->grad);
    free(l);
}

Loss *mseLoss(const Matrix *y, const Matrix *target) {
    ASSERT(y->rows == target->rows && y->cols == target->cols,
            "mseLoss: y and target must have the same shape");

    Loss *l = malloc(sizeof(Loss));
    l->loss = 0;
    l->grad = matrixLike(y);

    // Loss
    size_t n = y->cols * y->rows;
    for (size_t i = 0; i < n; ++i) {
        float dist = y->data[i] - target->data[i];
        l->loss += dist * dist;
    }
    l->loss /= 2 * n;

    // Grad
    float factor = 1.f / (float)n;
    for (size_t i = 0; i < n; ++i) {
        l->grad->data[i] = factor * (y->data[i] - target->data[i]);
    }

    return l;
}

