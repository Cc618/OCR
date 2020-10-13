#ifndef LAYERS_H
#define LAYERS_H

// Contains neural net's layers

#include "matrix.h"

typedef struct Layer_t {
    Matrix *(*forward)(Layer_t *l, const Matrix *x);

    Matrix *(*backward)(Layer_t *l, const Matrix *grad);

    // TODO : update(optimizer)

    // Frees only data, l is freed in layerFree
    void (*free)(Layer_t *l);
} Layer;

// Forward propagation
Matrix *layerForward(Layer *l, const Matrix *x);

// Backward propagation, accumulate gradients
// - grad : dLoss / dOutput
// Must free allocated data in forward
Matrix *layerBackward(Layer *l, const Matrix *grad);

void layerFree(Layer *l);

#endif // LAYERS_H

