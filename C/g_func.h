#ifndef G_FUNC_H
#define G_FUNC_H

#include "defs.h"
#include "globals.h"


void GridShuffle();
/*
FUNCOES DE ALEATORIEDADE
*/

//Retorno uma numero aleatorio inteiro entre 0 e max, com distribuicao uniforme
int xrand(unsigned int max);
/*Retorna um numero aleatorio entre 0 e COLOR_TOTAL, com um peso positivo a
resultados que nao apareceram ha muito tempo e negativo aos que ja apareceram muito*/
int pseudo_xrand(void);


/*
FUNCOES DE ENTIDADES

ENTIDADES sao os objetos do jogo. Possuem 5 elementos:

1: a textura que eh copiada para o renderer

2: uma SDL_Rect cujos w e h representam o tamnho da textura na janela. As
variaveis x e y da SDL_Rect podem ser usadas pelo programador como ele desejar.

3 e 4: 2 doubles: posX e posY, que representam a posicao do canto esquerdo
superior da textura na janela.

5: Um ultimo double, scale, que modifica w e h quando a textura eh renderizada
e tambem eh levado em consideracao quando se deseja pegar o centro da entidade.
*/
ENTITY* EntityCreate(SDL_Surface *surface, double posX, double posY);
void EntityDestroy(void *entity);
int EntityGetCenterX(ENTITY *ball);
int EntityGetCenterY(ENTITY *ball);
void EntitySetCenterX(ENTITY *ball, int x);
void EntitySetCenterY(ENTITY *ball, int y);
void Render(ENTITY* entity);

/*
FUNCOES DE BOLAS
*/
BALL *BallCreate(int color, int posX, int posY, int speedX, int speedY, int radius);
void BallUpdatePosition(BALL *ball);

//Cria uma nova bola para o jogador, de acordo com as configuracoes do jogo
BALL * BallGetAmmo(int posX, int posY, int radius, int pseudoRandom);

int Grid2PosX(int row, int column);
int Grid2PosY(int row, int column);
int * GetNearestGridPoint(int posX, int posY);

//Remove todas as bolas do GRID. Depois, cria (numBalls) novas bolas.
void GridReset(int numBalls);

/*Insere uma nova linha de bolas no topo da tela e desce todas as outras bolas da GRID.
Muda  o estado (GIRD2) de todas as bolas para -2*/
void GridInsert(void);

//Estados de bolas
/*
-2 : Derrubada por nao estar grudada no teto
-1 : Destrida por estar no cluster de bolas da mesma cor acertadas
 0 : Nao existe um bola aqui. Manipular ou ler um GRID com esse estado eh comportamento indefinido.
 1 : Bola existe.
 2 : Bola esta marcada para checar se esta conectada com o teto.
*/

int CountBallsUntil(int row,int column, int numBalls, int howMany);
int RemoveBalls(int row,int column);
int CheckCeiling(int row, int column);
void SaveConnectedBalls(int row, int column);

int BallRemoveBalls(BALL *ball);
int BallCountBallsUntil(BALL *ball, int numBalls, int howMany);
void BallSaveConnectedBalls(BALL *ball);

void BallAddToGrid(int row, int column, BALL **ball, int state);
void BallRemoveFromGrid(int row, int column, BALL **ball);

/*
FUNCOES DE INDICADORES

Indicadores sao entidades que possuem uma "ancora", se movando em volta dela
em um circulo, sempre apontando para a direcao do g_mouse_point (a posicao do
mouse, se o procramador chamar PointCenterMouse(&g_mouse_point)).
*/
INDICATOR* IndicatorCreate(int posX, int posY, int anchorX, int anchorY);
void IndicatorDestroy(INDICATOR* indicator);
void IndicatorUpdatePosition(INDICATOR* indicator);
void RenderIndicator(INDICATOR *entity);

/*
FUNCOES DE ESTRELAS

ESTRELAS sao entidades que aparecem no canto esquerdo da tela, se movendo
ate o canto direito. Seu tamanho eh proporcional a a sua velocidade, de forma
a dar uma perspectiva 3D.

Tambem podem entrar umum modo "turbo", que representa o classico sci-fi de
viagem no hiperespaco.
*/
STAR* StarCreate(int posY, int speedX);
int StarUpdatePosition(STAR* star);
void StarReset(STAR* star, int speedX);
void StarTurbo(STAR* star);
void StarUnTurbo(STAR* star);

/*
FUNCOES DE TEXTBOX

TEXTBOX sao entidades que contem texto.
*/
TEXTBOX* CreateTextBox(int posX, int posY, char* message, TTF_Font *font);
TEXTBOX* CreateTextBoxEx(int posX, int posY, char* message, TTF_Font *font, SDL_Color textColor);


SCOREBOARD * ScoreboardCreate(HIGHSCORES *highscores);
void ScoreboardDestroy(void *entity);
void ScoreboardSetCenter(SCOREBOARD *scoreboard, int centerX, int centerY);
void RenderScoreboard(SCOREBOARD *scoreboard);


ANIMATED *AnimatedCreate(int posX, int posY, int frameTotal, SDL_Surface **surfaces, SDL_Texture **textures, int frameInterval);
void AnimatedDestroy(void *entity);
void AnimatedDestroyKeepTextures(void *entity);
void AnimatedAnimate(ANIMATED *animated);

/*
FUNCOES DE BOTOES

BOTOES sao estruturas que possuem duas entidades: uma entidade chamada frame
que eh o botao em si e uma TEXTBOX que contem o texto do botao.
*/
BUTTON* ButtonCreate(int posX, int posY, char* string);
void CreateButtonArray(BUTTON **buttons, int num, ...);
int ButtonIsHighlighted(BUTTON *button);
int CheckButtons(int howMany, BUTTON **buttons);
void RenderButton(BUTTON *button);
void ButtonDestroy(BUTTON *button);
void ButtonSetCenterX(BUTTON *button, int x);
void ButtonSetCenterY(BUTTON *button, int y);
void RenderButtons(int howMany, BUTTON **buttons);
void DestroyButtonArray(int howMany, BUTTON **buttons);

/*
OUTRAS FUNCOES
*/
void printE(void);
void PointCenterMouse(POINT *point);

#endif
