#include "layers.h"
#include "error.h"
#include "initializer.h"

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

    // Override virtual functions
    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool))denseForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*))denseBackward;
    l->free = (void (*)(Layer *l))denseFree;

    // Init weights and gradients
    // TODO : Xavier initialisation for weight
    l->weight = randNormal(out, in);
    l->gradWeight = matrixZero(out, in);
    l->bias = matrixZero(out, 1);
    l->gradBias = matrixZero(out, 1);

    // Init training context
    l->x = NULL;

    return (Layer*)l;
}

// --- Activation ---
static Matrix *activationForward(Activation *l, const Matrix *x,
        bool training) {
    // Update training context
    if (training)
        l->x = matrixCopy(x);

    Matrix *y = matrixCopy(x);
    l->activation(y);

    return y;
}

static Matrix *activationBackward(Activation *l, const Matrix *grad) {
    // dLoss / dX = dY / dX * dLoss / dY
    Matrix *prevGrad = l->x;
    l->prime(prevGrad);
    matrixMulMat(prevGrad, grad);

    // "Free" training context
    // !!! : l->x is not freed since we return it but is nulled
    // This avoid allocating and copying a bunch of memory
    l->x = NULL;

    return prevGrad;
}

static Layer *activationNew(
        void (*activation)(Matrix *x),
        void (*prime)(Matrix *x)) {
    Activation *l = malloc(sizeof(Activation));

    // Set attributes
    l->activation = activation;
    l->prime = prime;

    // Override virtual functions
    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool))activationForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*))activationBackward;
    l->free = NULL;

    // Init training context
    l->x = NULL;

    return (Layer*)l;
}

Layer *sigmoidNew() {
    return activationNew(matrixSigmoid, matrixSigmoidPrime);
}
