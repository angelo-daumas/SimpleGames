#ifndef GAME_FILE_IO_H
#define GAME_FILE_IO_H

//FORMATO DO ARQUIVO DE HIGHSCORE/SCOREBOARD
/*
playerName0 score0
playerName1 score1
...
playerName8 score8
playerName9 score9
*/

//Le um arquvio formatado como Scoreboard e coloca os valores lidos em um struct HIGHSCORES
void ReadHighScores(FILE *file, HIGHSCORES *highscores);

//Escreve os valores de um struct HIGHSCORES em um arquivo, formatando-o como Scoreboard
void WriteHighScores(FILE *file, HIGHSCORES *highscores);

/*
Tente inserir uma pontuacao em um arquivo foramtado como Scoreboard. Retorna a posicao
alcancada (0-9) se obteve highscore ou -1 caso contrario. Precisa do arquivo input, onde
as highcores estao salvas e um arquivo output onde elas serao salvas.
*/
int InsertHighScore(FILE *input, FILE *output,  char *newPlayer, int newScore);

//Formata o arquivo como scoreboard. Todos os players sao "-" e as pontuacoes "0"
void GenerateEmptyHighscores(FILE *file);

/*
Tente inserir uma pontuacao em um arquivo foramtado como Scoreboard. Retorna a posicao
alcancada (0-9) se obteve highscore ou -1 caso contrario. Salva no arquivo as novas
posicoes do scoreboard.
*/
int FileNameInputHighScore(char *fileName, char *playerName, int score);

//Remove o ultimo caractere de uma string (substitui ele por '\0')
void RemoveLastCharFromString(char * string);

//Salva as posicoes, cores e estados atuais das bolas da GRID em SavedGame.txt
void SaveGameState();
//Le o arquivo SavedGame.txt e replica seus dados na GRID.
void LoadGameState();

#endif
