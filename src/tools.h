#ifndef TOOLS_H
#define TOOLS_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include "matrix.h"

typedef struct {
    int* array;
    int length;
} dyn_arr;

typedef struct{
	size_t x;
	size_t y;
} point;

typedef struct{
	point b;
	point c;
} rectangle;

/*typedef struct CoordList CoordList;
typedef struct ValueList ValueList;
struct CoordList
{
    int value;
    int* coord;
    CoordList* next;
};
struct ValueList
{
    int value;
    ValueList* fusion;
    ValueList* next;
};*/

//Print the actual surface on the renderer.
void printImage(SDL_Renderer * ren,SDL_Surface * sur, int x, int y);

//Modify the pixel (x,y) in the surface sur with the given color (pixel value)
void setpixel(SDL_Surface* sur, int x, int y, Uint32 pixel);

//Returns the Uint32 value of the pixel (x,y) in the surface sur.
Uint32 getpixel(SDL_Surface* sur, int x, int y);

//From a basic image, transform all pixels in black pixels or black pixels depending on the pixel's luminosity.
void modifImage(SDL_Surface *sur);

//Greyscale the image.
void imageToGrey(SDL_Surface *surface);

//Take an grey only image, size it, and put in it matrix. *surface the image, 
//rows and cols the height and weight of the new matrix.
Matrix *greyToMatrix(SDL_Surface *surface);

// Loads a bmp image in a matrix (grey shades)
// SDL must be loaded
Matrix *loadImage(const char *path);

//Transforms a surface based on a matrix, the matrix and the surface need to have the same size.
void matrixToGrey(SDL_Surface *surface, Matrix *matrix);

//Returns an array of lines with the length of this array.
dyn_arr getLines(Matrix *matrix);

//Draws the lines with an array that comes from getLines.
void drawLines(Matrix *matrix, dyn_arr arraylines);

//Stores the characters on a dynamic array.
dyn_arr getCaracters(Matrix *matrix,int top, int down);

//Separates the characters on the image.
void drawCaracters(Matrix *matrix, dyn_arr arraylines,int top, int down);

//Transforms a matrix into a binary matrix(changes directly the matrix).
void matrixToBinary(Matrix *matrix);

//Keeps the value between 0.0 and 1.0.
float preventOverflow(float value);

//Convolution on matrix with convo.
Matrix *convolution(Matrix *matrix, Matrix *convo);

//Draw a rectangle on the matrix
void drawRectangle(Matrix *matrix, rectangle rec);

#endif
