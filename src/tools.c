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

//Transforms a surface based on a matrix, the matrix and the surface need to have the same size.
void matrixToGrey(SDL_Surface *surface, Matrix *matrix) {
	int width = surface->w;
	int height = surface->h;
	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			float matrixValue = matrixGet(matrix, i, j);
			Uint8 average = matrixValue * 255.0f;
			Uint32 pixel = SDL_MapRGB(surface->format, average, average, average);
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

//m contient les données (pixels noir, pixels blancs)
int analyseCharacters(Matrix * mat, int haut, int bas, int gauche, int droite,int** result)
{
    //Premier parcours : on change les valeurs 1 en fonction de la valeur du caractère. On crée une première version de values et de coord, les variables n et m indiquant le nombre de valeurs utilisées et le nombre de caractères détectés, ainsi que ram qui rassemble les données de coord et values immédiatement modifiables.
    int x = 0;
    int y = haut;
    int n = 0;
    int m = 0;
    ValueList* ramValue = malloc(sizeof(ValueList*)*(bas-haut+1));
    CoordList* ramCoord = malloc(sizeof(CoordList*)*(bas-haut+1));
    while (x<(bas-haut)+1);
    {
        ramValue[x]=NULL;
        ramCoord[x]=NULL;
        //Les pointeurs de ram sont tous sur ram : on sait ainsi que ram n'a pas de données particulières.
        //Les premiers pointeurs sont ceux de values, les suivants sont coord, et les deux derniers pointent sur les structures dynamiques de coord et values, qui seront stockées dans l'étape 2 sous la forme d'un tableau, avec les valeurs coord et values comme adresse.
        x++;
    }
    x=gauche;
    int* column = malloc(sizeof(int)*(bas-haut)*3);
    //Début du premier parcours : on va de haut en bas, de gauche jusqu'à droite. Cela devrait à priori réduire le temps de calcul de la seconde étape... Mais l'inverse fonctionne également.
    while (x<=droite)
    {
        y=haut;
        //On crée un tableau fusions, qui va récapituler les valeurs fusionnées et les valeurs présentes dans la colonne. fusionscount est un marqueur.
        int fusionscount = (bas-haut)*2;
        while (fusionscount < (bas-haut)*3)
        {
            column[fusionscount*2]=0;
            fusionscount++;
        }
        fusionscount=0;
        while (y<=bas)
        {

            //Pour chaque pixel, s'il a comme valeur 1 (pixel noir), on regarde les quatre posés à gauche et juste en haut.
            //Si l'un de ces pixels a une valeur différente de 0, les deux pixels sont connectés.
            //Le pixel prend alors la valeur du caractère auquel il se connecte.
            //S'il n'y a pas d'autres pixels, alors il se forme une nouvelle valeur.
            //Si le pixel est connecté à deux valeurs différentes, celles-ci sont connectées (dans values).
            if (matrixGet(mat,y,x))
            {
                int neighbours[4];
                neighbours[0]=0;
                neighbours[1]=0;
                neighbours[2]=0;
                neighbours[3]=0;
                if (x>gauche)
                {
                    neighbours[2]=matrixGet(mat,y,x-1);
                    if (y<bas)
                    {
                        neighbours[3]=matrixGet(mat,y+1,x-1);
                    }
                }
                if (y>haut)
                {
                    neighbours[0]=matrixGet(mat,y-1,x);
                    if (x>gauche)
                    {
                        neighbours[1]=matrixGet(mat,y-1,x-1);
                    }
                }
                //Comparaison des pixels voisins. Z est un compteur, value est la valeur que prendra le pixel
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
                                //On cherche les emplacements des values dans fusion, pour savoir si elles sont déjà liées.
                                int w = 0;
                                char found = 0;
                                while (w<(haut-bas) && !found)
                                {
                                    if (column[w*2]==value || column[w*2+1]==value)
                                    {
                                        if (column[w*2]==neighbours[z] || column[w*2+1]==neighbours[z])
                                        {
                                            found=1;
                                            w-=1;
                                        }
                                    }
                                    w+=1;
                                }
                                if (!found)
                                {
                                    //Si elles ne sont pas dans les fusions, alors soit elles ont été liées avant la boucle actuelle, soit elles ne sont pas liées.
                                    //Dans ce cas, si l'on trouve value et neighbours[z] dans la ram, elles ne sont pas liées.
                                    //Et si l'on ne trouve que l'un des deux facteurs, elles sont liées.
                                    ValueList* a1=ramValue[bas-haut];
                                    ValueList* a2=ramValue[bas-haut];
                                    w=0;
                                    while (w<bas-haut)
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
                                    if (a1!=ramValue[bas-haut] && a2!=ramValue[bas-haut])
                                    {
                                        //Il ne reste plus qu'à fusionner les deux valeurs.
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
                //On a ainsi la valeur du pixel, plus qu'à l'assigner.
                //Si elle est nulle, il faut créer une nouvelle valeur. Il suffit d'augmenter m et n pour cela.
                char newvalue = 0;
                if (!value)
                {
                    n++;
                    m++;
                    value = n;
                    newvalue = 1;
                }
                matrixSet(mat,y,x,value);
                //Et derniere etape : indiquer que cette valeur se trouve dans la colonne.
                z=(bas-haut)*2;
                while (column[z]!=value && column[z]!=0)
                {
                    z++;
                }
                //En n'oubliant pas de modifier les coordonnées.
                if (newvalue)
                {
                    CoordList newcoord = newCoordList();
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
                    int* adress = 0;
                    while (ramCoord[adress]->value!=value)
                    {
                        adress++;
                    }
                    if (column[z]==0)
                    {
                        newcoord->coord[2]=x;
                        //On augmente la largeur de la valeur, car elle n'était jusque là pas présente dans la colonne.
                    }
                    if (newcoord->coord[3]<y)
                    {
                        newcoord->coord[3]=y;
                        //On augmente la hauteur de la valeur, car l'ordonnée actuelle est plus basse que celle déjà enregistrée.
                    }
                    if (newcoord->coord[1]>y)
                    {
                        newcoord->coord[1]=y;
                        //On baisse la hauteur de la valeur, car l'ordonnée actuelle est plus haute que celle déjà enregistrée.
                    }
                }
                column[z]=value;
            }
            y++;
        }
        //La colonne est finie. Maintenant, il faut modifier la ram avec les fusions effectuées et les valeurs créées.
        //Premièrement, les fusions.
        //Notez que si cela semble complexe, il y aura assez peu de fusions au final...
        int z =0;
        while (z<(bas-haut))
        {
            //Les fusions.
            //A chaque fusion, on prend la valeur de ram et on la place après la valeur de fusion, et on remplace tout dans le tableau column.
            //En gros, on fait un chainage de liste dynamique.
            if (column[z*2])
            {
                int value = column[z*2];
                int other = column[z*2+1];
                int w1 = -1;
                int w2 = -1;
                int w = 0;
                while (w<bas-haut)
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
                        //value et other sont dans ram. On prend alors other et on la place tout au bout de la liste des fusions de value.
                        //la place qu'occupait other dans ram devient un pointeur sur la ram.
                        ValueList adress = ram[w1];
                        while (adress->fusion!=NULL)
                        {
                            adress=adress->fusion;
                        }
                        adress->fusion=ramCoord[w2];
                        ramCoord[w2]=NULL;
                    }
                    else
                    {
                        //Dans l'autre cas, on prend la valeur présente dans ram. S'il y a fusion, il y a forcément une valeur dans ram.
                        ValueList newList = newValueList();
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
                        ValueList adress = ramValue[w1];
                        while (adress->fusion!=NULL)
                        {
                            adress=adress->fusion;
                        }
                        adress->fusion = newList;
                    }
                    m--;
                    //On a un caractère potentiel en moins.
                    //Et dernière étape : il faut remplacer toutes les valeurs other de column par la valeur value.
                    int v = 0;
                    while(v<(bas-haut)*2)
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
        //Maintenant, on retire les valeurs de la ram qui ne sont pas apparues dans la colonne.
        //Les valeurs ayant été fusionnées ne sont pour rappel plus dans la colonne.
        z=0;
        while (z<(bas-haut))
        {
            if (ramValue[z]!=NULL)
            {
                char found = 0;
                int value = ramValue[z]->value;
                int w = 0;
                while (w<(bas-haut) && !found)
                {
                    if (column[w+(bas-haut)*2]==value)
                    {
                        found = 1;
                    }
                }
                if (!found)
                {
                    //On place la valeur et toutes celles fusionnées dans la liste dynamique values.
                    ramValue[z]->next=ramValue[(bas-haut)];
                    ramValue[bas-haut]=ramValue[z];
                    ramValue[z]=NULL;
                }
            }
            z++;
        }
        //La même, mais cette fois avec les Coordonnées.
        z=0;
        while (z<bas-haut)
        {
            if (ramCoord[z]!=NULL)
            {
                char found = 0;
                int value = ramCoord[z]->value;
                int w = 0;
                while (w<(bas-haut) && !found)
                {
                    if (column[w+(bas-haut)*2]==value)
                    {
                        found = 1;
                    }
                }
                if (!found)
                {
                    //On place la valeur et ses coordonnées dans la liste dynamique coord.
                    ramCoord[z]->next=ramCoord[bas-haut];
                    ramCoord[bas-haut]=ramCoord[z];
                    ramCoord[z]=NULL;
                }
            }
            z++;
        }
        //Et on peut enfin changer de colonne.
        x++;
    }
    free(column);
    //Fin de l'étape 1 !
    //Maintenant qu'on a tout parcouru, on dispose du nombre d'éléments n et du nombre de caractères m.
    //Il ne nous reste donc plus qu'à compléter les dernières bribes de fusion, et à tout ranger dans un tableau.
    //On crée déjà les tableaux.
    int* coord1 = malloc(sizeof(int)*4*n);
    int * values1 = malloc(sizeof(int)*2*m);
    //Commencons par remplir coord1. Ce tableau renferme toutes les coordonnées des valeurs utilisées.
    CoordList adress = ramCoord[bas-haut];
    int x = 0;
    while (x<n)
    {
        CoordList next = adress->next;
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
    //Ensuite, on passe à values1. Pour cela, on va remplir tout en modifiant coord1 et les pixels à fusionner.
    x=0;
    ValueList otherList = ramValue[(bas-haut)];
    while (x<m)
    {
        //On prend l'ensemble de valeurs actuel, et pour chaque valeur liée, on va combiner les coordonnées...
        ValueList fusion = otherList;
        int * valuecoord = coord1+4*(fusion->value-1);
        while (fusion->fusion!=NULL)
        {
            ValueList next = fusion->fusion;
            free(fusion);
            fusion = next;
            int* othercoord=coord1+4*(fusion->value-1);
            if (othercoord[0]<valuecoord[0])
            {
                //Comparaison de x...
                valuecoord[0]=othercoord[0];
            }
            if (othercoord[1]<valuecoord[1])
            {
                //Comparaison de y...
                valuecoord[1]=othercoord[1];
            }
            if (othercoord[2]>valuecoord[2])
            {
                //Comparaison de xmax...
                valuecoord[2]=othercoord[2];
            }
            if (othercoord[3]>valuecoord[3])
            {
                //Comparaison de ymax...
                valuecoord[3]=othercoord[3];
            }
            //Et on remplace tous les pixels dans l'intervalle ayant la valeur à fusionner.
            int x1 = othercoord[0];
            while (x1<=othercoord[2])
            {
                int y1 = othercoord[1];
                while (y1<=othercoord[3])
                {
                    if (matrixGet(mat,y1,x1)==fusion->value)
                    {
                        matrixSet(mat,y1,x1,otherList->value);
                    }
                    y1++;
                }
                x1++;
            }
        }
        //Et enfin, on met la valeur dans values, et on passe à la valeur suivante.
        values1[x]=otherList->value;
    }
    //Maintenant, la matrice a terminé d'être analysée, on a toutes les données utiles.
    //La dernière opération consiste à former result, avec uniquement les données utiles. Allons-y.
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
    //On place ainsi result1 dans le pointeur result, et on retourne le nombre de mot m (qui est donc la longueur de result1).
    result[0]=result1;
    return m;
}

//Avec cette fonction, en entrant la matrice et les coordonnées d'une ligne, on peut obtenir :
//-> Le nombre de caractères dans cette ligne (valeur retournée)
//-> La localisation de chaque caractère : pour un caractère n (n<m), le caractère est formé de tous les pixels de valeur
//     result[0][5*n] dans le rectangle délimité par : x=result[0][5*n+1],y=result[0][5*n+2],w=result[0][5*n+3],h=result[0][5*n+4].
//A partir de là, envoyer les matrices au réseau ne devrait pas être trop compliqué...


//ATTENTION !!! Il ne faut pas oublier de free result[0] une fois que le programme est terminé !
