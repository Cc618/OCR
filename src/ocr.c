#include "ocr.h"
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

char *ocr(SDL_Surface *sur, Network *net, Dataset *dataset) {
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

    puts("Cc 0");

    //Image to Matrix
    imageToGrey(sur);
    Matrix *matrix = greyToMatrix(sur);
    matrixToGrey(sur, matrix);
    Matrix *inter = convolution(matrix, convo);
    Matrix *result = convolution(matrix, convo2);
    matrixToBinary(result);

    puts("Cc 1");

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

    //Line analysis
    dyn_arr dar = getLines(result);

    puts("Cc 3");
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

        for (size_t c = 0; c < nchars; ++c) {
            Prediction pred = predict(net, dataset, charMatrices[c]);
            text[textLen++] = pred.best;

            free(boxes[c]);
            matrixFree(charMatrices[c]);
        }

        // lineStr[nchars] = 0;

        // printf("> %s\n", lineStr);

        free(boxes);
        free(charMatrices);

        // TODO : Save lineStr
        // free(lineStr);
        // puts("");

        // return 0;
        //
        text[textLen++] = '\n';
    }

    puts("Cc 4");
    text[textLen] = 0;
    printf("Text (%zu) : %s\n", strlen(text), text);

    //Matrix Freedom
    // matrixToGrey(sur, result);
    matrixFree(matrix);
    matrixFree(convo);
    matrixFree(convo2);
    matrixFree(inter);
    matrixFree(result);

    puts("Cc 5");

    //Saving the image
    //SDL_SaveBMP(sur, "res/bloc3.bmp");

    return text;
}
