#ifndef LAYERS_H
#define LAYERS_H

// Contains neural net's layers

#include <stdbool.h>
#include "matrix.h"
#include "optimizer.h"

typedef struct Layer_t Layer;
struct Layer_t {
    // - free : Frees only data, l is freed in layerFree, can be NULL
    // - update : Can be NULL
#define LAYER_ATTRIBUTES \
    Matrix *(*forward)(Layer *l, const Matrix *x, bool training); \
    Matrix *(*backward)(Layer *l, const Matrix *grad); \
    void (*update)(Layer *l, Optimizer *o); \
    void (*free)(Layer *l);

    // Declare attributes (always at the top)
    LAYER_ATTRIBUTES;
};

// A fully connected layer
// Y = WX + B
typedef struct Dense_t {
    // Inherit from Layer
    LAYER_ATTRIBUTES;

    // Shape : [out, in]
    Matrix *weight;
    // Shape : [out, 1]
    Matrix *bias;
    Matrix *gradWeight;
    Matrix *gradBias;

    // Copy of x in forward
    Matrix *x;
} Dense;

// Function mapped to the input
typedef struct Activation_t {
    // Inherit from Layer
    LAYER_ATTRIBUTES;

    // y = activation(x)
    void (*activation)(Matrix *x);
    // dLoss / dX = dLoss / dY * dY / dX
    void (*prime)(Matrix *x);

    // Copy of x in forward
    Matrix *x;
} Activation;

// Softmax layer
typedef struct Softmax_t {
    // Inherit from Layer
    LAYER_ATTRIBUTES;

    // Copy of y in forward
    Matrix *y;
} Softmax;

// Forward propagation
// - training : If true, allocates data for the backward pass
Matrix *layerForward(Layer *l, const Matrix *x, bool training);

// Backward propagation, accumulate gradients
// - grad : dLoss / dOutput
// - Returns dLoss / dInput
// Must free allocated data in forward
Matrix *layerBackward(Layer *l, const Matrix *grad);

// Updates weights with computed gradients
void layerUpdate(Layer *l, Optimizer *opti);

void layerFree(Layer *l);

// - in : Input features
// - out : Output features
Layer *denseNew(size_t in, size_t out);

// Sigmoid activation layer
Layer *sigmoidNew();

// Softmax "activation" (not a subclass) layer
Layer *softmaxNew();

#endif // LAYERS_H
