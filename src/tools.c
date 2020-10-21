#include <stdio.h>
#include "matrix.h"
#include "tools.h"
#include <SDL2/SDL.h>

//This function permits to print the image in the renderer. Only for tests.
void printImage(SDL_Renderer * ren,SDL_Surface * sur, int x, int y) 
{
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren,sur);
    SDL_Rect dst;
    SDL_QueryTexture(tex,NULL,NULL,&dst.w,&dst.h);
    dst.x = x;
    dst.y = y;
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_RenderPresent(ren);
}

//Set a pixel with the x and y and pixel is the color.
void setpixel(SDL_Surface *sur, int x, int y, Uint32 pixel)
{
    int nbOctetsPerPixel = sur->format->BytesPerPixel;
    //p is the address of the pixel we want to modify
    Uint8 *p = (Uint8 *)sur->pixels + y * sur->pitch + x * nbOctetsPerPixel;
    switch(nbOctetsPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

//Take the color of a pixel with the x and y.
Uint32 getpixel(SDL_Surface *surface, int x, int y) //This function permits to have the Uint32 value of a pixel.
{
    int bpp = surface->format->BytesPerPixel;
    //Here p is the address to the pixel we want to retrieve
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else {
                return p[0] | p[1] << 8 | p[2] << 16;
                break;
	    }

            case 4:
                return *(Uint32 *)p;
                break;

            default:
                return 0;
    }
}

//From a basic image, transform all pixels in black pixels or black pixels depending on the pixel's luminosity.
void modifImage(SDL_Surface *sur)
{
    SDL_Color rgb;
    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    SDL_LockSurface(sur);
    //Uint32* pixels = (Uint32*)sur->pixels;
    int x = 0;
    int y = 0;
    int c = 0;
    while (x<sur->w)
    {
        y=0;
        while(y<sur->h)
        {
            SDL_GetRGB(getpixel(sur, x, y), sur->format, &rgb.r, &rgb.g, &rgb.b);
            //Calcul de la luminosite du pixel, avec comme seuil 119.
            c = (int)(0.299*rgb.r+0.587*rgb.g+0.114*rgb.b);
            c = (c>119)*255;
            if (rgb.r==0 && rgb.g == 0 && rgb.b == 0)
                setpixel(sur,x,y,SDL_MapRGBA(format, c, c, c, 255));
            y++;
        }
        x++;
    }
}

/*
//Test function
int main(int argc, char** argv)
{
    //Initialisation
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	if (SDL_Init(SDL_INIT_EVERYTHING)<0)
	{
		fprintf(stderr,"Erreur initialisation\n");
		return -1;
	}
	SDL_CreateWindowAndRenderer(500,500,0,&win,&ren);
	if (!win || !ren)
	{
		fprintf(stderr,"Erreur a la creation des fenetres\n");
		SDL_Quit();
		return -1;
	}
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderClear(ren);
	//Image Loading
	SDL_Surface * sur = SDL_LoadBMP("image2.bmp");
	printImage(ren,sur,0,0);
	//modifImage is only a test which permits to see if the conversion of pixels with a certain color in another color works.
	modifImage(ren,sur);
	printImage(ren,sur,100,0);
	SDL_RenderPresent(ren);
	SDL_Delay(4000);
	//Closure
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
} */

//Greyscale the image.
void imageToGrey(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Uint32 pixel = getpixel(surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
            r = g = b = average;
            pixel = SDL_MapRGB(surface->format, average, average, average);
            setpixel(surface, i , j, pixel); 
        }
    }
}

//Take an grey only image, size it, and put in it matrix. *surface the image, 
//rows and cols the height and weight of the new matrix.
Matrix greyToMatrix(SDL_Surface *surface, int rows, int cols) {
    Matrix *matrix = matrixNew(rows, cols);
    int width = surface->w;
    int height = surface->h;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++) {
            //Only the r channel, because r == g == b
            Uint8 r;
            //Resize
            Uint32 pixel = getpixel(surface, (i * width) / rows, (j * height) / cols);
            SDL_GetRGB(pixel, surface->format, &r, &r, &r);
            matrixSet(matrix, i, j, r);
        }
    }
    return *matrix;
}

//Returns an array of lines with the length of this array.
dyn_arr getLines(Matrix *matrix) {
    int cols = matrix->cols;
    int rows = matrix->rows;
    dyn_arr array_lines;
    array_lines.array = malloc(sizeof(int) * 32);
    array_lines.length = 0;
    int accu = 0;
    int true_length = 32;
    //Travels the matrix vertically, looking for a black pixel.
    for (int i = 0; i < cols; i++) {
        float pixel_color = matrixGet(matrix, i, cols / 2);
        //If it has been found.
        if (pixel_color == 1) {
            int j = 0;
            //We look if there is no other black pixels on this line
            while (matrixGet(matrix, i, j) != 1 && j < rows)
                j++;
            //If it was a line
            if (j == rows) {
                array_lines.length++;
                if (true_length <= accu)
                {
                    array_lines.array = realloc(array_lines.array, sizeof(int) * 10);
                    true_length += 10;
                }
                array_lines.array[accu] = i;
            }
        }
    }
    return array_lines;
}

//Draws the lines with an array that comes from getLines.
void drawLines(Matrix *matrix, dyn_arr arraylines) {
    int cols = matrix->cols;
    for (int i = 0; i < arraylines.length; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrixSet(matrix, arraylines.array[i], j, 0);
        } 
    }
    free(arraylines.array);
}

//Transforms a matrix into a binary matrix(changes directly the matrix).
void matrixToBinary(Matrix *matrix) {
	int rows = matrix->rows;
	int cols = matrix->cols;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			float value = matrixGet(matrix, i, j);
			if (value > 0.5)
				matrixSet(matrix, i, j, 1);
			else
				matrixSet(matrix, i, j, 0);
		}
	}
}

//Apply a convolution matrix on a another matrix
/*void convolution(Matrix *matrix, Matrix *convolution_matrix) {

}
*/
