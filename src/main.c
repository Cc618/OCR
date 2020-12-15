#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "tools.h"
#include "matrix.h"
#include "error.h"
#include "tools.h"
#include "layers.h"
#include "losses.h"
#include "initializer.h"
#include "optimizer.h"
#include "data.h"
#include "network.h"
#include "train.h"
#include "ai.h"
#include "save.h"
#include "analysis.h"
#include "gui.h"

Network *buildNetwork(size_t nClasses);

#define SESSION "1"
#define NET_DIR "model/"
// For training mode : #define NET_PATH (NET_DIR SESSION "/")
#define NET_PATH (NET_DIR)
#define DATA_PATH "data/dataset_bmp"

// Hyper parameters
static const size_t dispFreq = 5;
static const float learningRate = 1e-3;
static const unsigned int batchSize = 100;
static const size_t epochs = 30;
static const float momentum = .1f;

static void trainCallback(size_t epoch, size_t batch, float loss) {
    if (batch % dispFreq == 0)
        printf("Epoch %4zu, batch %3zu, loss %.2e\n", epoch, batch, loss);
}

// Builds the network used in the OCR
Network *buildNetwork(size_t nClasses) {
    Layer *layers[] = {
            denseNew(32 * 32, 128),
            sigmoidNew(),
            denseNew(128, 128),
            sigmoidNew(),
            denseNew(128, nClasses),
            softmaxNew(),
        };
    LossFunction criterion = nllLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize, momentum);

    Network *net = networkNew(sizeof(layers) / sizeof(Layer*), layers, flatten,
            opti, criterion);

    return net;
}

int trainMain() {
    puts("----- Training Mode -----");
    printf("> Training network (from scratch) in %s with dataset in %s\n",
            NET_PATH, DATA_PATH);

    Dataset *dataset = datasetNew(DATA_PATH);

    size_t nClasses = dataset->labelCount;
    printf("> Loaded dataset containing %zu classes, %zu images\n",
            nClasses, dataset->count);

    // Build network
    Network *net = buildNetwork(nClasses);

    // Config
    printf("> Config :\n"
            "- %zu layers\n"
            "- Epochs : %zu\n"
            "- Learning rate : %f\n"
            "- Batch size : %u\n"
            "- Momentum : %f\n",
            net->nLayers,
            epochs,
            learningRate,
            batchSize,
            momentum);

    // Net train
    puts("--- Train ---");
    train(net, dataset, epochs, batchSize, trainCallback);

    aiSave(net, dataset, NET_PATH);
    puts("> Network saved");

    // Show results (accuracy)
    puts("\n--- Accuracy ---");
    float loss = 0;
    size_t ok = 0;
    float avgProb = 0;
    for (size_t i = 0; i < dataset->count; ++i) {
        Matrix *pred = networkPredict(net, dataset->images[i]);

        avgProb += pred->data[dataset->labels[i]];

        Loss *error = net->criterion(pred, dataset->labels[i]);
        loss += error->loss;
        lossFree(error);

        // Argmax
        size_t imax = 0;
        for (size_t j = 1; j < pred->rows; ++j)
            if (pred->data[j] > pred->data[imax])
                imax = j;

        if (imax == dataset->labels[i])
            ++ok;

        matrixFree(pred);
    }

    loss /= dataset->count;
    avgProb /= dataset->count;

    printf("> %zu / %zu correct predictions (%.1f %%)\n", ok, dataset->count,
            (float)ok / dataset->count * 100);
    printf("> Total loss : %.4e\n", loss);
    printf("> Average prob : %.2e\n", avgProb);

    // Single image result
    puts("\n--- Single Prediction ---");
    size_t indices[] = { 20, 40, 60, 80, 100, 200, 300, 400, 500, 600 };
    for (size_t i = 0; i < sizeof(indices) / sizeof(size_t); ++i) {
        size_t imageIndex = indices[i];
        ASSERT(imageIndex < dataset->count, "> Too large index, not enough "
                "samples in the dataset");

        Matrix *x = dataset->images[imageIndex];
        unsigned char y = dataset->labels[imageIndex];
        char class = dataset->label2char[y];

        printf("> Prediction #%zu\n", i + 1);
        printf("- Giving a '%c' image to the network\n", class);

        Prediction imagePred = predict(net, dataset, x);

        printf("- Predicted a '%c' with probability %.1f %%\n\n",
                imagePred.best, imagePred.prob * 100.f);
    }

    // Free
    networkFree(net);
    datasetFree(dataset);

    return 0;
}

int appMain() {
    // Load network part
    Network *net = buildNetwork(1);
    Dataset *dataset = malloc(sizeof(Dataset));

    aiLoad(net, dataset, NET_PATH);

    // Load GUI
    if (SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        fprintf(stderr,"Initialisation error.\n");
        return -1;
    }

    gui(net, dataset);

    // Free everything
    networkFree(net);
    free(dataset);

    return 0;
}

int main(int argc,
        char **argv) {
    // Initialization
    srand(time(NULL));
    int err = 0;
    if (argc == 1)
        return appMain();
    else if (argc != 2)
        err = 1;
    else if (strcmp(argv[1], "train") == 0)
        trainMain();
    else
        err = 1;

    if (err) {
        puts("usage:");
        puts("    ocr         Launch the GUI app");
        puts("    ocr train   Train the network (use make dataset to generate"
                " the training data)");
    }

    return 0;
}
