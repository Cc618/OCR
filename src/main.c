#include <stdio.h>
#include <math.h>
#include "matrix.h"

// sin cannot be used since values must be casted
static float fun(float val) {
    return sin(val);
}

int main() {
    const float data[6] = {
            1, 4,
            2, 5,
            3, 6,
        };

    Matrix *m = matrixCreate(3, 2, data);

    matrixMap(m, fun);
    matrixPrint(m);

    matrixFree(m);

    return 0;
}

