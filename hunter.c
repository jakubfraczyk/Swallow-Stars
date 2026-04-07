#include "includes.h"
#include "hunter.h"

// Initialize our hunters

HUNTER* InitHunter(WIN* w, int x, int y, int dx, int dy){ 
    HUNTER* hunter = (HUNTER*)malloc(sizeof(HUNTER)); // Allocate memory for hunter
    hunter->win = w; // Use hunter on screen we want
    hunter->x = x;
    hunter->y = y;
    hunter->yMax = w->rows; // Get maximum size of window
    hunter->xMax = w->cols;
    hunter->dx = dx = RESET; 
    hunter->dy = dy = RESET;
    return hunter;
}

// We draw our hunter shape using rand function
// Shapes I used are 1x1, 1x2, 2x2, 1x3, 3x1, 1x5 where first value is width and second height

void drawHunterShape(HUNTER* hunter){
    int shape = rand() % 8; // We draw a hunter shape
        if(shape == 0){ // Hunter 2x1
            hunter->width = 2; 
            hunter->height = 1;
         } else if(shape == 1){ // Hunter 1x2
            hunter->width = 1; 
            hunter->height = 2;
        } else if(shape == 2){ // Hunter 2x2
            hunter->width = 2; 
            hunter->height = 2;
        } else if(shape == 3){ // Hunter 3x1
            hunter->width = 3; 
            hunter->height = 1;
        } else if(shape == 4){ // Hunter 1x3
            hunter->width = 1; 
            hunter->height = 3;
        } else if(shape == 5){ // Hunter 1x5
            hunter->width = 1; 
            hunter->height = 5;
        } else if(shape == 6){ // Hunter 5x1
            hunter->width = 5; 
            hunter->height = 1;
        } else if(shape == 7){ // Hunter 1x1
            hunter->width = 1; 
            hunter->height = 1;
        }
}

// We draw from which side our hunter will start

void setHunterPosition(HUNTER* hunter, WIN* gameWin){
    int wall = rand() % 4;
        if(wall == 0){ // Bottom
            hunter->x = rand() % (gameWin->cols - 2) + 1; // +1 beacuse I dont want to spawn him on border
            hunter->y = 1.0;
        } else if (wall == 1){ // Down
            hunter->x = rand() % (gameWin->cols - 2) + 1;
            hunter->y = (gameWin->rows -2) - (hunter->height - 1);
        } else if(wall == 2){ // Left
            hunter->x = 1.0;
            hunter->y = rand() % (gameWin->rows - 2) + 1;
        } else if(wall == 3){ // Right
            hunter->x = (gameWin->cols - 2) - (hunter->width - 1);
            hunter->y = rand() % (gameWin->rows - 2) + 1;
        }
}

// This is important function which set hunter and spawn him 

void spawnHunter(HUNTER allhunters[], int maxHunters, WIN* gameWin, BIRD* bird, GameConfig* config){
    for(int i = 0; i < maxHunters; i++){
        if(allhunters[i].active == INACTIVE){ // If hunter is inactive we activate him
            drawHunterShape(&allhunters[i]); // Use function to draw a shape
            setHunterPosition(&allhunters[i], gameWin); // Use function to set hunter position
            float vectorX = bird->x - allhunters[i].x; // We calculate diffrence between bird and hunter in x
            float vectorY = bird->y - allhunters[i].y; // We calculate diffrence between bird and hunter in y
            float length = sqrt(vectorX * vectorX + vectorY * vectorY); // We use pithagoras pattern to calculate shortest way 
            if(length == 0) { // If length is below 0 we need to set it to 1 because we cant divide by 0
                length = 1;
            }
            float speedFactor = (float)config->HUNTER_SPEED / 3.0f; // We set our hunter speed. I divide by 3.0 because normal hunter value speed from config is to high so I need to decrease it 
            // Next 2 lines are important logic of moving
            // We use trigonometric functions to find the shortest way to get to the bird 
            // If we divide our calculate difference by length(hypotenuse of triangle(przeciwprostokatna)) we can calculate our 
            // best dx and dy to add to our hunter to move into bird
            // Example: if vectorX = 3, vectorY = 4 our length will be 5 so 
            // our dx will be 3/5 = 0.6 and dy = 4/5 = 0.8 it is the best way to move hunter with precision
            allhunters[i].dx = (vectorX / length) * speedFactor; 
            allhunters[i].dy = (vectorY / length) * speedFactor;
            allhunters[i].bounces = config->HUNTER_BOUNCES; // Every hunter has quantity of bounces from config
            allhunters[i].active = ACTIVE; // Set our hunter active
            allhunters[i].attack_timer = RESET; // We reset attack timer
            allhunters[i].next_attack_timer = 50 + (rand() % 50); // Random time until next dash
            allhunters[i].win = gameWin; // Hunter window
            allhunters[i].color = rand() % 4; // We draw hunter color
            sprintf(allhunters[i].symbol, "%d", allhunters[i].bounces); // Display bounces left (put integer into string)
            return;
        }
    }
}

// Check if hunter hit wall and make bounce

void checkWallBounceHunter(HUNTER* hunter, float dashSpeed){
    int hit = 0;
    if(hunter->x <= 1 || (hunter->x + hunter->width >= hunter->win->cols -1)){ // Check bounce from X wall
        hunter->dx *= -1;
        hit = 1;
    }
    if(hunter->y <= 1 || (hunter->y + hunter->height >= hunter->win->rows -1)){ // Check bounce from Y wall
        hunter->dy *= -1;
        hit = 1;
    }
    if(hit == TRUE){ // If we hit wall we need to decrease bounces and decrease speed
        hunter->bounces--;
        hunter->dx /= dashSpeed;
        hunter->dy /= dashSpeed;
        sprintf(hunter->symbol, "%d", hunter->bounces); // Update displayed bounces
    }
    if(hunter->x <= 1){ // Keep hunter inside window from left 
        hunter->x = 1;
    } else if(hunter->x + hunter->width >= hunter->win->cols -1){ // From right
        hunter->x = (hunter->win->cols -1) - hunter->width;
    }
    if(hunter->y <= 1){ // From top
        hunter->y = 1;
    } else if(hunter->y + hunter->height >= hunter->win->rows -1){ // From bottom
        hunter->y = (hunter->win->rows -1) - hunter->height;
    }
}

// Function which move our hunter forward

void moveHunter(HUNTER* hunter, GAME* game, float dashSpeed){
    // We add our calculated dx and dy to our position and also wind effect but with lower impact
    hunter->x += hunter->dx + (game->wind->dx * 0.85);
    hunter->y += hunter->dy + (game->wind->dy * 0.85);
    if(game->safeZone->active == ACTIVE){ // If safe zone is active we check if hunter hit safe zone
        int hunterX = (int)hunter->x; // Get hunter positiom in int because safe zone use int
        int hunterY = (int)hunter->y;
        SAFE_ZONE * zone = game->safeZone; // Get safe zone from game structure
        // Check hunter collision with safe zone
        if(hunterX < zone->x + zone->width // Check if hunter left side is before safe zone right side
        && hunterX + hunter->width > zone->x // Check if hunter right side is after safe zone left side
        && hunterY < zone->y + zone->height // Check if hunter top side is before safe zone bottom side
        && hunterY + hunter->height > zone->y){ // Check if hunter bottom side is after safe zone top side
            // If true, hunter hit safe zone so we need to bounce him back
            hunter->x -= hunter->dx;
            hunter->y -= hunter->dy;
            hunter->dx = -hunter->dx;
            hunter->dy = -hunter->dy;
            hunter->bounces--;
            hunter->dx /= dashSpeed;
            hunter->dy /= dashSpeed;
            sprintf(hunter->symbol, "%d", hunter->bounces); // Update displayed bounces
            return;
        }
    }
    checkWallBounceHunter(hunter, dashSpeed); 
}

// Handle collisions using cell occupancy maps

void updateMap(GAME* game, GameConfig* config){
    // Clear collision map
    for(int y = 0; y < MAX_MAP_HEIGHT; y++){
        for(int x = 0; x < MAX_MAP_WIDTH; x++){
            game->collisionMap[y][x] = MAP_EMPTY;
        }
    }
    // Update collision map with active hunters
    for(int i = 0; i < config->MAX_HUNTERS; i++){
        HUNTER* hunter = &game->allHunters[i];
        if(hunter->active == ACTIVE){
            for(int y = 0; y < hunter->height; y++){
                for(int x = 0; x < hunter->width; x++){
                    int mapX = (int)hunter->x + x;
                    int mapY = (int)hunter->y + y;
                    // Ensure we are within bounds before updating map
                    if(mapX >= 0 && mapX < config->WINDOW_WIDTH && mapY >= 0 && mapY < config->WINDOW_HEIGHT && mapY < MAX_MAP_HEIGHT && mapX < MAX_MAP_WIDTH){
                        game->collisionMap[mapY][mapX] = MAP_HUNTER;
                    }
                }
            }
        }
    }
}

// Check if bird hit specified hunter

int Hit(int hunterX, int hunterY, HUNTER* hunter){
    int hunterActualX = (int)hunter->x; // Get hunter positiom
    int hunterActualY = (int)hunter->y;
    // Check collision 
    if(hunterX >= hunterActualX && hunterX < hunterActualX + hunter->width &&
       hunterY >= hunterActualY && hunterY < hunterActualY + hunter->height){
        return TRUE;
    } else {
        return FALSE;
    }
}

// Apply damage to bird and handle hunter deactivation

void applyDamage(GAME* game, GameConfig* config, HUNTER* hunter){
    if(game->immunity_timer > 0){ // If bird is immortal we skip damage application
        return;
    }
    game->lives -= config->HUNTER_DAMAGE; // Decrease bird lives by hunter damage from config
    game->immunity_timer = 5; // I need to set small immunity time to avoid multiple hits in one frame
    hunter->active = INACTIVE; // Deactivate hunter after hit
    if(game->lives <= 0){
        game->isRunning = FALSE; // If lives reach zero we stop the game
    }
}

// Function bellow check collisions between hunter and bird 

void checkHunterColision(HUNTER* hunter, BIRD* bird, GAME* game, GameConfig* config){
    if(game->safeZone->active == ACTIVE){ // If safe zone is active we dont check collisions
        return;
    }
    // I check colision in every step of bird movement to avoid effect that hunter miss bird because bird move too fast
    for(int step = 0; step <= (int)bird->birdspeed; step++){ // Int because birdspeed is float
        float progress; // Progress of bird movement from 0.0 to 1.0
        if(bird->birdspeed == 0){ // Avoid division by zero
            progress = 1.0f;
        } else {
            progress = (float)step / bird->birdspeed; // Calculate progress for every step
        }
        // We calculate bird position at current step by substracting percentage of movement from current position
        int birdX = (int)(bird->x - (bird->dx * bird->birdspeed *  (1.0f - progress))); 
        int birdY = (int)(bird->y - (bird->dy * bird->birdspeed *  (1.0f - progress)));
        // Check collision using collision map 
        if(birdY >= 0 && birdY < config->WINDOW_HEIGHT && birdY < MAX_MAP_HEIGHT){
            int mapHit = 0;
            // Check central point of bird
            if(birdX >= 0 && birdX < config->WINDOW_WIDTH && game->collisionMap[birdY][birdX] == MAP_HUNTER){
                mapHit = 1;
            // Check left side of bird
            } else if((birdX - 1) >= 0 && (birdX - 1 < config->WINDOW_WIDTH) && game->collisionMap[birdY][birdX - 1] == MAP_HUNTER){
                mapHit = 1;
            // Check right side of bird
            } else if((birdX + 1) < config->WINDOW_WIDTH && (birdX + 1) < MAX_MAP_WIDTH && game->collisionMap[birdY][birdX + 1] == MAP_HUNTER){
                mapHit = 1;
            } // If any map hit detected we do precise check
            int birdHit = 0;
            if(mapHit == 1){
                if(Hit(birdX, birdY, hunter) == TRUE || Hit(birdX - 1, birdY, hunter) == TRUE || Hit(birdX + 1, birdY, hunter) == TRUE){
                    birdHit = 1;
                }
            } // If HIT we apply damage
            if(birdHit == TRUE){
                // If hit is true we decrease bird life and set immunity timer
                applyDamage(game, config, hunter);
                break; // Exit loop if collision detected
            }
        }
    } 
}

// Hunter dash logic to quickly go for bird

void dashHunter(HUNTER* hunter, int maxHunters, BIRD* bird, GAME* game, GameConfig* config){
    if(game->safeZone->active == 1 || game->taxi->active > 0){ // If safe zone or taxi is active hunter dont dash
        return;
    }
    if(hunter->attack_timer > 0){ // If attack timer is active we decrease it
        hunter->attack_timer--;
        sprintf(hunter->symbol, "!!"); // Change symbol to show hunter is dashing
    
    if(hunter->attack_timer == 0){ // If timer reach zero we do dash
        float vectorX = bird->x - hunter->x; // Calculate diffrence between bird and hunter
        float vectorY = bird->y - hunter->y; 
        float length = sqrt(vectorX * vectorX + vectorY * vectorY); // Calculate length using pithagoras same like before
        if(length == 0){ // Avoid division by zero
            length = 1;
        }
        float dashSpeed = ((float)config->HUNTER_SPEED / 3.0f) * 1.5f; // Dash speed is higher than normal speed, I divide and multiply to get proper value
        hunter->dx = (vectorX / length) * dashSpeed; // Set new dx and dy for dash
        hunter->dy = (vectorY / length) * dashSpeed;
        sprintf(hunter->symbol, "%d", hunter->bounces); // Reset symbol to bounces left
        hunter->next_attack_timer = 90 + (rand() % 50); // Set next attack timer <4.5s, 7.0s>
    }
    return; // Exit function if we are dashing
    }
    if(hunter->next_attack_timer > 0){ // If next attack timer is active we decrease it
        hunter->next_attack_timer--;
    } else {
        hunter->attack_timer = 60; // Set attack timer to 60 frames. Wait to dash
    }
}

// Update all hunters in game

void updateHunter(HUNTER allhunters[] ,int maxHunters, BIRD* bird, GAME* game, GameConfig* config){
    if(game->immunity_timer > 0){ // Decrease bird immunity timer if active
        game->immunity_timer--;
    }
    float dashSpeed = ((float)config->HUNTER_SPEED / 3.0f) * 1.5f; // Dash speed same like in dash function(Dash speed is higher than normal speed, I divide and multiply to get proper value)
    for(int i = 0; i < maxHunters; i++){ // Update every hunter
        if(allhunters[i].active == ACTIVE){ // If hunter is active we update him
            dashHunter(&allhunters[i], maxHunters, bird, game, config); // Check if hunter need to dash
            if(allhunters[i].attack_timer == 0){ // If hunter start dash we move him with dash speed
                moveHunter(&allhunters[i], game, dashSpeed); 
                if(allhunters[i].bounces < 0){
                    allhunters[i].active = INACTIVE;
                continue; // Skip to next hunter
                }
            }
        checkHunterColision(&allhunters[i], bird, game, config);
        } 
    }
}

// Display hunters

void displayHunters(HUNTER allhunters[], int maxHunters){
    for(int i = 0; i < maxHunters; i++){
        if(allhunters[i].active == ACTIVE){ // If hunter is active we display him
            int colorID; 
            if(allhunters[i].color == 0){ // Set color based on hunter color value
                colorID = HUNTER_COLOR;
            } else if(allhunters[i].color == 1){
                colorID = HUNTER_COLOR_2;
            } else if(allhunters[i].color == 2){
                colorID = HUNTER_COLOR_3;
            } else if(allhunters[i].color == 3){
                colorID = HUNTER_COLOR_4;
            }
            wattron(allhunters[i].win->window, COLOR_PAIR(colorID));
            for(int y = 0; y < allhunters[i].height; y++){ // Draw hunter shape based on width and height
                for(int x = 0; x < allhunters[i].width; x++){
                    char symbol = 'H';
                    mvwaddch(allhunters[i].win->window, (int)allhunters[i].y + y, (int)allhunters[i].x + x,symbol);
                }
            }
            mvwprintw(allhunters[i].win->window, (int)allhunters[i].y, (int)allhunters[i].x, "%s", allhunters[i].symbol); // Print bounces left
            wattroff(allhunters[i].win->window, COLOR_PAIR(colorID));
            
        }
    }
}
