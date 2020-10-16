#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "layers.h"

int main(
        int __attribute__((unused)) argc,
        char __attribute__((unused)) **argv) {
    // Dataset
    const float xData[] = {
            0, 1,
        };
    Matrix *x = matrixCreate(2, 1, xData);

    // Network
    Layer *fc = denseNew(2, 4);
    Layer *activation = sigmoidNew();

    // Feed forward
    Matrix *y1 = layerForward(fc, x, true);
    Matrix *y2 = layerForward(activation, y1, true);

    // Back propagate
    // TODO : Real loss
    Matrix *dLoss = matrixLike(y2);
    Matrix *dLossDY2 = layerBackward(activation, dLoss);
    Matrix *dLossDY1 = layerBackward(fc, dLossDY2);

    // Display result
    matrixPrint(x);
    puts("> Result :");
    matrixPrint(y2);
    puts("> Last gradient :");
    matrixPrint(dLossDY1);

    // Free (each epoch)
    matrixFree(dLossDY1);
    matrixFree(dLossDY2);
    matrixFree(dLoss);
    matrixFree(y2);
    matrixFree(y1);

    // Free (each phase)
    matrixFree(x);

    // Free network
    layerFree(activation);
    layerFree(fc);

    return 0;
}

