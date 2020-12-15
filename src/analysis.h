#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include "matrix.h"
#include "tools.h"

typedef struct CoordList
{
	int value;
	int* coord;
	struct CoordList* next;
} CoordList;

typedef struct ValueList
{
	int value;
	struct ValueList* fusion;
	struct ValueList* next;
} ValueList;

CoordList* newCoordList();

ValueList* newValueList();

int analysis(Matrix *mat, int top, int down, int right, 
		int left, int **result);

void CaractersAnalysis(Matrix *mat, int top, int down, int left, int right);

// --- Cc --- //
// Returns all chars between startY to endY
void lineAnalysis(const Matrix *image, int startY, int endY,
        rectangle **boxes, Matrix **matrices, size_t *nchars);

#endif
