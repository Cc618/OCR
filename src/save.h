#ifndef SAVE_H
#define SAVE_H

#include <stdlib.h>
#include "matrix.h"

// Used to store information relative to saving / loading
typedef struct SaveContext_t {
    // Path to the directory where we save all matrices
    // Ends by a trailing '/'
    char *directory;
    size_t directoryLen;
    // Unique identifier for the next weight to load / save
    // A simple counter
    size_t weightId;
} SaveContext;

SaveContext *saveContextNew(const char *directory);

void saveContextFree(SaveContext *ctx);

// Generates a new string containing the path
// to the next weight
char *saveContextNextPath(SaveContext *ctx);

// Saves / loads the next matrix
void saveContextSave(SaveContext *ctx, const Matrix *m);
Matrix *saveContextLoad(SaveContext *ctx);

void matrixSave(char name[], const Matrix *m);

Matrix *matrixLoad(char name[]);

#endif
