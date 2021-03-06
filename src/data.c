#include "data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <linux/limits.h>
#include "error.h"
#include "tools.h"

#define NO_LABEL 0xff

Dataset *datasetNew(const char *dirPath) {
    // Contains an image and a link to all previous images
    // This is a singly linked list with back links
    typedef struct ImageItem_t {
        Matrix *image;
        unsigned char label;

        struct ImageItem_t *previous;
    } ImageItem;

    // Stat directory
    DIR *d;
    struct dirent *dir;
    d = opendir(dirPath);
    ASSERT(d, "datasetNew : Invalid directory");

    // Current file path (only one string is used)
    char *path = malloc(PATH_MAX + 1);
    path[PATH_MAX] = '\0';
    size_t dirPathLen = strlen(dirPath);
    memcpy(path, dirPath, dirPathLen);
    path[dirPathLen] = '/';

    // Prepare list
    size_t itemCount = 0;
    ImageItem *item = NULL;

    // Enumerate items
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(".", dir->d_name) == 0 ||
                strcmp("..", dir->d_name) == 0)
            continue;

        unsigned char label = dir->d_name[0];

        // Update path
        size_t nameLen = strlen(dir->d_name);
        memcpy(path + dirPathLen + 1, dir->d_name, nameLen + 1);

        // Create new item and append it to the list
        ImageItem *nextItem = malloc(sizeof(ImageItem));
        nextItem->image = loadImage(path);
        nextItem->label = label;
        nextItem->previous = item;
        item = nextItem;

        ++itemCount;
    }

    // Create dataset
    Dataset *dataset = malloc(sizeof(Dataset));
    dataset->count = itemCount;
    dataset->images = malloc(itemCount * sizeof(Matrix*));
    dataset->labels = malloc(itemCount * sizeof(char));

    dataset->labelCount = 0;
    for (size_t i = 0; i < 256; ++i) {
        dataset->label2char[i] = (char)NO_LABEL;
        dataset->char2label[i] = NO_LABEL;
    }

    // Iterate backwards the list
    size_t i = itemCount - 1;
    while (item) {
        dataset->images[i] = item->image;

        // Generate label
        char chr = item->label;

        // This char has no labels
        if (dataset->char2label[(unsigned)chr] == NO_LABEL) {
            // Link new label to this char
            unsigned char label = dataset->labelCount++;
            dataset->char2label[(unsigned)chr] = label;
            dataset->label2char[label] = chr;
        }

        dataset->labels[i] = dataset->char2label[(unsigned char)chr];

        // Take previous item and free the current one (not its content)
        ImageItem *oldItem = item;
        item = oldItem->previous;
        free(oldItem);
        --i;
    }

    free(path);
    closedir(d);

    return dataset;
}

void datasetFree(Dataset *d) {
    for (size_t i = 0; i < d->count; ++i) {
        matrixFree(d->images[i]);
    }
    free(d->images);
    free(d->labels);
    free(d);
}

void imagePrint(const Matrix *img) {
    for (size_t i = 0; i < img->cols; ++i)
        putchar('-');
    putchar('\n');

    for (size_t i = 0; i < img->rows; ++i) {
        for (size_t j = 0; j < img->cols; ++j)
            putchar(matrixGet(img, i, j) > .5f ? ' ' : '.');
        putchar('\n');
    }

    for (size_t i = 0; i < img->cols; ++i)
        putchar('-');
    putchar('\n');
}

// Just space separated values
void datasetSave(Dataset *d, const char *path) {
    FILE *file;
    file = fopen(path, "w");

    if (!file) {
        fprintf(stderr, "File : %s\n", path);
        ERROR("Can't open this file");
    }

    for (size_t i = 0; i < d->labelCount; ++i)
        fprintf(file, "%c ", d->label2char[i]);

    fclose(file);
}

void datasetLoad(Dataset *d, const char *path) {
    FILE *file;
    file = fopen(path, "r");

    if (!file) {
        fprintf(stderr, "File : %s\n", path);
        ERROR("Can't open this file");
    }

    char c, space;
    size_t i = 0;
    for ( ; i < 256 && fscanf(file, "%c%c", &c, &space) != EOF; ++i) {
        d->label2char[i] = c;
        d->char2label[(unsigned char)c] = i;
    }

    d->labelCount = i;

    fclose(file);
}
