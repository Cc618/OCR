#include "layers.h"
#include "error.h"

// --- Layer ---
Matrix *layerForward(Layer *l, const Matrix *x, bool training) {
    return l->forward(l, x, training);
}

Matrix *layerBackward(Layer *l, const Matrix *grad) {
    return l->backward(l, grad);
}

void layerFree(Layer *l) {
    if (l->free)
        l->free(l);

    free(l);
}

// --- Dense ---
static Matrix *denseForward(Dense *l, const Matrix *x, bool training) {
    ASSERT(x->rows == l->weight->cols && x->cols == 1,
            "denseForward : Invalid x shape");

    // Update training context
    if (training)
        l->x = matrixCopy(x);

    Matrix *y = matrixDot(l->weight, x);
    matrixAddMat(y, l->bias);

    return y;
}

static Matrix *denseBackward(Dense *l, const Matrix *grad) {
    // dL / dW = (dL/dY) . x^T
    Matrix *gradWeight = matrixOuter(grad, l->x);
    // TODO : Multiply by 1/batch_size ? -> or in optimizer.update
    matrixAddMat(l->gradWeight, gradWeight);

    // dL / dB = (dL/dY)
    matrixAddMat(l->gradBias, grad);

    matrixFree(gradWeight);

    // Free training context
    matrixFree(l->x);
    l->x = NULL;

    return matrixDotT(l->weight, grad);
}

void denseFree(Dense *l) {
    matrixFree(l->weight);
    matrixFree(l->gradWeight);
    matrixFree(l->bias);
    matrixFree(l->gradBias);
}

Layer *denseNew(size_t in, size_t out) {
    Dense *l = malloc(sizeof(Dense));

    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool)) denseForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*)) denseBackward;
    l->free = (void (*)(Layer *l)) denseFree;

    // TODO : Initialisation for weight
    l->weight = matrixZero(out, in);
    l->gradWeight = matrixZero(out, in);
    l->bias = matrixZero(out, 1);
    l->gradBias = matrixZero(out, 1);

    return (Layer*)l;
}

