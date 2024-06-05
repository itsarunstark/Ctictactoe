#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#ifndef WINDOW_H_
#define WINDOW_H_

extern int padding;
extern int font_index;

extern struct Colors{
    SDL_Color blue,black,yellow,red,white;
}Colors;
typedef struct Window{
    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Color bg_color;
    int width,height;
    short int FPS;
    bool exit_window;
    void (*gameHandler)(struct Window *);
    void (*eventHandler)(struct Window *, SDL_Event *event);
}Window;

typedef struct Fonts{
    TTF_Font *font12;
    TTF_Font *font10;
    TTF_Font *font16;
	TTF_Font *font22;
}Fonts;

typedef struct Text{
    SDL_Texture *texture;
    char text[250];
    short int has_surface;
    TTF_Font *font;
    SDL_Color color;
    SDL_Rect src,dest;
}Text;

extern Text *fonts_app[1024];
extern int font_length;

typedef struct Display{
    SDL_Texture *texture;
    SDL_Rect dest,src;
    short int has_texture,id;
    SDL_Color tile_color;
}Tile;

extern Text *texts[1024];
extern int text_length;
Window *getWindow(const char* title,int _width,int _height);
void destroyWindow(Window *);
void runLoop(Window *);
void loadFonts(Fonts *fonts,const char *font);
Text *generateFont(Window *window,Text *txt);
Tile *createTile(Window *window,Tile *display,int x,int y,int width,int height);
Text *getText(const char *text,TTF_Font *font,SDL_Color *color);
Text *updateTextBox(Window *window,Text *text,SDL_Color *bg_color,int x,int y);
// void updateTile(Window *window,Tile *tile);

#endif