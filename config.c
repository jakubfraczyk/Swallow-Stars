#include "includes.h"
#include "config.h"

// We load configurations from "config.txt" file in 3 functions
// Every if in these functions compares the key which is read from the file 
// by strcmp (string compare) function
// If value is a number we convert it by atoi (string to integer) or atof (string to float) functions

int getControls(GameConfig* config, char* key, char* value){
if (strcmp(key, "KEY_UP") == 0) {
        config->UP = value[0];
    } else if (strcmp(key, "KEY_DOWN") == 0) {
        config->DOWN = value[0];
    } else if (strcmp(key, "KEY_LEFT") == 0) {
        config->LEFT = value[0];
    } else if (strcmp(key, "KEY_RIGHT") == 0) {
        config->RIGHT = value[0];
    } else if (strcmp(key, "KEY_QUIT") == 0) {
        config->QUIT = value[0];
    } else if (strcmp(key, "TAXI") == 0){
        config->TAXI = value[0];
    } else if(strcmp(key, "SPEED_DOWN_KEY") == 0) {
        config->SPEED_DOWN = value[0];
    } else if (strcmp(key, "SPEED_UP_KEY") == 0) {
        config->SPEED_UP = value[0];
    } else {
        return 0; // Key not recognized
    }
    return 1; // Key recognized
}

int getSettings(GameConfig* config, char* key, char* value){
if (strcmp(key, "WINDOW_HEIGHT") == 0) {
        config->WINDOW_HEIGHT = atoi(value);
    } else if (strcmp(key, "WINDOW_WIDTH") == 0) {
        config->WINDOW_WIDTH = atoi(value);
    } else if (strcmp(key, "GAME_DURATION") == 0) {
        config->GAME_DURATION = atoi(value);
    } else if (strcmp(key, "SEED") == 0) {
        config->SEED = atoi(value);
    } else if (strcmp(key, "PLAYER_NAME") == 0) {
        strcpy(config->PLAYER_NAME, value);
    } else if (strcmp(key, "LIMIT_TAXI_USES") == 0){
        config->LIMIT_TAXI_USES = atoi(value);
    } else {
        return 0; // Key not recognized
    }
    return 1; // Key recognized
}

void getGameplay(GameConfig* config, char* key, char* value){
if (strcmp(key, "STAR_SPAWN_CHANCE") == 0) {
        config->STAR_SPAWN_CHANCE = atoi(value);
    }  else if (strcmp(key, "STAR_QUOTA") == 0) {
        config->STAR_QUOTA = atoi(value);
    } else if (strcmp(key, "MAX_LIVES") == 0) {
        config->MAX_LIVES = atoi(value);
    } else if (strcmp(key, "SWALLOW_SPEED_MIN") == 0) {
        config->SWALLOW_SPEED_MIN = atoi(value);
    } else if (strcmp(key, "SWALLOW_SPEED_MAX") == 0) {
        config->SWALLOW_SPEED_MAX = atoi(value);
    } else if (strcmp(key, "HUNTER_SPAWN_RATE") == 0) {
        config->HUNTER_SPAWN_RATE = atoi(value);
    } else if (strcmp(key, "HUNTER_SPEED") == 0) {
        config->HUNTER_SPEED = atof(value);
    } else if (strcmp(key, "HUNTER_BOUNCES") == 0) {
        config->HUNTER_BOUNCES = atoi(value);
    } else if (strcmp(key, "HUNTER_DAMAGE") == 0) {
        config->HUNTER_DAMAGE = atoi(value);
    } else if (strcmp(key, "MAX_HUNTERS") == 0){
        config->MAX_HUNTERS = atoi(value);
    } 
}

// Main configuration function which reads the file line by line 

GameConfig* CONFIG(void){
    GameConfig* config = (GameConfig*)malloc(sizeof(GameConfig));
    if(config == NULL){ //Check malloc success
        return NULL;
    }
    FILE* configFile = fopen("config.txt", "r"); // Open config file
    if(configFile == NULL){
        free(config);
        return NULL;
    }
    char line[128]; // Max line length
    char key[64]; // Max key length
    char value[64]; // Max value length
    // Read file line by line by fgets which reads until there are no more lines
    while (fgets(line, sizeof(line), configFile) != NULL) { // What, How, From where
        // Check for comments and empty lines 
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }
        // sscanf reads formatted input from a string and check if there are 2 elements in config
        if (sscanf(line, "%63s = %63s", key, value) == 2) {
            if(getControls(config, key, value)) {
                continue;
            }
            if(getSettings(config, key, value)) {
                continue;
            }
            getGameplay(config, key, value);
    }
}
    fclose(configFile);
    return config;
}
