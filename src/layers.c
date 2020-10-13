#include "layers.h"

Matrix *layerForward(Layer *l, const Matrix *x) {
    return l->forward(l, x);
}

Matrix *layerBackward(Layer *l, const Matrix *grad) {
    return l->backward(l, grad);
}

void layerFree(Layer *l) {
    l->free(l);

    free(l);
}

