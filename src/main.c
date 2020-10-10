#include <stdio.h>
#include "matrix.h"

int main() {
    const float data[6] = {
            1, 4,
            2, 5,
            3, 6,
        };

    Matrix *m = matrixCreate(3, 2, data);
    matrixSet(m, 0, 1, 42.f);

    Matrix *m2 = matrixCopy(m);
    matrixSet(m2, 0, 1, 8.f);

    matrixPrint(m);
    matrixPrint(m2);

    matrixFree(m2);
    matrixFree(m);

    return 0;
}

