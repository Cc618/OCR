#ifndef TRAIN_H
#define TRAIN_H

#include "network.h"
#include "data.h"

// Called each batch in train
typedef void (*TrainCallback)(size_t epoch, size_t batch, float loss);

// TODO : Save each epoch
void train(Network *net, Dataset *dataset, size_t epochs, size_t batchSize,
        TrainCallback traincb);

// Flatten input preprocessor
Matrix *flatten(const Matrix *x);

#endif // TRAIN_H
