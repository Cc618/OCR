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
#include "network.h"

int imgMain() {
    //Initialisation
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        fprintf(stderr,"Initialisation error.\n");
        return -1;
    }
    //Image Loading
    SDL_Surface *sur = SDL_LoadBMP("res/image.bmp");
    if (!sur)
    {
        fprintf(stderr,"Doesn't find the image.\n");
        return -1;
    }

    //Matrix Initialisation
    /*Matrix *convo = matrixZero(3, 3);
    matrixSet(convo, 0, 0, -1);
    matrixSet(convo, 0, 1, -1);
    matrixSet(convo, 0, 2, -1);
    matrixSet(convo, 1, 0, -1);
    matrixSet(convo, 1, 1, 8);
    matrixSet(convo, 1, 2, -1);
    matrixSet(convo, 2, 0, -1);
    matrixSet(convo, 2, 1, -1);
    matrixSet(convo, 2, 2, -1);*/

    //Image to Matrix
    imageToGrey(sur);
    Matrix *matrix = greyToMatrix(sur);
    matrixToGrey(sur, matrix);
    //Matrix *result = convolution(matrix, convo);
	
    //Line analysis
    dyn_arr dar = getLines(matrix);
    drawLines(matrix, dar);

    //Character analysis
    /*dyn_arr dar2 = getCaracters(matrix,0,dar.array[0]);
    drawCaracters(matrix,dar2,0,dar.array[0]);
    for (int i = 1; i+1 < dar.length; i+=2)
        {
            dyn_arr dar2 = getCaracters(matrix,dar.array[i],dar.array[i+1]);
            drawCaracters(matrix,dar2,dar.array[i],dar.array[i+1]);
	}*/
	
    //matrixToGrey(sur, result);
    matrixToGrey(sur, matrix);
    matrixFree(matrix);
    //matrixFree(convo);
    //matrixFree(result);

    //Image Printing
    SDL_CreateWindowAndRenderer(1000, 1200,0,&win,&ren);
    if (!win || !ren)
    {
        fprintf(stderr,"Error when building windows.\n");
        SDL_Quit();
        return -1;
    }
    SDL_SetRenderDrawColor(ren,0,0,255,255);
    SDL_RenderClear(ren);
    printImage(ren,sur,0,0);
    SDL_RenderPresent(ren);
    SDL_Delay(8000);
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

    // Build network
    Layer *layers[] = {
            denseNew(2, 4),
            sigmoidNew(),
            denseNew(4, 1)
        };
    LossFunction criterion = mseLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize);

    Network *net = networkNew(3, layers, opti, criterion);

    // // Net predict (eval)
    // puts("--- Predict ---");

    // Matrix *x = xs[0];
    // printf("Predicting value for matrix :\n");
    // matrixPrint(x);

    // Matrix *y = networkPredict(net, x);
    // printf("Output :\n");
    // matrixPrint(y);

    // matrixFree(y);

    // Net train
    puts("--- Train ---");

    for (size_t e = 1; e <= epochs; ++e) {
        float avgLoss = 0;

        // TODO : Use multiple batches for one epoch
        // For each sample
        for (size_t batch = 0; batch < batchSize; ++batch) {
            Matrix *x = xs[batch];
            Matrix *y = ys[batch];

            // Accumulate gradients
            avgLoss += networkBackward(net, x, y);
        }

        avgLoss /= batchSize;

        // Display result
        if (e % dispFreq == 0) {
            printf("Epoch %4lu", e);
            printf(", Loss : %.1e\n", avgLoss);
        }

        // Optimize
        networkUpdate(net);
    }

    // Show results
    for (size_t i = 0; i < batchSize; ++i) {
        Matrix *pred = networkPredict(net, xs[i]);

        printf("x = %.2f %.2f\n", xs[i]->data[0], xs[i]->data[1]);
        printf("y = %.2f (should be %.2f)\n\n", pred->data[0], ys[i]->data[0]);

        matrixFree(pred);
    }

    // Free
    networkFree(net);

    // Free batch (each phase)
    for (size_t i = 0; i < batchSize; ++i) {
        matrixFree(xs[i]);
        matrixFree(ys[i]);
    }
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
