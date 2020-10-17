#include <stdio.h>
#include <math.h>
#include <time.h>
#include "matrix.h"
#include "layers.h"
#include "initializer.h"

int main(int __attribute__((unused)) argc,
        char __attribute__((unused)) **argv) {
    // Dataset

    srand(time(NULL));

    const float xData[] = {
            0, 1,
        };
    Matrix *x = matrixCreate(2, 1, xData);

    // Network
    Layer *fc = denseNew(2, 4);
    Layer *activation = sigmoidNew();

    // Feed forward
    Matrix *y1 = layerForward(fc, x, true);
    Matrix *y2 = layerForward(activation, y1, true);

    // Back propagate
    // TODO : Real loss
    Matrix *dLoss = matrixLike(y2);
    Matrix *dLossDY2 = layerBackward(activation, dLoss);
    Matrix *dLossDY1 = layerBackward(fc, dLossDY2);

    // Display result
    matrixPrint(x);
    puts("> Result :");
    matrixPrint(y2);
    puts("> Last gradient :");
    matrixPrint(dLossDY1);

    // Free (each epoch)
    matrixFree(dLossDY1);
    matrixFree(dLossDY2);
    matrixFree(dLoss);
    matrixFree(y2);
    matrixFree(y1);

    // Free (each phase)
    matrixFree(x);

    // Free network
    layerFree(activation);

    // Create test matrices
    Matrix *a = randNormal(2, 3);
    Matrix *b = randNormal(3, 2);
    Matrix *c = randNormal(3, 3);

    // Display results
    matrixPrint(a);
    matrixPrint(b);
    matrixPrint(c);

    // // Display 5000 values created with randomFloatNorm() to verify normal distribution
    // float list[5000];
    // for (int i=0; i<5000; i++) {
    //     list[i] = randFloatNorm();
    // }
    // for (float k= -3; k < 3; k+=0.1) {
    //     int count = 0;
    //     for (int j = 0; j < 5000; j++) {
    //         float current = list[j];
    //         if (k < current && current < k+0.1) {
    //             count += 1;
    //         }
    //     }
    //     for (int l = 0; l < count; l++) {
    //         printf("=");
    //     }
    //     printf("\n");
    // }

    // Free test matrices
    matrixFree(a);
    matrixFree(b);
    matrixFree(c);
    layerFree(fc);

    return 0;
}

