#include "tools.h"
//Print the image.
void printImage(SDL_Renderer * ren,SDL_Surface * sur, int x, int y) //This function permits to print the image in the renderer. Only for tests.
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
void setPixel(SDL_Surface *sur, int x, int y, Uint32 pixel)
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
/*
//Test function
void modifImage(SDL_Renderer * ren,SDL_Surface * sur)//From a basic image, tranform all dark pixels in red pixels
{
    SDL_Color rgb;
    SDL_PixelFormat * format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    SDL_LockSurface(sur);
    Uint32* pixels = (Uint32*)sur->pixels;
    int x = 0;
    int y = 0;
    while (x<sur->w)
    {
        y=0;
        while(y<sur->h)
        {
            SDL_GetRGB(getpixel(sur, x, y), sur->format, &rgb.r, &rgb.g, &rgb.b);
            //printf("(%d %d %d %d)\n",rgb.r, rgb.g, rgb.b, rgb.a);
            if (rgb.r==0 && rgb.g == 0 && rgb.b == 0)
            {
                setPixel(sur,x,y,SDL_MapRGBA(sur->format, 255, 0, 0, 255));
            }
            y++;
        }
        x++;
    }
}
//Test function
int redToBlack(int argc, char** argv)
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
}*/
//Take an image, size it, and put in it matrix.
/*void imageToMatrix(SDL_Surface *surface) {
    int sizeofmatrix = 32;
    int static matrix[sizeofmatrix][sizeofmatrix];
    for (i = 0; i < sizeofmatrix; i++)
    {
        for (j = 0; j < sizeofmatrix; j++)
            matrix[i][j] = getpixel(*surface, (i * surface->w) / sizeofmatrix, (j * surface->h) / sizeofmatrix);
    } 
}*/
