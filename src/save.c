#include <stdio.h>
#include <stdlib.h>
#include "save.h"

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

	auto range = rows*cols;
	float values[range];

	for (auto k=0; k<range; k++) {
		fscanf(file, "%f", &values[k]);
	}

	fclose(file);

	return matrixCreate(rows, cols, values);
}
