#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "globals.h"
#include "g_func.h"
#include <math.h>

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE ALEATORIEDADE
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

//Codigo baseado em http://www.azillionmonkeys.com/qed/random.html
//Chance de rodar + de 1 vez: (RAND_MAX % max) / (RAND_MAX + 1.0)
//Para 6 cores, isso resultam em 4.656612873077393e-10
#define RAND_INV_RANGE(r) ( ( ((unsigned int)RAND_MAX + 1)) /  (r))
int xrand(unsigned int max){
	unsigned int x;
	do {
	    x = rand();
	} while (x >= max * RAND_INV_RANGE (max));
	return (int) (x / RAND_INV_RANGE (max));
}

//TODO: Checar dentro da funcao se a cor existe ou nao na GRID
int pseudo_xrand(void){
	static int C[COLOR_TOTAL] = {0}; //Inicializar contador

	int init = xrand(COLOR_TOTAL); //Comecar a checagem em uma posicao aleatoria
	int i = init;
	i++;

	int result, found = 0;
	for (i%=COLOR_TOTAL;i != init;i%=COLOR_TOTAL){ //Checar a probabilidade para cada cor
		if (xrand(2)-C[i] <= 0 && !found){
			printf("%d %d\n", i, C[i]);
			C[i] = -3; //Reduzir contador se a cor for escolhida
			result = i;
			found = 1;
		}
		else{
			C[i]++; //Aumentar o contador se a cor n foi escolhida
		}
		i++;
	}

	if (!found){ //Por fim, se nenhuma cor foi escolhida no loop, usar a cor inicial
		C[i] = -3;
		result = i;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE ENTIDADES
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

ENTITY* EntityCreate(SDL_Surface *surface, double posX, double posY){
	ENTITY *newEnt;

	//Criar Bola
	newEnt = (ENTITY *) malloc(sizeof(ENTITY));
	if (!newEnt) {
	printf("Erro na alocacao de memoria para criar nova entidade.\n");
	exit(1);
	}

	//Setar membros da bola
	(*newEnt).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	(*newEnt).image->w = surface->w;
	(*newEnt).image->h = surface->h;
	(*newEnt).texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	(*newEnt).posX = posX;
	(*newEnt).posY = posY;
	(*newEnt).scale = 1;

	newEnt->destroy = &EntityDestroy;

	return newEnt;
}

void EntityDestroy(void *entity){
	ENTITY* ball = (ENTITY*) entity;
	SDL_DestroyTexture(ball->texture);
	free(ball->image);
	free(ball);
}

int EntityGetCenterX(ENTITY *ball){
	return (*ball).posX + (*ball).image->w*ball->scale/2;
}

int EntityGetCenterY(ENTITY *ball){
	return (*ball).posY + (*ball).image->h*ball->scale/2;
}

void EntitySetCenterX(ENTITY *ball, int x){
	(*ball).posX = x - (*ball).image->w*ball->scale/2;
}

void EntitySetCenterY(ENTITY *ball, int y){
	(*ball).posY = y - (*ball).image->h*ball->scale/2;
}

void PointCenterMouse(POINT *point){
	float scaleX, scaleY;
	SDL_Rect viewPort;

	SDL_RenderGetScale(gRenderer, &scaleX, &scaleY);
	SDL_GetMouseState( &point->x, &point->y );
	SDL_RenderGetViewport(gRenderer, &viewPort);

	point->y /= scaleY;
	point->x /= scaleX;
	point->y -= viewPort.y;
	point->x -= viewPort.x;
}

void Render(ENTITY* entity) {
	float scaleX, scaleY;
	SDL_Rect dst_rect;

	SDL_RenderGetScale(gRenderer, &scaleX, &scaleY);

	dst_rect.x = (*entity).posX ;//*scaleX;
	dst_rect.y = (*entity).posY ;//*scaleY;
	dst_rect.w = entity->image->w * entity->scale;
	dst_rect.h = entity->image->h * entity->scale;

	if (SDL_RenderCopy(gRenderer, entity->texture,  NULL, &dst_rect) < 0) {
		printE();
	}
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE BOLAS
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
void EntityDestroyKeepTexture(void *entity){
  ENTITY* ball = (ENTITY*) entity;
  free(ball->image);
  free(ball);
}
//----------------------------------------------------------------------
//CRIACAO E DESTRUICAO
//----------------------------------------------------------------------
BALL* BallCreate(int color, int posX, int posY, int speedX, int speedY, int diameter) {
	BALL *newBall;

	//Criar Bola
	newBall = (BALL *) malloc(sizeof(BALL));
	if (!newBall) {
	printf("Erro na alocacao de memoria para criar nova bola.\n");
	exit(1);
	}


	//Setar membros da bola
	(*newBall).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	(*newBall).image->w = BALL_D;
	(*newBall).image->h = BALL_D;
	(*newBall).texture = g_ball_textures[color];

	(*newBall).color = color;
	(*newBall).posX = posX;
	(*newBall).posY = posY;
	(*newBall).speedX = speedX;
	(*newBall).speedY = speedY;
	(*newBall).diameter = BALL_D;
	(*newBall).radius = BALL_D/2;
	(*newBall).border = (newBall->image->h - BALL_D)/2;
	(*newBall).scale = BALL_SCALE;

	(*newBall).destroy = &EntityDestroyKeepTexture;


	return newBall;
}

int BallRemoveBalls(BALL *ball){
	int *grid;
	int result;
	grid = GetNearestGridPoint(EntGetCenterX(ball), EntGetCenterY(ball));
	result = RemoveBalls(grid[0],grid[1]);
	free(grid);
	return result;
}

//TODO: Melhorar essa funcao
//TODO: Fazer funcao de refill do ammo para remover bolas que n devem ficar na magazine
BALL * BallGetAmmo(int posX, int posY, int radius, int pseudoRandom){
	int color, proceed = 0;;
	if (1){ //g_settings_SmartAmmo
		for (color = 0; color < COLOR_TOTAL; color++){
			if(GRID_color_counter[color] > 0){
				proceed = 1;
				break;
			}
		}
			if (!proceed)
				return BallCreate(0, posX, posY, 0, 0, radius);;
	}

	if (pseudoRandom)
		color = pseudo_xrand();
	else
		color = xrand(COLOR_TOTAL);


	if (1){ //g_settings_SmartAmmo
		while (GRID_color_counter[color] < 1){
			color = xrand(COLOR_TOTAL);
		}
	}

	return BallCreate(color, posX, posY, 0, 0, radius);
}


//----------------------------------------------------------------------
//RECURSIVIDADE
//----------------------------------------------------------------------
void InitializeMatrixNeighbourThresholds(int row, int column, int *start, int *end){
	if (row%2){
		start[0] = column;
		end[0] = column+2;
	}
	else{
		start[0] = column-1;
		end[0] = column+1;
	}
	start[1] = column-1;
	end[1] = column+2;
}

int CountBallsUntil(int row,int column, int numBalls, int howMany){
	int i, j, color = GRID[row][column]->color;
	int start[2], end[2], count=0;

	GRID2[row][column] = 0; //Seta a bola para zero para ela n ser percebida por outras bolas

	//Devido a a estrutura da matriz, os limites do for devem ser diferentes, dependendo se a linha eh par ou impar
	InitializeMatrixNeighbourThresholds(row, column, start, end);

	for (i=row-1;i<row+2;i++){
		for(j=start[count];j<end[count];j++){
			if (GRID2[i][j]>0){
				if (GRID[i][j]->color == color){
					numBalls++; //Aumenta numero de bolas encontradas
					if (numBalls>=howMany){
						GRID2[row][column] = -2;
						return numBalls;
					}
					numBalls = CountBallsUntil(i,j,numBalls,howMany); //Seta numero de bolas para numero encontrado pela segunda bola
				}
			}
		}
		count++;
		count %= 2;
	}

	GRID2[row][column] = -2;
	return numBalls;
}

int BallCountBallsUntil(BALL *ball, int from, int until){
	int *grid;
	int numBalls;
	grid = GetNearestGridPoint(EntGetCenterX(ball), EntGetCenterY(ball));
	numBalls = CountBallsUntil(grid[0],grid[1], from, until);
	free(grid);
	return numBalls;
}

void SaveConnectedBalls(int row, int column){
	int i, j, start[2], end[2], count=0;
	printf("%d %d\n", row, column);

	GRID2[row][column] = 0; //Seta a bola para zero para ela n ser percebida por outras bolas

	//Devido a a estrutura da matriz, os limites do for devem ser diferentes, dependendo se a linha eh par ou impar
	InitializeMatrixNeighbourThresholds(row, column, start, end);

	for (i=row-1;i<row+2;i++){
		for(j=start[count];j<end[count];j++){
			printf("%d %d\n", i, j);
			if (GRID2[i][j] == -2){ //Checa se a bola vizinha havia sido derrubada
				SaveConnectedBalls(i,j); //Salva bola vizinha
			}
		}
		count++;
		count %= 2;
		printf("%d\n", count);
	}

	GRID2[row][column] = 1; //Salva a bola depois de salvar as vizinhas
}

void BallSaveConnectedBalls(BALL *ball){
	int *grid;
	grid = GetNearestGridPoint(EntGetCenterX(ball), EntGetCenterY(ball));
	SaveConnectedBalls(grid[0],grid[1]);
	free(grid);
}

int CheckCeiling(int row,int column){

	if (row==0)
		return 1;
	GRID2[row][column] = 0; //Seta a bola para zero para ela n ser percebida por outras bolas

	int i, j, start[2], end[2], count=0, shouldSave = 0;
	//Devido a a estrutura da matriz, os limites do for devem ser diferentes, dependendo se a linha eh par ou impar
	InitializeMatrixNeighbourThresholds(row, column, start, end);

	puts("ha");
	for (i=row-1;i<row+2;i++){
		for(j=start[count];j<end[count];j++){
			if (GRID2[i][j]>0){ //Checa se bola existe naquela posicao (e n esta marcada para ser destruida)
				if (CheckCeiling(i,j)){ //Se a bola estiver presa ao teto de alguma forma, a func retorna 1
					if (shouldSave)
						SaveConnectedBalls(row,column);
					GRID2[row][column] = 1; //Bola esta presa, loga ela pode existir normalmente
					return 1;
				}
				else if (GRID2[i][j] == -2)
					shouldSave = 1;
			}
		}
		count++;
		count %= 2;
	}

	GRID2[row][column] = -2;
	return 0;
}

int RemoveBalls(int row,int column){
	int i, j, delete = 0, color = GRID[row][column]->color;
	int start[2], end[2], count=0;

	GRID2[row][column] = 0; //Seta a bola para zero para ela n ser percebida por outras bolas

	//Devido a a estrutura da matriz, os limites do for devem ser diferentes, dependendo se a linha eh par ou impar
	InitializeMatrixNeighbourThresholds(row, column, start, end);

	for (i=row-1;i<row+2;i++){
		for(j=start[count];j<end[count];j++){
			printf("%d : %d %d\n", GRID2[i][j], i, j);
			if (GRID2[i][j]>0){
				if (GRID[i][j]->color == color){
					RemoveBalls(i,j);
					if (GRID2[i][j] > 0){ //Checa se bola ainda existe (acontece caso seja a ultima do grupo)
						GRID2[i][j] = -1; //Marca bola para ser destruida, se ela era a ultima bola do grupo
					}
					delete = 1; //Marca esta bola para ser destruida
				}
				else{
					GRID2[i][j] = 2; //Marca bola para o loop checar se ela ainda tem conexao com o teto
				}
			}
		}
		count++;
		count %= 2;
	}

	if (delete){
		DESTROYED = 1;
		GRID2[row][column] = -1;
		return 1;
 	}
	else{
		GRID2[row][column] = 1;
	}
	return 0;
}


//----------------------------------------------------------------------
//COLISAO E MOVIMENTO
//----------------------------------------------------------------------
int Quicar(int position, int wall){
	return wall + (wall - position);
}//Quique unidimensional completamente elastico (para as paredes laterais)



int BallCollide2(BALL *ball){
	int *grid;
	int centerX = EntGetCenterX(ball);
	int centerY = EntGetCenterY(ball);
	int diameter = ball->diameter;
	int row,column;
	int i,j;

	grid = GetNearestGridPoint(centerX, centerY);
	row = grid[0];
	column = grid[1];
	free(grid);

	for(i=row-2;i<row+2;i++) //A fileira de tras deve ser checada pois a bola pode vir de um angulo estranho
		for(j=column-3;j<column+4;j++)
			if (i>=0 && j>=0 && GRID2[i][j] != 0)
				if (distance(centerX-Grid2PosX(i,j) , centerY-Grid2PosY(i,j)) < diameter)
					return 1;

	return 0;
}


int *BallGridGetFreeCell(int posX, int posY, int row,int column){
	int i,j, search = 2;
	double minDistance = 2147483647,curDistance;
	static int grid[2];
	grid[0] = row;
	grid[1] = column;
	while ((grid[0] == row && grid[1] == column && GRID2[row][column] != 0) || search == 2){
		puts("search");
		for(i=row-search;i<row+search;i++){
			for(j=column-search;j<column+search;j++){
				if (j >= 0 && j < NUM_BALLS && i>= 0 && GRID2[i][j] == 0){
					curDistance = distance(  Grid2PosX(i,j) - posX , Grid2PosY(i,j) - posY);
					if ( curDistance  < minDistance){
							minDistance = curDistance;
							grid[0] = i;
							grid[1] = j;
					}
				}
			}
		}
		search++;
		search++;
	}
	printf("Left %d %d", i, j);
	return grid;
}

void WallCollide(BALL *ball, int position, int posOffset, int  wall, double *speed){
	position += posOffset;
	if (position > wall){
		*speed = -(*speed);
		printf("Bola quicou. x:%d y:%d\n", (int) (*ball).posX, (int) (*ball).posY);
		EntSetCenterX(ball, Quicar(position, wall) - posOffset);
	}
}

void BallPlaceFreeCell(BALL *ball){
	int *grid;
	int *grid2;
	grid = GetNearestGridPoint(ball->posX,ball->posY);
	grid2 = BallGridGetFreeCell(EntGetCenterX(ball), EntGetCenterY(ball), grid[0],grid[1]);
	BallAddToGrid(grid2[0], grid2[1], &ball, 1);
	free(grid);
}
void BallUpdatePosition(BALL *ball) {
	int centerX, centerY;

	(*ball).posX += (*ball).speedX;
	(*ball).posY += (*ball).speedY;

	centerX = EntGetCenterX(ball);
	centerY = EntGetCenterY(ball);

	//Atingiu borda da direita
	WallCollide(ball, centerX, ball->radius, VIDEO_W, &ball->speedX);
	//Atingiu borda inferior
	WallCollide(ball, centerY, ball->radius, VIDEO_H, &ball->speedY);
	//Atingiu borda da esquerda
	if (((((*ball).posX) + (*ball).border) < 0)){
		(*ball).speedX = -(*ball).speedX;
		printf("Bola quicou: x:%d y:%d\n", (int) (*ball).posX, (int) (*ball).posY);
		(*ball).posX = Quicar((*ball).posX+(*ball).border, 0) - (*ball).border;
	}
	//Atingiu borda superior
	if ((((*ball).posY) + (*ball).border) < 0) {
		if ((*ball).speedY != 0){ //Checar se bola ja foi presa
			(*ball).speedY = 0;
			(*ball).speedX = 0;
			(*ball).posY = 0;
			BallPlaceFreeCell(ball);
			printf("Bola presa. Coordenadas finais: x:%d y:%d\n", (int) (*ball).posX, (int) (*ball).posY);
		}
	}
	//Colidiu com outra bola
	else if (BallCollide2(ball)){
		if ((*ball).speedY != 0){ //Checar se bola ja foi presa
			(*ball).speedY = 0;
			(*ball).speedX = 0;
			BallPlaceFreeCell(ball);
			printf("Bola presa. Coordenadas finais: x:%d y:%d\n", (int) (*ball).posX, (int) (*ball).posY);
	}
	}
}


//----------------------------------------------------------------------
//FUNCOES DA MATRIZ DE BOLAS
//----------------------------------------------------------------------
int * GetNearestGridPoint(int posX, int posY){
	int *result = (int *) malloc(sizeof(int) * 2);
	int row;

	if (posY <= GRID_STARTY) row = 0; //Necessario para numeros negativos
	else row = posY/GRID_CELLY;

	result[0] = row;
	result[1] = 0;

	if (posX <= GRID_START) return result;
	//Fileira impar
	if (row%2){
		posX = posX - GRID_START;
		if (posX <= GRID_CELL) return result;
	}
	result[1] = posX/GRID_CELL;
	return result;
}

int Grid2PosX(int row, int column){
	if (row%2) return GRID_CELL + GRID_CELL*(column);
	return GRID_START + GRID_CELL*column;
}//Recebe uma linha e uma coluna e retorna a poiscao X desse elemento da matriz

int Grid2PosY(int row, int column){
	return GRID_STARTY + GRID_CELLY*row;
}//Recebe uma linha e uma coluna e retorna a poiscao Y desse elemento da matriz

void GridReset(int numBalls){
	int i,j,countBalls = 0;
	for(j=0;j<20;j++){
		for(i=0;i<NUM_BALLS;i++){
			if (GRID2[j][i] != 0){
				BallRemoveFromGrid(j,i, NULL);
			}
			if (countBalls < numBalls){
				BallAddToGrid(j, i, NULL, 1);
				countBalls++;
			}
		}
	}
}

void GridInsert(void){
	int i,j;
	for(i=18;i>=0;i--){
		for(j=0;j<NUM_BALLS;j++){
				if (GRID2[i][j] != 0){
					EntSetCenterX(GRID[i][j], Grid2PosX(i+1,j));
					EntSetCenterY(GRID[i][j], Grid2PosY(i+1,j));
					GRID2[i+1][j] = -2;
					GRID[i+1][j] = GRID[i][j];
					GRID2[i][j] = 0;
					GRID[i][j] = NULL;
				}
			}
		}
	for(j=0;j<NUM_BALLS;j++)
		BallAddToGrid(0,j, NULL, -2);
}

void GridShuffle(void){
	int i,j;
	for(j=0;j<20;j++){
		for(i=0;i<NUM_BALLS;i++){
			if (GRID2[j][i] != 0){
				BallRemoveFromGrid(j,i, NULL);
				BallAddToGrid(j, i, NULL, 1);
			}
		}
	}
}


void BallAddToGrid(int row, int column, BALL **ball, int state){
	if (ball == NULL)
		GRID[row][column] = BallCreate(xrand(COLOR_TOTAL), 0, 0, 0, 0, BALL_D);
	else
		GRID[row][column] = *ball;

	GRID2[row][column] = state;
	GRID_color_counter[GRID[row][column]->color]++;

	EntSetCenterX(GRID[row][column], Grid2PosX(row, column));
	EntSetCenterY(GRID[row][column], Grid2PosY(row, column));
}

void BallRemoveFromGrid(int row, int column, BALL **ball){
	GRID_color_counter[GRID[row][column]->color]--;
	GRID2[row][column] = 0;

	if (ball == NULL)
		EntDestroy(GRID[row][column]);
	else
		*ball = GRID[row][column];
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE INDICADORES
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

INDICATOR* IndicatorCreate(int posX, int posY, int anchorX, int anchorY){
	INDICATOR *newIndicator;
	SDL_Surface* image;

	//Criar Bola
	newIndicator = (INDICATOR *) malloc(sizeof(INDICATOR));
	if (!newIndicator) {
	printf("Erro na alocacao de memoria para criar indicador do mouse.\n");
	exit(1);
	}

	//Loadar Imagem
	image = g_indicator_surface;



	//Setar membros da bola
	(*newIndicator).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	//(*newIndicator).image->w = image->w;
	//(*newIndicator).image->h = image->h;
	(*newIndicator).image->w = 30;
	(*newIndicator).image->h = 30;
	(*newIndicator).texture = SDL_CreateTextureFromSurface(gRenderer, image);
	(*newIndicator).posX = posX;
	(*newIndicator).posY = posY;
	(*newIndicator).anchorX = anchorX;
	(*newIndicator).anchorY = anchorY;
	(*newIndicator).scale = INDICATOR_SCALE;
	newIndicator->angle = 0;

	newIndicator->destroy = &EntityDestroy;

	return newIndicator;
}

void IndicatorUpdatePosition(INDICATOR* indicator){
	double angle;
	int mouse_x = g_mouse_point.x, mouse_y = g_mouse_point.y;


	angle = atan2(mouse_y - indicator->anchorY, mouse_x - indicator->anchorX);
		//angle = atan2(mouse_y - indicator->anchorY, mouse_x  - indicator->anchorX);

	if (angle < -355 * (PI/360)){
		angle = -355 * (PI/360);
	}
	else if (angle > -5 * (PI/360)){
		if (angle > 180 * (PI/360))
			angle = -355 * (PI/360);
		else
			angle = -5 * (PI/360);
}

	indicator->angle = angle * 360/2/PI;
	EntSetCenterX( indicator, indicator->anchorX + cos(angle)*100);
	EntSetCenterY( indicator, indicator->anchorY + sin(angle)*100);
}

void RenderIndicator(INDICATOR *entity){
	float scaleX, scaleY;
	SDL_Rect dst_rect;

	SDL_RenderGetScale(gRenderer, &scaleX, &scaleY);

	dst_rect.x = (*entity).posX ;//*scaleX;
	dst_rect.y = (*entity).posY ;//*scaleY;
	dst_rect.w = entity->image->w * entity->scale;
	dst_rect.h = entity->image->h * entity->scale;

	if (SDL_RenderCopyEx(gRenderer, entity->texture,  NULL, &dst_rect, entity->angle, NULL, SDL_FLIP_NONE) < 0) {
		printE();
	}
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE ESTRELAS
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

STAR* StarCreate(int posY,int speedX){
	STAR *newStar;
	SDL_Surface* image;

	//Criar Bola
	newStar = (STAR *) malloc(sizeof(STAR));
	if (!newStar) {
	printf("Erro na alocacao de memoria para criar estrela.\n");
	exit(1);
	}

	//Loadar Imagem
	image = g_star_surface;


	//Setar membros da bola
	(*newStar).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	(*newStar).image->w = image->w;
	(*newStar).image->h = image->h;
	(*newStar).texture = SDL_CreateTextureFromSurface(gRenderer, image);
	(*newStar).posX = 0;
	(*newStar).posY = posY;
	(*newStar).speedX = speedX;
	(*newStar).scale = speedX/10.;
	(*newStar).turbo = '\0';
	(*newStar).turboSpeedX = speedX;
	(*newStar).count = 1;


	return newStar;
}

int StarUpdatePosition(STAR* star){

	(*star).posX += (*star).speedX;

	if (star->turbo){
		StarTurbo(star);
	}
	else StarUnTurbo(star);
	//Atingiu borda da direita
	if (star->posX > (VIDEO_W)){
		return 1;
	}
	return 0;
}

void StarReset(STAR* star, int speedX){
	star->posX = 0;
	star->posY = rand()%VIDEO_H;
	star->speedX = speedX;
	star->turboSpeedX = speedX;
	star->scale= (star->speedX)/STAR_SCALE_DIV;
	if (star->turbo){
		star->scale= (star->speedX+5)/STAR_SCALE_DIV;
		star->turboSpeedX = speedX*5;
		star->speedX = star->turboSpeedX;
		StarTurbo(star);
	}
}

void StarTurbo(STAR* star){
	if (!star->turbo){
		star->turboSpeedX = star->turboSpeedX*5;
		star->scale= (star->speedX+5)/STAR_SCALE_DIV;
		star->turbo = 'y';
	}
	if (star->count > 6){
		star->count = 1;
		if (star->speedX < star->turboSpeedX){
			star->speedX++;
		}
	}
	else star->count++;//printf("%d\n",++star->count);
		if (star->turboSpeedX > 14 && star->posX != 0 && (star->image->w < (g_star_surface->w + (star->turboSpeedX/5)*10) ) )
			star->image->w++;
		else if (star->turboSpeedX > 14)
			star->image->w = g_star_surface->w + star->turboSpeedX/5*10;
		else
			star->image->w = g_star_surface->w;


}

void StarUnTurbo(STAR* star){
	if (star->turbo){
		star->turboSpeedX /= 5;
		star->turbo = '\0';
		star->scale= (star->turboSpeedX)/STAR_SCALE_DIV;
	}
	if (star->count > 12){
		if (star->speedX > star->turboSpeedX){
			star->speedX--;
		}
		star->count = 1;
	}
	else star->count++;//printf("%d\n",++star->count);
		if (star->image->w > g_star_surface->w){
			star->image->w--;
			star->posX+= 2;
		}
		else
			star->image->w = g_star_surface->w;

}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE CAIXAS DE TEXTO
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

void GetTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, SDL_Color textColor);

TEXTBOX* CreateTextBox(int posX, int posY, char* message, TTF_Font *font){
	return CreateTextBoxEx(posX, posY, message, font, (SDL_Color) {255, 255, 255, 0});
}

TEXTBOX* CreateTextBoxEx(int posX, int posY, char* message, TTF_Font *font, SDL_Color textColor){
	TEXTBOX* newText;

	//Criar Bola
	newText = (TEXTBOX *) malloc(sizeof(TEXTBOX));
	if (!newText) {
		printf("Erro na alocacao de memoria para criar caixa de texto.\n");
		exit(1);
	}

	//Loadar Imagem
	(*newText).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	if (strlen(message) == 0)
		GetTextAndRect(gRenderer, posX, posY, " ", font, &(newText->texture), newText->image, textColor);
	else
		GetTextAndRect(gRenderer, posX, posY, message, font, &(newText->texture), newText->image, textColor);

	//Setar membros da bola
	(*newText).posX = posX;
	(*newText).posY = posY;
	(*newText).scale = 1;

	newText->destroy = &EntityDestroy;

	return newText;
}

void GetTextAndRect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, SDL_Color textColor) {
    int text_width;
    int text_height;
    SDL_Surface *surface;

    //surface = TTF_RenderText_Solid(font, text, textColor);
    surface = TTF_RenderText_Blended(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE SCOREBOARD
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------


SCOREBOARD * ScoreboardCreate(HIGHSCORES *highscores){
	SCOREBOARD *scoreboard;
	SDL_Surface *surface;
	surface = SDL_CreateRGBSurface(0, VIDEO_W/2 - 100, VIDEO_H - 200, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 128, 128, 128));
	int i;

	//Criar Bola
	scoreboard = (SCOREBOARD *) malloc(sizeof(SCOREBOARD));



	//Setar membros da bola
	(*scoreboard).image = (SDL_Rect*) malloc (sizeof(SDL_Rect));
	(*scoreboard).image->w = surface->w;
	(*scoreboard).image->h = surface->h;
	(*scoreboard).texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	scoreboard->scale = 1;
	scoreboard->destroy = &ScoreboardDestroy;

	//Loadar Imagem
	SDL_FreeSurface(surface);

	char numbers[11];
	//Setar as textboxes
	for (i=0;i<10;i++){
		sprintf(numbers, "%d", i);
		scoreboard->textboxes[0][i] = CreateTextBox(0,0, numbers, g_font_OpenSans);
		scoreboard->textboxes[1][i] = CreateTextBox(0,0, highscores->playerNames[i], g_font_OpenSans);
		sprintf(numbers, "%d", highscores->scores[i]);
		scoreboard->textboxes[2][i] = CreateTextBox(0,0, numbers, g_font_OpenSans);
	}
	return scoreboard;
}

void ScoreboardDestroy(void *entity){
	SCOREBOARD *scoreboard = (SCOREBOARD*) entity;
	int i,j;
	for(i=0;i<3;i++)
		for (j=0;j<10;j++)
			EntDestroy(scoreboard->textboxes[i][j]);
	EntityDestroy(entity);
}

void ScoreboardSetCenter(SCOREBOARD *scoreboard, int centerX, int centerY){
	int i;

	EntSetCenterX(scoreboard, centerX);
	EntSetCenterY(scoreboard, centerY);

	for (i=0;i<10;i++){
		EntSetCenterX(scoreboard->textboxes[0][i], scoreboard->posX + 10);
		EntSetCenterX(scoreboard->textboxes[1][i], scoreboard->posX + scoreboard->image->w / 2);
		scoreboard->textboxes[2][i]->posX = scoreboard->posX + scoreboard->image->w - scoreboard->textboxes[2][i]->image->w - 10;
		EntSetCenterY(scoreboard->textboxes[0][i], scoreboard->posY + (i+1) * (scoreboard->image->h/10) - (scoreboard->image->h/20));
		EntSetCenterY(scoreboard->textboxes[1][i], scoreboard->posY + (i+1) * (scoreboard->image->h/10) - (scoreboard->image->h/20));
		EntSetCenterY(scoreboard->textboxes[2][i], scoreboard->posY + (i+1) * (scoreboard->image->h/10) - (scoreboard->image->h/20));
	}
}

void RenderScoreboard(SCOREBOARD *scoreboard){
	int i,j;
	Render( (ENTITY*) scoreboard);
	for(i=0;i<3;i++)
		for (j=0;j<10;j++)
			Render( (ENTITY *) scoreboard->textboxes[i][j]);
}



ANIMATED *AnimatedCreate(int posX, int posY, int frameTotal, SDL_Surface **surfaces, SDL_Texture **textures, int frameInterval){
  int i;
  ANIMATED *animated = malloc(sizeof(ANIMATED));


  animated->frames = malloc(frameTotal*sizeof(SDL_Texture*));
  if (surfaces){
    for (i=0;i<frameTotal;i++){
      animated->frames[i] = SDL_CreateTextureFromSurface(gRenderer, surfaces[i]);
    }
  }
  else
    for (i=0;i<frameTotal;i++){
      animated->frames[i] = textures[i];
    }

  animated->frameTotal = frameTotal;
  animated->frameInterval = frameInterval;
  animated->frameCurrent = 0;

  animated->texture = animated->frames[0];
  animated->image = malloc (sizeof(SDL_Rect));

  if (surfaces){
    animated->image->w = surfaces[0]->w;
    animated->image->h = surfaces[0]->h;
  }
  else
    SDL_QueryTexture(textures[0], NULL, NULL, &animated->image->w, &animated->image->h);


  animated->posX = posX;
  animated->posY = posY;
  animated->scale = 1;

  if (surfaces)
    animated->destroy = &AnimatedDestroy;
  else
    animated->destroy = &AnimatedDestroyKeepTextures;

  return animated;
}

void AnimatedDestroy(void *entity){
  int i;
  ANIMATED *animated = (ANIMATED *) entity;
  animated->texture = NULL; //Nulificar textura para evitar double free
  for (i=0;i<animated->frameTotal; i++)
    SDL_DestroyTexture(animated->frames[i]);
  free(animated->frames);
  EntityDestroy(entity);
}

void AnimatedDestroyKeepTextures(void *entity){
  ANIMATED *animated = (ANIMATED *) entity;
  free(animated->frames);
  EntityDestroy(entity);
}

void AnimatedAnimate(ANIMATED *animated){
    if (!(animated->frameCurrent%animated->frameInterval))
      animated->texture = animated->frames[animated->frameCurrent/animated->frameInterval];
    animated->frameCurrent++;
    animated->frameCurrent %= animated->frameInterval*animated->frameTotal;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//FUNCOES DE BOTOES (NAO SAO ENTIDADES)
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

BUTTON* ButtonCreate(int posX, int posY, char* string){
	BUTTON *newButton;

	newButton = malloc(sizeof(BUTTON));

	SDL_Surface *surface;
	ENTITY *button;
	//surface = SDL_CreateRGBSurface(0, 201, 101, 32, 0, 0, 0, 0);
	surface = IMG_Load("Images/Button.png");
	//SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 128, 128, 128));
	button = EntityCreate(surface, 0,0);
	EntSetCenterX(button, posX);
	EntSetCenterY(button, posY);
	SDL_FreeSurface(surface);

	TEXTBOX *mybox;
	mybox = CreateTextBox(VIDEO_W/2,VIDEO_H/2,string, g_font_OpenSans);
	EntSetCenterX(mybox, EntGetCenterX(button));
	EntSetCenterY(mybox, EntGetCenterY(button));

	newButton->frame = button;
	newButton->text = mybox;


	return newButton;
}

#include <stdarg.h>

void CreateButtonArray(BUTTON **buttons, int num, ...) {
   va_list valist;
	 int i,x,y;

   va_start(valist, num); //Inicializar lista de variaveis

   //Iterar sobre as variaveis
   for (i = 0; i < num; i++) {
      x = va_arg(valist, int);
			y = va_arg(valist, int);
      buttons[i] = ButtonCreate(x,y,va_arg(valist, char*));
   }

   va_end(valist); //Limpar memoria
}

int ButtonIsHighlighted(BUTTON *button){
	if ( (g_mouse_point.x > button->frame->posX) && (g_mouse_point.x < (button->frame->posX + button->frame->image->w)) &&
					(g_mouse_point.y > button->frame->posY) && (g_mouse_point.y < (button->frame->posY + button->frame->image->h)) ){
		SDL_SetTextureColorMod(button->frame->texture,255,255,0);
		//SDL_SetTextureColorMod(button->text->texture,0,0,0);
		SDL_SetTextureColorMod(button->text->texture,255,255,0);
		return 1;
	}
	else{
		SDL_SetTextureColorMod(button->frame->texture,179,179,179);
		SDL_SetTextureColorMod(button->text->texture,179,179,179);
	}
	return 0;
}

int CheckButtons(int howMany, BUTTON **buttons){
	int i, highlight = -1;
	for (i=0;i<howMany;i++){
		if (ButtonIsHighlighted(buttons[i]))
			highlight = i;
	}
	return highlight;
}

void RenderButtons(int howMany, BUTTON **buttons){
	int i;
	for (i=0;i<howMany;i++){
		RenderButton(buttons[i]);
	}
}

void DestroyButtonArray(int howMany, BUTTON **buttons){
	int i;
	for (i=0;i<howMany;i++){
		ButtonDestroy(buttons[i]);
	}
}

void RenderButton(BUTTON *button){
	Render(button->frame);
	Render((ENTITY *) button->text);
}

void ButtonDestroy(BUTTON *button){
	EntDestroy(button->frame);
	EntDestroy(button->text);
  free(button);
}

void ButtonSetCenterX(BUTTON *button, int x){
	EntSetCenterX(button->frame, x);
	EntSetCenterX(button->text, x);
}

void ButtonSetCenterY(BUTTON *button, int y){
	EntSetCenterY(button->frame, y);
	EntSetCenterY(button->text, y);
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------




void printE(void) { // Exibe o erro ocorrido, destroi a janela (se houver uma) e termina o programa.

	printf("SDL error\n=========\n%s\n", SDL_GetError());
	if (g_window) {SDL_DestroyWindow(g_window);}
	TTF_Quit();
	SDL_Quit();
	exit(1);
}


//------------------------------------------
//CODE DUMP
//------------------------------------------

//Codigo de pseudo-random baseado no algoritmo do jogo DOTA 2

// int pseudo_xrand(void){
// 	static int C[COLOR_TOTAL] = {0};
//
// #define defC 84620902
//
// 	int init = xrand(COLOR_TOTAL);
// 	int i = init;
// 	i++;
//
// 	int result, found = 0;
// 	for (i%=COLOR_TOTAL;i != init;i%=COLOR_TOTAL){
// 		if (rand()-C[i]*defC < defC && !found){
// 			C[i] = 0;
// 			result = i;
// 		}
// 		else{
// 			C[i]++;
// 		}
// 		i++;
// 	}
//
// 	if (!found){
// 		C[i] = 0;
// 		result = i;
// 	}
// 	return result;
// }

//Codigo original do loop de RemoveBalls(int row,int column)

//
//
//
// if (row%2){ //Linhas pares e impares precisam iterar sobre enderecos diferentes da matriz
// 	i = row-1;
// 	for(j=column;j<column+2;j++){
// 		if (GRID2[i][j]>0){
// 			if (GRID[i][j]->color == color){
// 				RemoveBalls(i,j, playerScore);
// 				if (GRID2[i][j] > 0){ //Checa se bola ainda existe (acontece caso seja a ultima do grupo)
// 					playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 					GRID2[i][j] = -1; //Marca bola para ser destruida, se ela era a ultima bola do grupo
// 				}
// 				delete = 1; //Marca esta bola para ser destruida
// 			}
// 			else{
// 				GRID2[i][j] = 2; //Marca bola para o loop checar se ela ainda tem conexao com o teto
// 			}
// 		}
// 	}
// 	i++; //i=row
// 	for(j=column-1;j<column+2;j++){
// 		if (GRID2[i][j]>0 && j!=column){
// 			if (GRID[i][j]->color == color){
// 				RemoveBalls(i,j, playerScore);
// 				if (GRID2[i][j] > 0){
// 					playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 					GRID2[i][j] = -1;
// 				}
// 				delete = 1;
// 			}
// 			else{
// 				GRID2[i][j] = 2;
// 			}
// 		}
// 	}
// i++; //i=row+1
// for(j=column;j<column+2;j++){
// 	if (GRID2[i][j]>0){
// 		if (GRID[i][j]->color == color){
// 			RemoveBalls(i,j, playerScore);
// 			if (GRID2[i][j] > 0){
// 				playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 				GRID2[i][j] = -1;
// 			}
// 			delete = 1;
// 		}
// 		else{
// 			GRID2[i][j] = 2;
// 		}
// 	}
// }
// }
// else{
// 	if (row){
// 		i = row-1;
// 		for(j=column-1;j<column+1;j++){
// 			if (GRID2[i][j]>0){
// 				if (GRID[i][j]->color == color){
// 					RemoveBalls(i,j, playerScore);
// 					if (GRID2[i][j] > 0){
// 						playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 						GRID2[i][j] = -1;
// 					}
// 					delete = 1;
// 				}
// 				else{
// 					GRID2[i][j] = 2;
// 				}
// 			}
// 		}
// 	}
// 	i=row;
// 	for(j=column-1;j<column+2;j++){
// 		if (GRID2[i][j]>0 && j!=column){
// 			if (GRID[i][j]->color == color){
// 				RemoveBalls(i,j, playerScore);
// 				if (GRID2[i][j] > 0){
// 					playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 					GRID2[i][j] = -1;
// 				}
// 				delete = 1;
// 			}
// 			else{
// 				GRID2[i][j] = 2;
// 			}
// 		}
// 	}
// 	i++; //i=row+1
// 	for(j=column-1;j<column+1;j++){
// 		if (GRID2[i][j]>0){
// 			if (GRID[i][j]->color == color){
// 				RemoveBalls(i,j, playerScore);
// 				if (GRID2[i][j] > 0){
// 					playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
// 					GRID2[i][j] = -1;
// 				}
// 				delete = 1;
// 			}
// 			else{
// 				GRID2[i][j] = 2;
// 			}
// 		}
// 	}
// }


//
