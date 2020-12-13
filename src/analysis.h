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

void matrixImage(SDL_Renderer *renderer, Matrix *m);

int analysis(SDL_Renderer *renderer, Matrix *mat, int top, int down, int right, int left, int **result);

void CaractersAnalysis(SDL_Renderer *ren, Matrix *mat, int top, int down);

// --- Cc --- //
// Returns all chars between startY to endY
void lineAnalysis(const Matrix *image, int startY, int endY,
        rectangle **boxes, Matrix **matrices, size_t *nchars);

#endif
