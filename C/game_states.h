#ifndef GAME_STATES_H
#define GAME_STATES_H
void onEvent(BALL* ball, INDICATOR* indicator, int *pause); // Trata os eventos.
void RendererRescale(void);


void GameState_GameSession(ENTITY *backround, ENTITY *spaceship, STAR **STARS, char *playerName);
void 	GameState_InitSession(ENTITY *backround, ENTITY *spaceship, STAR **STARS, INDICATOR *indicator, SCORE *playerScore);
int GameState_PlaySession(ENTITY *backround, ENTITY *spaceship, BALL *ball, STAR **STARS, INDICATOR *indicator, SCORE *playerScore);
//void GameState_FinishSession(void); //TODO

void GameState_MainMenu(ENTITY *backround, ENTITY *spaceship, STAR **STARS, char *playerName);
void GameState_MainMenu_InputPlayerName(int strMaxSize, char *tempString, ENTITY *backround);
void GameState_MainMenu_Settings();
void GameState_MainMenu_ChangeSetting(int strMaxSize, char *tempString, int minValue, int maxValue, int *value);
void GameState_MainMenu_ViewScoreboard(char * playerName, ENTITY *backround); //TODO


int GameState_LevelEditor(ENTITY *backround);
#endif
