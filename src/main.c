#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "layers.h"

int main() {
    // Outer product test

    const float aData[] = {
            3, 4,
        };

    const float bData[] = {
            1, 2, 10,
        };

    // !!! Contains memory leaks
    matrixPrint(matrixOuter(
                matrixCreate(2, 1, aData), matrixCreate(3, 1, bData)));

    return 0;

    /*
    const float xData[] = {
            0, 1,
        };
    Matrix *x = matrixCreate(2, 1, xData);

    Layer *fc = denseNew(2, 4);

    Matrix *y = layerForward(fc, x, true);
    // TODO : back

    matrixPrint(x);
    puts("> Result :");
    matrixPrint(y);

    matrixFree(y);
    layerFree(fc);
    matrixFree(x);
    */

    return 0;
}

