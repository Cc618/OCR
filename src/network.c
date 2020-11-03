#include "network.h"
#include <string.h>

Network *networkNew(size_t nLayers, Layer *const *layers,
        Optimizer *opti, LossFunction criterion) {
    Network *net = malloc(sizeof(Network));

    net->nLayers = nLayers;
    net->layers = malloc(nLayers * sizeof(Layer*));
    memcpy(net->layers, layers, nLayers * sizeof(Layer*));

    net->opti = opti;
    net->criterion = criterion;

    return net;
}

Matrix *networkPredict(Network *net, const Matrix *x) {
    Matrix *y = matrixCopy(x);

    // Sequentially apply all layers to x
    for (size_t layer = 0; layer < net->nLayers; ++layer) {
        Matrix *oldY = y;
        y = layerForward(net->layers[layer], y, false);

        matrixFree(oldY);
    }

    return y;
}

void networkFree(Network *net) {
    for (size_t i = 0; i < net->nLayers; ++i)
        layerFree(net->layers[i]);

    free(net->layers);

    if (net->opti)
        free(net->opti);

    free(net);
}
