#ifndef BIRD_H
#define BIRD_H
#include "includes.h"

BIRD* InitBird(WIN* w, int x, int y, int dx, int dy);

void updateBird(BIRD *myBird, GAME* game);

void displayBird(BIRD *myBird, GAME* game);

#endif