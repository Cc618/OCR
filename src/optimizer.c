#include "optimizer.h"

void optimizerUpdate(Optimizer *o, Matrix *weights, Matrix *grad) {
    o->update(o, weights, grad);
}

void optimizerFree(Optimizer *o) {
    free(o);
}

// --- SGD ---
static void sgdUpdate(Optimizer *o, Matrix *weights, Matrix *grad) {
    float mom = ((SGD*)o)->momentum;

    // Optimize
    Matrix *g = matrixCopy(grad);

    // Momentum : grad[t] = grad[t] * (1 - momentum) + momentum * grad[t-1]
    matrixMul(g, 1 - mom);

    // weights -= learningRate / batchSize * grad = learningRatio * grad
    matrixMul(g, ((SGD*)o)->learningRatio);
    matrixSubMat(weights, g);

    matrixFree(g);

    // Reset
    // (grad[t - 1] * mom / (1 - mom)) * (1 - mom) = grad[t - 1] * mom
    // Avoids grad copies
    matrixMul(grad, mom / (1 - mom));
}

Optimizer *sgdNew(float learningRate, float batchSize, float momentum) {
    SGD *o = malloc(sizeof(SGD));

    o->update = (void (*)(Optimizer*, Matrix*, Matrix*))sgdUpdate;
    o->learningRatio = learningRate * batchSize;
    o->momentum = momentum;

    return (Optimizer*)o;
}
