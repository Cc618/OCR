#ifndef TRAIN_H
#define TRAIN_H

#include "network.h"
#include "data.h"

// Preprocess the input matrix x (before network)
typedef Matrix *(*Preprocessor)(const Matrix *x);

// TODO : Save each epoch
void train(Network *net, Dataset *dataset, size_t epochs, size_t batchSize,
        Preprocessor preprocessor);

#endif // TRAIN_H
