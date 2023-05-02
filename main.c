#include "SDL2/SDL.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <math.h>


#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#define WIDTH 400
#define HEIGHT 400
#define WSNAK 6
#define HSNAK 6
#define ENOUGH ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

//Global variables
int snake_size;
int strana = 0;

//Structs
typedef struct {
    int x, y;
}Pos;

typedef struct{
    int x, y, radius;
    int score;
}Circle; 

typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Surface *tmp;
    SDL_Texture *tmptex;
} SDL_S;

typedef struct{
    Circle circ;
    Pos poz[10000];
    SDL_S sdl_s;
    bool end;
    char *rez;
} Game; 

Game game;
//Enums
enum STRANA{
    LEFT = 1,
    RIGHT,
    UP,
    DOWN
};

//function prototypes
void rend(SDL_S *, Pos *, Circle *);
void inputCntrl(int *, bool *, SDL_S *);
void control_snake(Pos *, int);
void find_circle_loc(Circle *, Pos *);
void init(Pos *, Circle *, SDL_S *);
void colldet(Circle *, Pos *, bool *);
void Destroy(SDL_S *);
void gameLoop(void);
int main(void);

/***************
 ** SDL STUFF **
 ***************/
void kvadrat(SDL_S *sdl_s, int y, int x, int VELX, int VELY,int R, int G, int B){
    SDL_SetRenderDrawColor(sdl_s->renderer, R, G, B, 255);
    SDL_Rect rect1 = { y, x, VELX, VELY};
    SDL_RenderFillRect(sdl_s->renderer, &rect1); 
}
void _fonts_s(SDL_S *sdl_s){
    sdl_s->font = TTF_OpenFont("fonts/HeavyDataNerdFont-Regular.ttf", 21);
    if (sdl_s->font==NULL){
        printf("Failed to load font: %s", SDL_GetError());
    }
}
void rend(SDL_S *sdl_s, Pos *poz, Circle *circ){
        
    SDL_SetRenderDrawColor(sdl_s->renderer,  0, 0, 0, 0);

    SDL_RenderClear(sdl_s->renderer);
    
    for(int x = 0; x < WIDTH; x += 50){
        for(int y = 0; y < HEIGHT; y += 100){
            kvadrat(sdl_s, y, x, 50, 50, 6, 33, 13);
        }
        x+=50;
        for(int y = 50; y < HEIGHT; y += 100){
            kvadrat(sdl_s, y, x, 50, 50, 6, 33, 13);

        }
    }

    kvadrat(sdl_s, poz[0].x, poz[0].y, WSNAK, HSNAK, 243, 135, 21);

    for(int i = 1; i < snake_size; i++){
        kvadrat(sdl_s, poz[i].x, poz[i].y, WSNAK, HSNAK, 62, 254, 35);
    }

    if(!(circ->score % 30) && circ->score)
        circ->radius = 10;
    else 
        circ->radius = 5;

    for (int i = 0; i < circ->radius * 2; ++i) {
        for (int j = 0; j < circ->radius * 2; ++j) {
            int dx = circ->radius - i;
            int dy = circ->radius - j;
            if ((dx*dx + dy*dy) <= (circ->radius * circ->radius)) {
                SDL_RenderDrawPoint(sdl_s->renderer, circ->x + dx, circ->y + dy);
            }
        }
    }


    int len = snprintf(NULL, 0, "%d", circ->score);
    game.rez = malloc(len + 1);

    snprintf(game.rez, len + 1, "%d", circ->score);

    SDL_Color white = {255, 255, 255, 255};
    sdl_s->tmp = TTF_RenderText_Blended(sdl_s->font, game.rez, white);
    sdl_s->tmptex = SDL_CreateTextureFromSurface(sdl_s->renderer, sdl_s->tmp);
    SDL_Rect textrect = {360, 10, 30, 30};
    SDL_RenderCopy(sdl_s->renderer, sdl_s->tmptex, NULL, &textrect);



    SDL_RenderPresent(sdl_s->renderer);

}


void inputCntrl(int *strana, bool *end, SDL_S *sdl_s){

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
                if(sdl_s->window){
                    SDL_DestroyWindow(sdl_s->window);
                    sdl_s->window = NULL;
                    (*end) = 0;
                }
                break;
            case SDL_QUIT:
                (*end) = 0;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_LEFT:
                        if (*strana != RIGHT && *strana) *strana = LEFT;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        if (*strana != LEFT) *strana = RIGHT;
                        break;
                    case SDLK_w:
                    case SDLK_UP:
                        if (*strana != DOWN) *strana = UP;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        if (*strana != UP) *strana = DOWN;
                        break;
                    case SDLK_z:
                        if (SDL_GetModState() & KMOD_LSHIFT) *end = false;
                        break;
                }
                break;
        }
    }
}

/****************************************************************
 *                       Game "LOGIC"                           *
 ****************************************************************/

void control_snake(Pos *poz, int strana){
    if(strana){
        for(int i = snake_size - 1 ; i > 0; i--){
            poz[i].x = poz[i - 1].x;
            poz[i].y = poz[i - 1].y;
        }
    }
    switch(strana){
        case LEFT:
            poz[0].x-= WSNAK;
            break;
        case RIGHT:
            poz[0].x+= WSNAK;
            break;
        case UP:
            poz[0].y -= HSNAK;
            break;
        case DOWN:
            poz[0].y += HSNAK;
            break;
        default:
            break;
    }
    if(strana == DOWN && poz->y >= HEIGHT)
        poz->y = 0;
    if(strana == UP && poz->y <= 0 - HSNAK)
        poz->y = HEIGHT;
    if(strana == LEFT && poz->x <= 0)
        poz->x = WIDTH;
    if(strana == RIGHT && poz->x >= WIDTH)
        poz->x = 0 ;


}

void find_circle_loc(Circle *circ, Pos *poz){
    srand(time(0));
    int min_dis = 50, dis;
    do {
        circ->x = rand()%WIDTH;
        circ->y = rand()%HEIGHT;
        dis = min_dis;
        for (int i = 0; i < snake_size; i++) {
            int dx = poz[i].x - circ->x;
            int dy = poz[i].y - circ->y;
            int d = sqrt(dx*dx + dy*dy);
            if (d < dis) 
                dis = d;
        }
    }while(dis < min_dis);
}

void colldet(Circle *circ, Pos *poz, bool *end){
        if(((poz[0].x + WSNAK >= circ->x && poz[0].x + WSNAK <= circ->x + circ->radius * 2) ||
            (poz[0].x <= circ->x + circ->radius * 2 && poz[0].x >= circ->x)) && 
            (poz[0].y >= circ->y - circ->radius * 2 && poz[0].y <= circ->y + circ->radius * 2)){
                find_circle_loc(circ, poz);
                snake_size += WSNAK;
                circ->score += 5;
        }
        if(((poz[0].y + HSNAK >= circ->y && poz[0].y + HSNAK <= circ->y + circ->radius * 2) ||
            (poz[0].y <= circ->y + circ->radius && poz[0].y >= circ->y)) && 
            (poz[0].x >= circ->x - circ->radius * 2 && poz[0].x <= circ->x + circ->radius * 2)){
                find_circle_loc(circ, poz);
                snake_size += WSNAK;
                circ->score += 5;
        }
        for (int i = 2; i < snake_size; ++i) {
            if (poz[0].x == poz[i].x && poz[0].y == poz[i].y) {
                *end = 0;
            }
        }

}

/****************************************************************
 *                       Initialize variables                   *
 ****************************************************************/

void init(Pos *poz, Circle *circ, SDL_S *sdl_s){

    snake_size = 15;
    poz[0].x = WIDTH/2 - 1;
    poz[0].y = HEIGHT/2 - 1;
    for(int i = 1; i < snake_size; i++){
        poz[i].x = poz[i - 1].x - WSNAK;
        poz[i].y = HEIGHT/2 - 1;
    }

    find_circle_loc(circ, poz);
    circ->radius = 5;
    circ->score = 0;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &sdl_s->window, &sdl_s->renderer);

}
void gameLoop(void){
    bool end = 1;
   
#ifdef __EMSCRIPTEN__
#define P 1
#else
    while(end){
#endif
        inputCntrl(&strana, &end, &game.sdl_s);
        colldet(&game.circ, game.poz, &end);
        control_snake(game.poz, strana);
        rend(&game.sdl_s, game.poz, &game.circ);

        SDL_Delay(40);
#ifdef __EMSCRIPTEN__
#define P 1
        if(!end){
            init(game.poz, &game.circ, &game.sdl_s);
            end = 1;
            strana = RIGHT;
        }
#else
    }
#endif
}
void Destroy(SDL_S *sdl_s){
    SDL_DestroyWindow(sdl_s->window);
    SDL_DestroyRenderer(sdl_s->renderer);


    SDL_FreeSurface(sdl_s->tmp);
    free(game.rez);
    SDL_DestroyTexture(sdl_s->tmptex);

    TTF_CloseFont(sdl_s->font);
    TTF_Quit();
    SDL_Quit();
}
int main(void){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_EVENTS);
    TTF_Init();


    init(game.poz, &game.circ, &game.sdl_s);
    _fonts_s(&game.sdl_s);
#ifdef __EMSCRIPTEN__
  //  emscripten_set_main_loop_arg(gameLoop, &game,0, 0);

    emscripten_set_main_loop(gameLoop, 0, 1);
#else
    gameLoop();
#endif

    Destroy(&game.sdl_s);
    return 0;
}
