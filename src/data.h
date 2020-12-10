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

    // label2char[label] = char
    char label2char[256];
    // char2label[char] = label
    unsigned char char2label[256];
    size_t labelCount;
} Dataset;

// Generates a dataset with a path to a folder containing bmp files
Dataset *datasetNew(const char *dirPath);

void datasetFree(Dataset *d);

// Show an image in ascii art
void imagePrint(const Matrix *img);

// !!! Saves / loads only labels
// - path is a file name
void datasetSave(Dataset *d, const char *path);

void datasetLoad(Dataset *d, const char *path);

#endif // DATA_H
