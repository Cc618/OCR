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
#include "train.h"

#define PRINT_SIZE(X) printf("Size(%zu, %zu)\n", (X)->rows, (X)->cols);

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

// Hyper parameters
static const size_t dispFreq = 1;
static const size_t logFreq = 1;
static const float learningRate = 2e-3;
static const unsigned int batchSize = 16;
static const size_t epochs = 10;

static void trainCallback(size_t epoch, size_t batch, float loss) {
    if (batch % dispFreq == 0) {
        printf("\rEpoch %4zu, batch %3zu, loss %.1e", epoch, batch, loss);

        if (epoch % logFreq == 0 && batch == 0)
            puts("");
    }
}

void netMain() {
    // Replace by datasetNew for images
    Dataset *dataset = datasetNew("data/dataset_bmp");

    // Build network
    // TODO : Reduce size (detect nLabels)
    Layer *layers[] = {
            denseNew(32 * 32, 512),
            sigmoidNew(),
            // denseNew(256, 128),
            // sigmoidNew(),
            // denseNew(128, 32),
            // sigmoidNew(),
            denseNew(512, 256),
            softmaxNew(),
        };
    // LossFunction criterion = nllLoss;
    LossFunction criterion = nllLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize);

    Network *net = networkNew(sizeof(layers) / sizeof(Layer*), layers, flatten,
            opti, criterion);

    // Net train
    puts("--- Train ---");
    train(net, dataset, epochs, batchSize, trainCallback);
    puts("");

    // Show results
    for (size_t i = 19; i < 19 + 6; ++i) {
        // PRINT_SIZE(dataset->images[i]);
        Matrix *pred = networkPredict(net, dataset->images[i]);
        // PRINT_SIZE(pred);

        // printf("x = %.2f %.2f\n", dataset->images[i]->data[0],
        //         dataset->images[i]->data[1]);
        // TODO : Argmax
        printf("probs = %.3f %.3f\n", pred->data['a'], pred->data['A']);

        matrixFree(pred);
    }

    // Free
    networkFree(net);
    datasetFree(dataset);
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
