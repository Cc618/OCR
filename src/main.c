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
#include "ai.h"
#include "save.h"

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
static const size_t dispFreq = 5;
static const float learningRate = 1e-3;
static const unsigned int batchSize = 100;
static const size_t epochs = 15;
static const float momentum = .1f;

static void trainCallback(size_t epoch, size_t batch, float loss) {
    if (batch % dispFreq == 0)
        printf("Epoch %4zu, batch %3zu, loss %.2e\n", epoch, batch, loss);
}

void netMain() {
    Dataset *dataset = datasetNew("data/dataset_bmp");

    size_t nClasses = dataset->labelCount;
    printf("Loaded dataset containing %zu classes, %zu images\n",
            nClasses, dataset->count);

    // Build network
    Layer *layers[] = {
            denseNew(32 * 32, 128),
            sigmoidNew(),
            denseNew(128, nClasses),
            softmaxNew(),
        };
    LossFunction criterion = nllLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize, momentum);

    Network *net = networkNew(sizeof(layers) / sizeof(Layer*), layers, flatten,
            opti, criterion);

    // Net train
    puts("--- Train ---");
    train(net, dataset, epochs, batchSize, trainCallback);

    // Show results (accuracy)
    puts("\n--- Accuracy ---");
    float loss = 0;
    size_t ok = 0;
    float avgProb = 0;
    for (size_t i = 0; i < dataset->count; ++i) {
        Matrix *pred = networkPredict(net, dataset->images[i]);

        avgProb += pred->data[dataset->labels[i]];

        Loss *error = criterion(pred, dataset->labels[i]);
        loss += error->loss;
        lossFree(error);

        // Argmax
        size_t imax = 0;
        for (size_t j = 1; j < pred->rows; ++j)
            if (pred->data[j] > pred->data[imax])
                imax = j;

        if (imax == dataset->labels[i])
            ++ok;

        matrixFree(pred);
    }

    loss /= dataset->count;
    avgProb /= dataset->count;

    printf("%zu / %zu correct predictions (%.1f %%)\n", ok, dataset->count,
            (float)ok / dataset->count * 100);
    printf("Total loss : %.4e\n", loss);
    printf("Average prob : %.2e\n", avgProb);

    // Single image result
    puts("\n--- Single Prediction ---");
    size_t indices[] = { 20, 40, 60, 80, 100, 200, 300, 400, 500, 600 };
    for (size_t i = 0; i < sizeof(indices) / sizeof(size_t); ++i) {
        size_t imageIndex = indices[i];
        ASSERT(imageIndex < dataset->count, "Too large index, not enough "
                "samples in the dataset");

        Matrix *x = dataset->images[imageIndex];
        unsigned char y = dataset->labels[imageIndex];
        char class = dataset->label2char[y];

        printf("Prediction #%zu\n", i + 1);
        printf("Giving a '%c' image to the network\n", class);

        Prediction imagePred = predict(net, dataset, x);

        printf("Predicted a '%c' with probability %.1f %%\n\n",
                imagePred.best, imagePred.prob * 100.f);
    }

    // Free
    networkFree(net);
    datasetFree(dataset);
}

// Display an image of the dataset
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
        printf(" %c", dataset->label2char[dataset->labels[i]]);
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
