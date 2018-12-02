#ifndef GLOBALS_H
#define GLOBALS_H
#include "defs.h"

extern SDL_Window* g_window;
extern SDL_Event g_event;
extern Uint32 g_SubSystemsFlags;


extern char IMAGENS[COLOR_TOTAL][50]; //Imagens das bolas
extern int GRID_color_counter[COLOR_TOTAL];
extern SDL_Color g_ball_color[COLOR_TOTAL];

/*
Superficies e texturas
*/
//Superficie branca das dimensoes da tela. Pode ser usada para criar entidades retangulares de qualquer cor.
extern SDL_Surface* g_window_surface;
extern SDL_Surface* g_temp_surface;

/*
Sons e musicas
*/
extern Mix_Music* music;
extern Mix_Chunk* sound;
extern Mix_Chunk *g_sounnd1[2]; //Som de turbo 0 = turbo 1 = unturbo
extern Mix_Chunk *buttonClick;
extern Mix_Chunk *countPoints;

/*
Booleanas
*/
extern SDL_bool stop; // Se verdadeiro, sair do programa.
extern int temp_bool; // Usado para manter o jogo em loop
extern int main_menu; // Usado para verificar se o jogador quer voltar para o main menu

/*
Fontes
*/
extern TTF_Font *g_font_OpenSans;
extern TTF_Font *g_font_Hacker;
extern TTF_Font *g_font_StarWars;

/*
---------------------------
Globais usados em g_func
---------------------------
*/
extern POINT g_mouse_point;
extern int g_ball_counters[NUM_BALLS];
extern int DESTROYED;
extern SDL_Renderer* gRenderer;

/*
Superficies
*/
extern SDL_Surface* g_star_surface;
extern SDL_Texture* g_ball_textures[COLOR_TOTAL];
extern SDL_Surface* g_indicator_surface;

/*
Matrizes
*/
//Matriz de bolas
extern BALL *GRID[20][NUM_BALLS];
//Matriz com os estados das bolas
extern int **GRID2;
//Matriz que serve para tornar enderecos negativos valido para GRID2
extern int GRID3[100][100];

#endif
