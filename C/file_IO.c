#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "defs.h"
#include "file_IO.h"
#include "g_func.h"

void ReadHighScores(FILE *file, HIGHSCORES *highscores){
  int i;
  for (i=0;i<10;i++){
    fscanf(file, "%s %d", highscores->playerNames[i], &(highscores->scores[i]));
  }
}
void WriteHighScores(FILE *file, HIGHSCORES *highscores){
  int i;
  for (i=0;i<10;i++){
    fprintf(file, "%s %d\n", highscores->playerNames[i], highscores->scores[i]);
  }
}

int FileNameInputHighScore(char *fileName, char *playerName, int score){
  char tempName[PLAYER_NAME_MAX_CHARACTERS+20+10];
  FILE *file, *tempFile;
  int position;
//Create string for name of temp file
  strcpy(tempName, fileName);
  strcat(tempName, ".temp");
//Open files
  puts(fileName);
  file = fopen(fileName, "r");
  if (!file){
    while(!file){
    file = fopen(fileName, "w+");
  }
    GenerateEmptyHighscores(file);
    fclose(file);
    file = fopen(fileName, "r");
  }
  tempFile = fopen(tempName, "w");
//Insert highscores
  position = InsertHighScore(file, tempFile, playerName, score);
  fclose(file);
  fclose(tempFile);
  remove(fileName);
  rename(tempName, fileName);
  return position;
}

int InsertHighScore(FILE *input, FILE *output, char *newPlayer, int newScore){
  int position = -1, i;
  HIGHSCORES highscores;
  ReadHighScores(input, &highscores);
  for (i=0;i<10;i++){
    if (newScore > highscores.scores[i]){
        position = i;
        for (i=9;i>position;i--){
          strcpy(highscores.playerNames[i], highscores.playerNames[i-1]);
          highscores.scores[i] = highscores.scores[i-1];
        }
        strcpy(highscores.playerNames[i], newPlayer);
        highscores.scores[i] = newScore;
        break;
    }
  }
  WriteHighScores(output, &highscores);
  return position;
}

void GenerateEmptyHighscores(FILE *file){
  int i;
  HIGHSCORES highscores;
  for (i=0;i<10;i++){
      strcpy(highscores.playerNames[i], "-");
      highscores.scores[i] = 0;
    }
  WriteHighScores(file, &highscores);
}

//------------------------------



#include "globals.h"

void RemoveLastCharFromString(char *string){
  string[strlen(string)-1] = 0;
}


//------------------------------

void SaveGameState(){
  FILE *file;

  file = fopen("SavedGame.txt", "w");
  int i, j, color, x, y;
  for(i=0;i<20;i++){
    for(j=0;j<NUM_BALLS;j++){
      if (GRID2[i][j] != 0){
        color = GRID[i][j]->color;
        x = GRID[i][j]->posX;
        y = GRID[i][j]->posY;
      }
      else{
        color = -1;
        x = -1;
        y = -1;
      }
      fprintf(file, "%d %d %d %d %d %d\n", GRID2[i][j], i, j, color, x, y);
    }
  }
  fclose(file);
}

void LoadGameState(){
  FILE *file;

  GridReset(0);
  file = fopen("SavedGame.txt", "r");
  int state, i, j, color, x, y;
  for(i=0;i<20;i++){
    puts("line");
    for(j=0;j<NUM_BALLS;j++){
      fscanf(file, "%d %d %d %d %d %d", &state, &i, &j, &color, &x, &y);
      if (state !=0){
        GRID[i][j] = BallCreate(color, x, y, 0,0, 29);
        BallAddToGrid(i,j, &GRID[i][j],1);
      }
    }
  }
  fclose(file);
}

//FORMATO DO ARQUIVO DE HIGHSCORE
/*
playerName0 score0
playerName1 score1
...
playerName8 score8
playerName9 score9
*/
