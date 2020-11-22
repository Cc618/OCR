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
