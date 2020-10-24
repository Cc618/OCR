//m contient les donn�es (pixels noir, pixels blancs)
int analyseCharacters(Matrix * mat, int haut, int bas, int gauche, int droite,int** result)
{
    //Premier parcours : on change les valeurs 1 en fonction de la valeur du caract�re. On cr�e une premi�re version de values et de coord, les variables n et m indiquant le nombre de valeurs utilis�es et le nombre de caract�res d�tect�s, ainsi que ram qui rassemble les donn�es de coord et values imm�diatement modifiables.
    int x = 0;
    int y = haut;
    int n = 0;
    int m = 0;
    int** ram = malloc(sizeof(int*)*((bas-haut)*2+2));
    while (x<(bas-haut)*2+2);
    {
        ram[x]=NULL;
        //Les pointeurs de ram sont tous sur ram : on sait ainsi que ram n'a pas de donn�es particuli�res.
        //Les premiers pointeurs sont ceux de values, les suivants sont coord, et les deux derniers pointent sur les structures dynamiques de coord et values, qui seront stock�es dans l'�tape 2 sous la forme d'un tableau, avec les valeurs coord et values comme adresse.
        x++;
    }
    x=gauche;
    //D�but du premier parcours : on va de haut en bas, de gauche jusqu'� droite. Cela devrait � priori r�duire le temps de calcul de la seconde �tape... Mais l'inverse fonctionne �galement.
    while (x<droite)
    {
        y=haut;
        //On cr�e un tableau fusions, qui va r�capituler les valeurs fusionn�es et les valeurs pr�sentes dans la colonne. fusionscount est un marqueur.
        int* column = malloc(sizeof(int)*(bas-haut)*4);
        int fusionscount = 0;
        int valuecount = (bas-haut)*2;
        while (fusionscount < (bas-haut))
        {
            column[fusionscount*2]=0;
            column[fusionscount*2+1]=0;
            fusionscount++;
        }
        valuecount=0;
        while (y<bas)
        {

            //Pour chaque pixel, s'il a comme valeur 1 (pixel noir), on regarde les quatre pos�s � gauche et juste en haut.
            //Si l'un de ces pixels a une valeur diff�rente de 0, les deux pixels sont connect�s.
            //Le pixel prend alors la valeur du caract�re auquel il se connecte.
            //S'il n'y a pas d'autres pixels, alors il se forme une nouvelle valeur.
            //Si le pixel est connect� � deux valeurs diff�rentes, celles-ci sont connect�es (dans values).
            if (matrixGet(mat,x,y))
            {
                int neighbours[4];
                neighbours[0]=0;
                neighbours[1]=0;
                neighbours[2]=0;
                neighbours[3]=0;
                if (x>gauche)
                {
                    neighbours[2]=matrixGet(mat,x-1,y);
                    if (y<bas)
                    {
                        neighbours[3]=matrixGet(mat,x-1,y+1);
                    }
                }
                if (y>haut)
                {
                    neighbours[0]=matrixGet(mat,x,y-1);
                    if (x>gauche)
                    {
                        neighbours[1]=matrixGet(mat,x-1,y-1);
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
                                //On cherche les emplacements des values dans fusion, pour savoir si elles sont d�j� li�es.
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
                                    //Si elles ne sont pas dans les fusions, alors soit elles ont �t� li�es avant la boucle actuelle, soit elles ne sont pas li�es.
                                    //Dans ce cas, si l'on trouve value et neighbours[z] dans la ram, elles ne sont pas li�es.
                                    //Et si l'on ne trouve que l'un des deux facteurs, elles sont li�es.
                                    int* a1=ram[(bas-haut)*2];
                                    int* a2=ram[(bas-haut)*2];
                                    w=0;
                                    while (w<bas-haut)
                                    {
                                        if (ram[w][0]==value)
                                        {
                                            a1=&ram[w][0];
                                        }
                                        if (ram[w][0]==neighbours[z])
                                        {
                                            a2=&ram[w][0];
                                        }
                                        w++;
                                    }
                                    if (a1!=ram[(bas-haut)*2] && a2!=ram[(bas-haut)*2])
                                    {
                                        //Il ne reste plus qu'� fusionner les deux valeurs.
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
                //On a ainsi la valeur du pixel, plus qu'� l'assigner.
                //Si elle est nulle, il faut cr�er une nouvelle valeur. Il suffit d'augmenter m et n pour cela.
                char newvalue = 0;
                if (!value)
                {
                    n++;
                    m++;
                    value = n;
                    newvalue = 1;
                }
                matrixSet(mat,x,y,value);
                //Et derniere etape : indiquer que cette valeur se trouve dans la colonne.
                z=(bas-haut)*2;
                while (column[z]!=value && column[z]!=0)
                {
                    z+=2;
                }
                //En n'oubliant pas de modifier les coordonn�es.
                if (newvalue)
                {
                    int** newcoord = malloc(sizeof(int*)*6);
                    newcoord[0]=value;
                    newcoord[1]=x;
                    newcoord[2]=y;
                    newcoord[3]=1;
                    newcoord[4]=1;
                    newcoord[5]=&newcoord;
                    column[z+1]=&newcoord;
                    int adress = 0;
                    while (ram[(bas-haut)+adress]!=NULL)
                    {
                        adress++;
                    }
                    ram[(bas-haut)+adress]=&newcoord;
                }
                else
                {
                    int* adress = ram[(bas-haut)+column[z+1]];
                    if (column[z]==0)
                    {
                        adress[2]++;
                        //On augmente la largeur de la valeur, car elle n'�tait jusque l� pas pr�sente dans la colonne.
                    }
                    if (y>adress[1]+adress[3])
                    {
                        adress[3]++;
                        //On augmente la hauteur de la valeur, car l'ordonn�e actuelle est plus basse que celle d�j� enregistr�e.
                    }
                }
                column[z]=value;
            }
            y++;
        }
        //La colonne est finie. Maintenant, il faut modifier la ram avec les fusions effectu�es et les valeurs cr��es.
        //Premi�rement, les fusions.
        //Notez que si cela semble complexe, il y aura assez peu de fusions au final...
        int z =0;
        while (z<(bas-haut))
        {
            //Les fusions.
            //A chaque fusion, on prend la valeur de ram et on la place apr�s la valeur de fusion, et on remplace tout dans le tableau column.
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
                    if (ram[w][0]==value)
                    {
                        w1 = w;
                    }
                    if (ram[w][0]==other)
                    {
                        w2 = w;
                    }
                    w++;
                }
                if (w1 != w2)
                {
                    if (w1 != -1 && w2 != -1)
                    {
                        //value et other sont dans ram. On prend alors other et on la place tout au bout de la liste des fusions de value.
                        //la place qu'occupait other dans ram devient un pointeur sur la ram.
                        int* end = ram[w1];
                        while (end[1]!=&end)
                        {
                            end=end[1];
                        }
                        end[1]=&ram[w2];
                        ram[w2]=&ram;
                    }
                    else
                    {
                        //Dans l'autre cas, on prend la valeur pr�sente dans ram. S'il y a fusion, il y a forc�ment une valeur dans ram.
                        int** newfusion = malloc(sizeof(int*)*3);
                        if (w1==-1)
                        {
                            int v = w2;
                            w2=w1;
                            w1=v;
                            v = value;
                            value = other;
                            other = v;
                        }
                        newfusion[0]=other;
                        newfusion[1]=&newfusion;
                        newfusion[2]=&newfusion;
                        int* end = ram[w1];
                        while (end[1]!=&end)
                        {
                            end=end[1];
                        }
                        end[1]=&newfusion;
                    }
                    m--;
                    //On a un caract�re potentiel en moins.
                    //Et derni�re �tape : il faut remplacer toutes les valeurs other de column par la valeur value.
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
        //Les valeurs ayant �t� fusionn�es ne sont pour rappel plus dans la colonne.
        z=0;
        while (z<(bas-haut))
        {
            if (ram[z]!=&ram)
            {
                char found = 0;
                int value = ram[z][0];
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
                    //On place la valeur et toutes celles fusionn�es dans la liste dynamique values.
                    ram[z][2]=ram[(bas-haut)*2];
                    ram[(bas-haut)*2]=&ram[z];
                }
            }
            z++;
        }
        //La m�me, mais cette fois avec les Coordonn�es.
        while (z<(bas-haut)*2)
        {
            if (ram[z]!=&ram)
            {
                char found = 0;
                int value = ram[z][0];
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
                    //On place la valeur et toutes celles fusionn�es dans la liste dynamique values.
                    ram[z][5]=ram[(bas-haut)*2+1];
                    ram[(bas-haut)*2]=&ram[z];
                }
            }
            z++;
        }
        //Et on peut enfin changer de colonne.
        x++;
        free(column);
    }
    //Fin de l'�tape 1 !
    //Maintenant qu'on a tout parcouru, on dispose du nombre d'�l�ments n et du nombre de caract�res m.
    //Il ne nous reste donc plus qu'� compl�ter les derni�res bribes de fusion, et � tout ranger dans un tableau.
    //On cr�e d�j� les tableaux.
    int* coord1 = malloc(sizeof(int)*4*n);
    int * values1 = malloc(sizeof(int)*2*m);
    //Commencons par remplir coord1. Ce tableau renferme toutes les coordonn�es des valeurs utilis�es.
    int* adress = ram[(bas-haut)*2+1];
    int x = 0;
    while (x<n)
    {
        int y = 0;
        while (y<4)
        {
            coord1[4*(adress[0]-1)+y]=adress[1+y];
            y++;
        }
        free(adress);
        adress=adress[5];
        x++;
    }
    //Ensuite, on passe � values1. Pour cela, on va remplir tout en modifiant coord1 et les pixels � fusionner.
    x=0;
    adress = ram[(bas-haut)*2];
    while (x<m)
    {
        //On prend l'ensemble de valeurs actuel, et pour chaque valeur li�e, on va combiner les coordonn�es...
        int* fusion = adress;
        int * valuecoord = coord1+4*adress[0];
        while (fusion[1]!=&fusion)
        {
            fusion=fusion[1];
            int* othercoord=coord1+4*fusion[0];
            if (othercoord[0]<valuecoord[0])
            {
                //Comparaison de x...
                valuecoord[2]+=valuecoord[0]-othercoord[0];
                valuecoord[0]=othercoord[0];
            }
            if (othercoord[1]<valuecoord[1])
            {
                //Comparaison de y...
                valuecoord[3]+=valuecoord[1]-othercoord[1];
                valuecoord[1]=othercoord[1];
            }
            if (othercoord[0]+othercoord[2]>valuecoord[0]+valuecoord[2])
            {
                //Comparaison de w...
                valuecoord[2]=othercoord[0]+othercoord[2]-valuecoord[0];
            }
            if (othercoord[3]+othercoord[1]>valuecoord[3]+valuecoord[1])
            {
                //Comparaison de h...
                valuecoord[3]+=valuecoord[1]-othercoord[1]-valuecoord[1];
            }
            //Et on remplace tous les pixels dans l'intervalle ayant la valeur � fusionner.
            int x1 = othercoord[0];
            while (x1<othercoord[0]+othercoord[2])
            {
                int y1 = othercoord[1];
                while (y1<othercoord[1]+othercoord[3])
                {
                    if (mat[x1,y1]==fusion[0])
                    {
                        mat[x1,y1]=adress[0];
                    }
                    y1++;
                }
                x1++;
            }
        }
        //Et enfin, on met la valeur dans values, et on passe � la valeur suivante.
        values1[x]=adress[0];
        free(adress);
        adress = adress[1];
    }
    //Maintenant, la matrice a termin� d'�tre analys�e, on a toutes les donn�es utiles.
    //La derni�re op�ration consiste � former result, avec uniquement les donn�es utiles. Allons-y.
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
    free(ram);
    free(coord1);
    free(values1);
    //On place ainsi result1 dans le pointeur result, et on retourne le nombre de mot m (qui est donc la longueur de result1).
    result[0]=result1;
    return m;
}

//Avec cette fonction, en entrant la matrice et les coordonn�es d'une ligne, on peut obtenir :
//-> Le nombre de caract�res dans cette ligne (valeur retourn�e)
//-> La localisation de chaque caract�re : pour un caract�re n (n<m), le caract�re est form� de tous les pixels de valeur
//     result[0][5*n] dans le rectangle d�limit� par : x=result[0][5*n+1],y=result[0][5*n+2],w=result[0][5*n+3],h=result[0][5*n+4].
//A partir de l�, envoyer les matrices au r�seau ne devrait pas �tre trop compliqu�...


//ATTENTION !!! Il ne faut pas oublier de free result[0] une fois que le programme est termin� !
