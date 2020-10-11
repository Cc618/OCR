#include <stdio.h>
#include <math.h>
#include "matrix.h"

int main() {
    const float data[6] = {
            1, 4,
            2, 5,
            3, 6,
        };

    Matrix *m = matrixCreate(3, 2, data);

    printf("%f\n", matrixGet(m, 2, 0));

    // Should throw
    matrixGet(m, 42, 42);

    matrixPrint(m);

    matrixFree(m);

    return 0;
}

