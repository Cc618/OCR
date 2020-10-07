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
    matrixPrint(m);

    matrixFree(m);

    return 0;
}

