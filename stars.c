#include "includes.h"

// Display stars on the screen with blinking effect

void displayStars(STAR allstars[], int maxStars, long frameCount){
    for(int i = 0; i < maxStars; i++){ // For each star
        if(allstars[i].active == 1){ // If star is active
            int blink = (frameCount / 10) % 2; // Blink effect based on frame count
            if(blink == 0){  
            wattron(allstars[i].win->window, COLOR_PAIR(STAR_COLOR));
            mvwaddch(allstars[i].win->window, (int)allstars[i].y, allstars[i].x, '*');
            wattroff(allstars[i].win->window, COLOR_PAIR(STAR_COLOR));
            } else {
                wattron(allstars[i].win->window, COLOR_PAIR(STAR_COLOR) | A_BOLD);
                mvwaddch(allstars[i].win->window, (int)allstars[i].y, allstars[i].x, '+');
                wattroff(allstars[i].win->window, COLOR_PAIR(STAR_COLOR) | A_BOLD);
            }
            
        }
    }
}

// Set star speed based on speed level

void star_speed(STAR* star, SCORE* score, int speedLevel){
    // I must divide by float to have more precise movement
     if(speedLevel == 1){
            star->dy = 1/(float)20; 
     }  else if(speedLevel == 2){
            star->dy = 1/(float)15; 
     }  else if(speedLevel == 3){
            star->dy = 1/(float)10;
      } else if(speedLevel == 4){
            star->dy = 1/(float)5;
     }  else if(speedLevel == 5){
            star->dy = 1/(float)1;
    }
    star->speed = speedLevel;
}

// Spawn a new star at random position from the top

void spawnStar(STAR allstars[], int maxStars, WIN* gameWin, GAME* game){
    for(int i = 0; i < maxStars; i++){
        if(allstars[i].active == INACTIVE){ // Find an inactive star
            allstars[i].x = rand() % (gameWin->cols -2) +1; // Random x position within window bounds
            allstars[i].y = 1.0;
            allstars[i].dx = 0;
            allstars[i].dy = 1/(float)15; // Default speed
            allstars[i].symbol ='*';
            allstars[i].win = gameWin;
            allstars[i].active = 1;
            star_speed(&allstars[i], game->score, rand() % 5); // Random speed level between 1 and 5
            return;
        }
    }
}

// Move star based on wind and its own speed

void moveStar(STAR* star, GAME* game){
    float windX = game->wind->dx * 0.3f; // Wind influence on star
    float windY = game->wind->dy * 0.3f;
    if(windY < 0){ // Reduce upward wind effect
        windY *= 0.1f;
    }
    // Star movement
    star->x += windX; // Update x position with wind effect
    star->y += star->dy + windY; // Update y position with star speed and wind effect
    if((int)star->y >= star->win->rows - 1 || star->x < 1 || star->x > star->win->cols - 2){ // Check if star is out of bounds
        star->active = 0; //Deactivate star if it goes out of boundss
    }
}

// Update stars: move them and check for collection by the bird

void updateStars(STAR allstars[], int maxStars, BIRD* bird, SCORE* score, int speedlevel, GameConfig* config, GAME* game){
    for(int i = 0; i < maxStars; i++){ // For each star
        if(allstars[i].active == 1){ // If star is active
            moveStar(&allstars[i], game); // Move the star
            // Check if star is collected by bird
            int caught = 0;
            for(int step = 0; step <= (int)bird->birdspeed; step++){ // Check each step of bird's movement(same method as hunter collision)
                float progress;
                if(bird->birdspeed > 0){
                    progress = (float)step / bird->birdspeed; // Calculate progress of bird movement
                } else {
                    progress = 0; // Avoid division by zero
                }
                int birdCentralX = (int)(bird->x - (bird->dx * bird->birdspeed *  (1.0f - progress))); // Calculate bird's central x position during movement
                int birdCentralY = (int)(bird->y - (bird->dy * bird->birdspeed *  (1.0f - progress))); // Calculate bird's central y position during movement
                int starYcoordinate = (int)allstars[i].y; // Star's y position
                int starXcoordinate = (int)allstars[i].x; // Star's x position
                int hitX = (starXcoordinate >= birdCentralX -1 && starXcoordinate <= birdCentralX +1); // Check if star's x is within bird's width
                int hitY = (starYcoordinate == birdCentralY); // Check if star's y matches bird's y
                if(hitX && hitY){ // If true bird collected the star
                    game->stars_collected++;
                    // Increase score based on star speed
                    if(allstars[i].speed == 1){
                        score->score += 200;
                    } else if (allstars[i].speed == 2){
                        score->score += 250;
                    } else if (allstars[i].speed == 3){
                        score->score += 300;
                    } else if (allstars[i].speed == 4){
                        score->score += 350;
                    } else if (allstars[i].speed == 5){
                        score->score += 400;
                    }
                    allstars[i].active = 0; //Deactivate star
                    caught = 1; // Mark as caught to skip further checks
                    continue;
            }
        }
        if(caught){ // If star was caught, skip to next star
            continue;
        }
    }
}
}

