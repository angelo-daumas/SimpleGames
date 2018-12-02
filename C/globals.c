#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "defs.h"
#include "globals.h"

SDL_Window* g_window = NULL;

SDL_Event g_event;
Uint32 g_SubSystemsFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
BALL *GRID[20][NUM_BALLS] = {{NULL}};
int GRID3[100][100] = {{0}};
int **GRID2;



char IMAGENS[COLOR_TOTAL][50] = {BALL_RED, BALL_GREEN, BALL_BLUE, BALL_YELLOW, BALL_TEAL, BALL_LIGHTBLUE };
int GRID_color_counter[COLOR_TOTAL] = {0};

SDL_Color g_ball_color[COLOR_TOTAL] = {
{255,0,0}, {0,128,0}, {0,0,255}, {255,255,0}, {28,230,185}, {126,191,241}
};

int DESTROYED = 0;
SDL_Renderer* gRenderer = NULL;

SDL_Surface* g_star_surface;
SDL_Texture* g_ball_textures[COLOR_TOTAL];
SDL_Surface* g_indicator_surface;
SDL_Surface* g_temp_surface;


SDL_Surface* g_window_surface = NULL;

Mix_Music* music;
Mix_Chunk* sound;
Mix_Chunk *g_sounnd1[2];
Mix_Chunk *buttonClick;
Mix_Chunk *countPoints;

SDL_bool stop = SDL_FALSE; // Se verdadeiro, sair do programa.
int temp_bool = 1;
int main_menu = 1;

TTF_Font *g_font_OpenSans;
TTF_Font *g_font_Hacker;
TTF_Font *g_font_StarWars;


int g_ball_counters[NUM_BALLS] = {0};

POINT g_mouse_point;
