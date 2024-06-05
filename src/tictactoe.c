#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tools.h"
#include "window.h"
#define AI_SYM 1
#define PLAYER_SYM -1
#define OFFSET 20
#define GAP 1
#define THICKNESS 20
struct Colors Colors;

const Uint8 *keys;
int padding = 5;
void updateTile(Window *window,Tile *tile);
void loadDefaults(Window *);

short int inBox(int x,int y,SDL_Rect *dest){
    if(dest->x<x && dest->x+dest->w > x && dest->y < y && dest->y + dest->h > y)
        return 1;
    return 0;
    
}

struct Data{
    short int turn,player1_id,player2_id,counter,initialized_1,current_turn;
    short int (*player1)(Matrix *m);
    short int (*player2)(Matrix *m);
    int player1_delay,player2_delay;
    Tile display;
    Fonts fonts;
    int tile_width;
    Tile rects[9];
    Tile *active;
    short int is_active;
    short int _x;
    short int _o;
    short int level;
    Matrix matrix;
    Text *prompt,*gui_turn;
}Data;

//utility

Tile *getActive(int x,int y){
    for(int i=0;i<9;i++){
        if(inBox(x, y, &Data.rects[i].dest))
            return &Data.rects[i];     
    }
    return NULL;
}

int ai(){
    Move move;
    printf("AI's Turn : ");
    findBestMove(&Data.matrix, &move, Data.player2_id, Data.level);
    printf("I Go for [Move: ] %d , [Weight: ] %d\n",move.move,move.value);
    fillChoice(&Data.matrix, -1, move.move);
    return move.move;

}

short int player(Matrix *m){
    return 1;
}

int winnerHandler(Window *window){
    int status = checkWinner(&Data.matrix);
    if(status){
        if(status == Data.player1_id){
            printf("Player won the match.\n");
            strcpy(Data.prompt->text,"You Won, Press P to Play Again");
            updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height - 20);
        }else if(status == Data.player2_id){
            printf("AI Won the match.\n");
            strcpy(Data.prompt->text,"AI Won, Press P to Play Again");
            updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height - 20);
        }else{
            printf("It's Draw bitch.\n");
            strcpy(Data.prompt->text,"Match Draw, Press P to Play Again");
            updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height - 20);
        }
    }
    return status;
}

void homeHandler(Window *window){
    int screen_gap_x = Data.tile_width;
    int screen_gap_y = Data.tile_width;
    // Data.tile_width = screen_gap_x;
    
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(window->renderer, 25, 25, 25, 255);
    SDL_RenderClear(window->renderer);

    SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, 255);
    SDL_SetRenderTarget(window->renderer, Data.display.texture);
    SDL_RenderClear(window->renderer);

    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    //SDL_RenderDrawLine(window->renderer, 0, 0, 100, 100);
    for(int i=0;i<4;i++){
        SDL_RenderDrawLine(window->renderer,0,i*screen_gap_y,Data.display.dest.w,i*screen_gap_y);
        SDL_RenderDrawLine(window->renderer,i*screen_gap_x,0,i*screen_gap_x,Data.display.dest.h);
    }
    for(int i=0;i<9;i++){
        SDL_RenderCopy(window->renderer,Data.rects[i].texture,NULL,&Data.rects[i].dest);
    }
    if(Data.turn == 0){
        if(Data.counter%Data.player2_delay == 0){
            Data.counter = 1;
            Data.turn = 1;
             strcpy(Data.prompt->text,"Your Turn");
            updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height - 20);
            updateTile(window,&Data.rects[ai()]);
            if(winnerHandler(window)){
                Data.turn = -2;
            }
        }
        // printf("Data COUNTER: %d\n",Data.counter);
        Data.counter++;
    }else if(Data.turn == -1){
        loadDefaults(window);
        //ctn
        Data.turn = Data.current_turn;
        if(Data.turn == 1){
            strcpy(Data.prompt->text,"Your Turn");
        }else{
            strcpy(Data.prompt->text,"AI's Turn");
            updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height-20);
        }
        updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height-20);
                    
    }
    SDL_SetRenderTarget(window->renderer, NULL);
    SDL_RenderCopy(window->renderer,Data.display.texture, &Data.display.src, &Data.display.dest);
    for(int i=0;i<font_length;i++){
        if(fonts_app[i]->has_surface){
            // printf("Blitting.\n");
            SDL_RenderCopy(window->renderer, fonts_app[i]->texture, NULL, &fonts_app[i]->dest);
            // printf("Blitting. %s\n",SDL_GetError());
        }
    }
}

//Tile update functiion
void updateTile(Window *window,Tile *tile){
    SDL_SetRenderTarget(window->renderer, tile->texture);
    SDL_SetRenderDrawColor(window->renderer, tile->tile_color.r, tile->tile_color.g, tile->tile_color.b, tile->tile_color.a);
    SDL_RenderClear(window->renderer);
    int index = tile->id-1;
    Sint16 radius = tile->dest.w/2 - 10;
    Sint16 width = tile->dest.w;
    if(!Data.matrix.available[index]){
        if(Data.matrix.data[index] == Data._o){
            Sint16 Q = tile->dest.w/2;
            filledCircleRGBA(window->renderer, Q, Q, radius, 0 , 0, 0, 255);
            filledCircleRGBA(window->renderer, Q, Q, radius - THICKNESS, tile->tile_color.r , tile->tile_color.g, tile->tile_color.b, tile->tile_color.a);
        }else{
            thickLineRGBA(window->renderer, 10, 10, width - 10 ,width-10 ,THICKNESS, 0, 0, 0, 255);
            thickLineRGBA(window->renderer, 10, width - 10, width - 10 ,10 ,THICKNESS, 0, 0, 0, 255);
        }
    }
    SDL_SetRenderTarget(window->renderer, NULL);

}
//gameloop

void gameInputHandler(Window *window,SDL_Event *event){
    int mouse_x,mouse_y,index;
    Tile *active;
    // printf("ERROR gameInput: %s\n",SDL_GetError());
    SDL_SetRenderTarget(window->renderer, Data.display.texture);
    switch (event->type) {
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&mouse_x, &mouse_y);
            if(inBox(mouse_x, mouse_y, &Data.display.dest)){
                mouse_x -= Data.display.dest.x;
                mouse_y -= Data.display.dest.y;
                active = getActive(mouse_x, mouse_y);
                if(Data.is_active && active != Data.active){
                    Data.is_active = 0;
                    Data.active->tile_color.r = 255;
                    Data.active->tile_color.g = 255;
                    Data.active->tile_color.b = 255;
                    updateTile(window, Data.active);
                    // printf("Yes old active end id \n");
                    
                }else if(active && !Data.is_active){
                    Data.is_active = 1;
                    Data.active = active;
                    if(Data.matrix.available[Data.active->id-1]){
                        Data.active->tile_color.r = 0;
                        Data.active->tile_color.g = 125;
                        Data.active->tile_color.b = 240;
                    }else{
                        Data.active->tile_color.r = 255;
                        Data.active->tile_color.g = 50;
                        Data.active->tile_color.b = 24;
                    }
                    updateTile(window, Data.active);
                    // printf("New active found! id: %d\n",active->id);
                }

            }else{
                if(Data.is_active){
                    Data.is_active = 0;
                    Data.active->tile_color.r = 255;
                    Data.active->tile_color.g = 255;
                    Data.active->tile_color.b = 255;
                    updateTile(window, Data.active);
                }
            }
            break;
        
        case SDL_MOUSEBUTTONDOWN:
            if(Data.is_active){
                index = Data.active->id -1;
                // printf("Index. %d\n",index);
                if(Data.matrix.available[index]){
                   if(Data.turn == 1){
                    fillChoice(&Data.matrix, Data.player1_id, index);
                    Data.turn = 0; 
                    strcpy(Data.prompt->text,"AI's Turn");
                    updateTextBox(window, Data.prompt, &Colors.white, window->width/2, window->height-20);
                    updateTile(window, Data.active);
                    winnerHandler(window);
                   }
                }
            }
        case SDL_KEYDOWN:
            if(keys[SDL_SCANCODE_S] == 1){
                Data._o = Data._x+Data._o;
                Data._x = Data._o - Data._x;
                Data._o = Data._o - Data._x;
                for(int i=0;i<9;i++){
                    updateTile(window, Data.rects+i);
                }
            }if(keys[SDL_SCANCODE_P] == 1){
                Data.turn = -1;
            }if(keys[SDL_SCANCODE_T] == 1 && Data.turn == -2){
                Data.current_turn = !Data.current_turn;
                strcpy(Data.gui_turn->text, (Data.current_turn)?"First : You":"First : AI");
                updateTextBox(window, Data.gui_turn, &Colors.white, 10+Data.gui_turn->dest.w/2, window->height - 20);
                
            }
    }
    // printf("%s\n",SDL_GetError());
    
    
}

//default settings

void loadDefaults(Window *window){
    Data.counter = 1;
    Data.level = 9;
    Data.player1_delay = 0;
    Data.player2_delay = 30;
    Data.player1_id = 1;
    Data.player2_id = -1;
    Data.matrix.free = 9;
    if(!Data.initialized_1){
        Data._o = Data.player2_id;
        Data._x = Data.player1_id;
        Data.prompt = getText("Welcome , Press P to Play", Data.fonts.font16, &Colors.red);
        Data.gui_turn = getText((Data.current_turn)?"First: You":"First: AI", Data.fonts.font16, &Colors.red);
    }
    updateTextBox(window,Data.prompt , &Colors.white,window->width/2,window->height-20);
    updateTextBox(window,Data.gui_turn , &Colors.white,10+Data.gui_turn->dest.w/2,window->height-20);
    updateTextBox(window,Data.gui_turn , &Colors.white,10+Data.gui_turn->dest.w/2,window->height-20);
    Data.turn = -2;
    Data.is_active = 0;
    Data.initialized_1 = 1;
    for(int i=0;i<9;i++){
        Data.matrix.data[i] = 0;
        Data.matrix.available[i] = 1;
        updateTile(window, &Data.rects[i]);
    }

}

//Initialization

void init(){

    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    loadFonts(&Data.fonts,"./font.ttf");
    if(Data.fonts.font10){
        printf("(II): Font loaded.\n");
    }else{
        printf("EE: Font loaad failed with error: %s\n",SDL_GetError());
    }
    Colors.black = (SDL_Color){0,0,0,255};
    Colors.white = (SDL_Color){255,255,255,255};
    Colors.blue = (SDL_Color){0,135,240,255};
    Colors.yellow = (SDL_Color){255,255,18,255};
    Colors.red = (SDL_Color){255,28,18,255};
}

//Tiles creation
//main call

int main(int argc, char *argv[]){
    init();
    // printf("%s\n",SDL_GetError());
    Window *window = getWindow("Tic Tac Toe", 600, 650);
    keys = SDL_GetKeyboardState(NULL);
    int display_width = (window->width>window->height)?window->height:window->width;
    display_width -= OFFSET;
    display_width -= display_width%3;
    createTile(window, &Data.display,OFFSET/2,OFFSET/2,display_width,display_width);
    Data.tile_width = Data.display.dest.w/3;
    Data.initialized_1 = 0;
    Data.current_turn = 1;
    for(int i=0;i<9;i++){
        Data.rects[i].dest.x = (i%3)*Data.tile_width + GAP;
        Data.rects[i].dest.y = (i/3)*Data.tile_width + GAP;
        Data.rects[i].dest.w = Data.tile_width;
        Data.rects[i].dest.h = Data.tile_width;
        Data.rects[i].id = i+1;
        Data.rects[i].has_texture = 1;
        Data.rects[i].tile_color = (SDL_Color){255,255,255,255};
        createTile(window, Data.rects+i, Data.rects[i].dest.x, Data.rects[i].dest.y, Data.tile_width-GAP, Data.tile_width-GAP);
    }
    loadDefaults(window);
    window->gameHandler = homeHandler;
    window->eventHandler = gameInputHandler;
    runLoop(window);
    SDL_DestroyTexture(Data.display.texture);
    for(int i=0;i<9;i++){
        SDL_DestroyTexture(Data.rects[i].texture);
    }
    return 0;
}
