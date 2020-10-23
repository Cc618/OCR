#include <stdio.h>
#include <SDL2/SDL.h>
#include "tools.h"
#include "matrix.h"

int main()
{
    //Initialisation
	/*SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	if (SDL_Init(SDL_INIT_EVERYTHING)<0)
	{
		fprintf(stderr,"Erreur initialisation\n");
		return -1;
	}
	//Image Loading	
	SDL_Surface *sur = SDL_LoadBMP("res/image.bmp");
	if (!sur)
	{
		fprintf(stderr,"Ne trouve pas l'image\n");
		return -1;	
	}
	imageToGrey(sur);
	//Matrix *matrix = greyToMatrix(sur, sur->w, sur->h);
	//matrixToGrey(sur, matrix);
	//matrixFree(matrix);
	SDL_CreateWindowAndRenderer(1400, 1000,0,&win,&ren);
	if (!win || !ren)
	{
		fprintf(stderr,"Erreur a la creation des fenetres\n");
		SDL_Quit();
		return -1;
	}
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderClear(ren);
	printImage(ren,sur,0,0);
	SDL_RenderPresent(ren);
	SDL_Delay(8000);
	//Closure
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();*/
	Matrix *matrix = matrixZero(8, 8);
	matrixSet(matrix, 4, 4, 1);
	matrixSet(matrix, 5, 3, 1);
	matrixPrint(matrix);
	dyn_arr dar = getLines(matrix);
	drawLines(matrix, dar);
	printf("\n");
	matrixPrint(matrix);
	return 0;
}
