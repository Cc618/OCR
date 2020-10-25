#include <stdio.h>
#include "matrix.h"
#include "tools.h"
#include <SDL2/SDL.h>

CoordList* newCoordList()
{
    CoordList* newList = malloc(sizeof(CoordList*));
    int newcoord[4];
    int x=4;
    while (x)
    {
        x--;
        newcoord[x]=0;
    }
    newList->coord = newcoord;
    newList->next=NULL;
    newList->value=0;
    return newList;
}

ValueList* newValueList()
{
    ValueList* newList = malloc(sizeof(ValueList*));
    newList->next=NULL;
    newList->fusion=NULL;
    newList->value=0;
    return newList;
}

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
            SDL_GetRGB(getpixel(sur, x,y), sur->format, &rgb.r, &rgb.g, &rgb.b);
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

//Transforms a surface based on a matrix, the matrix and the surface
//need to have the same size.
void matrixToGrey(SDL_Surface *surface, Matrix *matrix) {
	int width = surface->w;
	int height = surface->h;
	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			float matrixValue = matrixGet(matrix, i, j);
			Uint8 average = matrixValue * 255.0f;
			Uint32 pixel = SDL_MapRGB(surface->format,average,average, average);
			setpixel(surface, j, i, pixel);
		}
	}
}

//Put the lines into a
dyn_arr getLines(Matrix *matrix) {
	int cols = matrix->cols;
    	int rows = matrix->rows;
	int accu = 0;
   	dyn_arr array_lines;
    	array_lines.array = malloc(sizeof(int) * 2000);
    	array_lines.length = 0;
	for (int i = 0; i < rows; i++) {
		int is_empty = 0;
		int j = 0;
		while (is_empty == 0 && j < cols) {
			float pixel_color = matrixGet(matrix, i, j);
			if (pixel_color != 1)
				is_empty = -1;
			j++;
		}
		if (is_empty == -1) {
			array_lines.array[accu] = i;
			array_lines.length++;
			accu++;
		}
	}
	dyn_arr array_lines2;
    	array_lines2.array = malloc(sizeof(int) * 500);
	array_lines2.length = 0;
	accu = 0;
	//We add a number if two pixels are far from one another.
	for (int j = 1; j < array_lines.length - 1; j++) {
		if (array_lines.array[j - 1] != array_lines.array[j] - 1) {
			array_lines2.array[accu] = array_lines.array[j - 1] + 1;
			accu++;
			array_lines2.array[accu] = array_lines.array[j] - 1;
			accu++;
		}
	}
	array_lines2.length = accu;
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

int analysis(Matrix* mat, int top, int down, int left,int right,int** result)
{
    //First step : we browse the matrix in order to associate each pixel
    //with a value which shows to which sign the pixel owns.
    //During this step, we count the number of value used, the number of
    //symbols detected, the values which owns to a same symbol and the
    //localisation of each value.
    int x = 0;
    int y = top;
    int n = 0;
    int m = 0;
    int h = down-top;
    ValueList** ramValue = malloc(sizeof(ValueList*)*(h+1));
    CoordList** ramCoord = malloc(sizeof(CoordList*)*(h+1));
    while (x<(h)+1)
    {
        ramValue[x]=NULL;
        ramCoord[x]=NULL;
        x++;
    }
    x=left;
    int* column = malloc(sizeof(int)*(h)*3);
    while (x<=right)
    {
        y=top;
        //fusion will memorize all fusions in the current column.
        int fusionscount = (h)*2;
        while (fusionscount < (h)*3)
        {
            column[fusionscount*2]=0;
            fusionscount++;
        }
        fusionscount=0;
        while (y<=down)
        {

            //For each pixel, we look the others pixel at its left and
            //over him. Then we can determine the value of the pixel.
            //If there are multiple values a pixel can have, it means that
            //these values have to be merged.
            if (!matrixGet(mat,y,x))
            {
                int neighbours[4];
                neighbours[0]=0;
                neighbours[1]=0;
                neighbours[2]=0;
                neighbours[3]=0;
                if (x>left)
                {
                    neighbours[2]=(int)matrixGet(mat,y,x-1)*100000;
                    if (y<down)
                    {
                        neighbours[3]=(int)matrixGet(mat,y+1,x-1)*100000;
                    }
                }
                if (y>top)
                {
                    neighbours[0]=(int)matrixGet(mat,y-1,x)*100000;
                    if (x>left)
                    {
                        neighbours[1]=(int)matrixGet(mat,y-1,x-1)*100000;
                    }
                }
                //Here is the determination of the value.
                int z = 1;
                int value = neighbours[0];
                while (z<4)
                {
                    if (neighbours[z])
                    {
                        if (!value)
                        {
                            value = neighbours[z];
                        }
                        else
                        {
                            if (value!=neighbours[z])
                            {
                                //We search if both merges values are in
                                //the column. If it is the case, it means
                                //that they have already been saved.
                                int w = 0;
                                char found = 0;
                                while (w<h && !found)
                                {
                                    if(column[w*2]==value||column[w*2+1]==value)
                                    {
                                        if (column[w*2]==neighbours[z])
                                        {
                                            found=1;
                                            w-=1;
                                        }
                                        else
                                        {
                                            if(column[w*2+1]==neighbours[z])
                                            {
                                                found=1;
                                                w-=1;
                                            }
                                        }
                                    }
                                    w+=1;
                                }
                                if (!found)
                                {
                                    //Else, we search for these values in the
                                    //ramValues. If there is only one value,
                                    //they have already been merge.
                                    ValueList* a1=ramValue[h];
                                    ValueList* a2=ramValue[h];
                                    w=0;
                                    while (w<h)
                                    {
                                        if (ramValue[w]->value==value)
                                        {
                                            a1=ramValue[w];
                                        }
                                        if (ramValue[w]->value==neighbours[z])
                                        {
                                            a2=ramValue[w];
                                        }
                                        w++;
                                    }
                                    if (a1!=ramValue[h] && a2!=ramValue[h])
                                    {
                                        //What remains to do is only to
                                        //say that they will be merged.
                                        column[fusionscount*2]=value;
                                        column[fusionscount*2+1]=neighbours[z];
                                        fusionscount++;
                                    }
                                }
                            }
                        }
                    }
                    z++;
                }
                //If the value found is null, it means that we are on a pixel
                //of a potential new symbol.
                char newvalue = 0;
                if (!value)
                {
                    n++;
                    m++;
                    value = n;
                    newvalue = 1;
                }
                matrixSet(mat,y,x,value*0.00001);
                //And last step : write in the column that the value is inside.
                z=(h)*2;
                while (column[z]!=value && column[z]!=0)
                {
                    z++;
                }
                //Don't forget to update ramCoord.
                if (newvalue)
                {
                    CoordList* newcoord = newCoordList();
                    newcoord->coord[0]=x;
                    newcoord->coord[1]=y;
                    newcoord->coord[2]=x;
                    newcoord->coord[3]=y;
                    newcoord->next=NULL;
                    newcoord->value=value;
                    int adress = 0;
                    while (ramCoord[adress]!=NULL)
                    {
                        adress++;
                    }
                    ramCoord[adress]=newcoord;
                }
                else
                {
                    int adress = 0;
                    while (ramCoord[adress]->value!=value)
                    {
                        adress++;
                    }
                    if (column[z]==0)
                    {
                        ramCoord[adress]->coord[2]=x;
                        //The value width.
                    }
                    if (ramCoord[adress]->coord[3]<y)
                    {
                        ramCoord[adress]->coord[3]=y;
                        //The value max height.
                    }
                    if (ramCoord[adress]->coord[1]>y)
                    {
                        ramCoord[adress]->coord[1]=y;
                        //The value min height.
                    }
                }
                column[z]=value;
            }
            else
            {
                matrixSet(mat,y,x,0);
            }
            y++;
        }
        //Column is finished ! Now we have to update ramValue with earned datas.
        int z =0;
        while (z<(h))
        {
            //Here is the merging...
            if (column[z*2])
            {
                int value = column[z*2];
                int other = column[z*2+1];
                int w1 = -1;
                int w2 = -1;
                int w = 0;
                while (w<h)
                {
                    if (ramValue[w]!=NULL)
                    {
                        if (ramValue[w]->value==value)
                        {
                            w1 = w;
                        }
                        if (ramValue[w]->value==other)
                        {
                            w2 = w;
                        }
                    }
                    w++;
                }
                if (w1 != w2)
                {
                    if (w1 != -1 && w2 != -1)
                    {
                        ValueList* adress = ramValue[w1];
                        while (adress->fusion!=NULL)
                        {
                            adress=adress->fusion;
                        }
                        adress->fusion=ramValue[w2];
                        ramValue[w2]=NULL;
                    }
                    else
                    {
                        ValueList* newList = newValueList();
                        if (w1==-1)
                        {
                            int v = w2;
                            w2=w1;
                            w1=v;
                            v = value;
                            value = other;
                            other = v;
                        }
                        newList->value=other;
                        ValueList* adress = ramValue[w1];
                        while (adress->fusion!=NULL)
                        {
                            adress=adress->fusion;
                        }
                        adress->fusion = newList;
                    }
                    m--;
                    //After the merging has been done, we decrement m because
                    //there is a potential symbol in less.
                    //Now, we replace all other values in the column.
                    int v = 0;
                    while(v<(h)*2)
                    {
                        if (column[v]==other)
                        {
                            column[v]=value;
                        }
                        v++;
                    }
                }
            }
            z++;

        }
        //Now, all values in the ramValue which have not been used in the
        //column have to be removed at the end of the ram, until the end of
        //the matrix browsing.
        z=0;
        while (z<(h))
        {
            if (ramValue[z]!=NULL)
            {
                char found = 0;
                int value = ramValue[z]->value;
                int w = 0;
                while (w<(h) && !found)
                {
                    if (column[w+(h)*2]==value)
                    {
                        found = 1;
                    }
                    w++;
                }
                if (!found||x==right)
                {
                    ramValue[z]->next=ramValue[(h)];
                    ramValue[h]=ramValue[z];
                    ramValue[z]=NULL;
                }
            }
            z++;
        }
        //The same, but with ramCoord.
        z=0;
        while (z<h)
        {
            if (ramCoord[z]!=NULL)
            {
                char found = 0;
                int value = ramCoord[z]->value;
                int w = 0;
                while (w<(h) && !found)
                {
                    if (column[w+(h)*2]==value)
                    {
                        found = 1;
                    }
                    w++;
                }
                if (!found||x==right)
                {
                    ramCoord[z]->next=ramCoord[h];
                    ramCoord[h]=ramCoord[z];
                    ramCoord[z]=NULL;
                }
            }
            z++;
        }
        //Et we can change of columns.
        x++;
    }
    free(column);
    //End of the first step !
    //We have now all the values we need, as n and m.
    //So we'll put datas in arrays.
    int* coord1 = malloc(sizeof(int)*4*n);
    int * values1 = malloc(sizeof(int)*2*m);
    //Let's start by filling coord.
    CoordList* adress = ramCoord[h];
    x = 0;
    while (x<n)
    {
        CoordList* next = adress->next;
        int y = 0;
        while (y<4)
        {
            coord1[4*(adress->value-1)+y]=adress->coord[y];
            y++;
        }
        free(adress);
        adress=next;
        x++;
    }
    //Then, values. By doing this, we will also complete the merges in the
    //matrix.
    x=0;
    ValueList* otherList = ramValue[h];
    while (x<m)
    {
        //Firstly, we have to update global coordonates.
        ValueList* fusion = otherList;
        int * valuecoord = coord1+4*(fusion->value-1);
        while (fusion->fusion!=NULL)
        {
            ValueList* next = fusion->fusion;
            free(fusion);
            fusion = next;
            int* othercoord=coord1+4*(fusion->value-1);
            if (othercoord[0]<valuecoord[0])
            {
                //xmin...
                valuecoord[0]=othercoord[0];
            }
            if (othercoord[1]<valuecoord[1])
            {
                //ymin...
                valuecoord[1]=othercoord[1];
            }
            if (othercoord[2]>valuecoord[2])
            {
                //xmax...
                valuecoord[2]=othercoord[2];
            }
            if (othercoord[3]>valuecoord[3])
            {
                //ymax...
                valuecoord[3]=othercoord[3];
            }
            //Here is the end of the merging.
            int x1 = othercoord[0];
            while (x1<=othercoord[2])
            {
                int y1 = othercoord[1];
                while (y1<=othercoord[3])
                {
                    if (matrixGet(mat,y1,x1)==fusion->value*0.00001)
                    {
                        matrixSet(mat,y1,x1,otherList->value*0.00001);
                    }
                    y1++;
                }
                x1++;
            }
        }
        values1[x]=otherList->value;
    }
    //Now, all datas are ready !
    //The last thing to do is to put all these datas in only one array, without
    //unuseful datas, for exemple coordonates of merged values.
    int * result1 = malloc(sizeof(int)*5*m);
    x=0;
    while (x<m)
    {
        y=1;
        result1[x*5]=values1[x];
        while (y<5)
        {
            result1[x*5+y]=coord1[values1[x]*4+y];
            y++;
        }
        x++;
    }
    free(ramValue);
    free(ramCoord);
    free(coord1);
    free(values1);
    //The array is put in the result, and we return m, which permits to know
    //the length of result.
    result[0]=result1;
    return m;
}
