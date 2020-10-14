#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "layers.h"

int main() {
    const float xData[] = {
            0, 1,
        };
    Matrix *x = matrixCreate(2, 1, xData);

    Layer *fc = denseNew(2, 4);

    Matrix *y = layerForward(fc, x, true);

    // y isn't the grad but has the same shape
    Matrix *grad = layerBackward(fc, y);

    matrixPrint(x);
    puts("> Result :");
    matrixPrint(y);

    matrixFree(y);
    layerFree(fc);
    matrixFree(x);

    return 0;
}

