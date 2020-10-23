#ifndef OPTIMIZER_H
#define OPTIMIZER_H

// An optimizer is useful to updates layers' weights

#include "matrix.h"

// Interface
typedef struct Optimizer_t Optimizer;
struct Optimizer_t {
    // - update : Updates weights with its gradient grad, reset grads
#define OPTIMIZER_ATTRIBUTES \
    void (*update)(Optimizer *o, Matrix *weights, Matrix *grad);

    OPTIMIZER_ATTRIBUTES;
};

typedef struct SGD_t {
    OPTIMIZER_ATTRIBUTES;

    // = learningRate / batchSize
    float learningRatio;
} SGD;

void optimizerUpdate(Optimizer *o, Matrix *weights, Matrix *grad);

void optimizerFree(Optimizer *o);

// TODO : Momentum
// Stochastic Gradient Descent with momentum optimizer
// w -= learningRate / batchSize * grad
Optimizer *sgdNew(float learningRate, float batchSize);

#endif // OPTIMIZER_H
