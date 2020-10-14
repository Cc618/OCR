#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "error.h"
#include "tools.h"

int main(int argc, char** argv) {
    // Matrix transposed dot product test
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

    matrixAddMat(a, b);
    matrixPrint(a);
    matrixSubMat(a, b);
    matrixPrint(a);
    matrixMulMat(a, b);
    matrixPrint(a);
    matrixDivMat(a, b);
    matrixPrint(a);

    matrixFree(a);
    matrixFree(b);
    matrixFree(m);

    return 0;
}

