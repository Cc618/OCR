#include <stdio.h>
#include <math.h>
#include <time.h>
#include "matrix.h"
#include "layers.h"
#include "losses.h"
#include "initializer.h"
#include "optimizer.h"

int main(int __attribute__((unused)) argc,
        char __attribute__((unused)) **argv) {
    srand(time(NULL));

    // Hyper parameters
    float learningRate = 5e-2;
    unsigned int batchSize = 4;

    // Dataset
    const float xData[] = {
            0, 1,
        };
    const float yData[] = {
            1
        };
    Matrix *x = matrixCreate(2, 1, xData);
    Matrix *y = matrixCreate(1, 1, yData);

    // Network
    Layer *fc = denseNew(2, 1);
    Layer *activation = sigmoidNew();
    LossFunction criterion = mseLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize);

    static const unsigned long long epochs = 100ull;

    for (unsigned long long e = 1; e <= epochs; ++e) {
        // Feed forward
        Matrix *y1 = layerForward(fc, x, true);
        Matrix *y2 = layerForward(activation, y1, true);

        Loss *error = criterion(y2, y);

        // Back propagate
        Matrix *dLoss = error->grad;
        Matrix *dLossDY2 = layerBackward(activation, dLoss);
        Matrix *dLossDY1 = layerBackward(fc, dLossDY2);

        // Optimize (each batch)
        layerUpdate(fc, opti);
        layerUpdate(activation, opti);

        // Display result
        // matrixPrint(x);
        printf("Epoch %3llu", e);
        printf(", Loss : %.3e", error->loss);
        printf(", Prediction : ");
        matrixPrint(y2);
        // puts("> Last gradient :");
        // matrixPrint(dLossDY1);
        // puts("> Updated gradient (should be 0) :");
        // matrixPrint(((Dense*)fc)->gradWeight);

        // Free (each epoch)
        lossFree(error);
        matrixFree(dLossDY1);
        matrixFree(dLossDY2);
        matrixFree(y2);
        matrixFree(y1);
    }

    // Free (each phase)
    matrixFree(y);
    matrixFree(x);

    // Free network
    layerFree(fc);
    layerFree(activation);
    optimizerFree(opti);

    // // Create test matrices
    // Matrix *a = randNormal(2, 3);
    // Matrix *b = randNormal(3, 2);
    // Matrix *c = randNormal(3, 3);

    // // Display results
    // matrixPrint(a);
    // matrixPrint(b);
    // matrixPrint(c);

    // // Display 5000 values created with randomFloatNorm() to verify normal distribution
    // float list[5000];
    // for (int i=0; i<5000; i++) {
    //     list[i] = randFloatNorm();
    // }
    // for (float k= -3; k < 3; k+=0.1) {
    //     int count = 0;
    //     for (int j = 0; j < 5000; j++) {
    //         float current = list[j];
    //         if (k < current && current < k+0.1) {
    //             count += 1;
    //         }
    //     }
    //     for (int l = 0; l < count; l++) {
    //         printf("=");
    //     }
    //     printf("\n");
    // }

    // // Free test matrices
    // matrixFree(a);
    // matrixFree(b);
    // matrixFree(c);
    // layerFree(fc);

    return 0;
}

