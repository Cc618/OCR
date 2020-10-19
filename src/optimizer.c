#include "optimizer.h"

void optimizerUpdate(Optimizer *o, Matrix *weights, Matrix *grad) {
    o->update(o, weights, grad);
}

void optimizerFree(Optimizer *o) {
    free(o);
}

// --- SGD ---
static void sgdUpdate(Optimizer *o, Matrix *weights, Matrix *grad) {
    // Optimize
    Matrix *g = matrixCopy(grad);
    matrixMul(g, ((SGD*)o)->learningRatio);

    // weights -= learningRate / batchSize * grad = learningRatio * grad
    matrixSubMat(weights, g);

    matrixFree(g);

    // Reset
    // TODO : Momentum
    matrixMul(grad, 0);
}

Optimizer *sgdNew(float learningRate, float batchSize) {
    SGD *o = malloc(sizeof(SGD));

    o->update = (void (*)(Optimizer*, Matrix*, Matrix*))sgdUpdate;
    o->learningRatio = learningRate * batchSize;

    return (Optimizer*)o;
}
