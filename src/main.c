#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "tools.h"
#include "matrix.h"
#include "error.h"
#include "tools.h"
#include "layers.h"
#include "losses.h"
#include "initializer.h"
#include "optimizer.h"
#include "data.h"

int imgMain() {
    //Initialisation
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        fprintf(stderr,"Erreur initialisation\n");
        return -1;
    }
    //Image Loading
    SDL_Surface *sur = SDL_LoadBMP("res/image.bmp");
    if (!sur)
    {
        fprintf(stderr,"Ne trouve pas l'image\n");
        return -1;
    }
    imageToGrey(sur);
    Matrix *matrix = greyToMatrix(sur);
    matrixToGrey(sur, matrix);
    dyn_arr dar = getLines(matrix);
    dyn_arr dar2 = getCaracters(matrix,0,dar.array[0]);
        drawCaracters(matrix,dar2,0,dar.array[0]);
    for (int i = 1; i+1 < dar.length; i+=2)
        {
            dyn_arr dar2 = getCaracters(matrix,dar.array[i],dar.array[i+1]);
            drawCaracters(matrix,dar2,dar.array[i],dar.array[i+1]);
        }
    drawLines(matrix, dar);
    matrixToGrey(sur, matrix);
    matrixFree(matrix);
    SDL_CreateWindowAndRenderer(1000, 800,0,&win,&ren);
    if (!win || !ren)
    {
        fprintf(stderr,"Erreur a la creation des fenetres\n");
        SDL_Quit();
        return -1;
    }
    SDL_SetRenderDrawColor(ren,0,0,255,255);
    SDL_RenderClear(ren);
    printImage(ren,sur,0,0);
    SDL_RenderPresent(ren);
    SDL_Delay(120000);
    //Closure
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void netMain() {
    // Hyper parameters
    static const float learningRate = 1e-2;
    static const unsigned int batchSize = 4;
    static const size_t epochs = 2000;
    static const size_t dispFreq = 100;

    // Dataset
    const float xData[] = {
            0, 0,
            0, 1,
            1, 0,
            1, 1,
        };
    const float yData[] = {
            0,
            1,
            1,
            0,
        };
    Matrix *xs[batchSize];
    Matrix *ys[batchSize];

    for (size_t i = 0; i < batchSize; ++i) {
        xs[i] = matrixCreate(2, 1, &xData[2 * i]);
        ys[i] = matrixCreate(1, 1, &yData[i]);
    }

    // Network
    Layer *fc1 = denseNew(2, 4);
    Layer *activation1 = sigmoidNew();
    Layer *fc2 = denseNew(4, 1);
    LossFunction criterion = mseLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize);

    // Just to display it at the end
    Matrix *display[batchSize];

    for (size_t e = 1; e <= epochs; ++e) {
        float avgLoss = 0;

        // For each batch
        for (size_t batch = 0; batch < batchSize; ++batch) {
            Matrix *x = xs[batch];
            Matrix *y = ys[batch];

            // Feed forward
            Matrix *y1 = layerForward(fc1, x, true);
            Matrix *y2 = layerForward(activation1, y1, true);
            Matrix *y3 = layerForward(fc2, y2, true);

            // Save to display
            if (e == epochs)
                display[batch] = matrixCopy(y3);

            Loss *error = criterion(y3, y);

            // Back propagate
            Matrix *dLoss = error->grad;
            Matrix *dLossDY3 = layerBackward(fc2, dLoss);
            Matrix *dLossDY2 = layerBackward(activation1, dLossDY3);
            Matrix *dLossDY1 = layerBackward(fc1, dLossDY2);

            avgLoss += error->loss;

            // Free (each epoch)
            lossFree(error);
            matrixFree(dLossDY1);
            matrixFree(dLossDY2);
            matrixFree(dLossDY3);
            matrixFree(y3);
            matrixFree(y2);
            matrixFree(y1);
        }

        avgLoss /= batchSize;

        // Display result
        if (e % dispFreq == 0) {
            // matrixPrint(x);
            printf("Epoch %4lu", e);
            printf(", Loss : %.1e\n", avgLoss);
            // printf(", Prediction : ");
            // matrixPrint(y3);
        }

        // Optimize (each batch)
        layerUpdate(fc2, opti);
        layerUpdate(activation1, opti);
        layerUpdate(fc1, opti);
    }

    // Show results
    for (size_t i = 0; i < batchSize; ++i) {
        printf("x = %.2f %.2f\n", xs[i]->data[0], xs[i]->data[1]);
        printf("y = %.2f (should be %.2f)\n\n",
                display[i]->data[0], ys[i]->data[0]);
    }

    // Free batch (each phase)
    for (size_t i = 0; i < batchSize; ++i) {
        matrixFree(display[i]);
        matrixFree(xs[i]);
        matrixFree(ys[i]);
    }

    // Free network
    layerFree(fc1);
    layerFree(fc2);
    layerFree(activation1);
    optimizerFree(opti);
}

int dataMain() {
    // Init sdl
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"Can't initialize SDL\n");
        return -1;
    }

    // // Load & print a bmp image //
    // Matrix *img = loadImage("data/dataset_bmp/A_Roboto.bmp");
    // printf("Loaded a %zux%zu image\n", img->cols, img->rows);

    // // Pretty print the image
    // for (size_t i = 0; i < img->cols; ++i)
    //     putchar('-');
    // putchar('\n');

    // for (size_t i = 0; i < img->rows; ++i) {
    //     for (size_t j = 0; j < img->cols; ++j)
    //         putchar(matrixGet(img, i, j) > .5f ? ' ' : '.');
    //     putchar('\n');
    // }

    // for (size_t i = 0; i < img->cols; ++i)
    //     putchar('-');
    // putchar('\n');

    // matrixFree(img);

    // Load & show images of a dataset //
    Dataset *dataset = datasetNew("data/dataset_bmp");

    printf("Labels of the dataset :");
    for (size_t i = 0; i < dataset->count; ++i)
        printf(" %c", dataset->labels[i]);
    puts("");

    puts("First image :");
    imagePrint(dataset->images[0]);

    datasetFree(dataset);

    SDL_Quit();

    return 0;
}

int main(int argc,
        char **argv) {
    // Initialization
    srand(time(NULL));

    int err = 0;
    if (argc != 2)
        err = 1;
    else if (strcmp(argv[1], "data") == 0)
        return dataMain();
    else if (strcmp(argv[1], "img") == 0)
        return imgMain();
    else if (strcmp(argv[1], "net") == 0)
        netMain();
    else
        err = 1;

    if (err) {
        puts("usage:");
        puts("    ocr img   Run image processing demo");
        puts("    ocr net   Run neural network demo");
    }

    return 0;
}
