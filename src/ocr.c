#include "ocr.h"
#include <math.h>
#include "tools.h"
#include "matrix.h"
#include "error.h"
#include "tools.h"
#include "layers.h"
#include "losses.h"
#include "initializer.h"
#include "optimizer.h"
#include "data.h"
#include "network.h"
#include "train.h"
#include "ai.h"
#include "save.h"
#include "analysis.h"
#include "gui.h"

#define SPACE_TEMPERATURE 1
#define GAP(i) ((int)boxes[i + 1]->b.x - (int)boxes[i]->c.x)

char *ocr(SDL_Surface *sur, Network *net, Dataset *dataset, int argangle) {
    char cpu_angle = argangle == -1;
    double angle = argangle;

    printf("angle = %f, cpu_angle = %d\n", angle, cpu_angle);

    // TODO : Steve : Implementer angle
    //Matrixes Initialisation
    //Gommage
    Matrix *convo = matrixZero(3, 3);
    matrixSet(convo, 0, 0, 0.1111111);
    matrixSet(convo, 0, 1, 0.1111111);
    matrixSet(convo, 0, 2, 0.1111111);
    matrixSet(convo, 1, 0, 0.1111111);
    matrixSet(convo, 1, 1, 0.1111111);
    matrixSet(convo, 1, 2, 0.1111111);
    matrixSet(convo, 2, 0, 0.1111111);
    matrixSet(convo, 2, 1, 0.1111111);
    matrixSet(convo, 2, 2, 0.1111111);

    //Contrast
    Matrix *convo2 = matrixZero(3, 3);
    matrixSet(convo2, 0, 0, 0);
    matrixSet(convo2, 0, 1, -1);
    matrixSet(convo2, 0, 2, 0);
    matrixSet(convo2, 1, 0, -1);
    matrixSet(convo2, 1, 1, 5);
    matrixSet(convo2, 1, 2, -1);
    matrixSet(convo2, 2, 0, 0);
    matrixSet(convo2, 2, 1, -1);
    matrixSet(convo2, 2, 2, 0);

    //Image to Matrix
    imageToGrey(sur);
    Matrix *matrix = greyToMatrix(sur);
    matrixToGrey(sur, matrix);
    Matrix *inter = convolution(matrix, convo);
    Matrix *result = convolution(matrix, convo2);
    matrixToBinary(result);




    //Angle detection
    if (cpu_angle != 0) {
        angle = angleDetection(matrix);
        printf("DETECTED %f\n", angle);
    }

    //Rotation
    if (angle != 0.0) {
        Matrix *test = rotation(matrix, angle);
        matrix = matrixCopy(test);

        // TODO
        // matrixToGrey(sur, matrix);
        // SDL_SaveBMP(sur, "weights/result.bmp");
    }




    //Block analysis
    /*rectangle bloc = {{result->cols - 1, 0}, {0, result->rows - 1}};
    rectangle *array = malloc(500);
    rect_arr arr = {array, 0};
    horizontalCut(result, bloc, 40, &arr, 1);

    //Print Rectangle Array
    for (size_t i = 0; i < arr.length; i++) {
	rectangle get_rect = arr.array[i];
	drawRectangle(result, get_rect);
	point b = get_rect.b;
	point c = get_rect.c;
	printf("b = (%li, %li) and c == (%li, %li)\n",
		b.x, b.y, c.x, c.y);
    }*/

    rectangle winrect = { {result->cols - 1, 0}, {0, result->rows - 1} };

    //Line analysis
    dyn_arr dar = getLines(result, winrect);

    char *text = malloc(10000);
    text[0] = 0;
    int textLen = 0;
    for (int line = 1; line < dar.length; line += 2) {
        // We suppose that there are less than 512 chars
        // char *lineStr = malloc(512);
        rectangle **boxes = malloc(sizeof(rectangle*) * 512);
        Matrix **charMatrices = malloc(sizeof(Matrix*) * 512);
        size_t nchars;

        lineAnalysis(result,
                dar.array[line - 1], dar.array[line],
                boxes, charMatrices, &nchars);

        // Stats
        double avgGap = 0;
        double stdGap = 0;
        for (size_t i = 0; i < nchars - 1; ++i) {
            double gap = GAP(i);

            if (gap < 0)
                continue;

            avgGap += gap;
        }
        avgGap /= nchars;

        for (size_t i = 0; i < nchars - 1; ++i) {
            double gap = GAP(i);
            if (gap < 0)
                continue;

            stdGap += (avgGap - gap) * (avgGap - gap);
        }
        stdGap = sqrt(stdGap / nchars);

        for (size_t c = 0; c < nchars; ++c) {
            Prediction pred = predict(net, dataset, charMatrices[c]);
            text[textLen++] = pred.best;

            // Detect space
            if (c < nchars - 1) {
                double gap = GAP(c);

                if (gap > avgGap + stdGap * SPACE_TEMPERATURE)
                    text[textLen++] = ' ';
            }

            free(boxes[c]);
            matrixFree(charMatrices[c]);
        }

        free(boxes);
        free(charMatrices);

        text[textLen++] = '\n';
    }

    text[textLen] = 0;

    //Matrix Freedom
    matrixFree(matrix);
    matrixFree(convo);
    matrixFree(convo2);
    matrixFree(inter);
    matrixFree(result);

    return text;
}
