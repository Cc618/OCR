#ifndef LAYERS_H
#define LAYERS_H

// Contains neural net's layers

#include <stdbool.h>
#include "matrix.h"

typedef struct Layer_t Layer;
struct Layer_t {
    // TODO : update(optimizer)
    // - free : Frees only data, l is freed in layerFree, can be NULL
#define LAYER_ATTRIBUTES \
    Matrix *(*forward)(Layer *l, const Matrix *x, bool training); \
    Matrix *(*backward)(Layer *l, const Matrix *grad); \
    void (*free)(Layer *l);

    // Declare attributes (always at the top)
    LAYER_ATTRIBUTES;
};

// A fully connected layer
// Y = WX + B
typedef struct Dense {
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

// Forward propagation
// - training : If true, allocates data for the backward pass
Matrix *layerForward(Layer *l, const Matrix *x, bool training);

// Backward propagation, accumulate gradients
// - grad : dLoss / dOutput
// - Returns dLoss / dInput
// Must free allocated data in forward
Matrix *layerBackward(Layer *l, const Matrix *grad);

void layerFree(Layer *l);

// - in : Input features
// - out : Output features
Layer *denseNew(size_t in, size_t out);

#endif // LAYERS_H

