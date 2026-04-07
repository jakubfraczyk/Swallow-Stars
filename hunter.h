#ifndef HUNTER_H
#define HUNTER_H
#include "includes.h"

HUNTER* InitHunter(WIN* w, int x, int y, int dx, int dy);

void spawnHunter(HUNTER allhunters[], int maxHunters, WIN* gameWin, BIRD* bird, GameConfig* config);

void drawHunterShape(HUNTER* hunter);

void moveHunter(HUNTER* hunter, GAME* game, float dashSpeed);

void setHunterPosition(HUNTER* hunter, WIN* gameWin);

void checkWallBounceHunter(HUNTER* hunter, float dashSpeed);

void dashHunter(HUNTER* hunter, int maxHunters, BIRD* bird, GAME* game, GameConfig* config);

void checkHunterColision(HUNTER* hunter, BIRD* bird, GAME* game, GameConfig* config);

void updateMap(GAME* game, GameConfig* config);

void updateHunter(HUNTER allhunters[] ,int maxHunters, BIRD* bird, GAME* game, GameConfig* config);

void displayHunters(HUNTER allhunters[], int maxHunters);

#endif