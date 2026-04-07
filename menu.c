#include "includes.h"

// Fucntion draw main game menu

void drawMenu(int highlight, char* options[], int count, GameConfig* config){
    int midY = config->WINDOW_HEIGHT / 2;
    int midX = config->WINDOW_WIDTH / 2 - 11; // "=== SWALLOW STARS ===" is 21 chars long so to center we -11
    clear(); // Clear the screen
    attron(A_BOLD | COLOR_PAIR(MAIN_COLOR));
    mvprintw(midY - 2, midX, "=== SWALLOW STARS ==="); // Title
    attroff(A_BOLD | COLOR_PAIR(MAIN_COLOR));
    mvprintw(midY, midX, "Use ARROWS to select, ENTER to confirm: "); // Instructions
    for(int i =  0; i< count; i++){
        // Highlight options based on user input
        if(i == highlight){
            attron(A_REVERSE); // Highlight selected option
            mvprintw(midY + 1 + i, midX, " -> %s", options[i]); // Print option with arrow
            attroff(A_REVERSE); // Turn off highlight
        } else{
            mvprintw(midY + 1 + i, midX, "    %s", options[i]);
        }
    }
    refresh();
}

void showSettings(GameConfig* config){
    clear();
    int midY = config->WINDOW_HEIGHT / 2; 
    int midX = config->WINDOW_WIDTH / 2 - 11; // "=== SWALLOW STARS ===" is 21 chars long so to center we -11
    attron(A_BOLD | COLOR_PAIR(MAIN_COLOR)); 
    mvprintw(midY - 2, midX, "=== SETTINGS ==="); // Title
    attroff(A_BOLD | COLOR_PAIR(MAIN_COLOR));
    // Display main settings
    mvprintw(midY, midX, "Player Name: %s", config->PLAYER_NAME);
    mvprintw(midY + 1, midX, "Game Duration: %d seconds", config->GAME_DURATION);
    mvprintw(midY + 2, midX, "Max Lives: %d", config->MAX_LIVES); 
    mvprintw(midY + 3, midX, "Star Quota: %d", config->STAR_QUOTA);
    mvprintw(midY + 4, midX, "Star Spawn Chance: %d%%", config->STAR_SPAWN_CHANCE);
    mvprintw(midY + 5, midX, "Limit Taxi Uses: %d", config->LIMIT_TAXI_USES);
    mvprintw(midY + 7, midX, "Hunter Spawn Rate: %d", config->HUNTER_SPAWN_RATE);
    mvprintw(midY + 8, midX, "Hunter Speed: %.2f", config->HUNTER_SPEED);
    mvprintw(midY + 9, midX, "Hunter Bounces: %d", config->HUNTER_BOUNCES);
    mvprintw(midY + 10, midX, "Hunter Damage: %d", config->HUNTER_DAMAGE);
    mvprintw(midY + 11, midX, "Max Hunters: %d", config->MAX_HUNTERS);
    mvprintw(midY + 13 ,midX, "Press ENTER to return to menu.");
    refresh();
    while(getch() != 10); // Wait for ENTER key
}

void showRanking(GameConfig* config){
    clear();
    int midX = config->WINDOW_WIDTH / 2 - 11; // "=== SWALLOW STARS ===" is 21 chars long so to center we -11
    attron(A_BOLD | COLOR_PAIR(MAIN_COLOR));
    mvprintw(4, midX, "=== RANKING ===");
    attroff(A_BOLD | COLOR_PAIR(MAIN_COLOR));

    FILE* file = fopen("ranking.txt", "r"); // Open ranking file
    LEADBOARD list[100] = {0}; // Temporary list to hold scores
    int count = 0; // Number of scores read
    if(file != NULL){ // Check if file opened successfully
        while(fscanf(file, "%d %s", &list[count].score, list[count].player_name) == 2 && count < 100){
            count++;
    }
    fclose(file);
    }
    // Bubble sort to sort scores in descending order 
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (list[j].score < list[j + 1].score) {
                LEADBOARD temp = list[j]; // Use temporary variable to swap
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
    mvprintw(5, midX, "POS   SCORE   NAME ");
    for(int i = 0; i < 10; i++){ // Display top 10 scores
        if (i < count) {
            if(i == 0) attron(A_BOLD | COLOR_PAIR(STAR_COLOR)); // For first place highlight
            mvprintw(7 + i, midX, "%2d.   %5d   %s", i + 1, list[i].score, list[i].player_name); // Print position, score and name. We used 5 spaces for score to align names
            if(i == 0) attroff(A_BOLD | COLOR_PAIR(STAR_COLOR));
        } else {
            mvprintw(7 + i, midX, "%2d.   -----   ---------- ", i + 1);
        }
    }
    mvprintw(19, midX, "[ PRESS ENTER ]");
    refresh();
    while(getch() != 10); // Wait for ENTER key
}

void drawWindCompass(GAME* game){
    WIND* wind = game->wind; // Wind get from game structure
    float dx = wind->dx;
    float dy = wind->dy;
    char arrow = '-';
    float strength = sqrtf(dx * dx + dy * dy); // We calculate wind strength using Pythagorean theorem
    if(strength > 0.1f){ // If wind strength is significant enough to show direction
        if(fabs(dx) > fabs(dy)) { // Float abs to get absolute value of dx and dy
            if(dx > 0){
                arrow = '>';
            } else {
                arrow = '<';
            }
        } else {
            if(dy > 0){
                arrow = 'v';
            } else {
                arrow = '^';
            }
        }
    }
    mvwprintw(game->statusWin.window, 1, 100, "Wind: [%c] Str: %.1f ", arrow, strength * 10.0f); // Display wind direction and strength, multiply strength to make it more visible
}


void drawGameStatus(GAME* game, GameConfig* config){
    werase(game->statusWin.window);
    box(game->statusWin.window, 0, 0);
    mvwprintw(game->statusWin.window, 1, 2, "PLayer: %s  |", config->PLAYER_NAME); // Show player name
    mvwprintw(game->statusWin.window, 1, 19, "HP %d |", game->lives); // Show lives
    mvwprintw(game->statusWin.window, 1, 26, "Time: %02d:%02d |",game->game_time->minutes, game->game_time->seconds); //Update time
    mvwprintw(game->statusWin.window, 1, 40, "Stars left: %d |", config->STAR_QUOTA - game->stars_collected); //Update stars left
    mvwprintw(game->statusWin.window, 1, 58, "Score: %d  |", game->score->score); //Update score
    mvwprintw(game->statusWin.window, 1, 72, "Level: %d |", game->hunter->change_level); // Update level
    mvwprintw(game->statusWin.window, 1, 84, "Taxi left: %d |", game->taxi->uses); // Update taxi uses left
    drawWindCompass(game); // Draw wind compass
    wrefresh(game->statusWin.window);
}

void displayEndGame(GAME* game){
    WINDOW* skyWin = game->skyWin.window; // Get game window
    int midY = game->config.WINDOW_HEIGHT / 2; // Calculate middle of the window
    int midX = game->config.WINDOW_WIDTH / 2 - 6; // Center X position for final score display
    if(game->game_won == TRUE){ // If game was won
        mvwprintw(skyWin, midY - 4, midX - 10, "CONGRATULATIONS! YOU WON!");
        game->game_won == FALSE;
    } else {
        mvwprintw(skyWin, midY - 4, midX - 10, "GAME OVER!");
    }
}
