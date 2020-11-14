#ifndef LOSSES_H
#define LOSSES_H

// Loss functions
// A loss function is almost like a layer, it takes an output and a target
// as input and returns the loss scalar with dLoss / dOutput

#include "matrix.h"

typedef struct Loss_t {
    float loss;
    // dLoss / dOutput
    Matrix *grad;
} Loss;

// Loss between network's output and the label
typedef  Loss *(*LossFunction)(const Matrix *outputProbs, char targetLabel);

void lossFree(Loss *l);

// TODO : Reimplement
// Mean squared error : 1 / (2 * N) * (sum of (y_i - target_i) ^ 2)
// dLoss / dY_i : 1 / N * (y_i - target_i)
// Loss *mseLoss(const Matrix *y, const Matrix *target);

// Negative log likelihood loss
// Used with softmax to make a categorical crossentropy loss
// !!! Use it only with softmax (only one gradient item != 0)
Loss *nllLoss(const Matrix *y, unsigned int target);

#endif // LOSSES_H
