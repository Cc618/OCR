#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include "network.h"
#include "data.h"
#include "ocr.h"
#include "save.h"

static Network *guiNet;
static Dataset *guiDataset;

#define COL_BLACK ((SDL_Color){0,0,0, 255})
#define COL_RED ((SDL_Color){230,50,30, 255})
#define COL_GREEN ((SDL_Color){120, 230, 150, 255})
#define COL_BG 230, 240, 255
#define COL_TEXTBG 220, 250, 180

void print_text(char* str, SDL_Surface *texte,int x, int y,
        TTF_Font *police,SDL_Surface *pSurf, SDL_Color color)
{
    if (str[0])
    {
        texte = TTF_RenderText_Blended(police, str, color);
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
int gui_analysis(int angle, SDL_Window *ecran,SDL_Surface *texte,
        TTF_Font *police,SDL_Surface *pSurf, SDL_Surface *surImage)
{
    puts("gui_analysis()");

    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
    print_text("Analysis running, please wait...",
                 texte,0,0,police, pSurf, COL_BLACK);
    SDL_UpdateWindowSurface(ecran);

    puts("Starting to fetch text");
    char* result = ocr(surImage, guiNet, guiDataset, angle);
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
            if(event.key.keysym.sym == SDLK_c &&
                    (SDL_GetModState() & KMOD_CTRL) &&
                    SDL_HasClipboardText())
            {
                char *tmp = SDL_GetClipboardText();
                size_t l = strlen(tmp);
                size_t l_copy = ((size_t)len + l < 100 ? l : 100 - (size_t)len);
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

        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));

        int dy = 60;
        int textY = -dy;

        print_text("New analysis",texte,70,textY += dy,
                   police, pSurf, COL_BLACK);
        print_text("Credits",texte,70,textY += dy,
                   police, pSurf, COL_BLACK);
        print_text("Exit",texte,70,textY += dy,
                   police, pSurf, COL_BLACK);
        print_text(">",texte,0,textY - (2 - choix) * dy,
                   police, pSurf, COL_BLACK);

        print_text("Result saved in the clipboard and in result.txt",
                texte,0,textY += dy,police, pSurf, COL_BLACK);
        print_text("* Result :",
                texte,0,textY += dy,police, pSurf, COL_BLACK);

        // Display result line by line
        char *nresult = strdup(result);
        char *token = strtok(nresult, "\n");
        while (token != NULL) {
            print_text(token, texte, 0, textY += dy,
                       police, pSurf, COL_RED);
            token = strtok(NULL, "\n");
        }
        free(nresult);

        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }

    free(result);

    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 3;
}

int gui_error_loading(SDL_Window *ecran,SDL_Surface *texte,
                       TTF_Font *police,SDL_Surface *pSurf)
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
        SDL_FillRect(pSurf, NULL,
                     SDL_MapRGB(pSurf->format, COL_BG));
        print_text("No image found...",
                   texte,0,0,police, pSurf, COL_BLACK);
        print_text("Enter new location",texte,
                   70,100,police, pSurf, COL_BLACK);
        print_text("Cancel",texte,70,200,
                   police, pSurf, COL_BLACK);
        print_text("Exit",texte,70,300,
                   police, pSurf, COL_BLACK);
        print_text(">",texte,0,100+choix*100,
                   police, pSurf, COL_BLACK);
        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 1;
}

int gui_credits(SDL_Window *ecran,SDL_Surface *texte,
                 TTF_Font *police,SDL_Surface *pSurf)
{
    SDL_Event event;
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
    print_text("The team :",texte,0,0,police, pSurf, COL_BLACK);
    print_text("Raphael Brenn",texte,70,100,police, pSurf, COL_BLACK);
    print_text("Leandre Perrot",texte,70,200,police, pSurf, COL_BLACK);
    print_text("Celian Raimbault",texte,70,300,police, pSurf, COL_BLACK);
    print_text("Steve Suissa",texte,70,400,police, pSurf, COL_BLACK);
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

int gui_home(SDL_Window *ecran,SDL_Surface *texte,
              TTF_Font *police,SDL_Surface *pSurf)
{
    SDL_Event event;
    char continuer = 1;
    int choix = 0;
    SDL_Surface *logo_sur = SDL_LoadBMP("res/logo.bmp");
    SDL_Rect logo_rec;
    logo_rec.x = 300;
    logo_rec.y = 300;
    printf("%i\n", logo_sur == NULL);
    logo_rec.w = logo_sur->w;
    logo_rec.h = logo_sur->h;
    while (continuer==1)
    {
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
        print_text("Welcome to Iron Minds' OCR !",texte,
                   0,0,police, pSurf, COL_BLACK);
        print_text("Start analysis",texte,70,100,
                   police, pSurf, COL_BLACK);
        print_text("Credits",texte,70,200,police, pSurf, COL_BLACK);
        print_text("Exit",texte,70,300,police, pSurf, COL_BLACK);
        print_text(">",texte,0,100+choix*100,police, pSurf, COL_BLACK);
        SDL_BlitSurface(logo_sur, NULL, pSurf, &logo_rec);
        SDL_UpdateWindowSurface(ecran);
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
    }
    SDL_FreeSurface(logo_sur);
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 2;
    return 3;
}

int text_selection(SDL_Window *ecran,SDL_Surface *texte,
                   TTF_Font *police,SDL_Surface *pSurf,char* rep)
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
        SDL_FillRect(pSurf,&champ, SDL_MapRGB(pSurf->format, COL_TEXTBG));
        print_text(rep,texte,-1,100,police, pSurf, COL_BLACK);
        SDL_UpdateWindowSurface(ecran);
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT)
            ending = 1;
        else if( event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_BACKSPACE && len)
            {
                rep[len - 1] = 0;
                len--;
            }
            if(event.key.keysym.sym == SDLK_v &&
               (SDL_GetModState() & KMOD_CTRL) && SDL_HasClipboardText())
            {
                char *tmp = SDL_GetClipboardText();
                size_t l = strlen(tmp);
                size_t l_copy = len + l < LEN_MAX ? l : LEN_MAX - len;
                strncpy(rep + len, tmp, l_copy);
                len += l_copy;
                SDL_free(tmp);
            }
            if(event.key.keysym.sym == SDLK_c &&
               (SDL_GetModState() & KMOD_CTRL))
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

        }
    }
    return ending;
}

int gui_select_image(SDL_Window *ecran,SDL_Surface *texte,
                     TTF_Font *police,SDL_Surface *pSurf,char*adresse)
{
    SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
    print_text("Please enter image location",
               texte,0,0,police, pSurf, COL_BLACK);
    print_text("CTRL+C / CTRL+V : Copy / paste",
               texte,0,200,police, pSurf, COL_BLACK);
    print_text("ENTER : Validate",texte,0,300,police, pSurf, COL_BLACK);
    print_text("ECHAP : Cancel",texte,0,400,police, pSurf, COL_BLACK);
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

int gui_angle(SDL_Window *ecran,SDL_Surface *texte, TTF_Font *police,
        SDL_Surface *pSurf)
{
    SDL_Event event;
    int a=0;
    int finalchoix=0;
    char* angle = (char*)malloc(sizeof(char)*4);
    angle[1]=' ';
    angle[3]=0;
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
                    if (choix<3)
                    {
                        choix = (choix+2)%3;
                    }
                    if (choix==3 && a<81)
                        a+=10;
                    if (choix==4 && a<90)
                        a+=1;
                    break;
                case SDLK_DOWN:
                    if (choix<3)
                    {
                        choix = (choix+1)%3;
                    }
                    if (choix==3 && a>9)
                        a-=10;
                    if (choix==4 && a>0)
                        a-=1;
                    break;
                case SDLK_RIGHT:
                    if (choix==3)
                        choix+=1;
                    break;
                case SDLK_LEFT:
                    if (choix==4)
                        choix-=1;
                    break;
                case SDLK_RETURN:
                    if (choix==2)
                        continuer=2;
                    if (choix==1)
                    {
                        finalchoix=1;
                        choix=3;
                    }
                    else
                    {
                        if (choix>2)
                            choix=1;
                        if (choix==0)
                            finalchoix=0;
                    }
                    break;
            }
        }
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
        angle[0]=48+a/10;
        angle[2]=48+a%10;
        print_text("Choose the angle to rotate the image",
                texte,0,0,police, pSurf, COL_BLACK);
        print_text("Let the software choose",
                texte,70,100,police, pSurf, COL_BLACK);
        print_text(angle,texte,409,200,
                police, pSurf, COL_GREEN);
        print_text("Type your angle :       (use arrows and press enter)",
                texte,70,200,police, pSurf, COL_BLACK);
        print_text("Confirm",texte,70,300,police, pSurf, COL_BLACK);
        if (choix<3)
            print_text(">",texte,0,100+choix*100,police,
                        pSurf, COL_BLACK);

        print_text("*",texte,40,100+finalchoix*100,
                   police, pSurf, COL_BLACK);

        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }
    free(angle);
    if (!continuer)
        return -2;
    if (finalchoix==0)
        return -1;
    return a;
}
int gui_image_validation(SDL_Window *ecran,SDL_Surface *texte,TTF_Font *police,
                         SDL_Surface *pSurf, SDL_Surface *surImage)
{
    SDL_Rect r;
    r.x=0; r.y=0; r.w=surImage->w; r.h=surImage->h;
    int winh=900; int winw = 900;
    float d=0.0f;
    if (r.w>winw)
        d = (float)winw/ (float)r.w;
    if (r.h>winh)
        d=(float)winh/(float)r.h;
    if (d)
    {
        r.w*=d;
        r.h*=d;
    }
    int now=0;
    SDL_Delay(700);
    puts("img valid 1");

    SDL_Renderer* ren;
    // SDL_Window *picture = SDL_CreateWindow("OCR - Picture",
    //                                        0,0,r.w,r.h,SDL_WINDOW_SHOWN);
    SDL_Window *picture = SDL_CreateWindow("OCR - Picture",
                                           0,0,500,500,SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(picture,-1,SDL_RENDERER_ACCELERATED);
    puts("img valid B");
    SDL_Texture *texImage = SDL_CreateTextureFromSurface(ren,surImage);
    puts("img valid 2");
    SDL_RenderCopy(ren, texImage, NULL, NULL);
    SDL_RenderPresent(ren);
    SDL_Event event;
    puts("img valid 3");
    char continuer = 1;
    int choix = 0;
    while (continuer==1)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT || (event.type == SDL_WINDOWEVENT
        && event.window.event == SDL_WINDOWEVENT_CLOSE))
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
                    continuer=1+now;
                    break;
            }
        }
        if (event.type == SDL_KEYUP && event.key.keysym.sym==SDLK_RETURN)
            now=1;
        SDL_FillRect(pSurf, NULL, SDL_MapRGB(pSurf->format, COL_BG));
        print_text("Is the picture found the one you want ?",
                   texte,0,0,police, pSurf, COL_BLACK);
        print_text("Confirm",texte,70,100,police, pSurf, COL_BLACK);
        print_text("Retry",texte,70,200,police, pSurf, COL_BLACK);
        print_text("Exit",texte,70,300,police, pSurf, COL_BLACK);
        print_text(">",texte,0,100+choix*100,police, pSurf, COL_BLACK);
        SDL_UpdateWindowSurface(ecran);
        SDL_Delay(10);
    }
    SDL_DestroyWindow(picture);
    SDL_DestroyRenderer(ren);
    SDL_DestroyTexture(texImage);
    if (!continuer || choix==2)
        return 0;
    if (!choix)
        return 6;
    return 2;
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
    char* adresse = (char*)malloc(sizeof(char)*(1024 + 1));
    ecran = SDL_CreateWindow("OCR - Iron Minds Corporations", 50, 50,
            1620, 960, SDL_WINDOW_SHOWN);
    SDL_Surface *pSurf = SDL_GetWindowSurface(ecran);
    SDL_Surface *surImage = NULL;
    police = TTF_OpenFont("res/Roboto-Regular.ttf", 44);
    printf("police : %d",police==NULL);
    int page = 1;
    int angle = 0;
    while (page)
    {
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
            puts("Page 4");
            if (!surImage)
            {
                page= gui_error_loading(ecran,texte,police,pSurf);
            }
            else
            {
                page = gui_image_validation(ecran,texte,police,pSurf,surImage);
            }
            break;
        case 5:
            page = gui_analysis(angle, ecran,texte,police,pSurf,surImage);
            break;
        case 6:
            puts("Page 6");
            angle = gui_angle(ecran,texte,police,pSurf);
            page = 5;
            if (angle == -2) {
                page = 0;
            }
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
