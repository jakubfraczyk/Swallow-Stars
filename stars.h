#ifndef STARS_H
#define STARS_H
#include "includes.h"

void displayStars(STAR allstars[], int maxStars, long frameCount);

void star_speed(STAR* star, SCORE* score, int speedlevel);

void spawnStar(STAR allstars[], int maxStars, WIN* gameWin, GAME* game);

void updateStars(STAR allstars[], int maxStars, BIRD* bird, SCORE* score, int speedlevel, GameConfig* config, GAME* game);

#endif