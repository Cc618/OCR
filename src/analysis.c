#include <stdio.h>
#include "matrix.h"
#include "analysis.h"
#include "tools.h"

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

void matrixImage(SDL_Renderer *renderer, Matrix *m) {
    SDL_Rect r;
    r.w = 1; r.h = 1;
    for (size_t i = 0; i < m->rows; ++i) {
        r.x = 0; r.y = i;
        for (size_t j = 0; j < m->cols; ++j)
        {
            //printf("%d\n",j);
            r.x = j;
            SDL_SetRenderDrawColor( renderer, 255*(MAT_GET(m, i, j)!=0), 0, 0, 255);
            SDL_RenderFillRect( renderer, &r);
        }
        SDL_RenderPresent(renderer);
    }
}

int analysis(SDL_Renderer *renderer,Matrix* mat, int top, int down, int left,int right,int** result)
{
    SDL_RenderClear(renderer);
    //printf("Strange value : %d\n",(int)(matrixGet(mat,11,2)*100000));
    printf("First step...");
    int colors[] = {0,0,0,255,0,0,0,255,0,0,0,255,255,255,0,255,0,255,0,255,255,255,125,0,255,0,125,125,0,255,125,255,0,0,125,255,0,255,125};
    SDL_Rect r;
    r.w = 1; r.h = 1;
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
        r.x=x*1;
        //fusion will memorize all fusions in the current column.
        int fusionscount = 0;
        while (fusionscount < h*3)
        {
            //printf("fusionscount : %d",fusionscount);
            column[fusionscount]=0;
            fusionscount++;
        }
        while (fusionscount < h*4)
        {
            //printf("fusionscount : %d",fusionscount);
            column[fusionscount+h]=column[fusionscount];
            fusionscount++;
        }
        fusionscount=0;
        //printf("Suite de colonne : %d\n",x);
        while (y<=down)
        {
            column[fusionscount*2]=0;
            column[fusionscount*2+1]=0;
            r.y=y*1;
            //printf("x : %d y : %d\n",x,y);
            //printf("Valeur associée : %d\n",(int)matrixGet(mat,y,x)*100000);
            //For each pixel, we look the others pixel at its left and
            //over him. Then we can determine the value of the pixel.
            //If there are multiple values a pixel can have, it means that
            //these values have to be merged.
            if (!(matrixGet(mat,y,x)*100000))
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
                                //printf("Ah !");
                                int w = 0;
                                //printf("Value et neighbours : %d %d\n",value, neighbours[z]);
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
                                //printf("Beh ! %d",found);
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
                                            //printf("Voilà.");
                                            if (ramValue[w]->value==value)
                                            {
                                                //printf("Cas 1.");
                                                a1=ramValue[w];
                                            }
                                            if (ramValue[w]->value==neighbours[z])
                                            {
                                                //printf("Cas 2.");
                                                a2=ramValue[w];
                                            }
                                        }
                                        w++;
                                    }
                                    //printf("Cée !");
                                    if (a1!=ramValue[h] && a2!=ramValue[h])
                                    {
                                        //printf("Nope !\n");
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
                                        //printf("Test - x : %d\n",x);
                                        while (w<=down && !found)
                                        {
                                            found=(int)(matrixGet(mat,w,x-1)*100000+0.5f)==value;
                                            w++;
                                        }
                                        //printf("Resultat : %d\n",found);
                                        if (!found)
                                        {
                                            column[fusionscount*2]=value;
                                            column[fusionscount*2+1]=neighbours[z];
                                            fusionscount++;
                                            //printf("FUSIOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOON !!!\n");
                                        }
                                    }
                                    //printf("Ouf.");
                                }
                            }
                        }
                    }
                    z++;
                }
                //printf("Hap. %d",value);
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
                /*
                SDL_SetRenderDrawColor(renderer, colors[((int)(matrixGet(mat, y,x)*100000+0.5f)%11)*3],
                                        colors[((int)(matrixGet(mat, y, x)*100000+0.5f)%11)*3+1],
                                        colors[((int)(matrixGet(mat, y, x)*100000+0.5f)%11)*3+2], 255);
                SDL_RenderFillRect( renderer, &r);
                SDL_RenderPresent(renderer);*/
                //And last step : write in the column that the value is inside.
                z=h*2;
                while (column[z]!=value && column[z]!=0)
                {
                    z++;
                }
                //Don't forget to update ramCoord.
                //printf("AJOUT A COLUMN : %d %d %d\n",z,value,column[z]);
                if (newvalue)
                {
                    //printf("Creation de valeur.");
                    //printValueList(ramValue,h);
                    //printCoordList(ramCoord,h);
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
                    //printf("RamValue mise a jour.");
                    //printValueList(ramValue,h);
                    //printCoordList(ramCoord,h);
                    CoordList* newcoord = newCoordList();
                    int ceciestuntest2 = 0;
                    //printf("Analyse...");
                    while (ceciestuntest2<4)
                    {
                        //printf("%d ",newcoord->coord[ceciestuntest2]);
                        ceciestuntest2++;
                    }
                    newcoord->coord[0]=x;
                    newcoord->coord[1]=y;
                    newcoord->coord[2]=x;
                    newcoord->coord[3]=y;
                    newcoord->next=NULL;
                    newcoord->value=value;
                    //printf("Affichage de la coord créée.");
                    //printCoordList(&newcoord,0);
                    //printf("Analyse...");
                    adress = 0;
                    while (ramCoord[adress]!=NULL)
                    {
                        adress++;
                    }
                    ramCoord[adress]=newcoord;
                    //printf("Création de valeur : %d\n",ramCoord[adress]->value);
                    //printf("Adresse de nouvelle valeur : %d\n",adress);
                    //printf("Analyse...");
                    int ceciestuntest = 0;
                    while (ceciestuntest<4)
                    {
                        //printf("%d ",(ramCoord[adress]->coord)[ceciestuntest]);
                        ceciestuntest++;
                    }
                    //printf("Valeur creee.");
                    //printValueList(ramValue,h);
                    //printCoordList(ramCoord,h);
                }
                else
                {
                    int adress = 0;
                    //printf("Et de 1 ! %d\n",value);
                    //printCoordList(ramCoord,h);
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
                        //printf("Adresse trouvée : %d\n",adress);
                        //printf("Analyse...");
                        int ceciestuntest = 0;
                        while (ceciestuntest<4)
                        {
                            //printf("%d ",ramCoord[adress]->coord[ceciestuntest]);
                            ceciestuntest++;
                        }
                        //printf("Et de 2 !");
                        if (column[z]==0)
                        {
                            ramCoord[adress]->coord[2]=x;
                            //The value width.
                        }
                        //printf("Et de 3 ! -> %d\n",(ramCoord[adress]->coord)[3]);
                        if (ramCoord[adress]->coord[3]<y)
                        {
                            ramCoord[adress]->coord[3]=y;
                            //The value max height.
                        }
                        //printf("Et de 4 !");
                        if (ramCoord[adress]->coord[1]>y)
                        {
                            ramCoord[adress]->coord[1]=y;
                            //The value min height.
                        }
                        //printf("Et de 5 !");
                    }
                }
                //printf("AJOUT A COLUMN : %d\n",z);
                column[z]=value;
                column[h*3+y-top]=value;
            }
            else
            {
                matrixSet(mat,y,x,0);
                column[h*3+y-top]=0;
            }
            //printf("print");
            //printf("Value : %d\n",(int)(matrixGet(mat,y,x)*100000));
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
                //printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
                int value = column[z*2];
                int other = column[z*2+1];
                //printf("Valeurs à combiner : %d et %d\n",value,other);
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
                //printf("Valeurs trouvées : %d et %d\n",w1,w2);
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
        //printf("Tri.");
        //printValueList(ramValue,h);
        //printCoordList(ramCoord,h);
        z=0;
        while (z<h)
        {
            if (ramValue[z]!=NULL)
            {
                char found = 0;
                int value = ramValue[z]->value;
                int w = 0;
                //printf("            ANALYSE : %d\n",value);
                while (w<h && !found)
                {
                    if (column[w+h*2]==value)
                    {
                        found = 1;
                    }
                    w++;
                }
                //printf("            FOUND : %d\n",found);
                if (!found||x==right)
                {
                    //printf("AJOUTE !!!!");
                    ramValue[z]->next=ramValue[h];
                    ramValue[h]=ramValue[z];
                    ramValue[z]=NULL;
                }
            }
            z++;
        }
        //The same, but with ramCoord.
        //printf("Tri ramValue fini.");
        //printValueList(ramValue,h);
        //printCoordList(ramCoord,h);
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
        //printf("Fin de colonne.");
        //printValueList(ramValue,h);
        //printCoordList(ramCoord,h);
        //printf("N et M : %d, %d\n",n,m);
    }
    free(column);
    //printf("First step !\n");
    //printValueList(ramValue,h);
    //printCoordList(ramCoord,h);
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
        //printf("On continue : %d\n",x);
        CoordList* next = adress->next;
        int y = 0;
        //printf("Value actuelle : %d, ",adress->value);
        while (y<4)
        {
            coord1[4*(adress->value-1)+y]=adress->coord[y];
            //printf("%d ",adress->coord[y]);
            y++;
        }
        //printf("\nOn libere : %d\n",x);
        free(adress->coord);
        free(adress);
        adress=next;
        x++;
    }
    //printCoordList(ramCoord,h);
    //printValueList(ramValue,h);
    //printf("Et de un...\n");
    //Then, values. By doing this, we will also complete the merges in the
    //matrix.
    x=0;
    ValueList* otherList = ramValue[h];
    //printf("N et M : %d, %d\n",n,m);
    while (x<m)
    {
        ValueList* fusion = otherList;
        if (otherList!=NULL)
        {
            int * valuecoord = coord1+4*(fusion->value-1);
            //printf("Repère.");
            while (fusion->fusion!=NULL)
            {
                //printf("Repere 2.");
                fusion = fusion->fusion;
                //printf("Value actuelle : %d\n",fusion->value);
            }
        }
        x++;
    }
    x=0;
    //printf("\n\n");
    //printValueList(ramValue,h);
    //printCoordList(ramCoord,h);
    //printf("COORD1 :\n");
    while(x<n)
    {
        //printf("%d : %d %d %d %d\n",x,coord1[4*x],coord1[4*x+1],coord1[4*x+2],coord1[4*x+3]);
        x++;
    }
    x=0;
    /*
    while(x<m)
    {
        printf("%d : %d\n",x,values1[x]);
        x++;
    }*/
    x=0;
    otherList = ramValue[h];
    while (x<m)
    {
        //printf("Boucle.");
        //Firstly, we have to update global coordonates.
        ValueList* fusion = otherList;
        ValueList* nextList = otherList->next;
        //printf("[%d]",fusion==NULL);
        if (otherList!=NULL)
        {
            int * valuecoord = coord1+4*(fusion->value-1);
            //printf("\nRepère.");
            while (fusion->fusion!=NULL)
            {
                //printf("Repere 2.");
                ValueList* next = fusion->fusion;
                //printf("Repere 3.");
                free(fusion);
                //printf("Repere 4.");
                fusion = next;
                //printf("Ah ? Value actuelle : %d\n",fusion->value);
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
                //printf("Merging terminé !");
                //Here is the end of the merging.
                int x1 = othercoord[0];
                while (x1<=othercoord[2])
                {
                    int y1 = othercoord[1];
                    while (y1<=othercoord[3])
                    {
                        //printf("STEVE");
                        if ((int)(matrixGet(mat,y1,x1)*100000+0.5f)==fusion->value)
                        {
                            r.x=x1*1;
                            r.y=y1*1;

                            matrixSet(mat,y1,x1,otherList->value*0.00001);
                            /*
                            SDL_SetRenderDrawColor(renderer, colors[((int)(matrixGet(mat, y1,x1)*100000+0.5f)%11)*3], colors[((int)(matrixGet(mat, y1, x1)*100000+0.5f)%11)*3+1], colors[((int)(matrixGet(mat, y1, x1)*100000+0.5f)%11)*3+2], 255);
                            SDL_RenderFillRect( renderer, &r);
                            SDL_RenderPresent(renderer);*/
                            //printf("> %d %d\n",x1,y1);
                        }
                        else
                        {
                            //printf("> %d %d Nope : %d\n",x1,y1,matrixGet(mat,y1,x1)*100000);
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
    //printf("Presque fini !\n");
    x=0;
    //printf("\n\n");
    //printValueList(ramValue,h);
    //printCoordList(ramCoord,h);
    //printf("COORD1 :\n");
    /*
    while(x<n)
    {
        //printf("%d : %d %d %d %d\n",x,coord1[4*x],coord1[4*x+1],coord1[4*x+2],coord1[4*x+3]);
        x++;
    }*/
    x=0;
    //printf("VALUES1 :\n");
    /*
    while(x<m)
    {
        //printf("%d : %d\n",x,values1[x]);
        x++;
    }
    */

    //printf("\nN et M : %d\n",n,m);
    x = 0;
    /*
    while (x<m)
    {
        printf("%d : %d\n",x,values1[x]);
        x++;
    }*/
    x = 0;
    /*while (x<n)
    {
        printf("%d : %d, %d, %d et %d\n",x,coord1[x*4],coord1[x*4+1],coord1[x*4+2],coord1[x*4+3]);
        x++;
    }*/
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
    //printf("Serait-ce la fin ?");
    free(ramValue);
    //printf("A...");
    free(ramCoord);
    //printf("B...");
    free(coord1);
    //printf("C...");
    free(values1);
    //printf("D...");
    //The array is put in the result, and we return m, which permits to know
    //the length of result.
    result[0]=result1;
    printf("FINI !!!\n");
    //printf("M : %d\n",m);
    /*
    x=left;
    while(x<=right)
    {
        r.x = x*1;
        y=top;
        while (y<=down)
        {
            r.y=y*1;
            if ((int)(matrixGet(mat, y,x)*100000+0.5f)==0)
            {
                SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, colors[((int)(matrixGet(mat, y,x)*100000+0.5f)%11)*3], colors[((int)(matrixGet(mat, y, x)*100000+0.5f)%11)*3+1], colors[((int)(matrixGet(mat, y, x)*100000+0.5f)%11)*3+2], 255);
            }
            SDL_RenderFillRect( renderer, &r);
            y++;
        }
        x++;
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);*/
    return m;
}

void CaractersAnalysis(SDL_Renderer *ren,Matrix* mat, int top, int down)
{
    int ** result = (int**)malloc(sizeof(int*));
    int length = analysis(ren,mat,top,down,0,mat->cols-1,result);
    SDL_Rect r;
    r.w = 10; r.h = 10;
    float deltaw;
    float deltah;
    size_t xd=0;
    size_t yd=0;
    float color;
    while (length>0)
    {
        SDL_RenderClear(ren);
        length--;
        int value = result[0][length*5];
        int x = result[0][length*5+1];
        deltaw = 32.0f / (1+result[0][length*5+3]-result[0][length*5+1]);
        deltah = 32.0f / (1+result[0][length*5+4]-result[0][length*5+2]);
        if (deltah<deltaw)
        {
            deltaw=deltah;
        }
        printf("%d Delta : %f\n",x,deltaw);
        Matrix *M = matrixZero(32, 32);
        while (x<=result[0][length*5+3])
        {
            r.x=10*((int)((x-result[0][length*5+1])*deltaw));
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
        matrixImage(ren,M);
        SDL_RenderPresent(ren);
        SDL_Delay(800);
    }
    SDL_RenderPresent(ren);
    SDL_Delay(800);
    free(result[0]);
    free(result);
}









// --- Cc --- //
// TODO : rm
#define LOGBOX(box) printf("((%zu, %zu), (%zu, %zu)) => w = %zu, h = %zu\n", \
        (box).b.x,(box).b.y,(box).c.x,(box).c.y,\
        (box).c.x - (box).b.x, (box).c.y - (box).b.y);


#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))


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
                // Take val at (x + j * w / 32, y + i * h / 32)
                float val = matrixGet(image, x + j * w / 32, y + i * h / 32);
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
                // Take val at (x + j * w / 32, y + i * h / 32)
                float val = matrixGet(image, x + j * w / 32, y + i * h / 32);
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

    // printf("x = %zu, y = %zu, px = %.1f\n", j, i,
    //         matrixGet(image, startY + i, j));


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

char *lineAnalysis(const Matrix *image, __attribute__((unused)) void *net,
        int startY, int lineY, int endY) {
    // We suppose that there are less than 512 chars
    char *str = malloc(512);

    // We use a traversal like it were a graph with 2 classes
    // (black / white pixels)
    // We suppose that the first pixel is white
    int height = endY - startY;
    int width = image->cols;
    int *visited = calloc(height * image->cols, sizeof(int));

    // printf("From %zu to %zu (width = %zu)\n", startY, endY, image->cols);

    int nchars = 0;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < image->cols; ++j) {
            if (!visited[i * width + j]) {
                // Found a char
                if (matrixGet(image, startY + i, j) < .5f) {
                    rectangle box = {
                        .b = { j, i },
                        .c = { j, i },
                    };

                    charBox(image, startY, height, visited, i, j, &box);

                    ++box.c.x;
                    ++box.c.y;

                    box.b.y += startY;
                    box.c.y += startY;

                    size_t w = box.c.x - box.b.x;
                    size_t h = box.c.y - box.b.y;

                    if (w <= 2 || height <= 2)
                        continue;

                    printf("> x = %d, y = %d\n", j, i + startY);
                    LOGBOX(box);
                    for (int y = box.b.y; y < box.c.y; ++y) {
                        for (int x = box.b.x; x < box.c.x; ++x) {
                            printf("%c", matrixGet(image, y, x) > .5f ?
                                    '.' : '#');
                        }
                        puts("");
                    }
                    puts("");

                    str[nchars] = '?';
                    ++nchars;

                    // return "";
                }
                else visited[i * width + j] = 1;
            }
        }
    }







    printf("nchars = %d\n", nchars);


    str[nchars] = 0;

    return str;





    // TODO : Order boxes by start x

    // int nchars = 0;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < image->cols; ++j) {
            // printf("%c", matrixGet(image, height + i, j) > .5f ?
            //         '.' : '#');
            // continue;

            // int i = lineY - startY;

            if (!visited[i * image->cols + j]) {
                // printf("i = %zu, j = %zu\n", i, j);

                // White
                if (matrixGet(image, startY + i, j) > .5f)
                    visited[i * image->cols + j] = 1;
                else {
                    // Found a char
                    rectangle box = {
                        .b = { j, i },
                        .c = { j, i }
                    };

                    // Get hitbox of the char and visit all pixels of it
                    charBox(image, (size_t)startY, height,
                            visited, i, j, &box);

                    box.c.x++;
                    box.c.y++;

                    size_t x = box.b.x;
                    size_t y = box.b.y;
                    size_t w = box.c.x - box.b.x;
                    size_t h = box.c.y - box.b.y;

                    // // Too small
                    // if (w < 2 || h < 2)
                    //     continue;

                    box.b.y += startY;
                    box.c.y += startY;

                    printf("Char %f\n", matrixGet(image, startY + i, j));
                    LOGBOX(box);

                    // continue;


                    // // Disp box
                    // for (size_t i = box.b.y; i < box.c.y; ++i) {
                    //     for (size_t j = box.b.x; j < box.c.x; ++j)
                    //         printf("%c", matrixGet(image, i, j) > .5f ?
                    //                 '.' : '#');
                    //     puts("");
                    // }
                    // puts("---");

                    // TODO : If too small, continue

                    // Matrix *resized = charResize(image, &box);

                    // Disp resized
                    // for (size_t i = 0; i < 32; ++i) {
                    //     for (size_t j = 0; j < 32; ++j)
                    //         printf("%c", matrixGet(resized, i, j) > .5f ?
                    //                 '.' : '#');
                    //     puts("");
                    // }
                    // puts("---");

                    // return str;



                    // TODO : Detect
                    str[nchars] = '?';

                    ++nchars;
                    // matrixFree(resized);
                }
            }
        }

        // puts("");
    }


    printf("Found %d chars\n", nchars);

    free(visited);

    str[nchars] = 0;

    // TODO
    return str;
}
