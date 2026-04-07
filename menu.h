#ifndef MENU_H
#define MENU_H
#include "includes.h"

void drawMenu(int highlight, char* options[], int count, GameConfig* config);

void showSettings(GameConfig* config);

void showRanking(GameConfig* config);

void drawWindCompass(GAME* game);

void drawGameStatus(GAME* game, GameConfig* config);

void displayEndGame(GAME* game);
#endif