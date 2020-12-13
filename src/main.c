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
#include "analysis.h"
#include "gui.h"

#define PRINT_SIZE(X) printf("Size(%zu, %zu)\n", (X)->rows, (X)->cols);

Network *buildNetwork(size_t nClasses);

#define SESSION "1"
#define NET_DIR "weights/"
#define NET_PATH (NET_DIR SESSION "/")
#define DATA_PATH "data/dataset_bmp"

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
    // SDL_Surface *sur = SDL_LoadBMP("res/image.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/arev.bmp"); // Too light
    // SDL_Surface *sur = SDL_LoadBMP("res/arev_bold.bmp");
    SDL_Surface *sur = SDL_LoadBMP("res/cctext.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/cctext2.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/soutenance.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/neurones.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/hello.bmp");
    // SDL_Surface *sur = SDL_LoadBMP("res/v_u.bmp");
    if (!sur)
    {
        fprintf(stderr,"Doesn't find the image.\n");
        return -1;
    }

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
    // TODO : Remove free : drawLines(result, dar);


    // --- Init net ---
    // Load network part
    Network *net = buildNetwork(1);
    Dataset *dataset = malloc(sizeof(Dataset));

    aiLoad(net, dataset, NET_PATH);

    for (int line = 1; line < dar.length; line += 2) {
        // printf("Line %d -> %d\n", dar.array[line - 1], dar.array[line]);

        // We suppose that there are less than 512 chars
        char *lineStr = malloc(512);
        rectangle **boxes = malloc(sizeof(rectangle*) * 512);
        Matrix **charMatrices = malloc(sizeof(Matrix*) * 512);
        size_t nchars;

         lineAnalysis(result,
                dar.array[line - 1], dar.array[line],
                boxes, charMatrices, &nchars);

        for (size_t c = 0; c < nchars; ++c) {
            // // Disp resized
            // for (size_t i = 0; i < 32; ++i) {
            //     for (size_t j = 0; j < 32; ++j)
            //         printf("%c", matrixGet(charMatrices[c], i, j) > .5f ?
            //                 '.' : '#');
            //     puts("");
            // }
            // puts("---");

            Prediction pred = predict(net, dataset, charMatrices[c]);
            lineStr[c] = pred.best;
            // printf("P = %.1f %%\n", pred.prob * 100);

            free(boxes[c]);
            matrixFree(charMatrices[c]);
        }

        lineStr[nchars] = 0;

        printf("> %s\n", lineStr);

        free(boxes);
        free(charMatrices);

        // TODO : Save lineStr
        free(lineStr);
        puts("");

        // return 0;
    }

    // Free net
    networkFree(net);
    free(dataset);


    //Matrix Freedom
    matrixToGrey(sur, result);
    matrixFree(matrix);
    matrixFree(convo);
    matrixFree(convo2);
    matrixFree(inter);
    matrixFree(result);

    //Saving the image
    //SDL_SaveBMP(sur, "res/bloc3.bmp");

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
    SDL_Delay(3000);
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
static const size_t epochs = 50;
static const float momentum = .08f;

static void trainCallback(size_t epoch, size_t batch, float loss) {
    if (batch % dispFreq == 0)
        printf("Epoch %4zu, batch %3zu, loss %.2e\n", epoch, batch, loss);
}

// Builds the network used in the OCR
Network *buildNetwork(size_t nClasses) {
    Layer *layers[] = {
            denseNew(32 * 32, 256),
            sigmoidNew(),
            denseNew(256, 128),
            sigmoidNew(),
            denseNew(128, nClasses),
            softmaxNew(),
        };
    LossFunction criterion = nllLoss;
    Optimizer *opti = sgdNew(learningRate, batchSize, momentum);

    Network *net = networkNew(sizeof(layers) / sizeof(Layer*), layers, flatten,
            opti, criterion);

    return net;
}

int trainMain() {
    puts("----- Training Mode -----");
    printf("> Training network (from scratch) in %s with dataset in %s\n",
            NET_PATH, DATA_PATH);

    Dataset *dataset = datasetNew(DATA_PATH);

    size_t nClasses = dataset->labelCount;
    printf("> Loaded dataset containing %zu classes, %zu images\n",
            nClasses, dataset->count);

    // Build network
    Network *net = buildNetwork(nClasses);

    // Config
    printf("> Config :\n"
            "- %zu layers\n"
            "- Epochs : %zu\n"
            "- Learning rate : %f\n"
            "- Batch size : %u\n"
            "- Momentum : %f\n",
            net->nLayers,
            epochs,
            learningRate,
            batchSize,
            momentum);

    // Net train
    puts("--- Train ---");
    train(net, dataset, epochs, batchSize, trainCallback);

    aiSave(net, dataset, NET_PATH);
    puts("> Network saved");

    // Show results (accuracy)
    puts("\n--- Accuracy ---");
    float loss = 0;
    size_t ok = 0;
    float avgProb = 0;
    for (size_t i = 0; i < dataset->count; ++i) {
        Matrix *pred = networkPredict(net, dataset->images[i]);

        avgProb += pred->data[dataset->labels[i]];

        Loss *error = net->criterion(pred, dataset->labels[i]);
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

    printf("> %zu / %zu correct predictions (%.1f %%)\n", ok, dataset->count,
            (float)ok / dataset->count * 100);
    printf("> Total loss : %.4e\n", loss);
    printf("> Average prob : %.2e\n", avgProb);

    // Single image result
    puts("\n--- Single Prediction ---");
    size_t indices[] = { 20, 40, 60, 80, 100, 200, 300, 400, 500, 600 };
    for (size_t i = 0; i < sizeof(indices) / sizeof(size_t); ++i) {
        size_t imageIndex = indices[i];
        ASSERT(imageIndex < dataset->count, "> Too large index, not enough "
                "samples in the dataset");

        Matrix *x = dataset->images[imageIndex];
        unsigned char y = dataset->labels[imageIndex];
        char class = dataset->label2char[y];

        printf("> Prediction #%zu\n", i + 1);
        printf("- Giving a '%c' image to the network\n", class);

        Prediction imagePred = predict(net, dataset, x);

        printf("- Predicted a '%c' with probability %.1f %%\n\n",
                imagePred.best, imagePred.prob * 100.f);
    }

    // Free
    networkFree(net);
    datasetFree(dataset);

    return 0;
}

int appMain() {
    // Load network part
    Network *net = buildNetwork(1);
    Dataset *dataset = malloc(sizeof(Dataset));

    aiLoad(net, dataset, NET_PATH);

    // Load GUI
    if (SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        fprintf(stderr,"Initialisation error.\n");
        return -1;
    }

    gui();

    // Free everything
    networkFree(net);
    free(dataset);

    return 0;
}

int main(int argc,
        char **argv) {
    // Initialization
    srand(time(NULL));

    // TODO : Remove all *Main() except train

    int err = 0;
    if (argc == 1)
        return appMain();
    else if (argc != 2)
        err = 1;
    else if (strcmp(argv[1], "img") == 0)
        return imgMain();
    else if (strcmp(argv[1], "train") == 0)
        trainMain();
    else
        err = 1;

    if (err) {
        puts("usage:");
        puts("    ocr         Launch the GUI app");
        puts("    ocr train   Train the network (use make dataset to generate"
                " the training data)");
    }

    return 0;
}
