#ifndef LAYERS_H
#define LAYERS_H

// Contains neural net's layers

#include "matrix.h"

typedef struct Layer_t {
    // Forward propagation
    Matrix *(*forward)(const Matrix *x);

    // Backward propagation, accumulate gradients
    // - grad : dLoss / dOutput
    Matrix *(*backward)(const Matrix *grad);

    // TODO : update(optimizer)
} Layer;

#endif // LAYERS_H

