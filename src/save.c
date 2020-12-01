#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "save.h"
#include "error.h"

#define WEIGHT_SUFFIX ".matrix"

// May be not defined by the system
#ifndef strdup
char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    void *str = malloc(len);

    return (char*)memcpy(str, s, len);
}
#endif

SaveContext *saveContextNew(const char *directory) {
    SaveContext *ctx = malloc(sizeof(SaveContext));
    ctx->directory = strdup(directory);
    ctx->directoryLen = strlen(directory);
    ctx->weightId = 0;

    return ctx;
}

void saveContextFree(SaveContext *ctx) {
    free(ctx->directory);
    free(ctx);
}

char *saveContextNextPath(SaveContext *ctx) {
    ASSERT(ctx->weightId < 10000, "Invalid weight id (too big)");

    char *path = malloc(ctx->directoryLen + 4 + sizeof(WEIGHT_SUFFIX) + 1);
    sprintf(path, "%s%zu%s", ctx->directory, ctx->weightId, WEIGHT_SUFFIX);

    ++ctx->weightId;

    return path;
}

void saveContextSave(SaveContext *ctx, const Matrix *m) {
    char *path = saveContextNextPath(ctx);

    matrixSave(path, m);
    free(path);
}

Matrix *saveContextLoad(SaveContext *ctx) {
    char *path = saveContextNextPath(ctx);

    Matrix *m = matrixLoad(path);
    free(path);

    return m;
}

void matrixSave(char name[], const Matrix *m) {
	size_t rows = m->rows;
	size_t cols = m->cols;

	FILE *file;

	file = fopen(name, "w");

	fprintf(file, "%zu", rows);
	fprintf(file, " %zu", cols);

	for (size_t i=0; i<rows; i++) {
		for (size_t j=0; j<cols; j++) {
			fprintf(file, " %f", MAT_GET(m, i, j));
		}
	}

	fclose(file);
}

Matrix *matrixLoad(char name[]) {
	size_t rows;
	size_t cols;

	FILE *file;

	file = fopen(name, "r");

	if (file == NULL) {
		exit(EXIT_FAILURE);
	}

	fscanf(file, "%zu", &rows);
	fscanf(file, "%zu", &cols);

	size_t range = rows*cols;
	float values[range];

	for (size_t k=0; k<range; k++) {
		fscanf(file, "%f", &values[k]);
	}

	fclose(file);

	return matrixCreate(rows, cols, values);
}
