#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tools.c"
#include "matrix.c"

int main()
{
    //Initialisation
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	if (SDL_Init(SDL_INIT_EVERYTHING)<0)
	{
		fprintf(stderr,"Erreur initialisation\n");
		return -1;
	}
	//Image Loading
	SDL_Surface *sur = SDL_LoadBMP("image.bmp");
	if (!sur)
	{
		fprintf(stderr,"Ne trouve pas l'image\n");
		return -1;
	}
	imageToGrey(sur);
	Matrix *matrix = greyToMatrix(sur);
	matrixToGrey(sur, matrix);
	dyn_arr dar = getLines(matrix);
	dyn_arr dar2 = getCaracters(matrix,0,dar.array[0]);
    drawCaracters(matrix,dar2,0,dar.array[0]);
	for (int i = 1; i+1 < dar.length; i+=2)
    {
        dyn_arr dar2 = getCaracters(matrix,dar.array[i],dar.array[i+1]);
        drawCaracters(matrix,dar2,dar.array[i],dar.array[i+1]);
    }
	drawLines(matrix, dar);
	matrixToGrey(sur, matrix);
	matrixFree(matrix);
	SDL_CreateWindowAndRenderer(1000, 800,0,&win,&ren);
	if (!win || !ren)
	{
		fprintf(stderr,"Erreur a la creation des fenetres\n");
		SDL_Quit();
		return -1;
	}
	SDL_SetRenderDrawColor(ren,0,0,255,255);
	SDL_RenderClear(ren);
	printImage(ren,sur,0,0);
	SDL_RenderPresent(ren);
	SDL_Delay(5000);
	//Closure
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
