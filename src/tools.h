#ifndef TOOLS_H
#define TOOLS_H
#include <SDL2/SDL.h>
#include<stdio.h>

//modify the pixel (x,y) in the surface sur with the given color (pixel value)
void setPixel(SDL_Surface* sur, int x, int y, Uint32 pixel);

//returns the Uint32 value of the pixel (x,y) in the surface sur.
Uint32 getPixel(SDL_Surface* sur, int x, int y);

//print the actual surface on the renderer.
void printImage(SDL_Renderer * ren,SDL_Surface * sur, int x, int y);

#endif