#ifndef DATA_H
#define DATA_H

// Dataset related functions

#include <stddef.h>
#include "matrix.h"

// Gathers a batch of images
typedef struct Dataset_t {
    size_t count;

    // Arrays containing images and labels of size count
    Matrix **images;
    unsigned char *labels;
} Dataset;

// Generates a dataset with a path to a folder containing bmp files
Dataset *datasetNew(const char *dirPath);

void datasetFree(Dataset *d);

// Show an image in ascii art
void imagePrint(const Matrix *img);

#endif // DATA_H
