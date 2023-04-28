#include "SDL2/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define WIDTH 400
#define HEIGHT 400
#define WSNAK 6
#define HSNAK 6

//Global variables
int snake_size;

//Structs
typedef struct {
    int x, y;
}Pos;

typedef struct{
    int x, y, radius;
    int score;
}Circle; 

//Enums
enum STRANA{
    LEVO = 1,
    DESNO,
    GORE,
    DOLE
};

//function prototypes
void cntrlEvent(SDL_Window *, bool *, Pos *);
void rend(SDL_Renderer *, Pos *, Circle *);
void inputCntrl(int *);
void control_snake(Pos *, int);
void find_circle_loc(Circle *, Pos *);
void init(Pos *, Circle *);
void colldet(Circle *, Pos *, bool *);
int main(int argc, char **argv);

/***************
 ** SDL STUFF **
 ***************/
void cntrlEvent(SDL_Window *window, bool *end, Pos *poz){

    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_WINDOWEVENT_CLOSE:
                if(window){
                    SDL_DestroyWindow(window);
                    window = NULL;
                    (*end) = 0;
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        (*end) = 0;
                        break;
                }
                break;
            case SDL_QUIT:
                (*end) = 0;
                break;
        }
    }

}

void rend(SDL_Renderer *renderer, Pos *poz, Circle *circ){

    SDL_SetRenderDrawColor(renderer,  0, 0, 0, 0);

    SDL_RenderClear(renderer);

    for(int i = 0; i < snake_size; i++){
        SDL_SetRenderDrawColor(renderer,62, 254, 35, 255);
        SDL_Rect rect = { poz[i].x, poz[i].y, WSNAK, HSNAK };
        SDL_RenderFillRect(renderer, &rect);
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
                SDL_RenderDrawPoint(renderer, circ->x + dx, circ->y + dy);
            }
        }
    }

    SDL_RenderPresent(renderer);

}


void inputCntrl(int *strana){

     const uint8_t *state = SDL_GetKeyboardState(NULL);

    if((state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT] ) && *strana != DESNO && *strana) *strana =  LEVO;
    if((state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) && *strana != LEVO ) *strana =  DESNO;
    if((state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]   ) && *strana != DOLE ) *strana =  GORE;
    if((state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN] ) && *strana != GORE ) *strana =  DOLE;

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
        case LEVO:
            poz[0].x-= WSNAK;
            break;
        case DESNO:
            poz[0].x+= WSNAK;
            break;
        case GORE:
            poz[0].y -= HSNAK;
            break;
        case DOLE:
            poz[0].y += HSNAK;
            break;
        default:
            break;
    }
    if(strana == DOLE && poz->y >= HEIGHT)
        poz->y = 0;
    if(strana == GORE && poz->y <= 0 - HSNAK)
        poz->y = HEIGHT;
    if(strana == LEVO && poz->x <= 0)
        poz->x = WIDTH;
    if(strana == DESNO && poz->x >= WIDTH)
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

void init(Pos *poz, Circle *circ){

    poz[0].x = WIDTH/2 - 1;
    poz[0].y = HEIGHT/2 - 1;
    for(int i = 1; i < snake_size; i++){
        poz[i].x = poz[i - 1].x - WSNAK;
        poz[i].y = HEIGHT/2 - 1;
    }

    find_circle_loc(circ, poz);
    circ->radius = 5;
    circ->score = 0;
}

int main(int argc, char *argv[]){
    
    Circle circ;
    Pos poz[10000]; 
    int strana = 0;
    bool end = 1;
    snake_size = 15;
    init(poz, &circ);

    SDL_Window *window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               WIDTH, HEIGHT, 0
                            );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1 ,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Init(SDL_INIT_VIDEO);

    while(end){
        cntrlEvent(window, &end, poz);
        rend(renderer, poz, &circ);
        inputCntrl(&strana);      
        colldet(&circ, poz, &end);
        control_snake(poz, strana);
        SDL_Delay(40);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}
