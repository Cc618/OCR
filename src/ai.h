#ifndef AI_H
#define AI_H

// High level network IO

#include "network.h"
#include "data.h"
#include "matrix.h"

// Prediction result
typedef struct Prediction_t {
    // Char with the best probability
    char best;
    // best's probability
    float prob;
} Prediction;

// Returns the char with the highest probability to be within the image
Prediction predict(Network *net, Dataset *dataset, const Matrix *image);

// Main function to save / load everything in the network part
void aiSave(Network *net, Dataset *dataset, const char *path);

void aiLoad(Network *net, Dataset *dataset, const char *path);

#endif // AI_H
