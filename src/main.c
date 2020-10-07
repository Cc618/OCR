#include <stdio.h>
#include "matrix.h"

int main() {
    Matrix *m = matrixNew(3, 2);

    matrixSet(m, 0, 1, 42.f);
    matrixPrint(m);

    matrixFree(m);

    return 0;
}

