#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "matrix.h"
#include "layers.h"
#include "losses.h"
#include "initializer.h"
#include "optimizer.h"

void imgMain() {
    // TODO
}

void netMain() {
    // Hyper parameters
    static const float learningRate = 1e-2;
    static const unsigned int batchSize = 4;
    static const size_t epochs = 1500;
    static const size_t dispFreq = 100;

    // Dataset
    const float xData[] = {
            0, 0,
            0, 1,
            1, 0,
            1, 1,
        };
    const float yData[] = {
            0,
            1,
            1,
            0,
        };
    Matrix *xs[batchSize];
    Matrix *ys[batchSize];

    for (size_t i = 0; i < batchSize; ++i) {
        xs[i] = matrixCreate(2, 1, &xData[2 * i]);
        ys[i] = matrixCreate(1, 1, &yData[i]);
    }

    // Network
    Layer *fc1 = denseNew(2, 4);
    Layer *activation1 = sigmoidNew();
    Layer *fc2 = denseNew(4, 1);
    // TODO Layer *activation2 = sigmoidNew();
    LossFunction criterion = mseLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize);

    // Just to display it at the end
    Matrix *display[batchSize];

    for (size_t e = 1; e <= epochs; ++e) {
        float avgLoss = 0;

        // For each batch
        for (size_t batch = 0; batch < batchSize; ++batch) {
            Matrix *x = xs[batch];
            Matrix *y = ys[batch];

            // Feed forward
            Matrix *y1 = layerForward(fc1, x, true);
            Matrix *y2 = layerForward(activation1, y1, true);
            Matrix *y3 = layerForward(fc2, y2, true);

            // Save to display
            if (e == epochs)
                display[batch] = matrixCopy(y3);

            Loss *error = criterion(y3, y);

            // Back propagate
            Matrix *dLoss = error->grad;
            Matrix *dLossDY3 = layerBackward(fc2, dLoss);
            Matrix *dLossDY2 = layerBackward(activation1, dLossDY3);
            Matrix *dLossDY1 = layerBackward(fc1, dLossDY2);

            avgLoss += error->loss;

            // Free (each epoch)
            lossFree(error);
            matrixFree(dLossDY1);
            matrixFree(dLossDY2);
            matrixFree(dLossDY3);
            matrixFree(y3);
            matrixFree(y2);
            matrixFree(y1);
        }

        avgLoss /= batchSize;

        // Display result
        if (e % dispFreq == 0) {
            // matrixPrint(x);
            printf("Epoch %4lu", e);
            printf(", Loss : %.1e\n", avgLoss);
            // printf(", Prediction : ");
            // matrixPrint(y3);
        }

        // Optimize (each batch)
        layerUpdate(fc2, opti);
        layerUpdate(activation1, opti);
        layerUpdate(fc1, opti);
    }

    // Show results
    for (size_t i = 0; i < batchSize; ++i) {
        printf("x = %.2f %.2f\n", xs[i]->data[0], xs[i]->data[1]);
        printf("y = %.2f (should be %.2f)\n\n", display[i]->data[0], ys[i]->data[0]);
    }

    // Free batch (each phase)
    for (size_t i = 0; i < batchSize; ++i) {
        matrixFree(display[i]);
        matrixFree(xs[i]);
        matrixFree(ys[i]);
    }

    // Free network
    layerFree(fc1);
    layerFree(fc2);
    layerFree(activation1);
    optimizerFree(opti);
}

int main(int argc,
        char **argv) {
    // Initialization
    srand(time(NULL));

    int err = 0;
    if (argc != 2)
        err = 1;
    else if (strcmp(argv[1], "img") == 0)
        imgMain();
    else if (strcmp(argv[1], "net") == 0)
        netMain();
    else
        err = 1;

    if (err) {
        puts("usage:");
        puts("    ocr img   Run image processing demo");
        puts("    ocr net   Run neural network demo");
    }

    return 0;
}
