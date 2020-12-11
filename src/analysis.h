#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include "matrix.h"


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

#endif
