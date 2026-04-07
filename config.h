#ifndef CONFIG_H
#define CONFIG_H
#include "includes.h"

int getControls(GameConfig* config, char* key, char* value);

int getSettings(GameConfig* config, char* key, char* value);

void getGameplay(GameConfig* config, char* key, char* value);

GameConfig* CONFIG(void);

#endif