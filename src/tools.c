#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "tools.h"

//This function permits to print the image in the renderer. Only for tests.
void printImage(SDL_Renderer *ren,SDL_Surface *sur, int x, int y)
{
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren,sur);
    SDL_Rect dst;
    SDL_QueryTexture(tex,NULL,NULL,&dst.w,&dst.h);
    dst.x = x;
    dst.y = y;
    dst.w = 1000;
    dst.h = 1200;
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
Uint32 getpixel(SDL_Surface *surface, int x, int y)
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
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

            case 4:
                return *(Uint32 *)p;
                break;

            default:
                return 0;
    }
}

//From a basic image, transform all pixels in white pixels or black pixels
//depending on the pixel's luminosity.
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
            SDL_GetRGB(getpixel(sur, x,y),
                    sur->format, &rgb.r, &rgb.g, &rgb.b);
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
Matrix *greyToMatrix(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    Matrix *matrix = matrixZero(height, width);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++) {
            //Only the r channel, because r == g == b
            Uint8 r;
            //Resize
            Uint32 pixel = getpixel(surface, j, i);
            SDL_GetRGB(pixel, surface->format, &r, &r, &r);
            matrixSet(matrix, i, j, r / 255.0f);
        }
    }
    return matrix;
}

Matrix *loadImage(const char *path) {
    SDL_Surface *sur = SDL_LoadBMP(path);

    if (!sur) {
        fprintf(stderr,"loadImage : Invalid bmp file %s", path);
        return NULL;
    }

    imageToGrey(sur);
    Matrix *matrix = greyToMatrix(sur);

    SDL_FreeSurface(sur);

    return matrix;
}

//Transforms a surface based on a matrix, the matrix and the surface
//need to have the same size.
void matrixToGrey(SDL_Surface *surface, Matrix *matrix) {
	int width = surface->w;
	int height = surface->h;
	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			float matrixValue = matrixGet(matrix, i, j);
			Uint8 average = matrixValue * 255.0f;
			Uint32 pixel = SDL_MapRGB(surface->format,average,
					average, average);
			setpixel(surface, j, i, pixel);
		}
	}
}

//Put the lines in a dynamic array.
dyn_arr getLines(Matrix *matrix, rectangle bloc) {
	point c = bloc.c;
	point b = bloc.b;
	size_t cx = c.x;
	size_t cy = c.y;
	size_t bx = b.x;
	size_t by = b.y;
	int accu = 0;
	//1) Stocks all the black pixels in a array.
   	dyn_arr array_lines;
    	array_lines.array = (int*) malloc(sizeof(int) * 2000);
    	array_lines.length = 0;
	for (size_t i = by; i < cy; i++) {
		int is_empty = 0;
		size_t j = cx;
		while (is_empty == 0 && j < bx) {
			float pixel_color = matrixGet(matrix, i, j);
			if (pixel_color != 1)
				is_empty--;
			j++;
		}
		if (is_empty < 0) {
			array_lines.array[accu] = i;
			array_lines.length++;
			accu++;
		}
	}

	//2)On this array, we take only the pixels that are far away from one 
	//another.
	dyn_arr array_lines2;
    	array_lines2.array = (int*) malloc(sizeof(int) * 500);
	array_lines2.length = 1;
	array_lines2.array[0] = array_lines.array[0];
	accu = 1;
	//We add a number if two pixels are far from one another.
	for (int j = 1; j < array_lines.length - 1; j++) {
		if (array_lines.array[j - 1] != array_lines.array[j] - 1) {
			array_lines2.array[accu] = array_lines.array[j - 1]
			       	+ 1;
			accu++;
			array_lines2.array[accu] = array_lines.array[j] - 1;
			accu++;
		}
	}
	array_lines2.length = accu + 1;
	array_lines2.array[array_lines2.length - 1] = 
		array_lines.array[array_lines.length - 1];
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
            matrixSet(matrix, arraylines.array[i], j, 0);
        }
    }
    free(arraylines.array);
}

//Separates the characters on the image.
void drawCaracters(Matrix *matrix, dyn_arr arraylines,int top, int down)
{
    for (int j = 0; j < arraylines.length; j++)
    {
        for (int i = top; i <down; i++)
        {
            matrixSet(matrix, i, arraylines.array[j], 0);
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
				matrixSet(matrix, i, j, 1.0);
			else
				matrixSet(matrix, i, j, 0.0);
		}
	}
}

//Keeps the value between 0.0 and 1.0.
float preventOverflow(float value) {
	if (value > 1.0)
		return 1.0;
	if (value < 0.0)
		return 0.0;
	return value;
}

//Return the convolution matrix of matrix with convo
Matrix *convolution(Matrix *matrix, Matrix *convo) {
	size_t rows1 = matrix->rows;
	size_t rows2 = convo->rows;
	size_t cols1 = matrix->cols;
	size_t cols2 = convo->cols;
	Matrix *result = matrixZero(rows1, cols1);
	//Indexes for matrix : i and j, for convo : k and l.
	for (size_t i = 0; i < rows1; i++) {
		for (size_t j = 0; j < cols1; j++) {
			float accu = 0;
			for (size_t k = 0; k < rows2; k++) {
				for (size_t l = 0; l < cols2; l++) {
					size_t x = i + k - (rows2 / 2);
					size_t y = j + l - (cols2 / 2);
					if (x < rows1 && y < cols1 
					 && x > 0 && y > 0) {
						float mat_val = 
						       matrixGet(matrix, x, y);
						float conv_val = 
						       matrixGet(convo, k, l);	
						accu += mat_val * conv_val;
					}
				}
			}
			accu = preventOverflow(accu);
			matrixSet(result, i, j, accu);
		}
	}
	//Edge Handeling (not the best)
	for (size_t i = 0; i < rows1; i++) {
		matrixSet(result, i, 0, 1);
	}
	for (size_t j = 0; j < cols1; j++) {
		matrixSet(result, 0, j, 1);;
	}
	return result;
}

//Cut the square horizontally
void horizontalCut(Matrix *matrix, rectangle bloc, size_t threshold, 
		rect_arr *arr, short krisbool) {
	point b = bloc.b;
	point c = bloc.c;
	size_t bx = b.x;
	size_t by = b.y;
	size_t cy = c.y;
	size_t cx = c.x;
	//We search all the colomn looking for a blank line.
	size_t i = by;
	//accu represents the number of correct colomns.
	size_t accu = 0;
	//begin is a boolean which indicates if we habe already find a black 
	//pixel.
	//1) Search a black pixel.
	short begin = 0;
	while (begin != 1 && i < cy) {
		size_t j = cx;
		while (begin != 1 && j < bx) {
			//If we find a black pixel.
			if (matrixGet(matrix, i, j) != 1) {
				begin++;
			}
			j++;
		}
		i++;
	}
	//2) Go in depth to see if we can split.
	while (begin != 0 && i < cy && accu < threshold) {
		//clean is a boolean to know if a line is blank or not.
		short clean = 1;
		size_t j = cx;
		//If it is we continue until threshold.
		while (clean != 0 && j < bx) {
			//If it isn't we restart on the following lines.
			if (matrixGet(matrix, i, j) != 1) {
				clean--;
				accu = 0;
			}
			j++;
		}
		if (j == bx) {
			accu++;
		} 
		i++;
	}
	//3) Split the blocs and do the recursion.
	//If we arrive at threshold, we split in half the bloc and restart 
	//the slicing on both.
	if (accu == threshold && i != 1) {
		rectangle up = {b, {cx, i - 1}};
		rectangle down = {{bx, i}, c};
		horizontalCut(matrix, up, threshold, arr, 0);
		horizontalCut(matrix, down, threshold, arr, 0);
	}
	//If we cannot split, we execute verticalCut on it.
	else if (accu != 0) {
		if (krisbool != 0) {
			arr->array[arr->length] = bloc;
			arr->length++;	
		}
		else {
			verticalCut(matrix, bloc, 30, arr, 1);
		}
	}
}

void verticalCut(Matrix *matrix, rectangle bloc, size_t threshold, 
		rect_arr *arr, short krisbool) {
	point b = bloc.b;
	point c = bloc.c;
	size_t bx = b.x;
	size_t by = b.y;
	size_t cx = c.x;
	size_t cy = c.y;
	//We search all the vertical line looking for a blank horizontal line.
	//On parcourt toute la ligne à la recherche d'une ligne blanche
	size_t i = cx;
	//accu represents the number of correct colomns.
	size_t accu = 0;
	//begin is used as a boolean for if we already find a black pixel
	short begin = 0;
	while (begin != 1 && i < bx) {
		size_t j = by;
		while (begin != 1 && j < cy) {
			//If we find a black pixel
			if (matrixGet(matrix, j, i) != 1) {
				begin++;
			}
			j++;
		}
		i++;
	}
	while (begin != 0 && i < bx && accu < threshold) {
		//clear is used as a boolean for knowing if the line is 
		//empty or not
		short clean = 1;
		size_t j = by;
		//If it's empty, we continue until threshold
		while (clean != 0 && j < cy) {
			//If it's not, we restart on the following lines
			if (matrixGet(matrix, j, i) != 1) {
				clean--;
				accu = 0;
			}
			j++;
		}
		if (j == cy) {
			accu++;
		}
		i++;
	}
	//If we arrive at threshold, we split the bloc in half and restart 
	//the slicing on both
	if (accu == threshold && i != 1) {
		rectangle left = {{i - 1, by}, c};
		rectangle right = {b, {i, cy}};
		verticalCut(matrix, left, threshold, arr, 0);
		verticalCut(matrix, right, threshold, arr, 0);
	}
	//If we can't, we stock the array.
	else if (accu != 0) {
		if (krisbool != 0) {
			arr->array[arr->length] = bloc;
			arr->length++;
		}
		else {
			horizontalCut(matrix, bloc, 60, arr, 1);
		}
	}
}

//Draw a rectangle on the matrix with a rectangle
void drawRectangle(Matrix *matrix, rectangle rec) {
	point b = rec.b;
	point c = rec.c;
	size_t ax = c.x;
	size_t ay = b.y;
	size_t bx = b.x;
	size_t cy = c.y;
	//AB and CD
	for (size_t i = ax; i < bx; i++) {
		matrixSet(matrix, ay, i, 0);
		matrixSet(matrix, cy, i, 0);
	}
	//AC and BD
	for (size_t j = ay; j < cy; j++) {
		matrixSet(matrix, j, ax, 0);
		matrixSet(matrix, j, bx, 0); 
	}
}

//Turns the image on a teta angle
Matrix *rotation(Matrix *matrix, double angle) {
	double teta = (2 * 3.141559 * angle) / 360.0;
	double costeta = cos(-teta);
	double sinteta = sin(-teta);
	int rows = (int)matrix->rows;
	int cols = (int)matrix->cols;
	int x0 = cols / 2;
	int y0 = rows / 2;
	Matrix *result = matrixZero(rows, cols);
	matrixAdd(result, 1.0);
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			int xoff = x - x0;
			int yoff = y - y0;
			int x2 = (int)(xoff * costeta -yoff * sinteta + x0);
			int y2 = (int)(xoff * sinteta +yoff * costeta + y0);
			if (x2 >= 0 && y2 >= 0 && x2 < cols && y2 < rows) {
				float value = matrixGet(matrix, y, x);
				matrixSet(result, y2, x2, value);
			}
		}
	}
	return result;
}

double angleDetection(Matrix *matrix) {
	size_t rows = matrix->rows;
	size_t cols = matrix->cols;
	size_t dmax = sqrt(rows * rows + cols * cols);
	int tableau[dmax][90];

	//1) Fill the matrix with zeros.
	for (size_t d = 0; d < dmax; d++) {
		for (size_t theta = 0; theta < 90; theta++) {
			tableau[d][theta] = 0;
		}
	}

	//2) Fill the matrix with the correct values.
	for (size_t x = 0; x < cols; x++) {
		for (size_t y = 0; y < rows; y++) {
			if (matrixGet(matrix, y, x) == 0) {
				for (size_t theta = 0; theta < 90; theta++) {
					size_t d = y * sin(theta) + x * 
						cos(theta);
					tableau[d][theta]++;
				}
			}
		}
	}

	//3) Finds max of the matrix
	size_t max = 0;
	double result = 0.0;
	for (size_t d = 0; d < dmax; d++) {
		for (size_t theta = 0; theta < 90; theta++) {
			int value = tableau[d][theta]; 
			if ((size_t) value > max) {
				max = value;
				result = (double) theta;
			}
		}
	}
	
	return result;
}
