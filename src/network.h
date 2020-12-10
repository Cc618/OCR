#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>
#include "matrix.h"
#include "optimizer.h"
#include "layers.h"
#include "losses.h"

// Preprocess the input matrix x (before network)
typedef Matrix *(*Preprocessor)(const Matrix *x);

typedef struct Network_t {
    // All layers ordered sequentially
    size_t nLayers;
    Layer **layers;

    // Training components, may be NULL
    Optimizer *opti;
    LossFunction criterion;

    // Other components
    Preprocessor preprocess;
} Network;

// Layers will be freed within networkFree
// Training components may be NULL
Network *networkNew(size_t nLayers, Layer *const *layers,
        Preprocessor preprocess, Optimizer *opti, LossFunction criterion);

// Forward propagation
Matrix *networkPredict(Network *net, const Matrix *x);

// Backward propagation
// Returns the error term
float networkBackward(Network *net, const Matrix *x, unsigned char label);

// Optimize
void networkUpdate(Network *net);

// IO save / load
// path is '/' terminated
void networkSave(const Network *net, const char *path);

void networkLoad(Network *net, const char *path);

void networkFree(Network *net);

#endif // NETWORK_H
