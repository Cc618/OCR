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

    // Training components, may be NULL
    Optimizer *opti;
    LossFunction criterion;
} Network;

// TODO : Load network (setup only eval mode)

// Layers will be freed within networkFree
// Training components may be NULL
Network *networkNew(size_t nLayers, Layer *const *layers,
        Optimizer *opti, LossFunction criterion);

// Forward propagation
Matrix *networkPredict(Network *net, const Matrix *x);

void networkFree(Network *net);

#endif // NETWORK_H
