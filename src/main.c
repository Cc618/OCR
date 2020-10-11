#include <stdio.h>
#include <math.h>
#include "matrix.h"

int main() {
    const float data1[] = {
            1, 2,
            0, 1,
            2, 3,
        };

    const float data2[] = {
            2, 5,
            6, 7,
            1, 8,
        };

    Matrix *a = matrixCreate(3, 2, data1);
    Matrix *b = matrixCreate(3, 2, data2);
    Matrix *m = matrixDotT(a, b);

    matrixPrint(m);

    matrixFree(a);
    matrixFree(b);
    matrixFree(m);

    return 0;
}

