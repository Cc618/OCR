#ifndef OCR_H
#define OCR_H

#include <SDL2/SDL.h>
#include "network.h"
#include "data.h"

// Returns the text within sur
char *ocr(SDL_Surface *sur, Network *net, Dataset *dataset, int angle);

#endif // OCR_H
