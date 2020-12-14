#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "network.h"
#include "data.h"
#include "ocr.h"

static Network *guiNet;
static Dataset *guiDataset;

void print_text(char* str, SDL_Window *ecran,SDL_Surface *texte,int x, int y, TTF_Font *police,SDL_Surface *pSurf)
{
    if (str[0])
    {
        texte = TTF_RenderText_Blended(police, str, (SDL_Color){0,0,0, 255});
        SDL_Rect position;
        position.x = x;
        position.y = y;
        if (x==-1 && texte->w>800)
        {
            position.x=800-texte->w;
        }
        SDL_BlitSurface(texte, NULL, pSurf, &position);
    }
}
int gui_analysis(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf, SDL_Surface *surImage)
{
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 200, 255));
    print_text("Analysis running, please wait...",ecran,texte,0,0,police, pSurf);
    SDL_UpdateWindowSurface(ecran);
    //Lancement de la fonction analyse sur SDL_Surface *surImage.
    //A retourner : la string result, et sa longueur len.
    char* result = ocr(surImage, guiNet, guiDataset);
    // TODO
    puts("OK");
    printf("> Got result %s\n", result);
    return;
    int len = strlen(result);

    FILE* fichier = NULL;
    fichier = fopen("result.txt", "w");
    fprintf(fichier,"%s",result);
    fclose(fichier);
    SDL_Event event;
    char continuer = 1;
    int choix = 0;
    while (continuer==1)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            continuer = 0;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_v && (SDL_GetModState() & KMOD_CTRL) && SDL_HasClipboardText())
            {
                char *tmp = SDL_GetClipboardText();
                size_t l = strlen(tmp);
                size_t l_copy = len + l < 100 ? l : 100 - len;
                strncpy(result + len, tmp, l_copy);
                len += l_copy;
                SDL_free(tmp);
            }
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                    choix = (choix+2)%3;
                    break;
                case SDLK_DOWN:
                    choix = (choix+1)%3;
                    break;
                case SDLK_RETURN:
                    continuer=2;
                    break;
            }
        }
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 200, 255));
        print_text("Analysis done",ecran,texte,0,0,police, pSurf);
        print_text("New analysis",ecran,texte,70,100,police, pSurf);
        print_text("Credits",ecran,texte,70,200,police, pSurf);
        print_text("Exit",ecran,texte,70,300,police, pSurf);
        print_text("You can see your results by pressing Ctrl+V or reading "
                "result.txt file.",ecran,texte,0,400,police, pSurf);
        print_text(">",ecran,texte,0,100+choix*100,police, pSurf);
        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }

    // TODO : free(result);

    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 3;
}

int gui_error_loading(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf)
{
    SDL_Event event;
    char continuer = 1;
    int choix = 0;
    while (continuer==1)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            continuer = 0;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                    choix = (choix+2)%3;
                    break;
                case SDLK_DOWN:
                    choix = (choix+1)%3;
                    break;
                case SDLK_RETURN:
                    continuer=2;
                    break;
            }
        }
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 255, 200, 200));
        print_text("No image found...",ecran,texte,0,0,police, pSurf);
        print_text("Enter new location",ecran,texte,70,100,police, pSurf);
        print_text("Cancel",ecran,texte,70,200,police, pSurf);
        print_text("Exit",ecran,texte,70,300,police, pSurf);
        print_text(">",ecran,texte,0,100+choix*100,police, pSurf);
        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 1;
}

int gui_image_validation(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf, SDL_Surface *surImage)
{
    SDL_Renderer* ren;
    ren = SDL_CreateRenderer(ecran,-1,SDL_RENDERER_ACCELERATED);
    SDL_Texture *texImage = SDL_CreateTextureFromSurface(ren,surImage);
    float delta = 400.0f/surImage->w;
    if (surImage->h>surImage->w)
    {
        delta = 400.0f/surImage->h;
    }
    SDL_Rect dst1;
    dst1.x = 400;
    dst1.y = 100;
    dst1.w = (int)(delta*surImage->w);
    dst1.h = (int)(delta*surImage->h);
    SDL_Rect dst2;
    dst2.x = 0;
    dst2.y = 0;
    dst2.w = 800;
    dst2.h = 500;
    SDL_Rect dst3;
    dst3.x = 0;
    dst3.y = 0;
    dst3.w = 50;
    dst3.h = 80;
    SDL_Rect dst4;
    dst4.x = 0;
    dst4.y = 0;
    dst4.w = 50;
    dst4.h = 80;
    //Préparation des images...
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 255, 200));
    print_text(">",ecran,texte,0,0, police,pSurf);
    SDL_Texture *texIcone = SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,100,100);
    SDL_SetTextureBlendMode(texIcone,SDL_BLENDMODE_BLEND);
    SDL_Texture *texScreen = SDL_CreateTextureFromSurface(ren,pSurf);
    SDL_SetRenderTarget(ren,texIcone);
    SDL_RenderCopy(ren, texScreen, NULL, &dst2);
    SDL_SetRenderTarget(ren,NULL);
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 255, 200));
    print_text("Here is the image",ecran,texte,0,0,police, pSurf);
    print_text("Confirm",ecran,texte,70,100,police, pSurf);
    print_text("Abort",ecran,texte,70,200,police, pSurf);
    texScreen = SDL_CreateTextureFromSurface(ren,pSurf);
    //Début du code
    SDL_Event event;
    char continuer = 1;
    int choix = 0;
    while (continuer==1)
    {
        dst4.y=100+100*choix;
        SDL_RenderCopy(ren, texScreen, NULL, &dst2);
        SDL_RenderCopy(ren, texImage, NULL, &dst1);
        SDL_RenderCopy(ren, texIcone, &dst3, &dst4);
        SDL_RenderPresent(ren);
        SDL_Delay(10);
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            continuer = 0;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                    choix = (choix+1)%2;
                    break;
                case SDLK_DOWN:
                    choix = (choix+1)%2;
                    break;
                case SDLK_RETURN:
                    continuer=2;
                    break;
            }
        }
    }
    SDL_DestroyTexture(texImage);
    SDL_DestroyTexture(texScreen);
    SDL_DestroyTexture(texIcone);
    SDL_DestroyRenderer(ren);
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 5;
    return 1;
}

int gui_credits(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf)
{
    SDL_Event event;
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 200, 255));
    print_text("The team :",ecran,texte,0,0,police, pSurf);
    print_text("Raphael Brenn",ecran,texte,70,100,police, pSurf);
    print_text("Leandre Perrot",ecran,texte,70,200,police, pSurf);
    print_text("Celian Raimbault",ecran,texte,70,300,police, pSurf);
    print_text("Steve Suissa",ecran,texte,0,400,police, pSurf);
    SDL_UpdateWindowSurface(ecran);
    char continuer = 1;
    while (continuer==1)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            continuer = 0;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym==SDLK_RETURN)
                continuer=2;
        }
    }
    return (continuer==2);
}

int gui_home(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf)
{
    SDL_Event event;
    char continuer = 1;
    int choix = 0;
    while (continuer==1)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            continuer = 0;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                    choix = (choix+2)%3;
                    break;
                case SDLK_DOWN:
                    choix = (choix+1)%3;
                    break;
                case SDLK_RETURN:
                    continuer=2;
                    break;
            }
        }
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 200, 255));
        print_text("Welcome to Iron Minds' OCR !",ecran,texte,0,0,police, pSurf);
        print_text("Start analysis",ecran,texte,70,100,police, pSurf);
        print_text("Credits",ecran,texte,70,200,police, pSurf);
        print_text("Exit",ecran,texte,70,300,police, pSurf);
        print_text(">",ecran,texte,0,100+choix*100,police, pSurf);
        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 3;
}

int text_selection(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf,char* rep)
{
    SDL_Rect champ;
    champ.x=0;
    champ.y=90;
    champ.h=100;
    champ.w=800;
    size_t LEN_MAX = 100;
    size_t len = LEN_MAX;
    while (len)
    {
        rep[len]=0;
        len--;
    }
    rep[len]=0;
    SDL_Event event;
    char ending = 0;
    while(!ending)
    {
        SDL_FillRect(pSurf,&champ, SDL_MapRGB(pSurf->format, 200, 255, 200));
        print_text(("%s",rep),ecran,texte,-1,100,police, pSurf);
        SDL_UpdateWindowSurface(ecran);
        SDL_bool has_type = SDL_FALSE;
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT)
            ending = 1;
        else if( event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_BACKSPACE && len)
            {
                rep[len - 1] = 0;
                len--;
                has_type = SDL_TRUE;
            }
            if(event.key.keysym.sym == SDLK_v && (SDL_GetModState() & KMOD_CTRL) && SDL_HasClipboardText())
            {
                char *tmp = SDL_GetClipboardText();
                size_t l = strlen(tmp);
                size_t l_copy = len + l < LEN_MAX ? l : LEN_MAX - len;
                strncpy(rep + len, tmp, l_copy);
                len += l_copy;
                SDL_free(tmp);
                has_type = SDL_TRUE;
            }
            if(event.key.keysym.sym == SDLK_c && (SDL_GetModState() & KMOD_CTRL))
                SDL_SetClipboardText(rep);
            if(event.key.keysym.sym == SDLK_RETURN && len)
                ending=2;
            if(event.key.keysym.sym == SDLK_ESCAPE)
                ending=3;
        }
        else if(event.type == SDL_TEXTINPUT)
        {
            size_t l = strlen(event.text.text);
            size_t l_copy = len + l < LEN_MAX ? l : LEN_MAX - len;
            strncpy(rep + len, event.text.text, l_copy);
            len += l_copy;
            has_type = SDL_TRUE;
        }
    }
    return ending;
}

int gui_select_image(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,SDL_Surface *pSurf,char*adresse)
{
    SDL_Event event;
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, 200, 200, 255));
    print_text("Please enter image location",ecran,texte,0,0,police, pSurf);
    print_text("CTRL+C / CTRL+V : Copy / paste",ecran,texte,0,200,police, pSurf);
    print_text("ENTER : Validate",ecran,texte,0,300,police, pSurf);
    print_text("ECHAP : Cancel",ecran,texte,0,400,police, pSurf);
    int a = text_selection(ecran,texte,police,pSurf,adresse);
    printf("%d,%s\n",a,adresse);
    if (a==2)
    {
        return 4;
    }
    if (a==3)
    {
        return 1;
    }
    return 0;
}

int gui(Network *net, Dataset *dataset)
{
    guiNet = net;
    guiDataset = dataset;

    SDL_Window *ecran = NULL;
    SDL_Surface *texte = NULL; //*fond = NULL;
    TTF_Font *police = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    char* adresse = (char*)malloc(sizeof(char)*(100 + 1));
    ecran = SDL_CreateWindow("OCR - Iron Minds Corporations", 100, 100,
            1080, 720, SDL_WINDOW_SHOWN);
    SDL_Surface *pSurf = SDL_GetWindowSurface(ecran);
    SDL_Surface *surImage = NULL;
    police = TTF_OpenFont("res/Roboto-Regular.ttf", 65);
    printf("police : %d",police==NULL);
    int page = 1;
    while (page)
    {
        printf("%d",page);
        switch(page)
        {
        case 1:
            page = gui_home(ecran,texte,police,pSurf);
            break;
        case 2:
            page = gui_select_image(ecran,texte,police,pSurf,adresse);
            break;
        case 3:
            page = gui_credits(ecran,texte,police,pSurf);
            break;
        case 4:
            surImage = SDL_LoadBMP(adresse);
            if (!surImage)
            {
                page= gui_error_loading(ecran,texte,police,pSurf);
            }
            else
            {
                page= gui_image_validation(ecran,texte,police,pSurf,surImage);
            }
            break;
        case 5:
            page = gui_analysis(ecran,texte,police,pSurf,surImage);
            break;
        }
    }
    TTF_CloseFont(police);
    TTF_Quit();
    SDL_FreeSurface(texte);
    if (surImage)
    {
        SDL_FreeSurface(surImage);
    }
    SDL_Quit();
    return 0;
}


/*
 * TODO :
A MODIFIER :
-> Police
-> Mise en forme (couleurs, taille et disposition des caractères...)
-> Dans le main : appeler gui()
-> Taille de nom de fichier maximale : actuellement à 100
-> Lancement de l'analyse à partir de gui_analysis (voir commentaires)
*/
