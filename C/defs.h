#ifndef DEFS_H
#define DEFS_H

/*
---------------------------
Codigos
---------------------------
*/
#define ERROR -1
#define FALSE 0
#define OK 0
#define TRUE 1


/*
---------------------------
Matriz de bolas
---------------------------
*/
#define GRID_CELL 64//(32*2)
#define GRID_START (GRID_CELL/2)
#define GRID_CELLY (28*2)
#define GRID_STARTY (GRID_CELLY/2)

#define NUM_BALLS 19 //Numero de bolas por fileira


/*
---------------------------
Janela
---------------------------
*/
#define VIDEO_H 960
#define VIDEO_W (1280-GRID_START)
#define NOME "Space Burster | Grupo Danti"

/*
---------------------------
Definicoes do jogo
---------------------------
*/
#define BG_R 0 //Cor de fundo (preto)
#define BG_G 0
#define BG_B 0

#define GAME_SPEED (1000/60) //Tempo ente cada quadro, em milisegundos

/*
---------------------------
Entidades
---------------------------
*/
#define BALL_SCALE 1
#define BALL_D (61) //Diametro da bola
#define BALL_R (BALL_D/2)

#define BALL_START_X (VIDEO_W/2 -BALL_R) //Posicao inicial da bola do jogador
#define BALL_START_Y (VIDEO_H - BALL_D - GRID_CELLY)

//Cores das Bolas:
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_YELLOW 3
#define COLOR_TEAL 4
#define COLOR_LIGHTBLUE 5

#define COLOR_TOTAL 6

// #define BALL_RED "Images/Balls/Planet1.png"//"Images/Balls/circle_new_red.bmp"
// #define BALL_GREEN "Images/Balls/Planet6.png"//"Images/Balls/circle_new_green.bmp"
// #define BALL_BLUE  "Images/Balls/Planet3.png"//"Images/Balls/circle_new_blue.bmp"
// #define BALL_YELLOW "Images/Balls/PlanetEarth.png"//"Images/Balls/circle_new_yellow.bmp"
// #define BALL_TEAL  "Images/Balls/Planet5.png"//"Images/Balls/circle_new_teal.bmp"
// #define BALL_LIGHTBLUE "Images/Balls/Planet4.png"//"Images/Balls/circle_new_lightblue.bmp"

#define BALL_RED "Images/Balls/circle_new_red.bmp"
#define BALL_GREEN "Images/Balls/circle_new_green.bmp"
#define BALL_BLUE  "Images/Balls/circle_new_blue.bmp"
#define BALL_YELLOW "Images/Balls/circle_new_yellow.bmp"
#define BALL_TEAL  "Images/Balls/circle_new_teal.bmp"
#define BALL_LIGHTBLUE "Images/Balls/circle_new_lightblue.bmp"

#define INDICATOR_SCALE 1.2

#define STAR_MAX_SPEED 6 //Velocidade maxima das estrelas
#define STAR_SCALE_DIV 10. //Divisor para escala das estrelas
#define STAR_SPEED (rand()%5 + STAR_MAX_SPEED-5)

/*
---------------------------
Pontuacao por bolas destruidas
---------------------------
*/
#define POINTS_DEFAULT 10
#define POINTS_EXTRA (2*POINTS_DEFAULT)
#define POINTS_FALLING (10*POINTS_DEFAULT)

#define PLAYER_NAME_MAX_CHARACTERS 20

#define SP_START 0 //Spaceship initial position


/*
---------------------------
Math
---------------------------
*/

#define PI 3.14159265358979323846
#define distance(a,b) sqrt(pow(a,2) + pow(b,2))


/*
---------------------------
Structs
---------------------------
*/

/*
Other
*/
typedef struct _SCORE{
	int value;
	int bonus;
	int multiplier;
} SCORE;

typedef struct _POINT{
	int x,y;
} POINT;

typedef struct _HIGHSCORES{
  char playerNames[10][20];
	int scores[10];
} HIGHSCORES;

/*
Entities
*/
#define IS_ENTITY \
	SDL_Texture* texture; \
	SDL_Rect* image; \
	double posX,posY,scale; \
	void(*destroy)(void *entity);

typedef struct _ENTITY {
	IS_ENTITY
} ENTITY;

typedef struct _BALL {
	IS_ENTITY
	double speedX, speedY;
	int radius, diameter, border,color;

} BALL;

typedef struct _INDICATOR {
	IS_ENTITY
	int anchorX, anchorY;
	double angle;
} INDICATOR;

typedef struct _STAR {
	IS_ENTITY
	int speedX;
	int turboSpeedX;
	char turbo;
	int count;
} STAR;

typedef struct _TEXTBOX {
	IS_ENTITY
} TEXTBOX;

typedef struct _SCOREBOARD{
  IS_ENTITY
  HIGHSCORES highscores;
  TEXTBOX *textboxes[3][10];
} SCOREBOARD;

typedef struct _ANIMATED{
  IS_ENTITY
  SDL_Texture **frames;
	int frameTotal, frameCurrent, frameInterval;
} ANIMATED;


/*
Multi-Entity
*/
typedef struct _BUTTON{
	ENTITY *frame;
	TEXTBOX *text;
} BUTTON;

/*
Funcoes genericas de entidades
*/
#define EntGetCenterX(a) EntityGetCenterX((ENTITY *) a)
#define EntGetCenterY(a) EntityGetCenterY((ENTITY *) a)
#define EntSetCenterX(a,b) EntitySetCenterX((ENTITY *) a,b)
#define EntSetCenterY(a,b) EntitySetCenterY((ENTITY *) a,b)
#define EntDestroy(a) (a)->destroy((ENTITY*) a)



#endif
