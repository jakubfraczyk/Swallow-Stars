#ifndef GAME_H
#define GAME_H
#include "includes.h"

WIN sky_area(GameConfig* config);

WIN status_area(GameConfig* config);



GameConfig setupDifficulty(GameConfig config, int level);

void allocateGameMemory(GAME* game, GameConfig* config);

void setupGameEntities(GAME* game, GameConfig* config);

void setupGameState(GAME* game, GameConfig* config);

void timeLogic(GAME* game, GameConfig* config);

void gameTime(GAME* game);

void increase_difficulty(GAME* game, GameConfig* config);



void mvup(BIRD *myBird);

void mvdown(BIRD *myBird);

void mvleft(BIRD *myBird);

void mvright(BIRD *myBird);

int getmv(BIRD *myBird, GameConfig* config, GAME* game);



void displaySafeZone(SAFE_ZONE* zone, WIN* win);

void updateSafeZone(GAME* game);

void displayAlbatross(ALBATROSS* albatross);

void updateAlbatross(GAME* game, GameConfig* config);



void updateWind(GAME* game);

void initParticles(GAME* game);

void updateParticles(GAME* game);

void displayParticles(GAME* game);



SCORE* InitScore(WIN* w);

int finalScore(GAME* game, char* difficultyLabel);

void saveScore(GAME* game, int score, int y, int x);



void CLEANUP(GAME* game, GameConfig* config);


#endif 