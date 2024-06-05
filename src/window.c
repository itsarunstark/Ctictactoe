#include "window.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int text_length = 0;

Text *fonts_app[1024];
int font_length = 0;
// extern int padding;
void dummyGameHandler(Window *window){
    //I do nothing. I am dummy.
}
Tile *createTile(Window *window,Tile *display,int x,int y,int width,int height){
    display->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA8888,\
    SDL_TEXTUREACCESS_TARGET, width, height);
    // printf("EE: %s\n",SDL_GetError());
    display->dest.w = width;
    display->dest.h = height;
    display->src.x = 0;
    display->src.y = 0;
    display->src.w = width;
    display->src.h = height;
    display->dest.x = x;
    display->dest.y = y;
    return display;
}



void loadFonts(Fonts *fonts,const char *font){
    fonts->font10 = TTF_OpenFont(font, 10);
    fonts->font12 = TTF_OpenFont(font, 12);
    fonts->font16 = TTF_OpenFont(font, 16);
}

Text *generateFont(Window *window,Text *text){
    if(text->has_surface){
        SDL_DestroyTexture(text->texture);
    }
    SDL_Surface *surf = TTF_RenderText_Blended(text->font, text->text, text->color);
    // printf("%s HEHE\n",SDL_GetError());
    text->dest.w = surf->w;
    text->dest.h = surf->h;
    text->src.x = text->src.y = 0;
    text->src.w = surf->w;
    text->src.h = surf->h;
    text->has_surface = 1;
    text->texture = SDL_CreateTextureFromSurface(window->renderer, surf);
    SDL_FreeSurface(surf);
    return text;
}

Text *getText(const char *text,TTF_Font *font,SDL_Color *color){
    Text *txt = (Text *)malloc(sizeof(Text));
    strcpy(txt->text, text);
    txt->color.r = color->r;
    txt->color.g = color->g;
    txt->color.b = color->b;
    txt->color.a = color->a;
    txt->font = font;
    txt->has_surface = 0;
    fonts_app[font_length++] = txt;
    return txt;
}

Text *updateTextBox(Window *window,Text *text,SDL_Color *bg_color,int x,int y){
    text = generateFont(window,text);
    text->dest.x = padding;
    text->dest.y = padding;
    SDL_Texture *back = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, text->dest.w+2*padding, text->dest.h+2*padding);
    SDL_SetRenderTarget(window->renderer, back);
    SDL_SetRenderDrawColor(window->renderer, bg_color->r, bg_color->g, bg_color->b, bg_color->a);
    SDL_RenderClear(window->renderer);
    SDL_RenderCopy(window->renderer, text->texture, &text->src, &text->dest);
    SDL_DestroyTexture(text->texture);
    text->texture = back;
    text->dest.w += 2*padding;
    text->dest.h += 2*padding;
    text->dest.x = x - (text->dest.w)/2;
    text->dest.y = y - (text->dest.h)/2;
    SDL_SetRenderTarget(window->renderer, NULL);
    // printf("Called.\n");
    return text;
}


void dummyEventHandler(Window *window,SDL_Event *event){
    //Hey, I am dummy.
}

Window *getWindow(const char *title,int width,int height){
    Window *window = (Window *)malloc(sizeof(Window));
    window->width = width;
    window->height = height;
    window->FPS = 30;
    if(!window){
        printf("Window allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    window->screen = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);
    if(!window->screen){
        fprintf(stderr, "SDL ERROR WINDOW CREATION FAILED WITH ERROR: %s\n", SDL_GetError());
        free(window);
        exit(EXIT_FAILURE);
    }
    window->renderer = SDL_CreateRenderer(window->screen, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    if(!window->renderer){
        fprintf(stderr, "SDL ERROR WINDOW CREATION FAILED WITH ERROR: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->screen);
        free(window);
        exit(EXIT_FAILURE);
    }
    window->exit_window = false;
    window->bg_color = (SDL_Color){20,100,165,255};
    window->eventHandler = dummyEventHandler;
    window->gameHandler = dummyGameHandler;
    window->surface = SDL_GetWindowSurface(window->screen);
    return window;
}

void runLoop(Window *window){
    SDL_Event event;
    while(!window->exit_window){
        while(SDL_PollEvent(&event)){
            window->eventHandler(window,&event);
            switch (event.type) {
                case SDL_QUIT:
                    window->exit_window = 1;
            }
        }
        SDL_SetRenderDrawColor(window->renderer, window->bg_color.r, window->bg_color.g, window->bg_color.b, window->bg_color.a);
        SDL_RenderClear(window->renderer);
        window->gameHandler(window);
        SDL_RenderPresent(window->renderer);
        SDL_Delay(1000./window->FPS);
    }
    destroyWindow(window);
}

void destroyWindow(Window *window){
    for(int i=0;i<font_length;i++){
        if(fonts_app[i]){
            SDL_DestroyTexture(fonts_app[i]->texture);
            free(fonts_app[i]);
        }
    }
    SDL_DestroyRenderer(window->renderer);
    SDL_FreeSurface(window->surface);
    SDL_DestroyWindow(window->screen);
    free(window);
    printf("Window is completely destoyed.\n");
}