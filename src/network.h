#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>
#include "matrix.h"
#include "optimizer.h"
#include "layers.h"
#include "losses.h"

typedef struct Network_t {
    // All layers ordered sequentially
    size_t nLayers;
    Layer **layers;
} Network;

// TODO : Load network (setup only eval mode)

// Layers will be freed within networkFree
Network *networkNew(size_t nLayers, Layer *const *layers);

// Forward propagation
Matrix *networkPredict(Network *net, const Matrix *x);

void networkFree(Network *net);

#endif // NETWORK_H
