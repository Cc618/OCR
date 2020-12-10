#include "ai.h"

Prediction predict(Network *net, Dataset *dataset, const Matrix *image) {
    Matrix *pred = networkPredict(net, image);

    // Argmax
    size_t imax = 0;
    for (size_t j = 1; j < pred->rows; ++j)
        if (pred->data[j] > pred->data[imax])
            imax = j;

    Prediction p = {
        .best = dataset->label2char[imax],
        .prob = pred->data[imax],
    };

    matrixFree(pred);

    return p;
}

void aiSave(Network *net, Dataset *dataset, const char *path) {
    size_t pathLen = strlen(path);
    char *datasetPath = malloc(pathLen + sizeof("dataset.data") + 1);
    memcpy(datasetPath, path, pathLen);
    memcpy(datasetPath + pathLen, "dataset.data", sizeof("dataset.data") + 1);

    networkSave(net, path);
    datasetSave(dataset, datasetPath);

    free(datasetPath);
}

void aiLoad(Network *net, Dataset *dataset, const char *path) {
    size_t pathLen = strlen(path);
    char *datasetPath = malloc(pathLen + sizeof("dataset.data") + 1);
    memcpy(datasetPath, path, pathLen);
    memcpy(datasetPath + pathLen, "dataset.data", sizeof("dataset.data") + 1);

    networkLoad(net, path);
    datasetLoad(dataset, datasetPath);

    free(datasetPath);
}
