#include "includes.h"
#include "game_mechanics.h"

// Next two functions create game windows 

WIN sky_area(GameConfig* config){ //Prints sky window
    WIN skyWin; // Create sky window
    skyWin.window = newwin(config->WINDOW_HEIGHT, config->WINDOW_WIDTH, STARTY, STARTX); // Tell window what resolution is this area
    skyWin.rows = config->WINDOW_HEIGHT;
    skyWin.cols = config->WINDOW_WIDTH;
    wbkgd(skyWin.window, COLOR_PAIR(MAIN_COLOR)); // Initialize the color of the sky
    werase(skyWin.window);
    box(skyWin.window, SCREEN_0, SCREEN_0);
    wrefresh(skyWin.window);
    return skyWin; // Return copy of the structure to create new window 
} 

// Status area works same like sky area but has diffrent resolution 

WIN status_area(GameConfig* config){ //Prints status area
    WIN statusWin;
    statusWin.window = newwin(STATUS_HEIGHT, config->WINDOW_WIDTH, STARTY + config->WINDOW_HEIGHT, STARTX);
    statusWin.rows = STATUS_HEIGHT;
    statusWin.cols = config->WINDOW_WIDTH;
    wbkgd(statusWin.window, COLOR_PAIR(STAT_COLOR)); // Initialize the color of the status
    werase(statusWin.window);
    box(statusWin.window, SCREEN_0, SCREEN_0);
    wrefresh(statusWin.window);
    return statusWin;
}

// Next function create options to choose what difficulty you want to play 
// It increase hunter spawn rate, speed, bounces

GameConfig setupDifficulty(GameConfig config, int level){
    if(level == LEVEL_EASY){
        config.HUNTER_SPAWN_RATE = EASY_HUNTER_SPAWN_RATE;
        config.HUNTER_SPEED = EASY_HUNTER_SPEED;
        config.HUNTER_BOUNCES = EASY_HUNTER_BOUNCES;
        config.MAX_HUNTERS = EASY_MAX_HUNTERS;
        config.MAX_LIVES += EASY_MAX_LIVES;
        strcpy(config.DIFFICULTY_LABEL, "Easy");
    } else if(level == LEVEL_MEDIUM){
        config.HUNTER_SPAWN_RATE = MEDIUM_HUNTER_SPAWN_RATE;
        config.HUNTER_SPEED = MEDIUM_HUNTER_SPEED;
        config.HUNTER_BOUNCES = MEDIUM_HUNTER_BOUNCES;
        config.MAX_HUNTERS = MEDIUM_MAX_HUNTERS;
        strcpy(config.DIFFICULTY_LABEL, "Medium");
    } else if(level == LEVEL_HARD){
        config.HUNTER_SPAWN_RATE = HARD_HUNTER_SPAWN_RATE;
        config.HUNTER_SPEED = HARD_HUNTER_SPEED;
        config.HUNTER_BOUNCES = HARD_HUNTER_BOUNCES;
        config.MAX_HUNTERS = HARD_MAX_HUNTERS;
        strcpy(config.DIFFICULTY_LABEL, "Hard");
    }
    return config; // Return config to keep the choose
}

// Simply function to initialize score into game

SCORE* InitScore(WIN* w){
    SCORE* score = (SCORE*)malloc(sizeof(SCORE));
    score->score = 0;
    return score;
}

// Allocate game memory keep every structure in one place in memory

void allocateGameMemory(GAME* game, GameConfig* config){
    game->score = InitScore(&game->skyWin); // !!!!!!!!!!!
    game->allHunters = (HUNTER*)malloc(sizeof(HUNTER) * HARD_LIMIT_HUNTERS); // We need to bounded it by hard limit because if not there will be infinitely many hunters
    game->game_time = (TIME*)malloc(sizeof(TIME));
    game->taxi = (ALBATROSS*)malloc(sizeof(ALBATROSS));
    game->wind = (WIND*)malloc(sizeof(WIND));
    game->safeZone = (SAFE_ZONE*)malloc(sizeof(SAFE_ZONE));
}

// We need to setup basic game variables before each game 

void setupGameEntities(GAME* game, GameConfig* config){
    // Bird setup
    game->bird->birdspeed = BIRD_INITIAL_SPEED; // Initial bird speed
    // Hunter logic
    game->hunter->change_level = INIT_CHANGE_LEVEL;
    // Taxi/Albatross setup
    game->taxi->active = INACTIVE;
    game->taxi->width = ALBATROSS_WIDTH;
    game->taxi->height = ALBATROSS_HEIGHT;
    sprintf(game->taxi->symbol, "<A>");
    game->taxi->uses = config->LIMIT_TAXI_USES;
    game->taxi->win = &game->skyWin; // We need to tell albatross where to draw itself
    // Wind setup
    game->wind->dx = NO_WIND;
    game->wind->dy = NO_WIND;
    game->wind->timer = RESET;
    game->wind->targetDx = NO_WIND;
    game->wind->targetDy = NO_WIND;
    // Safe zone setup
    game->safeZone->active = INACTIVE;
    game->safeZone->x = SAFE_ZONE_START_X;
    game->safeZone->y = SAFE_ZONE_START_Y;
    game->safeZone->width = SAFE_ZONE_WIDTH;
    game->safeZone->height = SAFE_ZONE_HEIGHT;
    // Initialize stars and hunters
    for(int i = 0; i < MAX_STARS; i++){ // Activate stars
        game->allStars[i].active = INACTIVE;
    }
    for(int i = 0; i < HARD_LIMIT_HUNTERS; i++){ // Activate hunters
        game->allHunters[i].active = INACTIVE;
        game->allHunters[i].win = &game->skyWin;
        game->allHunters[i].bounces = RESET;
    }
}

// We need to setup basic game info

void setupGameState(GAME* game, GameConfig* config){
    game->lives = config->MAX_LIVES;
    game->ch = RESET;
    game->frameCount = RESET;
    game->start_time = time(NULL);
    game->isRunning = TRUE;
    game->game_won = FALSE;
}

// Basic time logic which help us to increase difficulty through the game and setup timer which reset minutes and seconds 

void timeLogic(GAME* game, GameConfig* config){
    // Difficulty levels time thresholds
    game->game_time->level_1 = (config->GAME_DURATION / COUNT_DIFICULTY_LEVELS) * 5;
    game->game_time->level_2 = (config->GAME_DURATION / COUNT_DIFICULTY_LEVELS) * 4;
    game->game_time->level_3 = (config->GAME_DURATION / COUNT_DIFICULTY_LEVELS) * 3;
    game->game_time->level_4 = (config->GAME_DURATION / COUNT_DIFICULTY_LEVELS) * 2;
    game->game_time->level_5 = (config->GAME_DURATION / COUNT_DIFICULTY_LEVELS) * 1;
    // Time setup
    game->game_time->elapsed_secs = RESET;
    game->game_time->remaining_time = config->GAME_DURATION; // We setup game time in config
    game->game_time->minutes = RESET;
    game->game_time->seconds = RESET;
    game->immunity_timer = RESET;
}

// Function which calculate time and minutes

void gameTime(GAME* game){
    game->game_time->elapsed_secs = (int)difftime(time(NULL), game->start_time); // Calculate how many seconds passed since game started(difftime return double and calculate difference between two values in time_t format)
    game->game_time->remaining_time = game->config.GAME_DURATION - game->game_time->elapsed_secs;
    if(game->game_time->remaining_time <= ZERO){ // End game when time is less than 0
        game->isRunning = FALSE;
        game->game_time->remaining_time = ZERO;
    }
    game->game_time->minutes = game->game_time->remaining_time / MINUTE;
    game->game_time->seconds = game->game_time->remaining_time % MINUTE;
}

// We increase difficulty of the game over time

void increase_difficulty(GAME* game, GameConfig* config){
        int timeLeft = game->game_time->remaining_time; // How many seconds remain
        int currentLvl = game->hunter->change_level; // We need to know which lvl is now
        // In every level we increase the game constant
        if(timeLeft < game->game_time->level_1 && currentLvl == LEVEL_1){
            config->HUNTER_SPAWN_RATE += I_SPAWN;
            config->HUNTER_SPEED += I_SPEED;
            config->HUNTER_BOUNCES += I_BOUNCE;
            config->MAX_HUNTERS += I_MAX;
            game->hunter->change_level = LEVEL_2;
        } else if(timeLeft < game->game_time->level_2 && currentLvl == LEVEL_2){
            config->HUNTER_SPAWN_RATE += I_SPAWN;
            config->HUNTER_SPEED += I_SPEED;
            config->HUNTER_BOUNCES += I_BOUNCE;
            config->MAX_HUNTERS += I_MAX;
            game->hunter->change_level = LEVEL_3;
        } else if(timeLeft < game->game_time->level_3 && currentLvl == LEVEL_3){
            config->HUNTER_SPAWN_RATE += I_SPAWN;
            config->HUNTER_SPEED += I_SPEED;
            config->HUNTER_BOUNCES += I_BOUNCE;
            config->MAX_HUNTERS += I_MAX;
            game->hunter->change_level = LEVEL_4;
        } else if(timeLeft < game->game_time->level_4 && currentLvl == LEVEL_4){
            config->HUNTER_SPAWN_RATE += I_SPAWN;
            config->HUNTER_SPEED += I_SPEED;
            config->HUNTER_BOUNCES += I_BOUNCE;
            config->MAX_HUNTERS += I_MAX;
            game->hunter->change_level = LEVEL_5;
        } 
}

// Movement functions

void mvup(BIRD *myBird){ // Bird goes up
    myBird->symbol = '^';
    myBird->dy = BIRD_DY_UP;
    myBird->dx = BIRD_DX_UP;
}

void mvdown(BIRD *myBird){ // Bird goes down
    myBird->symbol = 'v';
    myBird->dy = BIRD_DY_DOWN;
    myBird->dx = BIRD_DX_DOWN;
}

void mvleft(BIRD *myBird){ // Bird goes left
    myBird->symbol = '<';
    myBird->dx = BIRD_DX_LEFT;
    myBird->dy = BIRD_DY_LEFT;
}
void mvright(BIRD *myBird){ // Bird goes right
    myBird->symbol = '>';
    myBird->dx = BIRD_DX_RIGHT;
    myBird->dy = BIRD_DY_RIGHT;
}

// Move bird by input, using keys from config
// Increase and decrease bird speed by input and we recall albatross 

int getmv(BIRD *myBird, GameConfig* config, GAME* game){
    if(game->taxi->active == ALBATROSS_GOING_BASE){ // If albatross carry us to the base we cant move
        return 0;
    }
    int choice = wgetch(myBird->win->window); // Get our key 
    if (choice == (int)config->UP){ // Move bird up
        mvup(myBird);
    } else if (choice == (int)config->DOWN){ // Move bird down
        mvdown(myBird);
    } else if (choice == (int)config->LEFT){ // Move bird left 
        mvleft(myBird);
    } else if (choice == (int)config->RIGHT){ // Move bird right
        mvright(myBird);
    } else if (choice == (int)config->SPEED_UP){ // Increase bird speed
        myBird->birdspeed += BIRD_SPEED_STEP; 
        if (myBird->birdspeed > BIRD_MAX_SPEED){ // Max speed from config
            myBird->birdspeed = BIRD_MAX_SPEED;
        }
    } else if (choice == (int)config->SPEED_DOWN){ // Decrease bird speed
        myBird->birdspeed -= BIRD_SPEED_STEP;
        if (myBird->birdspeed < BIRD_MIN_SPEED){ // Min speed from config
            myBird->birdspeed = BIRD_MIN_SPEED;
        }
    }  else if(choice == (int)config->TAXI && game->taxi->active == INACTIVE && game->taxi->uses > ZERO){ // We racall the albatross
        game->taxi->active = TRUE;
        game->taxi->x = ALBATROSS_START_X;
        game->taxi->y = ALBATROSS_START_Y;
        game->taxi->uses--;
    }
    return choice;
}

// Display safe zone 

void displaySafeZone(SAFE_ZONE* zone, WIN* win){
    if(zone->active == FALSE){
        return;
    }
    wattron(win->window, COLOR_PAIR(STAT_COLOR));
    wattron(win->window, A_BOLD);
    mvwprintw(win->window, zone->y, zone->x, "-----"); // Place where bird wait to return to game
    mvwprintw(win->window, zone->y + 1, zone->x, "-   -");
    mvwprintw(win->window, zone->y + 2, zone->x, "-----");
    wattroff(win->window, A_BOLD);
    wattroff(win->window, COLOR_PAIR(STAT_COLOR));
}

// We need to calculate how long bird will be in zone and activate it when albatross carry us there

void updateSafeZone(GAME* game){
    SAFE_ZONE* zone = game->safeZone; // !!! dlaczego tak a nie mozemy w wywolaniu funkcji tego uzyc
    BIRD* bird = game->bird;
    if(zone->active == TRUE){ // If zone is active, bird stay in zone
        zone->timer--; // Time logic
        bird->x = zone->x + 1;
        bird->y = zone->y + 1;
        game->immunity_timer = 5; // Bird need to be immunite in safe zone
        if(zone->timer <= ZERO){
            zone->active = FALSE;
            bird->dx = 1; // Bird need to be in motion when leave safe zone 
            bird->dy = 0;
            bird->y = zone->y + zone->height + 2;
            game->immunity_timer = 20; // Bird need to be immunite for a small bit of time to hunters cant hurt bird
        }
        return;
    }
}

// Display our taxi/albatross 

void displayAlbatross(ALBATROSS* albatross){
    if(albatross->active > ZERO){ // If albatross goes for bird or carry her we need to display him
        wattron(albatross->win->window, COLOR_PAIR(ALBATROSS_COLOR));
        wattron(albatross->win->window, A_BOLD);
        mvwprintw(albatross->win->window, (int)albatross->y, (int)albatross->x, "%s", albatross->symbol); // We need to use int because ncurses use int values, x and y are in float
        wattroff(albatross->win->window, A_BOLD);
        wattroff(albatross->win->window, COLOR_PAIR(ALBATROSS_COLOR));
    }
}

// Albatross logic to go for bird and carry her to the base

void updateAlbatross(GAME* game, GameConfig* config){
    ALBATROSS* albatross = game->taxi; // Get albatross from game structure
    BIRD* bird = game->bird; // Get bird from game structure
    if(albatross->active == ZERO){ // if albatross is not active despawn him
        return;
    } else if(albatross->active == ALBATROSS_FLYING){ // Albatross goes for bird
        // We calculate constantly every frame where is bird and albatross goes for her using pitagoras theory for the shortest way to get bird
        float vectorX = bird->x - albatross->x; // Calculate diffrence between x bird and albatross x
        float vectorY = bird->y - albatross->y; // Calculate diffrence between y bird and albatross y
        float length = sqrt(vectorX * vectorX + vectorY * vectorY); // We use pattern lebgth = sqrt(x2 + y2) for the shortest way 
        if(length <= CLOSE_TO_BIRD){ // If albatross is close to the bird we pick up bird 
            albatross->active = ALBATROSS_GOING_BASE; // Albatross change state 
            game->immunity_timer = 999; // Bird need to be immortal 
        } else { // If bird is not close we need to move close to bird 
            albatross->x += (vectorX / length) * ALBATROSS_SPEED; // Albatross need to be faster than bird
            albatross->y += (vectorY / length) * ALBATROSS_SPEED;
        }
    } else if (albatross->active == 2){ // If we catch bird we go to the safe zone
        float targetX = 2.0; // Target x where is safe zone
        float targetY = 2.0; // Target y where is safe zone
        float vectorX = targetX - albatross->x; // We use same mechanic like in to catch bird (calculate the shortest way to go to safe zone)
        float vectorY = targetY - albatross->y;
        float length = sqrt(vectorX * vectorX + vectorY * vectorY);
        if(length < 1.0){ // If albatross is close, we put bird in safe zone
            albatross->active = 0; // Deactive albatross
            game->safeZone->active = 1;
            game->safeZone->timer = 60; // Time how long bird is in safe zone (about 3 seconds)
            bird->x = targetX; // Bird x equal safe zone x
            bird->y = targetY; // Bird y equal safe zone y
        } else { // Else if albatross is not close, we fly towards safe zone
            albatross->x += (vectorX / length) * ALBATROSS_SPEED;
            albatross->y += (vectorY / length) * ALBATROSS_SPEED;
            bird->x = albatross->x + 1; // Bird fly with albatross
            bird->y = albatross->y;
        }
    }
}

// Wind mechanism

void updateWind(GAME* game){
    WIND* wind = game->wind; // Create wind variable from game structure 
    if(wind->timer > 0){ // Time down
        wind->timer--;
    } else {
        wind->timer = 100 + (rand() % 200); // Wind change direction every 100-300 frames(about 5-15 seconds)
        int direction = rand() % 5; // We draw direction 
        float strength = 0.4f; // Strength of the wind
        wind->targetDx = 0.0f; // Use of target to smooth change of wind direction
        wind->targetDy = 0.0f;
        if(direction == 1){ // Left
            wind->targetDx = -strength; // We set target direction
        } else if(direction == 2){ // Right
            wind->targetDx = strength;
        } else if(direction == 3){ // Up
            wind->targetDy = -strength;
        } else if(direction == 4){ // Down
            wind->targetDy = strength;
        }
    }   // We smoothly change wind direction to avoid sudden changes
        // Every second is about 60 frames so we change wind in 2% steps
        // So in every second wind change by 33% to target direction
        float step = 0.02f;
        // For example if target is 0.4 and current is 0.0 we add 0.4 * 0.02 = 0.008 to current so new current is 0.008. 
        //It will repeat until current reach target
        wind->dx += (wind->targetDx - wind->dx) * step; 
        wind->dy += (wind->targetDy - wind->dy) * step;
}

// We create particles "." in the air to know without looking at compass in which direction blows

void initParticles(GAME* game){
    for(int i = 0; i < PARTICLES; i++){
        game->dust[i].x = rand() % (game->skyWin.cols - 2) + 1; // We need -2 because without border and +1 because we dont want particles on border
        game->dust[i].y = rand() % (game->skyWin.rows - 2) + 1; // Same for y
    }
}

void updateParticles(GAME* game){
    float speed = 2.0f; // speed of the parcticles
    for(int i = 0; i < PARTICLES; i++){
        game->dust[i].x += game->wind->dx * speed; // Update x dust
        game->dust[i].y += game->wind->dy * speed; // Update y dust
        int maxW = game->skyWin.cols - 2; // We must know max width, -2 because without border
        int maxH = game->skyWin.rows - 2; // We must know max height, -2 because without border
        if(game->dust[i].x < 1){ // If close to the border it cant fly over it 
            game->dust[i].x = maxW;
        } else if(game->dust[i].x > maxW){ // In diffrent way
            game->dust[i].x = 1;
        }
        if(game->dust[i].y < 1){ // same but with y border
            game->dust[i].y = maxH;
        } else if(game->dust[i].y > maxH){
            game->dust[i].y = 1;
        }
    }
}

// We need to display our particles

void displayParticles(GAME* game){
    wattron(game->skyWin.window, COLOR_PAIR(MAIN_COLOR));
    for(int i = 0; i < PARTICLES; i++){
        mvwaddch(game->skyWin.window, (int)game->dust[i].y, (int)game->dust[i].x, '.'); // int because ncurser use integer and not float
    }
    wattroff(game->skyWin.window, COLOR_PAIR(MAIN_COLOR));
}

// We calculate our final score with this pattern
// (Score from stars + score from lives + score from remaining time) * multiplier of each level

int finalScore(GAME* game, char* difficultyLabel){
    int scoreStars = game->score->score; // Score depends of star speed
    int scoreLives = game->lives * 500; // Remaining lives * 500 to balance score
    int scoreTime = game->game_time->remaining_time * 100; // Remaining seconds * 100 to balance score
    if(scoreLives < 0){ // If score lives go below 0  we get 0 points from lives
        scoreLives = 0;
    }
    if(scoreTime < 0){ // If time end we get 0 points from time
        scoreTime = 0;
    }
    if(game->lives <= 0){ // If we dead we get 0 points from reamining time. Use it to avoid special killing 
        scoreTime = 0;
    }
    float multiplier = EASY_MULTIPLIER;
    if (strcmp(game->config.DIFFICULTY_LABEL, "Medium") == 0) { // Compare names and choose which multiplier we use
        multiplier = MEDIUM_MULTIPLIER;
    }
    if (strcmp(game->config.DIFFICULTY_LABEL, "Hard") == 0){ // Same for hard mode
        multiplier = HARD_MULTIPLIER;
    }
    return (int)((scoreStars + scoreLives + scoreTime) * multiplier); // Use of our pattern, we us int to avoid score with coma
}

// Function below save our score into ranking file

void saveScore(GAME* game, int score, int y, int x){
    WINDOW* skyWin = game->skyWin.window; // We need our game window
    int midY = game->config.WINDOW_HEIGHT / 2; // We calculate our mid y
    int midX = game->config.WINDOW_WIDTH / 2 - 6; // we calculate our mid x, - 6  beacuse "Enter Name: " is 12 character long
    mvwprintw(skyWin, midY + 4, midX - 10, "Enter Name: ");
    wrefresh(skyWin); // We need refresh screen
    nodelay(skyWin, FALSE); // We need to wait for user input
    echo(); 
    curs_set(1); // We see our cursor
    char playerName[64]; // Player name
    mvwgetnstr(skyWin, midY + 4, midX + 2, playerName, 15); // We write our name on the screen which is 15 char long max
    noecho(); 
    curs_set(0); // Hide our cursor
    FILE* file = fopen("ranking.txt", "a"); // Create and the open our ranking file
    if(file){
        fprintf(file, "%d %s\n", score, playerName); // We save our score and player name in the file
        fclose(file); // Close our file
        mvwprintw(skyWin, midY + 6, midX, "SCORE SAVED!"); // Show information that our score has been saved
    }
    wrefresh(skyWin); // Refresh our game window
    sleep(1); // Wait 1 second
}

// We need to free our memory from mallocs to avoid memory leak

void CLEANUP(GAME* game, GameConfig* config){
    endwin(); //Ends ncurses
    free(game->bird);
    free(game->score);
    free(game->allHunters);
    free(game);
    free(game->game_time);
    free(game->taxi);
    free(game->safeZone);
    free(game->wind);
}