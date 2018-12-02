/*
Jogo Space Burster - Computação I - UFRJ 2017/2
Professor: Adriano Joaquim de Oliveira Cruz

Alunos:
-------
ANGELO AUGUSTO COSTA RIBEIRO DAUMAS   DRE: 115046541
DANIEL BAYERL VIEIRA                  DRE: 117234580
TIAGO SANTOS MARTINS DE MACEDO        DRE: 116022689
-------

Descrição:
-------
-------
Hotkeys:
-------
N/A
-------
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include "defs.h"
#include "globals.h"
#include "g_func.h"
#include "game_states.h"
#include "file_IO.h"



// Funcoes globais (escopo restrito a este arquivo):

void MainInitialize(ENTITY **backround, ENTITY **spaceship, STAR **STARS);
void DisplayVersion(void); //Mostra a versao do SDL para debug
void LoadSurfaces(void); //Coloca as imagens na memoria
void LoadSounds(void); //Coloca os sons na memoria
void LoadFonts(); //Coloca os fontes na memoria
void CreateRendererAndWindow(void); //Cria renderizador e janela
void Initialize_Grid2_Global(void); //Inicializa GRID2 e seta ele com um certo offset a GRID3


// Variaveis globais (escopo restrito a este arquivo):
/*
TODO: All Settings
TODO: Get Sounds from laptop


TODO: AmmoRemove(BALL **ammo, int position, BALL **ball)
Remove uma bola da position, opcionalmente fazendo o ponteiro *ball apontar para ela. Depois, move cada bola atras dela para a frente e insere uma bola no final.
TODO: AmmoCheck(BALL **ammo)
Checka o ammo, removendo bolas que nao sejam de uma cor valida, usando a funcao acima.
*/

int main(int argc, char *argv[])
{
	//Entidades
	STAR *STARS[20];
	ENTITY *backround;
	ENTITY *spaceship;

	//// Primeiramente, inicializar as coisas.
	MainInitialize(&backround, &spaceship, STARS);


	//Gamestates e loop principal
	char playerName[PLAYER_NAME_MAX_CHARACTERS+1] = "PlayerName";
	FILE *file = fopen("Settings/PlayerName.config", "r");
	if (!file)
		GameState_MainMenu_InputPlayerName(PLAYER_NAME_MAX_CHARACTERS, playerName, backround);
	else{
		fgets(playerName, PLAYER_NAME_MAX_CHARACTERS+1, file);
		fclose(file);
	}
	while (!stop && temp_bool){
		if (main_menu){
			GameState_MainMenu(backround, spaceship, STARS, playerName);
			main_menu = 0;
		}
		GameState_GameSession(backround, spaceship, STARS, playerName);
	}
	// Se o loop acima terminou, o programa foi fechado. Hora de dizer adeus.
	SDL_DestroyWindow(g_window);
	Mix_FreeChunk(sound);
	TTF_Quit();
	SDL_Quit();
	return 0;
}

void MainInitialize(ENTITY **backround, ENTITY **spaceship, STAR **STARS){
	Initialize_Grid2_Global(); //Validar enderecos negativos de GRID2
	srand( time(NULL) ); //Setar semente da aleatoriedade

	if (SDL_Init(g_SubSystemsFlags) < 0)
		printE();
	if (TTF_Init() < 0)
		printE();

	// Criar a janela e renderizador.
	CreateRendererAndWindow();

	//Mostrar versao e loadar arquivos na memoria
	DisplayVersion();
	puts("Loading...");
	puts("...Images...");
	LoadSurfaces();
	puts("...Sounds...");
	LoadSounds();
	Mix_VolumeMusic(32);
	puts("...Fonts...");
	LoadFonts();
	puts("...Loading Finished");

	int j;
	puts("Initializing Entities...");
	*backround = EntityCreate(g_window_surface, 0,0); //Backround
	SDL_SetTextureColorMod((*backround)->texture, BG_R, BG_G, BG_B);
	(*spaceship) = EntityCreate(g_temp_surface,VIDEO_W-1050,0); //Spaceship
	(*spaceship)->image->w *=4;
	(*spaceship)->image->h *=4;
	EntSetCenterX(*spaceship, VIDEO_W/2);
	SDL_FreeSurface(g_temp_surface);
	for(j=0;j<20;j++){ //Stars
		STARS[j] = StarCreate(rand()%VIDEO_H, STAR_SPEED);
		STARS[j]->posX = rand()%VIDEO_W;
	}
	puts("...Initialization Finished");
}

void DisplayVersion(void){
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("We compiled against SDL version %d.%d.%d ...\n",
		   compiled.major, compiled.minor, compiled.patch);
	printf("But we are linking against SDL version %d.%d.%d.\n",
		   linked.major, linked.minor, linked.patch);
}

void LoadSurfaces(void){
	int i;
	SDL_Surface *ballSurfaces[COLOR_TOTAL];

	g_star_surface = SDL_LoadBMP("Images/star.bmp");
	if (!g_star_surface)
		printE();

	for (i=0;i<COLOR_TOTAL;i++){
		 ballSurfaces[i] = IMG_Load(IMAGENS[i]);
		 if (!ballSurfaces[i])
			printE();
		 SDL_SetColorKey(ballSurfaces[i], SDL_TRUE, SDL_MapRGB(ballSurfaces[i]->format,255,255,255));
		 g_ball_textures[i] = SDL_CreateTextureFromSurface(gRenderer, ballSurfaces[i]);
		 SDL_FreeSurface(ballSurfaces[i]);
	g_indicator_surface = IMG_Load("Images/Indicator2.png");
	if (!g_indicator_surface)
		printE();
	}

	//g_temp_surface = SDL_LoadBMP("Images/modest-rocket-coloring-pages-top-child-coloring-de.bmp");
	g_temp_surface = IMG_Load("Images/deathstar0.png");
	if (!g_temp_surface)
		printE();
	SDL_SetColorKey(g_temp_surface, SDL_TRUE, SDL_MapRGB(g_temp_surface->format,161,127,255));

	g_window_surface = SDL_CreateRGBSurface(0, VIDEO_W, VIDEO_H, 32, 0, 0, 0, 0);
	if (!g_window_surface)
		printE();
	SDL_FillRect(g_window_surface, NULL, SDL_MapRGB(g_window_surface->format, 255, 255, 255));
}

void LoadSounds(void){
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
		printE();
	Mix_AllocateChannels(16);

	music = Mix_LoadMUS("Sounds/Take On Me (8 Bit Remix Cover Version) [Tribute to A-ha] - 8 Bit Universe.mp3");
 		if (!music)
 			printE();
	sound = Mix_LoadWAV("Sounds/Blop-Mark_DiAngelo-79054334.wav");
		if (!sound)
			printE();
	g_sounnd1[0] = Mix_LoadWAV("Sounds/126422__cabeeno-rossley__level-up.wav");
		if (!g_sounnd1[0])
			printE();
	g_sounnd1[1] = Mix_LoadWAV("Sounds/126423__cabeeno-rossley__shoot-laser.wav");
		if (!g_sounnd1[1])
					printE();
	buttonClick = Mix_LoadWAV("Sounds/333047__christopherderp__videogame-menu-button-clicking-sound-18.wav");
		if (!g_sounnd1[1])
					printE();
	countPoints = Mix_LoadWAV("Sounds/126419__cabeeno-rossley__counting-coins.wav");
		if (!g_sounnd1[1])
					printE();
}

void LoadFonts(){
	g_font_OpenSans = TTF_OpenFont("Fonts/OpenSans-Regular.ttf", 28);
	g_font_Hacker = TTF_OpenFont("Fonts/Hacker.ttf", 24);
	g_font_StarWars = TTF_OpenFont("Fonts/Starjhol.ttf", 64);

	TTF_SetFontHinting(g_font_OpenSans, TTF_HINTING_LIGHT);
	if(!g_font_OpenSans) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		// handle error
	}
	if(!g_font_Hacker) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		// handle error
	}
	if(!g_font_StarWars) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		// handle error
	}
}

void CreateRendererAndWindow(void){
	g_window = SDL_CreateWindow(NOME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
	if (!g_window)
		printE();

	gRenderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
	if (!gRenderer)
		printE();
	SDL_RenderSetLogicalSize(gRenderer, VIDEO_W, VIDEO_H);
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
}

/*
O proposito dessa funcao eh proteger o programa contra erros de memoria relacionados
ao acesso de indeces negativo do array que indica o estado (existencia, marcada pra destruir, etc.)
das bolas. Ou seja, seu objetivo final eh que GRID2[-1][-1] seja um endereco valido.
*/
void Initialize_Grid2_Global(void){
	GRID2 = malloc(100*sizeof(int*)); //Mallocar GRID2 TODO: Tem algum jeito melhor?
	int offset1 = 40;
	int offset2 = 40;

	int x,y; //Setar os ponteiros de GRID2 com um offset a GRID3
	 for (x=0;x<100;x++)
			GRID2[x] = &GRID3[x][offset2];
	printf( "%p %p\n", (void*) &GRID3[0][0], (void*) &GRID2[-1][-1]);

	int pointerError = 0; //Checar tudo para ter certeza de que nao ocorreram erros
	for (x=0;x<100;x++)
		for (y=0;y<100;y++){
			if(&GRID3[x][y] != &GRID2[x][y-offset2]){
				printf("%d %d\n", x,y);
				pointerError =1;
			}
			else if (GRID3[x][y] != 0 || GRID2[x][y-offset2] != 0){
				printf("%d | %d : %d %d | %d %d\n", GRID3[x][y], GRID2[x][y-offset2], x,y,x-1,y-1);
				pointerError = 1;
			}
		}

	GRID2+=offset1;

	for (x=0;x<100;x++)
		for (y=0;y<100;y++){
			if(&GRID3[x][y] != &GRID2[x-offset1][y-offset2]){
				printf("%d %d\n", x,y);
				pointerError =1;
			}
			else if (GRID3[x][y] != 0 || GRID2[x-offset1][y-offset2] != 0){
				printf("%d | %d : %d %d | %d %d\n", GRID3[x][y], GRID2[x-offset1][y-offset2], x,y,x-1,y-1);
				pointerError = 1;
			}
		}


		if (pointerError){
			puts("CRITICAL POINTER ERROR");
			exit(EXIT_FAILURE);
		}
		else{
			printf("Array GRID2[i][j] valido para enderecos (%d < i < %d) e (%d < j < %d) \n", -offset1, 99-offset1, -offset2, 99-offset2);
		}
}
