#ifndef INCLUDES_H
#define INCLUDES_H


/*  ------------------------------------------------
            Includes and Configuration
    ------------------------------------------------  */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>

/*  ------------------------------------------------
                Configuration Constants
    ------------------------------------------------  */

// Starting positions
#define STARTY 1
#define STARTX 1
#define SCREEN_0 0
#define STATUS_HEIGHT 3
#define MAX_MAP_HEIGHT 50
#define MAX_MAP_WIDTH 200

// Game colors
#define MAIN_COLOR 1
#define STAT_COLOR 2
#define BIRD_COLOR 3
#define DAMAGED_BIRD_COLOR 4
#define ALMOST_DEAD_BIRD 5
#define STAR_COLOR 6
#define HUNTER_COLOR 7
#define HUNTER_COLOR_2 8
#define HUNTER_COLOR_3 9
#define HUNTER_COLOR_4 10
#define ALBATROSS_COLOR 11

// Game limits
#define MAX_STARS 30
#define HARD_LIMIT_HUNTERS 50
#define PARTICLES 50

// Defines of variables
#define ACTIVE 1
#define INACTIVE 0
#define TRUE 1
#define FALSE 0
#define RESET 0
#define ZERO 0
#define INITIAL_SPEED 1.0f
#define NO_WIND 0.0f

// Define time constants
#define MINUTE 60

// Define bird 
#define BIRD_INITIAL_SPEED 1.0f
#define BIRD_MAX_SPEED 5.0f
#define BIRD_MIN_SPEED 0.5f
#define BIRD_SPEED_STEP 0.5f

// Bird up
#define BIRD_DY_UP -1
#define BIRD_DX_UP 0
// Bird down
#define BIRD_DY_DOWN 1
#define BIRD_DX_DOWN 0
// Bird left
#define BIRD_DY_LEFT 0
#define BIRD_DX_LEFT -1
// Bird right
#define BIRD_DY_RIGHT 0
#define BIRD_DX_RIGHT 1

// Define hunter
#define HUNTER_WIDTH 3
#define HUNTER_HEIGHT 1
#define HUNTER_ATTACK_COOLDOWN 50
#define HUNTER_NEXT_ATTACK_MIN 30
#define HUNTER_NEXT_ATTACK_MAX 100
#define INIT_CHANGE_LEVEL 1
#define MAP_EMPTY 0
#define MAP_HUNTER 1
// Define albatross
#define ALBATROSS_FLYING 1
#define ALBATROSS_GOING_BASE 2
#define ALBATROSS_WIDTH 3
#define ALBATROSS_HEIGHT 1
#define ALBATROSS_START_X 1
#define ALBATROSS_START_Y 1
#define ALBATROSS_SPEED 1.5

#define CLOSE_TO_BIRD 1.5f

// Define safe zone
#define SAFE_ZONE_START_X 1
#define SAFE_ZONE_START_Y 1
#define SAFE_ZONE_WIDTH 5
#define SAFE_ZONE_HEIGHT 3


// Define of score multipliers
#define EASY_MULTIPLIER 1.0f
#define MEDIUM_MULTIPLIER 1.5f
#define HARD_MULTIPLIER 2.0f
#define EASY_LABEL "EASY"
#define MEDIUM_LABEL "MEDIUM"
#define HARD_LABEL "HARD"
#define TIME_BONUS_MULTIPLIER 100
#define LIFE_BONUS_MULTIPLIER 500

// Setup difficulty levels
#define LEVEL_EASY 0

#define EASY_HUNTER_SPAWN_RATE 5
#define EASY_HUNTER_SPEED 1.0f
#define EASY_HUNTER_BOUNCES 3
#define EASY_MAX_HUNTERS 3
#define EASY_MAX_LIVES 3

#define LEVEL_MEDIUM 1
#define MEDIUM_HUNTER_SPAWN_RATE 8
#define MEDIUM_HUNTER_SPEED 1.3f
#define MEDIUM_HUNTER_BOUNCES 4
#define MEDIUM_MAX_HUNTERS 4

#define LEVEL_HARD 2
#define HARD_HUNTER_SPAWN_RATE 15
#define HARD_HUNTER_SPEED 1.8f
#define HARD_HUNTER_BOUNCES 6
#define HARD_MAX_HUNTERS 6

// Define dificulty halfway through the game
#define COUNT_DIFICULTY_LEVELS 6

#define I_SPAWN 3
#define I_SPEED 0.3f
#define I_BOUNCE 2
#define I_MAX 1
#define ADD_LIFES 3
#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 3
#define LEVEL_4 4
#define LEVEL_5 5


/*  ------------------------------------------------
                Data Structures
    ------------------------------------------------  */

// Game configuration structure

typedef struct {
    // Control keys
    char UP;
    char DOWN;
    char LEFT;
    char RIGHT;
    char QUIT;
    char SPEED_DOWN;
    char SPEED_UP;
    char TAXI;
    // Window and system settings
    int WINDOW_HEIGHT;
    int WINDOW_WIDTH;
    int SEED;
    // Game rules
    int GAME_DURATION;
    int STAR_QUOTA;
    int STAR_SPAWN_CHANCE;
    int LIMIT_TAXI_USES;
    // Player configuration
    char PLAYER_NAME[64];
    int MAX_LIVES;
    int SWALLOW_SPEED_MIN;
    int SWALLOW_SPEED_MAX;
    // Hunter configuration
    int HUNTER_SPAWN_RATE;
    float HUNTER_SPEED;
    int HUNTER_BOUNCES;
    int HUNTER_DAMAGE;
    int MAX_HUNTERS;
    // Additional configuration
    int IS_RANKED;
    char DIFFICULTY_LABEL[16];
} GameConfig;

// Window structure

typedef struct{
    WINDOW *window;
    int x, y;
    int rows, cols;
    int color;
} WIN;

// Bird structure

typedef struct{
    float x, y; 
    float dx, dy;
    int yMax, xMax;
    char symbol;
    float birdspeed;
    int color;
    WIN* win;
} BIRD;

// Hunter structure

typedef struct{
    float x, y;
    float dx, dy;
    int yMax, xMax;
    int bounces;
    char symbol[4];
    int active;
    int color;
    int attack_timer;
    int next_attack_timer;
    int width;
    int height;
    int change_level;
    WIN* win;
} HUNTER;

// Albatross (Taxi) structure

typedef struct{
    float x, y;
    float dx, dy;
    int active;
    int width, height;
    char symbol[8];
    int uses;
    WIN* win;
} ALBATROSS;

// Safe Zone structure

typedef struct{
    int x, y;
    int width, height;
    int active;
    int timer;
} SAFE_ZONE;

// Star structure

typedef struct{
    float y , x;
    float dx, dy;
    int yMax, xMax;
    int speed;
    char symbol;
    int active;
    int color;
    WIN* win;
} STAR;

// Score structure

typedef struct{
    int score;
} SCORE;

// Wind structure

typedef struct{
    float dx, dy;
    float targetDx, targetDy;
    int timer;
} WIND;

// Ranking structure

typedef struct {
    int score;
    char difficulty[10];
    char player_name[64];
    int results;
} LEADBOARD;

// Time structure

typedef struct{
    int elapsed_secs;
    int remaining_time;
    int minutes;
    int seconds;
    int level_1;
    int level_2;
    int level_3;
    int level_4;
    int level_5;
    int level_by_time;
} TIME;

// Particle structure

typedef struct{
    float x, y;
} PARTICLE;

// Main Game structure

typedef struct{
    WIN skyWin; // GAME WINDOW
    WIN statusWin; // STATUS WINDOW
    BIRD* bird;
    HUNTER* hunter;
    ALBATROSS* taxi;
    SCORE* score;
    WIND* wind;
    TIME* game_time;
    SAFE_ZONE* safeZone;
    PARTICLE dust[40];
    STAR allStars[MAX_STARS];
    HUNTER* allHunters;
    GameConfig config;
    int collisionMap[MAX_MAP_HEIGHT][MAX_MAP_WIDTH]; // Collision map for hunters
    int ch; // Input character
    long frameCount; 
    int gameLeaved;
    time_t start_time; // Game start time
    int isRunning;
    int lives;
    int immunity_timer;
    int stars_collected;
    int game_won;
} GAME;

#endif 