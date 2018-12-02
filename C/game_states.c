#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "globals.h"
#include "g_func.h"
#include "game_states.h"
#include "file_IO.h"

/*
TODO: CAMPAIGN MODE

Requirements:
-Load Game Settings
-Load GRID state (done)
-Campaign Menu

Optional:
-Level editor (done)
*/
/*GLOBAL SETTINGS*/

//Game Settings
int g_settings_StartingRows = 5;
int g_settings_MinBallsToExplode = 3;
int g_settings_AmmoTotal = 5;
int g_settings_LifeTotal = 5;
int g_settings_SimpleShuffle = 0;
int g_settings_CrazyShuffle = 0;
int g_settings_MaxAmmo = 0;
int g_settings_PseudoRandom = 1;


//Graphic Settings
int g_settings_FixedAspectRatio = 1;
int g_settings_MovingStars = 1;
int g_settings_RenderStars = 1;

//Sound Settings
int g_settings_MusicVolume = 32;
int g_settings_SoundVolume = 128;



 /*FUNCTIONS*/

void RenderStars(int howMany, STAR **STARS, int updatePosition){
	int j;
  if (g_settings_RenderStars){
  	if (updatePosition && g_settings_MovingStars){
  		for(j=0;j<howMany;j++){ //Tratando estrelas______________________________________
  			if ( StarUpdatePosition(STARS[j]) )
  				StarReset(STARS[j],STAR_SPEED);
  			Render( (ENTITY *) STARS[j]);
  		}
  	}
  	else{
  		for(j=0;j<howMany;j++){ //Tratando estrelas______________________________________
  			Render( (ENTITY *) STARS[j]);
  		}
  	}
  }
}

void RendererRescale(void){
	static float scaleX = 1,scaleY = 1, lastSetting = 0;
  float newScaleX,newScaleY;

	if (g_settings_FixedAspectRatio){
		SDL_RenderGetScale(gRenderer, &newScaleX, &newScaleY);
	}
	else if (!g_settings_FixedAspectRatio){
		int w,h;
		SDL_GetWindowSize(g_window, &w, &h);
		newScaleX = w/(float)VIDEO_W;
		newScaleY = h/(float)VIDEO_H;
	}

	if (newScaleX != scaleX || newScaleY != scaleY || lastSetting != g_settings_FixedAspectRatio){
		printf("Rescaling: %f %f\n", newScaleX, newScaleY);
		if (!g_settings_FixedAspectRatio)
			SDL_RenderSetViewport(gRenderer, NULL);
		SDL_RenderSetScale(gRenderer,newScaleX,newScaleY);
		scaleX = newScaleX;
		scaleY = newScaleY;
    lastSetting = g_settings_FixedAspectRatio;
	}
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//TRATAMENTO DE EVENTOS
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

void onEventPaused(int *resume, SDL_Event *event, int *mousePress){
	switch(event->type) {
		case SDL_QUIT:
			stop = SDL_TRUE; // Termina o loop em main.
			temp_bool = 0;
        	break;
		case SDL_KEYDOWN:
    		if ((*event).key.keysym.sym == SDLK_ESCAPE)
				*resume = 1;
			break;
    	case SDL_MOUSEBUTTONDOWN:
			*mousePress = 1;
			break;
		default: break;
	}
}

void onEvent(BALL* ball, INDICATOR* indicator, int *pause) {
	double angle;
	switch(g_event.type) {
		case SDL_QUIT:
        	stop = SDL_TRUE; // Termina o loop em main.
          temp_bool = 0;
        	break;
		case SDL_KEYDOWN:
    		if (g_event.key.keysym.sym == SDLK_ESCAPE)
				  *pause = 1;
				if (g_event.key.keysym.sym == SDLK_INSERT)
					LoadGameState();
      	break;
    	case SDL_MOUSEBUTTONDOWN:
			if (!ball) break;
			angle = atan2(EntGetCenterY(indicator) - EntGetCenterY(ball) , EntGetCenterX(indicator) - EntGetCenterX(ball) );
			if((*ball).speedY == 0 && (*ball).posY == BALL_START_Y){ // Ter certeza que a bola ainda n foi lancada.
				(*ball).speedY = (sin(angle) * 20);
				(*ball).speedX = (cos(angle) * 20);
				SaveGameState();
				printf("Angulo de saida Desejado/Final: %lf %lf\n", angle*360/2/PI, atan2((*ball).speedY,(*ball).speedX)*360/2/PI);
			}
			else printf("Angulo invalido ou bola em movimento\n");
			break;
		default: break;
	}
}

void onEventTyping(SDL_Event *e, int *exit, int *renderText, char *inputText, int strMaxSize, int onlyDigits){
	char *tempString;
  if( e->type == SDL_QUIT ){
    stop = 1;
    temp_bool = 0;
  }
  else if( e->type == SDL_KEYDOWN ){
		//Handle escape
		if( e->key.keysym.sym == SDLK_ESCAPE ){
			if (strlen(inputText) == 0)
				strcpy(inputText, "-");
			*exit = 1;
		}
    //Handle DELETE
		if( e->key.keysym.sym == SDLK_DELETE && strlen(inputText) > 0 ){
      strcpy(inputText, "");
			*renderText = 1;
		}
		//Handle backspace
    if( e->key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0 ){
      //lop off character
      RemoveLastCharFromString(inputText);
      *renderText = 1;
  	}
    //Handle copy
    else if( e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL ){
      SDL_SetClipboardText( inputText );
    }
    //Handle paste
    else if( e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL ){
			tempString = SDL_GetClipboardText();
      strncpy(inputText, tempString, strMaxSize);
			free(tempString);
      *renderText = 1;
    }
  }
  //Special text input event
  else if( e->type == SDL_TEXTINPUT ){
    //Not copy or pasting
    if( strlen(inputText) < strMaxSize && !( e->text.text[ 0 ] == ' ' || (( e->text.text[ 0 ] == 'c' || e->text.text[ 0 ] == 'C' ) &&
		( e->text.text[ 0 ] == 'v' || e->text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL )) ){
      //Append character
			if (!(onlyDigits && ( e->text.text[ 0 ] - '0' ) > 9 )){
	      strcat(inputText, e->text.text);
	      *renderText = 1;
			}
    }
  }
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//GAME STATES - GAME SESSION
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

// PauseSession_Change

int GameState_PauseSession(ENTITY *backround, ENTITY *spaceship, BALL *ball, STAR **STARS, INDICATOR *indicator){
	int i,j;
	SDL_Event event;

	if (Mix_PlayingMusic())
		Mix_PauseMusic();

	BUTTON *button = ButtonCreate(VIDEO_W/2, VIDEO_H/2, "Restart");
	BUTTON *button2 = ButtonCreate(VIDEO_W/2, VIDEO_H/2+300, "Main Menu");

	ENTITY *filter = EntityCreate(g_window_surface, 0 , 0);
	SDL_SetTextureColorMod(filter->texture, 0, 0, 0);
	SDL_SetTextureAlphaMod(filter->texture, 128);
	SDL_SetTextureBlendMode(filter->texture, SDL_BLENDMODE_BLEND);

	SDL_ShowCursor(SDL_ENABLE);
	int mousePress,resume=0;
	while (!resume && !stop){
		PointCenterMouse(&g_mouse_point);
		mousePress = 0;
		while (SDL_PollEvent(&event))
			onEventPaused(&resume, &event, &mousePress);

		if (ButtonIsHighlighted(button) && mousePress){
			Mix_PlayChannel(-1, buttonClick, 0);
			resume = 1;
			stop = SDL_TRUE;
			temp_bool = 1;
		}

		if (ButtonIsHighlighted(button2) && mousePress){
			Mix_PlayChannel(-1, buttonClick, 0);
			resume = 1;
			stop = SDL_TRUE;
			temp_bool = 1;
			main_menu = 1;
		}

		RendererRescale();
		SDL_RenderClear(gRenderer);
		Render(backround);
		RenderStars(20, STARS, FALSE);
		Render(spaceship);
		Render( (ENTITY *) indicator);

		for(j=0;j<20;j++)
			for(i=0;i<NUM_BALLS;i++)
				if (GRID2[j][i] != 0)
					Render( (ENTITY *) GRID[j][i]);
		if (ball)
			Render( (ENTITY *) ball);

		Render(filter);
		RenderButton(button);
		RenderButton(button2);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(GAME_SPEED);
	}
	Mix_ResumeMusic();
	ButtonDestroy(button);
	ButtonDestroy(button2);
	EntDestroy(filter);
	SDL_ShowCursor(SDL_DISABLE);
	return 0;
}

void TestGrid();

void FinishSession_Initialize(int gameResult, ENTITY **filter, ENTITY *spaceship, SDL_Texture **spriteTextures, ANIMATED **explosion){
  int i;

  if (gameResult == 1)
    SDL_SetTextureColorMod((*filter)->texture, 0, 255, 0);
  else
    SDL_SetTextureColorMod((*filter)->texture, 255, 0, 0);
  SDL_SetTextureAlphaMod((*filter)->texture, 128);
  SDL_SetTextureBlendMode((*filter)->texture, SDL_BLENDMODE_BLEND);

  SDL_Surface *spriteFrames[23];
  char tempString[50];
  for (i=0;i<23;i++){
    sprintf(tempString, "Images/Sprites/Explosion%d.png", i+1);
    spriteFrames[i] = IMG_Load(tempString);
    if (!spriteFrames[i])
      printE();
    spriteTextures[i] = SDL_CreateTextureFromSurface(gRenderer, spriteFrames[i]);
  }
  for (i=0;i<10;i++){
    explosion[i] = AnimatedCreate(xrand(spaceship->image->w) + spaceship->posX, xrand(spaceship->image->h) + spaceship->posY,23,NULL,spriteTextures,2);
    explosion[i]->frameCurrent = 2*i;
    explosion[i]->scale = (xrand(20) + 10) / 10.;
    while(distance(EntGetCenterX(explosion[i]) - EntGetCenterX(spaceship), EntGetCenterY(explosion[i]) - EntGetCenterY(spaceship)) > spaceship->image->w/2){
      printf("%lf %d\n", distance(EntGetCenterX(explosion[i]) - EntGetCenterX(spaceship), EntGetCenterY(explosion[i]) - EntGetCenterY(spaceship)), spaceship->image->w/2);
      EntSetCenterX(explosion[i], xrand(spaceship->image->w) + spaceship->posX);
      EntSetCenterY(explosion[i], xrand(spaceship->image->h) + spaceship->posY);
    }
  }

  for (i=0;i<23;i++)
   SDL_FreeSurface(spriteFrames[i]);

}

void GameState_FinishSession(int gameResult, ENTITY *backround, ENTITY *spaceship, STAR **STARS, INDICATOR *indicator, SCORE *playerScore){
	int i, starCount = 0, skip = 0;


	ENTITY *filter = EntityCreate(g_window_surface, 0 , 0);
  SDL_Texture *spriteFrames[23];
  ANIMATED *explosion[10];

  FinishSession_Initialize(gameResult, &filter, spaceship, spriteFrames, explosion);

	while (!stop && spaceship->posY>0 && !skip) {
		PointCenterMouse(&g_mouse_point);
		while (SDL_PollEvent(&g_event)) // Tratando eventos._______________________
			onEvent(NULL,indicator, &skip);
		// if (pause)
		// 	pause = GameState_PauseSession(backround, spaceship, NULL, STARS, indicator);

		RendererRescale();//Tratando janela e backround____________________________
		SDL_RenderClear(gRenderer);
		Render(backround);

		RenderStars(20, STARS, starCount);
		if (starCount) starCount = 0;
		else starCount++;

		Render(spaceship); //Tratando nave_________________________________________
		spaceship->posY -= 1;
		IndicatorUpdatePosition(indicator); //Tratando indicador___________________
		RenderIndicator(indicator);

		Render(filter);
    for (i=0;i<10 && (gameResult == -1);i++){
      AnimatedAnimate(explosion[i]);
      Render((ENTITY*) explosion[i]);
      explosion[i]->posY--;
    }

		SDL_RenderPresent(gRenderer);//Fim, apresentar e esperar proximo quadro_____
		SDL_Delay(GAME_SPEED);
	}
  for (i=0;i<10;i++){
    EntDestroy(explosion[i]);
    SDL_DestroyTexture(spriteFrames[i]);
  }
  EntDestroy(filter);
}

void GameSession_HandleHighscores(char *playerName, SCORE playerScore){
	int newHighScore = 0;
	char playerHighscoreFileName[PLAYER_NAME_MAX_CHARACTERS+20] = "Highscores/";
	strcat(playerHighscoreFileName, playerName);
	strcat(playerHighscoreFileName, ".txt");
	newHighScore = FileNameInputHighScore("Highscores/Global/highscores.txt", playerName, playerScore.value);
	if (newHighScore >= 0)
		printf("New global high score! Rank> %d\n", newHighScore+1);
	newHighScore = FileNameInputHighScore(playerHighscoreFileName, playerName, playerScore.value);
	if (newHighScore >= 0)
		printf("New personal high score! Rank> %d\n", newHighScore+1);
}


//TODO: Segmentation fault quando a bola do jogador ta presa e vc tenta loadar uma partida salva
void GameState_GameSession(ENTITY *backround, ENTITY *spaceship, STAR **STARS, char *playerName){
  Mix_PlayMusic(music, -1);
  spaceship->posY = 0;
	GridReset(g_settings_StartingRows * NUM_BALLS); //Resetar GRID e encher com fileiras iniciais
	SDL_ShowCursor(SDL_ENABLE);

	BALL *ball = BallGetAmmo(BALL_START_X, BALL_START_Y, BALL_D, g_settings_PseudoRandom);
	INDICATOR *indicator = IndicatorCreate(0,0,EntGetCenterX(ball), EntGetCenterY(ball));
	SCORE playerScore = {0,0,1};

  puts("Creating new session");
	if (1)
  	GameState_InitSession(backround, spaceship, STARS, indicator, &playerScore);

  puts("New session started!");
	spaceship->posY = VIDEO_H-100; //Setar posicao caso o jogador pule a cutscene
	int gameResult =
  GameState_PlaySession(backround, spaceship, ball, STARS, indicator, &playerScore);

	spaceship->posY = VIDEO_H-100;
	if (gameResult)
		GameState_FinishSession(gameResult, backround, spaceship, STARS, indicator, &playerScore);

	SDL_ShowCursor(SDL_ENABLE);
  EntDestroy(indicator);
	GameSession_HandleHighscores(playerName, playerScore); //Verificar highscores
  stop = 0;
  printf("%d\n", playerScore.value);
  Mix_HaltMusic();
  GridReset(0);
  SDL_Delay(GAME_SPEED);
}


void GameState_InitSession(ENTITY *backround, ENTITY *spaceship, STAR **STARS, INDICATOR *indicator, SCORE *playerScore){
	int max =0, max2=0, starCount = 0, i, j, skip = 0;

	while (!stop && spaceship->posY<VIDEO_H-100 && !skip) {
		PointCenterMouse(&g_mouse_point);
		while (SDL_PollEvent(&g_event)) // Tratando eventos._______________________
			onEvent(NULL,indicator, &skip);

		RendererRescale();//Tratando janela e backround____________________________
		SDL_RenderClear(gRenderer);
		Render(backround);
		RenderStars(20, STARS, starCount);
		if (starCount) starCount = 0;
		else starCount++;
		Render(spaceship); //Tratando nave_________________________________________
		spaceship->posY += 1;
		IndicatorUpdatePosition(indicator); //Tratando indicador___________________
		RenderIndicator(indicator);

		if (spaceship->posY > VIDEO_H -500){ //Tratando bolas______________________
  		for(j=0;j<max;j++)
  			for(i=0;i<NUM_BALLS;i++)
  				if (GRID2[j][i] != 0 && (j < max || i < max2))
  					Render( (ENTITY *) GRID[j][i]);

  		if (max < 6){ //Determinar linha e coluna maximas
  			if (max2 < NUM_BALLS)
  				max2++;
  			else{
  				max2 = 0;
  				max++;
				}
			}
		}

		SDL_RenderPresent(gRenderer);//Fim, apresentar e esperar proximo quadro_____
		SDL_Delay(GAME_SPEED);
		}
}


//TODO: Reduce Size of this function
int GameState_PlaySession(ENTITY *backround, ENTITY *spaceship, BALL *ball, STAR **STARS, INDICATOR *indicator, SCORE *playerScore){
  int new_time = SDL_GetTicks();

	int numError = 0;
  int i,j,counter = 0;
  int pause = 0;
  TEXTBOX *mybox;
	char scoreString[30];
	sprintf(scoreString, "Score: %09d", playerScore->value);
  mybox = CreateTextBox(0,VIDEO_H,scoreString, g_font_Hacker);
	mybox->posY -= mybox->image->h;
	SDL_SetTextureColorMod(mybox->texture, 35, 180, 0);
	int shownScore = 0;

	#define AMMO_TOTAL g_settings_AmmoTotal
	BALL **ballAmmo;
	ballAmmo = malloc(g_settings_AmmoTotal* sizeof (BALL));

	int remainingAmmo;
	if (g_settings_MaxAmmo)
		remainingAmmo = g_settings_MaxAmmo;
	else
		remainingAmmo = 2147483647;

	int gameOver = 0; //-1 for loss, 1 for victory


	for(i=0;i<AMMO_TOTAL;i++){
		ballAmmo[i] = BallGetAmmo(BALL_START_X + BALL_D * i, BALL_START_Y + BALL_D, BALL_D, g_settings_PseudoRandom);
		ballAmmo[i]->scale = 0.5;
		ballAmmo[i]->posX =/*-30 + BALL_START_X*/ 0 + BALL_D * ballAmmo[i]->scale * (AMMO_TOTAL-(i+1)) ;
	}

	int coinCountChannel = -1;

	ENTITY *forceField = EntityCreate(g_window_surface, 0 , 0);
	forceField->image->h = 5;
	SDL_SetTextureColorMod(forceField->texture, 65,105,225);
	SDL_SetTextureAlphaMod(forceField->texture, 128);
	SDL_SetTextureBlendMode(forceField->texture, SDL_BLENDMODE_BLEND);

	EntSetCenterY(forceField, Grid2PosY(14,0)+GRID_CELLY/2);

	TEXTBOX *forceFieldText =  CreateTextBox(0,BALL_START_Y,"FORCE FIELD 100%", g_font_Hacker);
	SDL_SetTextureColorMod(forceFieldText->texture, 65,105,225);
	SDL_SetTextureAlphaMod(forceFieldText->texture, 200);
	SDL_SetTextureBlendMode(forceFieldText->texture, SDL_BLENDMODE_BLEND);
	//forceFieldText->scale = 1.5;

	SDL_Surface *surface = IMG_Load("Images/LoadBar.png");
	ENTITY *hyperBar = EntityCreate(surface, VIDEO_W, VIDEO_H);
	TEXTBOX *hyperText =  CreateTextBox(VIDEO_W,VIDEO_H,"Hyperdrive", g_font_Hacker);
	SDL_SetTextureBlendMode(hyperBar->texture, SDL_BLENDMODE_BLEND);
	hyperBar->posX -= hyperBar->image->w + 10;
	hyperBar->posY -= hyperBar->image->h + 5;
	hyperText->posX = hyperBar->posX - hyperText->image->w;
	hyperText->posY -= hyperText->image->h + 5;
	SDL_FreeSurface(surface);
	ENTITY *hyperBar2= EntityCreate(g_window_surface, 0 , 0);
	hyperBar2->posX = hyperBar->posX + 4;
	hyperBar2->posY = hyperBar->posY + 4;
	hyperBar2->image->w = 0;
	hyperBar2->image->h = hyperBar->image->h - 8;

	int hyperDrive = 0;//hyperBar->image->w - 8;

	surface = IMG_Load("Images/light.png");
	ENTITY **lifeLight = malloc(g_settings_LifeTotal*sizeof(*lifeLight));
	for(i=0;i<g_settings_LifeTotal;i++){
		lifeLight[i] = EntityCreate(surface, VIDEO_W - (5 + surface->w) - i*(surface->w+2), hyperBar->posY - surface->h - 3);
		SDL_SetTextureColorMod(lifeLight[i]->texture, 0,255,0);
		SDL_SetTextureBlendMode(lifeLight[i]->texture, SDL_BLENDMODE_BLEND);
	}
	SDL_FreeSurface(surface);

	SDL_SetTextureColorMod(indicator->texture, g_ball_color[ball->color].r, g_ball_color[ball->color].g, g_ball_color[ball->color].b);


  while (!stop && !gameOver){
		PointCenterMouse(&g_mouse_point);

		while (SDL_PollEvent(&g_event)) // Tratando eventos.________________________
			onEvent(ball,indicator, &pause);


		if (pause){
			Mix_Pause(-1);
			pause = GameState_PauseSession(backround, spaceship, ball, STARS, indicator);
			new_time = SDL_GetTicks();
			Mix_Resume(-1);
		}

		if (SDL_GetTicks() > new_time){ //Calcular gamestate de acordo com FPS

			RendererRescale(); //Redefinindo escala para o renderizador
			if (counter == 0)
			   BallUpdatePosition(ball); // Reposicionando bola.
			IndicatorUpdatePosition(indicator); //Reposicionando indicador de direcao

			//Resetar a window do jogo
			SDL_RenderClear(gRenderer);

			/*
			Checar se as vidas do jogador acabaram
			*/
			if (numError == g_settings_LifeTotal){
				if (g_settings_SimpleShuffle)
					GridShuffle(); //Shuffle, se estiver habilitado
				GridInsert();
				SaveConnectedBalls(0,0); //Salvar bolas, pois GridInsert seta seu estado para -2

				numError++; //Aumentar numError para a condicao desse if ser falsa
				counter = 16;
				for(i=0;i<g_settings_LifeTotal;i++) //Tornar esfera de vida vermelha
					SDL_SetTextureColorMod(lifeLight[i]->texture, 0,255,0);
			}

			/*
			-Tratando destruicao e animacao das bolas apos jogada
			-Tratando municao do jogador, hiperdrive e Crazy Shuffle
			*/
			if ( (ball->speedY == 0 && (int) ball->posY !=BALL_START_Y) || numError > g_settings_LifeTotal ){
				if (counter > 30){

					if (DESTROYED){ //Remover 30 pontos, pois as primeiras 3 bolas so valem metade
						DESTROYED = 0;
						playerScore->value -= 3 * ((POINTS_EXTRA - POINTS_DEFAULT)*playerScore->multiplier + playerScore->bonus);
					}

					for(j=0;j<20;j++){
						for(i=0;i<NUM_BALLS;i++){
							if (GRID2[j][i] < 0){
								if (++GRID2[j][i]){ //equivalente a GRID2[j][i] != -1
									playerScore->value += POINTS_FALLING*playerScore->multiplier + playerScore->bonus;
								}
								else{ //equivalente a GRID2[j][i] == -1
									playerScore->value += POINTS_EXTRA*playerScore->multiplier + playerScore->bonus;
								}
								BallRemoveFromGrid(j,i, NULL);
							}
						}
					}

					if (hyperDrive >= 135){//Começar hyperdrive se a barra estiver cheia
						if (!playerScore->bonus) //So tocar o som se o hiperdrive foi atingido
            	Mix_PlayChannel(-1, g_sounnd1[0], 0);
						for(j=0;j<20 && g_settings_MovingStars;j++){
							if (!STARS[j]->turbo){
								StarTurbo(STARS[j]);
							}
						}
						playerScore->bonus += 20; //Efeito do hiperdrive na pontuacao
					}

						for (i=0;i<COLOR_TOTAL;i++){
							printf("Bolas da cor %d: %d\n", i, GRID_color_counter[i]);
						}

						if (numError > g_settings_LifeTotal) //Resetar o numero de erros caso uma linha tenha sido inserida
							numError = 0;

						if (g_settings_CrazyShuffle) //Crazy Shuffle, se estiver habilitado
							GridShuffle();

						ball = ballAmmo[0];
						ball->posX = BALL_START_X;
						ball->posY = BALL_START_Y;
						ball->scale = 1;
						for(i=1;i<AMMO_TOTAL;i++){
							ballAmmo[i-1] = ballAmmo[i];
							ballAmmo[i]->posX =/*-30 + BALL_START_X*/ 0 + BALL_D * ballAmmo[i]->scale * (AMMO_TOTAL-(i)) ;
						}
						ballAmmo[AMMO_TOTAL -1] = BallGetAmmo(BALL_START_X + BALL_D * (AMMO_TOTAL - 1), BALL_START_Y + BALL_D, BALL_D, g_settings_PseudoRandom);
						ballAmmo[AMMO_TOTAL -1]->scale = 0.5;
						ballAmmo[AMMO_TOTAL -1]->posX = /*-30 + BALL_START_X*/ 0 + BALL_D * ballAmmo[AMMO_TOTAL-1]->scale * (AMMO_TOTAL - (AMMO_TOTAL));
						counter = 0;
						if (!remainingAmmo)
							gameOver = -1;
						if (g_settings_MaxAmmo)
							remainingAmmo--;
						SDL_SetTextureColorMod(indicator->texture, g_ball_color[ball->color].r, g_ball_color[ball->color].g, g_ball_color[ball->color].b);
				}
				else if (counter ==15){
					counter++;
					if (BallCountBallsUntil(ball,1,g_settings_MinBallsToExplode) >= g_settings_MinBallsToExplode){
						puts("removing");
						BallSaveConnectedBalls(ball);
						BallRemoveBalls(ball);
					}
					else{
						BallSaveConnectedBalls(ball);
						numError = (numError+1);
						SDL_SetTextureColorMod(lifeLight[g_settings_LifeTotal-numError]->texture, 255,0,0);
						counter = 30;
					}
					for (i=19;i>=0;i--){ //Checar ao contrario pode ser computacionalmente mais eficiente
						for (j=NUM_BALLS-1;j>=0;j--){
							if (GRID2[i][j] == 2){
								if (DESTROYED){
									puts("checking ceiling");
									CheckCeiling(i,j);
								}
								else
									GRID2[i][j] = 1;
							}
						}
					}
					if (DESTROYED){
						Mix_PlayChannel(-1, sound, 0);
						for(j=0;j<20;j++)
							for(i=0;i<NUM_BALLS;i++)
								if (GRID2[j][i] < 0){
									//GRID[j][i]->texture = SDL_CreateTextureFromSurface(gRenderer, g_temp_surface);
									GRID[j][i]->scale = GRID[j][i]->scale * 1.05;
									EntSetCenterX(GRID[j][i], Grid2PosX(j, i));
									EntSetCenterY(GRID[j][i], Grid2PosY(j, i));
								}


						if (hyperDrive<135){
							hyperDrive+=27;
						}
					}
					else{
						hyperDrive=0;
						if (playerScore->bonus){
							Mix_PlayChannel(-1, g_sounnd1[1], 0);
							playerScore->bonus = 0;
            }
						for(j=0;j<20 && g_settings_MovingStars;j++){
							if (STARS[j]->turbo){
								StarUnTurbo(STARS[j]);
							}
						}
					}

				}
				//Expandir as bolas que estao marcadas para serem destruidas
				else if (counter >15 && counter%2){
					counter++;
					if (DESTROYED || numError > 5){
						for(j=0;j<20;j++)
							for(i=0;i<NUM_BALLS;i++)
								if (GRID2[j][i] < 0){
									//GRID[j][i]->texture = SDL_CreateTextureFromSurface(gRenderer, g_temp_surface);
									GRID[j][i]->scale = GRID[j][i]->scale * 1.05;
									EntSetCenterX(GRID[j][i], Grid2PosX(j, i));
									EntSetCenterY(GRID[j][i], Grid2PosY(j, i));
								}
					}
				}
				else{
					counter++;
				}
			}
			//Mudar a textura da pontuacao, caso ela tenha aumentado
			if (shownScore < playerScore->value){
				if (coinCountChannel < 0){
					coinCountChannel = Mix_PlayChannel(-1, countPoints, -1);
				}
				shownScore += (playerScore->value - shownScore)/50 + 1;
				printf("Score: %09d\n", (int) shownScore);
				sprintf(scoreString, "Score: %09d", (int) shownScore);
				EntDestroy(mybox);
				mybox = CreateTextBox(0,VIDEO_H,scoreString, g_font_Hacker);
				mybox->posY -= mybox->image->h;
				SDL_SetTextureColorMod(mybox->texture, 35, 180, 0);
				if (shownScore >= playerScore->value){
					Mix_HaltChannel(coinCountChannel);
					coinCountChannel = -1;
				}
			}

			/*
			Mover e renderizar elementos de fundo
			*/
			Render(backround);
			RenderStars(20, STARS, TRUE);
			Render(spaceship);
			//Render( (ENTITY *) indicator);
			RenderIndicator(indicator);

			//Renderizar bolas da matriz
			int ballExists = 0;
			for(j=0;j<20;j++)
				for(i=0;i<NUM_BALLS;i++)
					if (GRID2[j][i] != 0){
						Render( (ENTITY *) GRID[j][i]);
						ballExists = 1;
						if (j>=14 && GRID2[j][i] > 0)
							gameOver = -1;
					}

			if (!ballExists && !gameOver)
				gameOver = 1;

			//Renderizar municao
			for(i=0;i<AMMO_TOTAL;i++)
				if (i < remainingAmmo)
					Render((ENTITY*)ballAmmo[i]);

			//Renderizar bola
			Render( (ENTITY *) ball);

			/*
			Renderizando elemntos da interface (parte de baixo da tela)
			*/
			if (xrand(90))
				Render(forceField);

			Render( (ENTITY *) mybox);
			Render( (ENTITY *) forceFieldText);
			Render(hyperBar);

			if (hyperDrive > hyperBar2->image->w)
				hyperBar2->image->w++;

			else if (hyperDrive < hyperBar2->image->w && hyperBar2)
				hyperBar2->image->w--;


			Render(hyperBar2);
			Render( (ENTITY *) hyperText);

			for(i=0;i<g_settings_LifeTotal;i++)
				Render(lifeLight[i]);

			SDL_RenderPresent( gRenderer ); // Atualizar tela
			new_time += GAME_SPEED; //Definir tempo para o quadro seguinte
		}
		SDL_Delay(1); //Evitar uso de CPU, mas continuar capturando eventos
	}
	puts("freeing ball");
	if ((int) ball->speedY || ball->posY == BALL_START_Y )
		EntDestroy(ball); //So destruir a bola caso ela nao esteja na GRID
	for (i=0;i<AMMO_TOTAL;i++)
		EntDestroy(ballAmmo[i]);
	puts("ball freed");
	if (coinCountChannel > -1){
		coinCountChannel = Mix_HaltChannel(coinCountChannel);
	}

	EntDestroy(mybox);
	EntDestroy(forceField);
	EntDestroy(forceFieldText);
	EntDestroy(hyperBar);
	EntDestroy(hyperBar2);
	EntDestroy(hyperText);
	for(i=0;i<g_settings_LifeTotal;i++)
		EntDestroy(lifeLight[i]);

  return gameOver;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//GAME STATES - MAIN MENU
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

void MainMenu_Initialize(ENTITY *spaceship, int howManyButtons, BUTTON **button, TEXTBOX **credits,Mix_Music **menuMusic, ENTITY **title, TEXTBOX *playerNameContainer){
	SDL_Surface *surface;

	spaceship->posY = SP_START;

	CreateButtonArray(button, howManyButtons, VIDEO_W/2, VIDEO_H/2 + 375, "Start Game", VIDEO_W/2 + 450, VIDEO_H/2 + 250, "Highscores",
	VIDEO_W/2 - 450, VIDEO_H/2 + 250, "Player Name", VIDEO_W/2 - 150, VIDEO_H/2  + 250, "Settings", VIDEO_W/2 + 151, VIDEO_H/2 + 250, "Toggle Sound",
VIDEO_W/2 - 300, VIDEO_H/2 + 375, "Editor", VIDEO_W/2 + 301, VIDEO_H/2 + 375, "Campaign");

	button[0]->frame->image->h++;
	button[0]->text->posX++;
	credits[0] = CreateTextBox(0, 0, "A game by:", g_font_OpenSans);
	credits[1] = CreateTextBox(10, (credits[0])->posY + (credits[0])->image->h, "Angelo Daumas", g_font_OpenSans);
	SDL_SetTextureColorMod(credits[1]->texture, 255,0,0);
	credits[2] = CreateTextBox(10, (credits[1])->posY + (credits[1])->image->h, "Daniel Vieira", g_font_OpenSans);
	SDL_SetTextureColorMod(credits[2]->texture, 255,255,0);
	credits[3] = CreateTextBox(10, (credits[2])->posY + (credits[2])->image->h, "Tiago Macedo", g_font_OpenSans);
	SDL_SetTextureColorMod(credits[3]->texture, 0,0,255);

	if (!(*menuMusic)){ //Criar musica do menu apenas uma vez
		printf("Created Menu Music\n");
		*menuMusic = Mix_LoadMUS("Sounds/Star Wars Throne Room Theme (8 Bit Remix Cover Version) [Tribute to Star Wars] - 8 Bit Universe.mp3");
			if (!menuMusic)
				printE();
	}
	Mix_PlayMusic(*menuMusic, -1);

	surface = IMG_Load("Images/StarWarsTitle.png");
	if (!surface)
		printE();
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format,255,255,255));
	*title = EntityCreate(surface, 0-GRID_CELL/2, 0);
	SDL_FreeSurface(surface);
	SDL_SetTextureBlendMode((*title)->texture, SDL_BLENDMODE_BLEND);

	playerNameContainer->posY -= playerNameContainer->image->h;
	EntSetCenterX(playerNameContainer, VIDEO_W/2);
}

void MainMenu_HandleButtons(int howManyButtons, BUTTON **button, char *playerName, TEXTBOX **playerNameContainer, ENTITY *backround, int mousePress, int *resume){
	int highlight;
	highlight = CheckButtons(howManyButtons, button);
	if (highlight == 0 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		*resume = 1;
		stop = SDL_TRUE;
		temp_bool = 1;
	}
	else if (highlight == 1 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		GameState_MainMenu_ViewScoreboard(playerName, backround);
	}
	else if (highlight == 2 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		GameState_MainMenu_InputPlayerName(PLAYER_NAME_MAX_CHARACTERS+1, playerName, backround);
		EntDestroy(*playerNameContainer);
		*playerNameContainer = CreateTextBox(0, VIDEO_H, playerName, g_font_OpenSans);
		(*playerNameContainer)->posY -= (*playerNameContainer)->image->h;
		EntSetCenterX(*playerNameContainer, VIDEO_W/2);
	}
	else if (highlight == 3 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		GameState_MainMenu_Settings();
	}
	else if (highlight == 4 && mousePress){
		if (Mix_VolumeMusic(0)){
			Mix_VolumeChunk(g_sounnd1[0],0);
			Mix_VolumeChunk(g_sounnd1[1],0);
			Mix_VolumeChunk(sound,0);
			Mix_VolumeChunk(buttonClick,0);
      Mix_VolumeChunk(countPoints,0);
		}
		else{
			Mix_VolumeMusic(g_settings_MusicVolume);
			Mix_VolumeChunk(g_sounnd1[0],g_settings_SoundVolume);
			Mix_VolumeChunk(g_sounnd1[1],g_settings_SoundVolume);
			Mix_VolumeChunk(sound,g_settings_SoundVolume);
			Mix_VolumeChunk(buttonClick,g_settings_SoundVolume);
			Mix_VolumeChunk(countPoints,g_settings_SoundVolume);
			Mix_PlayChannel(-1, buttonClick, 0);
		}
	}
	else if (highlight == 5 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		GameState_LevelEditor(backround);
	}
}

#define MainMenu_BUTTONS 7
#define MainMenu_CREDITS 4
void GameState_MainMenu(ENTITY *backround, ENTITY *spaceship, STAR **STARS, char *playerName){
	int i, resume=0, mousePress;
	SDL_Event event;
	ENTITY *title;
	BUTTON *button[MainMenu_BUTTONS];
	TEXTBOX *playerNameContainer = CreateTextBox(0, VIDEO_H, playerName, g_font_OpenSans);
	TEXTBOX *credits[MainMenu_CREDITS];
	static Mix_Music *menuMusic; //statico, para que so seja necessario loadar uma vez a musica

	MainMenu_Initialize(spaceship, MainMenu_BUTTONS, button, credits, &menuMusic, &title, playerNameContainer);
	puts("Main Menu\n");
	while (!resume && !stop){
		mousePress = 0;
		PointCenterMouse(&g_mouse_point);
		while (SDL_PollEvent(&event)) // Tratando eventos.
			onEventPaused(&resume, &event, &mousePress);

		MainMenu_HandleButtons(MainMenu_BUTTONS, button, playerName, &playerNameContainer, backround, mousePress, &resume);

		RendererRescale();
		SDL_RenderClear(gRenderer);
		Render(backround);
		Render(spaceship);
		Render((ENTITY *) playerNameContainer);
		RenderButtons(MainMenu_BUTTONS,button);
		Render(title);
		for (i=0;i<MainMenu_CREDITS;i++)
			Render((ENTITY *)credits[i]);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(GAME_SPEED);
	}
	/*Cleanup*/
	EntDestroy(playerNameContainer);
	EntDestroy(title);
	for (i=0;i<MainMenu_CREDITS;i++)
		EntDestroy(credits[i]);
	DestroyButtonArray(MainMenu_BUTTONS, button);
}


void GameState_MainMenu_InputPlayerName(int strMaxSize, char *tempString, ENTITY *backround){
	SDL_Event event;

	int renderText = 0;
	int goBack = 0;

	TEXTBOX *nameBox =  CreateTextBoxEx(0,0,tempString, g_font_OpenSans, (SDL_Color) {0,0,0} );
	ENTITY *inputBox = EntityCreate(g_window_surface, 0 , 0);
	inputBox->image->w = 201;
	inputBox->image->h = 101;

	EntSetCenterX(nameBox, VIDEO_W/2);
	EntSetCenterY(nameBox, VIDEO_H/2);
	EntSetCenterX(inputBox, VIDEO_W/2);
	EntSetCenterY(inputBox, VIDEO_H/2);

	TEXTBOX *text[1] = {CreateTextBoxEx(0,inputBox->posY,"Please insert your name:", g_font_StarWars, (SDL_Color) {255,255,0} )};
	EntSetCenterX(text[0],VIDEO_W/2);
	text[0]->posY -= text[0]->image->h + 2;

	SDL_StartTextInput();
	while (!goBack && !stop){
		renderText = 0;
		while (SDL_PollEvent(&event)) // Tratando eventos.________________________
			onEventTyping(&event, &goBack, &renderText, tempString, strMaxSize, FALSE);

			RendererRescale();
			SDL_RenderClear(gRenderer);
			Render(backround);

			if (renderText && strlen(tempString) < PLAYER_NAME_MAX_CHARACTERS){
				EntDestroy(nameBox);
				nameBox = CreateTextBoxEx(0,0,tempString,g_font_OpenSans, (SDL_Color) {0,0,0} );
				EntSetCenterX(nameBox, EntGetCenterX(inputBox));
				EntSetCenterY(nameBox, EntGetCenterY(inputBox));
			}

			Render(inputBox);
			Render((ENTITY *) nameBox);
      Render( (ENTITY*) text[0]);

			SDL_RenderPresent(gRenderer);
			SDL_Delay(GAME_SPEED);
	}
	SDL_StopTextInput();
	FILE *file = fopen("Settings/PlayerName.config", "w");
	if (!file)
		puts("Failed to create player name file.");
	else{
		fprintf(file, "%s", tempString);
		fclose(file);
	}
	EntDestroy(inputBox);
	EntDestroy(nameBox);
	EntDestroy(text[0]);
}

void MainMenu_Settings_HandleButtons(int mousePress, int howManyButtons, BUTTON **button){
	int highlight;
	highlight = CheckButtons(howManyButtons, button);
	if (highlight == 0 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		char tstr[5] = "";
		sprintf(tstr, "%d", g_settings_MinBallsToExplode);
		GameState_MainMenu_ChangeSetting(3, tstr, 2, 5, &g_settings_MinBallsToExplode);
	}
	else if (highlight == 1 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		char tstr[5] = "";
		sprintf(tstr, "%d", g_settings_MaxAmmo);
		GameState_MainMenu_ChangeSetting(3, tstr, 0, 255, &g_settings_MaxAmmo);
	}
	else if (highlight == 2 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		char tstr[5] = "";
		sprintf(tstr, "%d", g_settings_StartingRows);
		GameState_MainMenu_ChangeSetting(3, tstr, 1, 10, &g_settings_StartingRows);
	}
	else if (highlight == 3 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		char tstr[5] = "";
		sprintf(tstr, "%d", g_settings_AmmoTotal);
		GameState_MainMenu_ChangeSetting(3, tstr, 1, 10, &g_settings_AmmoTotal);
	}
	else if (highlight == 4 && mousePress){
		Mix_PlayChannel(-1, buttonClick, 0);
		char tstr[5] = "";
		sprintf(tstr, "%d", g_settings_PseudoRandom);
		GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_PseudoRandom);
	}
  else if (highlight == 5 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_SimpleShuffle);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_SimpleShuffle);
  }
  else if (highlight == 6 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_CrazyShuffle);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_CrazyShuffle);
  }
  else if (highlight == 7 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_FixedAspectRatio);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_FixedAspectRatio);
    if (g_settings_FixedAspectRatio){
    int wW, wH;
      SDL_GetWindowSize(g_window, &wW, &wH);
      SDL_SetWindowSize(g_window, wW - 10, wH - 10);
      RendererRescale();
      SDL_SetWindowSize(g_window, wW, wH);
      RendererRescale();
    }
  }
  else if (highlight == 8 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_LifeTotal);
    GameState_MainMenu_ChangeSetting(3, tstr, 1, 10, &g_settings_LifeTotal);
  }
  else if (highlight == 9 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_MovingStars);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_MovingStars);
  }
  else if (highlight == 10 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_RenderStars);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 1, &g_settings_RenderStars);
  }
	else if (highlight == 11 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_SoundVolume);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 128, &g_settings_SoundVolume);
		Mix_VolumeChunk(g_sounnd1[0],g_settings_SoundVolume);
		Mix_VolumeChunk(g_sounnd1[1],g_settings_SoundVolume);
		Mix_VolumeChunk(sound,g_settings_SoundVolume);
		Mix_VolumeChunk(buttonClick,g_settings_SoundVolume);
		Mix_VolumeChunk(countPoints,g_settings_SoundVolume);
  }
	else if (highlight == 12 && mousePress){
    Mix_PlayChannel(-1, buttonClick, 0);
    char tstr[5] = "";
    sprintf(tstr, "%d", g_settings_MusicVolume);
    GameState_MainMenu_ChangeSetting(3, tstr, 0, 128, &g_settings_MusicVolume);
		Mix_VolumeMusic(g_settings_MusicVolume);
  }
	else if (highlight == 13 && mousePress){
		static int fullscreen = 0;
		Mix_PlayChannel(-1, buttonClick, 0);
		if (!fullscreen)
			SDL_SetWindowFullscreen(g_window, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen(g_window, 0);
		fullscreen++;
		fullscreen%=2;
	}
}

void GameState_MainMenu_Settings(){
	SDL_Event event;
	BUTTON *button[14];
	int howManyButtons = 14;
	CreateButtonArray(button, howManyButtons, 200, 100, "Cluster Size", 200, 200, "Max Shots",
	200, 300, "Starting Rows", 200, 400, "Ammo Size", 200, 500, "Pseudorandom", 200, 600, "Shuffle",
  200, 700, "Crazy Shuffle", VIDEO_W-200, 100, "Aspect Ratio", 200, 800, "Lives", VIDEO_W-200, 200, "Moving Stars",
  VIDEO_W-200, 300, "Show Stars", VIDEO_W-200, 400, "Sound Volume", VIDEO_W-200, 500, "Music Volume",
VIDEO_W-200, 600, "Full-Screen");

	int goBack = 0, mousePress = 0;

	while (!goBack && !stop){
		PointCenterMouse(&g_mouse_point);
		mousePress = 0;
		while (SDL_PollEvent(&event))
			onEventPaused(&goBack, &event, &mousePress);

		RendererRescale();
		SDL_RenderClear(gRenderer);

		MainMenu_Settings_HandleButtons(mousePress, howManyButtons, button);

		RenderButtons(howManyButtons,button);

		SDL_RenderPresent(gRenderer);
		SDL_Delay(GAME_SPEED);
	}
}


void GameState_MainMenu_ChangeSetting(int strMaxSize, char *tempString, int minValue, int maxValue, int *value){
	SDL_Event event;
	int renderText = 0, goBack = 0;
	BUTTON *button = ButtonCreate(VIDEO_W/2, VIDEO_H/2, tempString);

	SDL_StartTextInput();
	while (!goBack && !stop){
		renderText = 0;
		while (SDL_PollEvent(&event))
			onEventTyping(&event, &goBack, &renderText, tempString, strMaxSize, TRUE);

		RendererRescale();
		SDL_RenderClear(gRenderer);

		if (renderText){
			EntDestroy(button->text);
			button->text = CreateTextBox(0,0,tempString,g_font_OpenSans);
			EntSetCenterX(button->text, EntGetCenterX(button->frame));
			EntSetCenterY(button->text, EntGetCenterY(button->frame));
		}

		RenderButton(button);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(GAME_SPEED);
	}
	sscanf(tempString, "%d", value );
	if (*value < minValue){
		*value = minValue;
		sprintf(tempString, "%d", *value);
	}
	else if (*value > maxValue){
		*value = maxValue;
		sprintf(tempString, "%d", *value);
	}

	printf("Setting changed to %d\n", *value);
	SDL_StopTextInput();
}

void GameState_MainMenu_ViewScoreboard(char * playerName, ENTITY *backround){
	SDL_Event event;
	int goBack = 0, mousePress = 0;

	HIGHSCORES highscores;
	FILE *file = fopen("Highscores/Global/highscores.txt", "r");
	if (!file){
		puts("IO Error: could not open Highscores/Global/highscores.txt");
		return;
	}
	ReadHighScores(file, &highscores);
	fclose(file);

	char playerHighscoreFileName[PLAYER_NAME_MAX_CHARACTERS+20] = "Highscores/";
	strcat(playerHighscoreFileName, playerName);
	strcat(playerHighscoreFileName, ".txt");

	HIGHSCORES playerHighscores;
	file = fopen(playerHighscoreFileName, "r");
	if (!file){
		printf("New player. Trying to create highscore file.\n");
		file = fopen(playerHighscoreFileName, "w");
		GenerateEmptyHighscores(file);
		fclose(file);
		file = fopen(playerHighscoreFileName, "r");
		if (!file){
			printf("IO Error: could not open %s\n", playerHighscoreFileName);
			return;
		}
	}
	ReadHighScores(file, &playerHighscores);
	fclose(file);

	SCOREBOARD *scoreboard = ScoreboardCreate(&highscores);
	ScoreboardSetCenter(scoreboard, VIDEO_W/4, VIDEO_H/2);
	SCOREBOARD *playerScoreboard = ScoreboardCreate(&playerHighscores);
	ScoreboardSetCenter(playerScoreboard, 3*(VIDEO_W/4), VIDEO_H/2);

	int i;
	for(i=0;i<10;i++){
		if (!(strcmp(highscores.playerNames[i], playerName))){
			SDL_SetTextureColorMod(scoreboard->textboxes[0][i]->texture, 255, 255, 0);
			SDL_SetTextureColorMod(scoreboard->textboxes[1][i]->texture, 255, 255, 0);
			SDL_SetTextureColorMod(scoreboard->textboxes[2][i]->texture, 255, 255, 0);
		}
	}

	TEXTBOX *globalTitle = CreateTextBoxEx(0, scoreboard->posY, "All Scores", g_font_StarWars, (SDL_Color) {255,255,0});
	TEXTBOX *playerTitle = CreateTextBoxEx(0, playerScoreboard->posY, "Your Scores", g_font_StarWars, (SDL_Color) {255,255,0});
	playerTitle->posY -= playerTitle->image->h + 3;
	globalTitle->posY -= globalTitle->image->h + 3;
	EntSetCenterX(playerTitle, EntGetCenterX(playerScoreboard));
	EntSetCenterX(globalTitle, EntGetCenterX(scoreboard));

	while (!goBack && !stop){
		PointCenterMouse(&g_mouse_point);
		mousePress = 0;
		while (SDL_PollEvent(&event))
			onEventPaused(&goBack, &event, &mousePress);

		RendererRescale();
		SDL_RenderClear(gRenderer);
		Render(backround);

		RenderScoreboard(scoreboard);
		RenderScoreboard(playerScoreboard);
		Render( (ENTITY*) playerTitle);
		Render( (ENTITY*) globalTitle);

		SDL_RenderPresent(gRenderer);
		SDL_Delay(GAME_SPEED);
	}
	EntDestroy(scoreboard);
	EntDestroy(playerScoreboard);
	EntDestroy(globalTitle);
	EntDestroy(playerTitle);
}



//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//GAME STATES - Level Editor
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

int GameState_LevelEditor(ENTITY *backround){
  int new_time = SDL_GetTicks();
	SDL_Event event;
	int i,j, goBack = 0, mousePress = 0;
	BALL *ball = NULL;
	BALL *palette[COLOR_TOTAL];

	GridReset(0);
	for (i=0;i<COLOR_TOTAL;i++)
		palette[i] = BallCreate(i, 0 + 2*i*GRID_CELL, VIDEO_H - BALL_D - 2, 0, 0, BALL_D);

  while (!stop && !goBack){
		PointCenterMouse(&g_mouse_point);
		while (SDL_PollEvent(&event)) // Tratando eventos
			onEventPaused(&goBack, &event, &mousePress);

		if (SDL_GetTicks() > new_time){ //Calcular gamestate de acordo com FPS
			RendererRescale(); //Redefinindo escala para o renderizador
			SDL_RenderClear(gRenderer);
			Render(backround);

			if (mousePress){
				if (ball){
					int *grid = GetNearestGridPoint(g_mouse_point.x, g_mouse_point.y);
					if (grid[0] < 14){
						if (GRID2[grid[0]][grid[1]] != 0){
							EntDestroy(GRID[grid[0]][grid[1]]);
						}
					BALL *tempBall = BallCreate(ball->color, 0, 0, 0,0, BALL_D);
					BallAddToGrid(grid[0], grid[1], &tempBall, 1); //Criar nova bola no grid
					}
					else{
						EntDestroy(ball); //Destruir bola se o usuaria clicou na parte de baixo da tela
						ball = NULL;
					}
					free(grid);
				}
				else{
					for (i=0;i<COLOR_TOTAL;i++){ //Verificar se jogador clicou em uma bola da paleta
						if (distance(g_mouse_point.x - EntGetCenterX(palette[i]), g_mouse_point.y - EntGetCenterY(palette[i])) < BALL_D){
							ball = BallCreate(i, g_mouse_point.x, g_mouse_point.y, 0, 0, BALL_D);
						}
					}
					if (!ball){
						int *grid = GetNearestGridPoint(g_mouse_point.x, g_mouse_point.y);
						if (GRID2[grid[0]][grid[1]] != 0)
							BallRemoveFromGrid(grid[0],grid[1],&ball);
						free(grid);
					}
				}
			}
			else{
				if (ball){ //Ajustar posicao da bola para o centro do mouse
					EntSetCenterX(ball, g_mouse_point.x);
					EntSetCenterY(ball, g_mouse_point.y);
				}
			}

			for(j=0;j<20;j++) //Renderizar matriz de bolas
				for(i=0;i<NUM_BALLS;i++)
					if (GRID2[j][i] != 0)
						Render( (ENTITY *) GRID[j][i]);
			for (i=0;i<COLOR_TOTAL;i++) //Renderizar paleta de bolas
				Render( (ENTITY *) palette[i]);
			if (ball) //Renderizar bola se ela existir
				Render( (ENTITY *) ball);

			SDL_RenderPresent( gRenderer ); // Atualizar tela
			new_time += GAME_SPEED; //Definir tempo para o quadro seguinte
			mousePress = 0;
		}
		SDL_Delay(1); //Evitar uso de CPU, mas continuar capturando eventos
	}
	SaveGameState();
	if (ball)
		EntDestroy(ball); //So destruir a bola caso ela exista
	for (i=0;i<COLOR_TOTAL;i++)
		EntDestroy(palette[i]);
  return 0;
}


















void TestGrid(){
  GridReset(0);
  int a = 1, b =5;
  GRID[a][b] = BallCreate(0, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
  a=2;
  b=6;
  GRID[a][b] = BallCreate(1, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
  a=2;
  b=7;
  GRID[a][b] = BallCreate(2, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
  a=2;
  b=8;
  GRID[a][b] = BallCreate(3, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
  a=1;
  b=8;
  GRID[a][b] = BallCreate(1, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
  a=0;
  b=8;
  GRID[a][b] = BallCreate(2, Grid2PosX(a,b), Grid2PosY(a,b), 0,0,29);
	BallAddToGrid(a,b, &GRID[a][b], 1);
}
