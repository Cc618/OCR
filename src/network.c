#include "network.h"
#include <string.h>

Network *networkNew(size_t nLayers, Layer *const *layers,
        Preprocessor preprocess, Optimizer *opti, LossFunction criterion) {
    Network *net = malloc(sizeof(Network));

    net->nLayers = nLayers;
    net->layers = malloc(nLayers * sizeof(Layer*));
    memcpy(net->layers, layers, nLayers * sizeof(Layer*));

    net->preprocess = preprocess;
    net->opti = opti;
    net->criterion = criterion;

    return net;
}

Matrix *networkPredict(Network *net, const Matrix *x) {
    Matrix *y = net->preprocess(x);

    // Sequentially apply all layers to x
    for (size_t layer = 0; layer < net->nLayers; ++layer) {
        Matrix *oldY = y;
        y = layerForward(net->layers[layer], y, false);

        matrixFree(oldY);
    }

    return y;
}

float networkBackward(Network *net, const Matrix *x, unsigned char label) {
    // Create 2 stacks
    // Contains forward matrices
    Matrix **forwardStack = malloc(sizeof(Matrix*) * (net->nLayers + 1));
    // Contains backward gradients
    Matrix **backwardStack = malloc(sizeof(Matrix*) * (net->nLayers + 1));

    // Forward
    forwardStack[0] = net->preprocess(x);
    for (size_t i = 0; i < net->nLayers; ++i) {
        forwardStack[i + 1] = layerForward(net->layers[i],
                forwardStack[i], true);
    }

    // Loss
    Loss *error = net->criterion(forwardStack[net->nLayers], label);

    // Backward
    backwardStack[0] = matrixCopy(error->grad);
    for (size_t i = 0; i < net->nLayers; ++i) {
        Layer *layer = net->layers[net->nLayers - i - 1];
        backwardStack[i + 1] = layerBackward(layer, backwardStack[i]);
    }

    float loss = error->loss;

    // Free
    lossFree(error);

    for (size_t i = 0; i <= net->nLayers; ++i) {
        matrixFree(backwardStack[i]);
        matrixFree(forwardStack[i]);
    }

    free(backwardStack);
    free(forwardStack);

    return loss;
}

void networkUpdate(Network *net) {
    // Sequentially apply all layers to x
    for (size_t layer = 0; layer < net->nLayers; ++layer) {
        layerUpdate(net->layers[layer], net->opti);
    }
}

void networkSave(const Network *net, const char *path) {
    SaveContext *ctx = saveContextNew(path);

    for (size_t layer = 0; layer < net->nLayers; ++layer)
        layerSave(net->layers[layer], ctx);

    saveContextFree(ctx);
}

void networkLoad(Network *net, const char *path) {
    SaveContext *ctx = saveContextNew(path);

    for (size_t layer = 0; layer < net->nLayers; ++layer)
        layerLoad(net->layers[layer], ctx);

    saveContextFree(ctx);
}

void networkFree(Network *net) {
    for (size_t i = 0; i < net->nLayers; ++i)
        layerFree(net->layers[i]);

    free(net->layers);

    if (net->opti)
        free(net->opti);

    free(net);
}
