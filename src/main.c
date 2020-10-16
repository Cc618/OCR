#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "layers.h"

int main(int __attribute__((unused)) argc, char __attribute__((unused)) **argv) {
    const float xData[] = {
            0, 1,
        };
    Matrix *x = matrixCreate(2, 1, xData);

    Layer *fc = denseNew(2, 4);

    // Feed forward
    Matrix *y = layerForward(fc, x, true);
    matrixAdd(y, 1);
    matrixSigmoidPrime(y);

    // Back propagate
    // y isn't the grad but has the same shape
    Matrix __attribute__((unused)) *grad = layerBackward(fc, y);

    matrixPrint(x);
    puts("> Result :");
    matrixPrint(y);

    matrixFree(y);
    layerFree(fc);
    matrixFree(x);

    return 0;
}

