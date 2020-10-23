#include <stdio.h>
#include "matrix.h"
#include "tools.h"
#include <SDL2/SDL.h>

//This function permits to print the image in the renderer. Only for tests.
void printImage(SDL_Renderer *ren,SDL_Surface *sur, int x, int y) 
{
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren,sur);
    SDL_Rect dst;
    SDL_QueryTexture(tex,NULL,NULL,&dst.w,&dst.h);
    dst.x = x;
    dst.y = y;
    dst.w = 1400;
    dst.h = 1000;
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
Matrix *greyToMatrix(SDL_Surface *surface, int rows, int cols) {
    Matrix *matrix = matrixZero(rows, cols);
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
    return matrix;
}

//Transforms a surface based on a matrix, the matrix and the surface need to have the same size.
void matrixToGrey(SDL_Surface *surface, Matrix *matrix) {
	int width = surface->w;
	int height = surface->h;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			float matrixValue = matrixGet(matrix, i, j);
			Uint8 average = 255 * matrixValue;
			Uint32 pixel = SDL_MapRGB(surface->format, average, average, average);
			setpixel(surface, i, j, pixel);
		}
	}
}
dyn_arr getLines(Matrix *matrix) {
	int cols = matrix->cols;
    	int rows = matrix->rows;
	int accu = 0;
	int true_length = 32;
   	dyn_arr array_lines;
    	array_lines.array = malloc(sizeof(int) * 32);
    	array_lines.length = 0;
	for (int i = 0; i < rows; i++) {
		int is_empty = 0;
		int j = 0;
		while (is_empty == 0 && j < cols) {
			float pixel_color = matrixGet(matrix, i, j);
			if (pixel_color != 0)
				is_empty = -1;
			j++;
		}
		if (is_empty == -1) {
			array_lines.array[accu] = i;
			array_lines.length++;
			accu++;
			if (accu >= true_length) {
				array_lines.array = realloc(array_lines.array, sizeof(int) * 10);
                  		true_length += 10;
			}
		}
	}
	dyn_arr array_lines2;
    	array_lines2.array = malloc(sizeof(int) * array_lines.length);
	array_lines2.length = 0;
	accu = 1;
	array_lines2.array[0] = array_lines.array[0] - 1;
	for (int j = 1; j < array_lines.length - 1; j++) {
		if (array_lines.array[j - 1] != array_lines.array[j] - 1) {
			array_lines2.array[accu] = array_lines.array[j] - 1;
			accu++;
		}
	}
	array_lines2.length = accu;
	if (array_lines.length > 1) {
		array_lines2.array[accu] = array_lines.array[array_lines.length - 1] + 1;
		array_lines2.length++;
	}
	free(array_lines.array);
	return array_lines2;

}

//Draws the lines with an array that comes from getLines.
void drawLines(Matrix *matrix, dyn_arr arraylines) {
    int cols = matrix->cols;
    for (int i = 0; i < arraylines.length; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrixSet(matrix, arraylines.array[i], j, 1);
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
