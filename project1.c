/*
    =======================================================================
                                SWALLOW STARS
    =======================================================================
    FILE: PROJECT1.c
    -----------------------------------------------------------------------
    AUTHOR: Jakub Frączyk
    Group: 2A
    -----------------------------------------------------------------------
    Gdansk University of Technology
    Faculty of Electronics, Telecommunications and Informatics
    Data Engineering 
    -----------------------------------------------------------------------
    CONTROLS:
        W - Move Up
        S - Move Down
        A - Move Left
        D - Move Right
        Q - Quit the game

    GAME MECHANICS:
        Move the bird using W, A, S, D keys.
        Collect stars (*) to increase score. Faster stars give more points.
        Avoid hunters (H) whose have different behaviors and speeds.
        Use albatross (<A>) to escape danger, limited uses.
        Wind affects movement of stars and bird.
        Game difficulty increases over time with more stars and hunters.
        Final score calculated based on stars collected, time left, and lives remaining.
        Save your score to the leaderboard.

    COMPILATION:
    gcc project1.c config.c hunter.c bird.c game_mechanics.c menu.c stars.c -lncurses -lm -o PROJECT1

    =======================================================================
*/

// Include necessary headers

#include "includes.h"
#include "hunter.h"
#include "bird.h"
#include "config.h"
#include "game_mechanics.h"
#include "menu.h"
#include "stars.h"

// Initialize screen

void screen(void){
    initscr(); // Start curses mode
    cbreak(); // We dont need to wait for ENTER
    noecho(); // We dont see input characters
    curs_set(0); // Hide cursor
    start_color(); // Enable color functionality
    init_pair(MAIN_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(STAT_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(BIRD_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(DAMAGED_BIRD_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(ALMOST_DEAD_BIRD, COLOR_RED, COLOR_BLACK);
    init_pair(STAR_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(HUNTER_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(HUNTER_COLOR_2, COLOR_CYAN, COLOR_BLACK);
    init_pair(HUNTER_COLOR_3, COLOR_RED, COLOR_BLACK);
    init_pair(HUNTER_COLOR_4, COLOR_BLUE, COLOR_BLACK);
    init_pair(ALBATROSS_COLOR, COLOR_WHITE, COLOR_BLUE);
    refresh(); // Refresh the screen to apply changes
}

// We initialize game and all its components

GAME* INITGAME(GameConfig* config){
    GAME* game = (GAME*)malloc(sizeof(GAME)); // Allocate memory for GAME structure 
    if(game == NULL){ // Check for successful allocation
        fprintf(stderr, "Memory allocation failed for GAME structure.\n");
        exit(EXIT_FAILURE); // Exit if allocation fails
    }
    game->config = *config; // We copy the config values into the game structure to not change original config
    game->skyWin = sky_area(config); // Create game window
    game->statusWin = status_area(config); // Create status window
    game->bird = InitBird(&game->skyWin, config->WINDOW_WIDTH/2, config->WINDOW_HEIGHT/2, 0, 0 ); // Initialize bird in the center of the window
    game->hunter = InitHunter(&game->skyWin, config->WINDOW_WIDTH/2, config->WINDOW_HEIGHT/2, 0, 0); // Initialize hunter
    game->score = InitScore(&game->skyWin); // Initialize score
    allocateGameMemory(game, config);
    setupGameEntities(game, config);
    timeLogic(game, config);
    initParticles(game);
    setupGameState(game, config);
    srand(config->SEED); // Seed random number generator for consistent randomness
    nodelay(game->bird->win->window, TRUE); //Dont wait for input
    return game;
}

// Update game state each frame

void updateGame(GAME* game, GameConfig* config){
    if(rand() % 100 < config->STAR_SPAWN_CHANCE){ //Chance to spawn a star each frame
        spawnStar(game->allStars, MAX_STARS, &game->skyWin, game);
    }
    if(rand() % 100 < config->HUNTER_SPAWN_RATE){ // Chance to spawn a hunter each frame
        spawnHunter(game->allHunters, config->MAX_HUNTERS, &game->skyWin, game->bird, config);
    }
    increase_difficulty(game, config); 
    updateStars(game->allStars, MAX_STARS, game->bird, game->score, game->bird->birdspeed, config, game); //Update star positions
    updateSafeZone(game);
    updateHunter(game->allHunters, config->MAX_HUNTERS,game->bird, game, config);
    updateMap(game, config);
    updateAlbatross(game, config);
    updateWind(game);
    updateParticles(game);
    updateBird(game->bird, game);
    gameTime(game);
}

// Display all game elements

void displayGame(GAME* game, GameConfig* config){
    werase(game->skyWin.window); //Clear the window
    box(game->skyWin.window, SCREEN_0, SCREEN_0); // Draw border
    displayParticles(game);
    if(game->taxi->active == ALBATROSS_GOING_BASE || game->safeZone->active == TRUE){ // Display safe zone if albatross is taking us to base
        displaySafeZone(game->safeZone, &game->skyWin);
    }
    displayBird(game->bird, game);
    displayStars(game->allStars, MAX_STARS, game->frameCount);
    displayHunters(game->allHunters, config->MAX_HUNTERS);  
    displayAlbatross(game->taxi);
    wrefresh(game->skyWin.window); // Refresh to show changes
    drawGameStatus(game, config);
}

// Handle end of game and display final score

void END_GAME(GAME* game){
    WINDOW* statusWin = game->statusWin.window; // Get status window
    WINDOW* skyWin = game->skyWin.window; // Get game window
    int midY = game->config.WINDOW_HEIGHT / 2; // Calculate middle of the window
    int midX = game->config.WINDOW_WIDTH / 2 - 6; // Center X position for final score display
    int finalScoreValue = finalScore(game, game->config.DIFFICULTY_LABEL); // Get final score
    int timeScore; // Time bonus score
    if(game->gameLeaved == TRUE){ // If game was leaved early, give no score
        finalScoreValue = ZERO;
        timeScore = ZERO;
        game->score->score = ZERO;
        game->lives = ZERO;
    }
    if(game->lives <= 0){ // No lives left
        timeScore = ZERO;
    } else {
        timeScore = game->game_time->remaining_time * TIME_BONUS_MULTIPLIER;
    }
    werase(statusWin); // Clear status window
    werase(skyWin); // Clear game window
    box(statusWin, 0, 0); // Draw border for status window
    box(skyWin, 0, 0); // Draw border for game window
    displayEndGame(game);
    mvwprintw(skyWin, midY - 2, midX - 10, "Stars: %d  Lives: %d", game->score->score, game->lives * LIFE_BONUS_MULTIPLIER);
    mvwprintw(skyWin, midY - 1, midX - 10, "Time Bonus: %d", timeScore);
    mvwprintw(skyWin, midY + 0, midX - 10, "Multiplier: (%s)", game->config.DIFFICULTY_LABEL);
    wattron(skyWin, A_BOLD); // Bold for final score
    mvwprintw(skyWin, midY + 2, midX - 10, "FINAL SCORE: %d", finalScoreValue);
    wattroff(skyWin, A_BOLD);
    wrefresh(skyWin); // Refresh to show final score
    saveScore(game, finalScoreValue, midY + 4, midX - 6);
    wrefresh(skyWin); 
    flushinp(); // Clear input buffer
    wgetch(skyWin); // Wait for user input before exiting
}

// Main game loop

void GAMELOOP(GAME* game, GameConfig* config){
    int star_quota = config->STAR_QUOTA; // Get star goal from config
    while(game->isRunning){ // While game is running
        game->frameCount++; // Increment frame count
        game->ch = getmv(game->bird, config, game); // Get user input
        updateGame(game, config);
        displayGame(game, config);
        if(game->stars_collected >= star_quota){ // Check if star goal is reached
            game->isRunning = FALSE;
            star_quota = RESET;
            game->game_won = TRUE;
        }
        usleep(50000); // Control game speed
        if(game->ch == (int)config->QUIT){ // If quit key is pressed
            game->isRunning = FALSE;
            game->gameLeaved = TRUE;
        }
    }
    END_GAME(game);
}

// Main function

int main(void) {
    GameConfig* config = CONFIG(); // Load configuration
    if (config == NULL) { // Check if config loaded successfully
        fprintf(stderr, "Unable to load configuration!\n");
        return 1;
    }
    screen(); // Initialize screen
    keypad(stdscr, TRUE); // Enable arrow keys
    noecho(); // We dont see input characters
    curs_set(0); // Hide cursor
    char *options[] = { // Menu options
        "PLAY: Easy",
        "PLAY: Medium",
        "PLAY: Hard",
        "Settings",
        "Ranking",
        "Exit Game"
    };
    int countOptions = 6;
    int choice = LEVEL_MEDIUM; // Set default menu choice
    int menu = 1; // Menu loop control
    while(menu){
        drawMenu(choice, options, countOptions, config);
        int key = getch();
        switch(key){
            case KEY_UP:
                choice--;
                if(choice < ZERO){
                    choice = countOptions - 1;
                }
                break;
            case KEY_DOWN:
                choice++;
                if(choice >= countOptions){ // Wrap around menu selection
                    choice = ZERO; 
                }
                break;
            case 10: // Enter key
                if(choice >= 0 && choice <= 2){ // Start game with selected difficulty
                    GameConfig lvlConfig = setupDifficulty(*config, choice); // Setup difficulty
                    GAME* game = INITGAME(&lvlConfig); // Initialize game
                    if (game != NULL) { // Check if game initialized successfully
                        GAMELOOP(game, &lvlConfig); // Start game loop
                        CLEANUP(game, &lvlConfig); // Cleanup after game ends
                    } 
                } else if(choice == 3){
                    showSettings(config);
                } else if(choice == 4){
                    showRanking(config);
                } else if(choice == 5){
                    menu = 0;
                }
                break;
        }
    }
    endwin(); // End ncurses
    free(config); // Free config memory
    return 0;
}