#include "network.h"
#include <string.h>

Network *networkNew(size_t nLayers, Layer *const *layers) {
    Network *net = malloc(sizeof(Network));

    net->nLayers = nLayers;
    net->layers = malloc(nLayers * sizeof(Layer*));
    memcpy(net->layers, layers, nLayers * sizeof(Layer*));

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
    free(net);
}
