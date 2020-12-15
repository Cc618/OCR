#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "analysis.h"
#include "tools.h"

// To detect i / j we add a padding for more precision
#define IJ_PAD 4
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

CoordList* newCoordList()
{
	CoordList* newList = (CoordList*) malloc(sizeof(CoordList));
	int* newcoord = (int*) malloc(sizeof(int) * 4);
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
	ValueList* newList = (ValueList*)malloc(sizeof(ValueList));
	newList->next=NULL;
	newList->fusion=NULL;
	newList->value=0;
	return newList;
}

int analysis(Matrix* mat, int top,int down, int left,int right,int** result)
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
    ValueList** ramValue = (ValueList**) malloc(sizeof(ValueList*)*(h+1));
    CoordList** ramCoord = (CoordList**) malloc(sizeof(CoordList*)*(h+1));
    while (x<h+1)
    {
        ramValue[x]=NULL;
        ramCoord[x]=NULL;
        x++;
    }
    x=left;
    int* column = (int*) malloc(sizeof(int)*h*5);
    while (x<=right)
    {
        y=top;
        //fusion will memorize all fusions in the current column.
        int fusionscount = 0;
        while (fusionscount < h*3)
        {
            column[fusionscount]=0;
            fusionscount++;
        }
        while (fusionscount < h*4)
        {
            column[fusionscount+h]=column[fusionscount];
            fusionscount++;
        }
        fusionscount=0;
        while (y<=down)
        {
            column[fusionscount*2]=0;
            column[fusionscount*2+1]=0;
            //For each pixel, we look the others pixel at its left and
            //over him. Then we can determine the value of the pixel.
            //If there are multiple values a pixel can have, it means that
            //these values have to be merged.
            if (!(matrixGet(mat,y,x)))
            {
                int neighbours[4];
                neighbours[0]=0;
                neighbours[1]=0;
                neighbours[2]=0;
                neighbours[3]=0;
                if (x>left)
                {
                    neighbours[2]=column[h*4+y-top];
                    if (y<down)
                    {
                        neighbours[3]=column[h*4+y+1-top];
                    }
                }
                if (y>top)
                {
                    neighbours[0]=(int)(matrixGet(mat,y-1,x)*100000+0.5f);
                    if (x>left)
                    {
                        neighbours[1]=column[h*4+y-1-top];
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
                                    if(column[w*2]==value||
                                            column[w*2+1]==value)
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
                                    //they have already been merged.
                                    ValueList* a1=ramValue[h];
                                    ValueList* a2=ramValue[h];
                                    w=0;
                                    while (w<h)
                                    {
                                        if (ramValue[w]!=NULL)
                                        {
                                            if (ramValue[w]->value==value)
                                            {
                                                a1=ramValue[w];
                                            }
                                            if (ramValue[w]->value==neighbours[z])
                                            {
                                                a2=ramValue[w];
                                            }
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
                                    else
                                    {
                                        w=top;
                                        found=0;
                                        while (w<=down && !found)
                                        {
                                            found=(int)(matrixGet(mat,w,x-1)*100000+0.5f)==value;
                                            w++;
                                        }
                                        if (!found)
                                        {
                                            column[fusionscount*2]=value;
                                            column[fusionscount*2+1]=neighbours[z];
                                            fusionscount++;
                                        }
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
                z=h*2;
                while (column[z]!=value && column[z]!=0)
                {
                    z++;
                }
                //Don't forget to update ramCoord.
                if (newvalue)
                {
                    ValueList* newList = newValueList();
                    newList->value=value;
                    newList->fusion = NULL;
                    newList->next=NULL;

                    int adress = 0;
                    while (ramValue[adress]!=NULL)
                    {
                        adress++;
                    }
                    ramValue[adress]=newList;
                    CoordList* newcoord = newCoordList();
                    newcoord->coord[0]=x;
                    newcoord->coord[1]=y;
                    newcoord->coord[2]=x;
                    newcoord->coord[3]=y;
                    newcoord->next=NULL;
                    newcoord->value=value;
                    adress = 0;
                    while (ramCoord[adress]!=NULL)
                    {
                        adress++;
                    }
                    ramCoord[adress]=newcoord;
                }
                else
                {
                    int adress = 0;
                    char booleen = 0;
                    while (!booleen && adress != h)
                    {
                        if (ramCoord[adress]!=NULL)
                        {
                            booleen = ramCoord[adress]->value==value;
                            if (booleen)
                            {
                                adress--;
                            }
                        }
                        adress++;
                    }
                    if (adress != h)
                    {
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
                }
                column[z]=value;
                column[h*3+y-top]=value;
            }
            else
            {
                matrixSet(mat,y,x,0);
                column[h*3+y-top]=0;
            }
            y++;
        }
        //Column is finished !
        //Now we have to update ramValue with earned datas.
        int z =h*3;
        z=0;
        while (z<h)
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
                if (w1 != w2 && w1!=-1 && w2!=-1)
                {
                    ValueList* adress = ramValue[w1];
                    while (adress->fusion!=NULL)
                    {
                        adress=adress->fusion;
                    }
                    adress->fusion=ramValue[w2];
                    ramValue[w2]=NULL;
                    m--;
                    //After the merging has been done, we decrement m because
                    //there is a potential symbol in less.
                    //Now, we replace all other values in the column.
                    int v = 0;
                    while(v<h*4)
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
        while (z<h)
        {
            if (ramValue[z]!=NULL)
            {
                char found = 0;
                int value = ramValue[z]->value;
                int w = 0;
                while (w<h && !found)
                {
                    if (column[w+h*2]==value)
                    {
                        found = 1;
                    }
                    w++;
                }
                if (!found||x==right)
                {
                    ramValue[z]->next=ramValue[h];
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
        //Et we can change of column.
        x++;
    }
    free(column);
    //End of the first step !
    //We have now all the values we need, as n and m.
    //So we'll put datas in arrays.
    int* coord1 = (int*) malloc(sizeof(int)*4*n);
    int * values1 = (int*) malloc(sizeof(int)*m);
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
        free(adress->coord);
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
        ValueList* nextList = otherList->next;
        if (otherList!=NULL)
        {
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
                        if ((int)(matrixGet(mat,y1,x1)*100000+0.5f)==fusion->value)
                        {
                            matrixSet(mat,y1,x1,otherList->value*0.00001);
                        }
                        y1++;
                    }
                    x1++;
                }

            }
            values1[x]=otherList->value;
            x++;
        }
        otherList=nextList;
    }
    //Now, all datas are ready !
    //The last thing to do is to put all these datas in only one array, without
    //unuseful datas, for exemple coordonates of merged values.
    int * result1 = (int*) malloc(sizeof(int)*5*m);
    x=0;
    while (x<m)
    {
        y=0;
        result1[x*5]=values1[x];
        while (y<4)
        {
            result1[x*5+y+1]=coord1[(values1[x]-1)*4+y];
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

void CaractersAnalysis(Matrix* mat, int top, int down, int left, int right)
{
    int ** result = (int**)malloc(sizeof(int*));
    int length = analysis(mat,top,down,left,right,result);
    float deltaw;
    float deltah;
    size_t xd=0;
    size_t yd=0;
    float color;
    while (length>0)
    {
        length--;
        int value = result[0][length*5];
        int x = result[0][length*5+1];
        deltaw = 32.0f / (1+result[0][length*5+3]-result[0][length*5+1]);
        deltah = 32.0f / (1+result[0][length*5+4]-result[0][length*5+2]);
        if (deltah<deltaw)
        {
            deltaw=deltah;
        }
        Matrix *M = matrixZero(32, 32);
        while (x<=result[0][length*5+3])
        {
            int y = result[0][length*5+2];
            while (y<=result[0][length*5+4])
            {
                color = (float)((int)(matrixGet(mat,y,x)*100000+0.5f)==value);
                int x1=0;
                float xmax = (x+1-result[0][length*5+1])*deltaw;
                float ymax = (y+1-result[0][length*5+2])*deltaw;
                while (x1+(x-result[0][length*5+1])*deltaw<xmax)
                {
                    xd=x1+(size_t)((x-result[0][length*5+1])*deltaw);
                    int y1 = 0;
                    while (y1+(y-result[0][length*5+2])*deltaw<ymax)
                    {
                        yd=y1+(size_t)((y-result[0][length*5+2])*deltaw);
                        matrixSet(M,yd,xd,color);
                        y1++;
                    }
                    x1++;
                }
                y++;
            }
            x++;
        }
    }
    free(result[0]);
    free(result);
}

// Returns a 32x32 px matrix using nearest neighbors
static Matrix *charResize(const Matrix *image, rectangle *box) {
    Matrix *mat = matrixNew(32, 32);

    size_t x = box->b.x;
    size_t y = box->b.y;
    size_t w = box->c.x - box->b.x;
    size_t h = box->c.y - box->b.y;

    // Set new width to 32
    // Keep aspect ratio
    if (w > h) {
        // w2 / h2 = w / h
        // 1 / h2 = w / h / w2
        // h2 = h * w2 / w = h * 32 / w
        size_t newHeight = h * 32 / w;

        for (size_t i = 0; i < newHeight; ++i)
            for (size_t j = 0; j < 32; ++j) {
                // Take val at (x + j * w / 32, y + i * h / newHeight)
                float val = matrixGet(image, y + i * h / newHeight,
                        x + j * w / 32);
                matrixSet(mat, i, j, val);
            }

        // Fill bottom with white
        for (size_t i = newHeight; i < 32; ++i)
            for (size_t j = 0; j < 32; ++j)
                matrixSet(mat, i, j, 1.f);
    } else {
        size_t newWidth = w * 32 / h;

        for (size_t j = 0; j < newWidth; ++j)
            for (size_t i = 0; i < 32; ++i) {
                float val = matrixGet(image, y + i * h / 32,
                        x + j * w / newWidth);
                matrixSet(mat, i, j, val);
            }

        // Fill right with white
        for (size_t j = newWidth; j < 32; ++j)
            for (size_t i = 0; i < 32; ++i)
                matrixSet(mat, i, j, 1.f);
    }

    return mat;
}

// Fetches the hitbox of the character
// Modifies box
static void charBox(const Matrix *image,
        size_t startY, size_t height,
        int *visited,
        size_t i, size_t j, rectangle *box) {
    if (visited[i * image->cols + j])
        return;

    visited[i * image->cols + j] = 1;

    // Not a char
    if (matrixGet(image, startY + i, j) > .5f)
        return;

    // Update box
    box->b.x = MIN(box->b.x, j);
    box->b.y = MIN(box->b.y, i);
    box->c.x = MAX(box->c.x, j);
    box->c.y = MAX(box->c.y, i);

    // Test each neighbor
    if (i > 0)
        charBox(image, startY, height, visited, i - 1, j, box);

    if (j > 0)
        charBox(image, startY, height, visited, i, j - 1, box);

    if (i < height - 1)
        charBox(image, startY, height, visited, i + 1, j, box);

    if (j < image->cols - 1)
        charBox(image, startY, height, visited, i, j + 1, box);
}

// To sort boxes using the first x
static int boxCmp(const rectangle **a, const rectangle **b) {
    return (*a)->b.x >= (*b)->b.x;
}

void lineAnalysis(const Matrix *image, int startY, int endY,
        rectangle **boxes, Matrix **matrices, size_t *nchars) {
    // We use a traversal like it were a graph with 2 classes
    // (black / white pixels)
    // We suppose that the first pixel is white
    int height = endY - startY;
    int width = image->cols;
    int *visited = calloc(height * image->cols, sizeof(int));

    *nchars = 0;
    for (size_t i = 0; i < (size_t)height; ++i) {
        for (size_t j = 0; j < image->cols; ++j) {
            if (!visited[i * width + j]) {
                // Found a char
                if (matrixGet(image, startY + i, j) < .5f) {
                    rectangle *box = malloc(sizeof(rectangle));
                    *box = (rectangle) {
                        .b = { j, i },
                        .c = { j, i },
                    };

                    charBox(image, startY, height, visited, i, j, box);

                    ++box->c.x;
                    ++box->c.y;

                    size_t pad = 2;
                    box->b.y += startY - pad;
                    box->c.y += startY + pad;
                    box->b.x -= pad;
                    box->c.x += pad;

                    size_t w = box->c.x - box->b.x;

                    if (w <= 2 || height <= 2) {
                        free(box);
                        continue;
                    }

                    boxes[*nchars] = box;

                    ++*nchars;
                }
                else visited[i * width + j] = 1;
            }
        }
    }

    free(visited);

    // Sort by x
    qsort(boxes, *nchars, sizeof(rectangle*),
            (int(*)(const void*, const void*))boxCmp);

    // Detect Is and Js
    for (size_t c = 1; c < *nchars; ++c) {
        rectangle *base = boxes[c - 1];
        rectangle *point = boxes[c];

        // Found
        if (point->c.y <= base->b.y &&
                point->b.x >= base->b.x - IJ_PAD &&
                point->c.x <= base->c.x + IJ_PAD) {
            base->b.y = point->b.y;

            // Remove point
            free(boxes[c]);
            boxes[c] = NULL;
            c++;
        }
    }

    // Remove NULLs
    rectangle **cpy = malloc(sizeof(rectangle*) * *nchars);
    memcpy(cpy, boxes, sizeof(rectangle*) * *nchars);
    size_t oldNchars = *nchars;
    *nchars = 0;
    for (size_t i = 0; i < oldNchars; ++i) {
        if (cpy[i]) {
            boxes[(*nchars)++] = cpy[i];
        }
    }

    free(cpy);

    // Resize
    for (size_t c = 0; c < *nchars; ++c) {
        matrices[c] = charResize(image, boxes[c]);
    }
}
