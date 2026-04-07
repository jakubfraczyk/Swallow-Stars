#include "includes.h"
#include "bird.h"

// Function which initializes bird structure
BIRD* InitBird(WIN* window, int x, int y, int dx, int dy){  
    // Allocate memory for BIRD structure
    BIRD* bird = (BIRD*)malloc(sizeof(BIRD));
    if(bird == NULL){ // Check for memory allocation failure
        fprintf(stderr, "Error allocating memory for bird\n");
        return NULL;
    }
    bird->win = window; // Assign game window to bird
    bird->x = x; 
    bird->y = y;
    bird->symbol = '>';
    bird->yMax = window->rows;
    bird->xMax = window->cols;
    bird->dx = dx = 1; // By this bird starts in motion to the right
    bird->dy = dy = 0;
    return bird;
}

// Update bird position based on its speed and direction

void updateBird(BIRD* myBird, GAME* game){
    // Calculate where bird need to move based on theirs dy and dx 
    // values multiplied by birds speed and by add wind speed
    float playerMoveX = (myBird->dx * myBird->birdspeed) + game->wind->dx;
    float playerMoveY = (myBird->dy * myBird->birdspeed) + game->wind->dy;
    // Add calculated position to old position
    myBird->x += playerMoveX;
    myBird->y += playerMoveY;
    // Check to keep bird inside game window
    if(myBird->x < 2){
        myBird->x = 2; // 2 because bird is 3 characters wide and it is bounded from left
    }
    if(myBird->x > myBird->win->cols - 3){
        myBird->x = myBird->win->cols - 3; // Check if bird is inside windor from right, we need to substrack 3 because we calculate from rigtht colums and if x is equal to cols we need to x - 3 beacuse x - 1 is frame
    }
    if(myBird->y < 1){ 
        myBird->y = 1; // Check if bird is inside window from top
    }
    if(myBird->y > myBird->win->rows -2){
        myBird->y = myBird->win->rows -2; // Same for bottom but only - 2 beacuse bird is 1 character high
    }
}

// Function which displays bird colors and symbol based on direction

void displayBird(BIRD *myBird, GAME* game){
    char leftPart, bodyPart, rightPart; // Bird is represented by 3 characters
    int flap = (game->frameCount / 3) % 2; // Bird flapping effect based on frame count
        if(myBird->dx == 1){ // Moving right and body parts for this direction
            bodyPart = '-';
            rightPart = '>';
            if(flap == TRUE){
                leftPart = '~';
            } else {
                leftPart = '\\';
            }
        }  else if(myBird->dx == - 1){ // Moving left and mechanism same like for right
            leftPart = '<';
            bodyPart = '-';
            if(flap == TRUE){
                rightPart = '~';
            } else {
                rightPart = '/';
            }
        }  else if(myBird->dy == -1 ){ // Moving up and same like above
            bodyPart = '^';
            if(flap == TRUE){
                leftPart = '/';
                rightPart = '\\';
            } else {
                leftPart = '|';
                rightPart = '|';
            }
        } else if(myBird->dy == 1){ // Moving down and same like above
            bodyPart = 'v';
            if(flap == TRUE){
                leftPart = '\\';
                rightPart = '/';
            } else {
                leftPart = '-';
                rightPart = '-';
            }
        }
    // We initialize color based on bird health
    int colorPair;
    if(game->lives > (game->config.MAX_LIVES / 3) * 2){ // Check if bird has more than 2/3 of lives
        colorPair = BIRD_COLOR; 
    } else if(game->lives > (game->config.MAX_LIVES / 3)){ // Check if bird has more than 1/3 of lives
        colorPair = DAMAGED_BIRD_COLOR;
    } else { // Bird has less than 1/3 of lives
        colorPair = ALMOST_DEAD_BIRD;
    }
    // Display bird with color based on health
        wattron(myBird->win->window, COLOR_PAIR(colorPair) | A_BOLD); // Set color and bold attribute
        mvwaddch(myBird->win->window, myBird->y, myBird->x -1, leftPart); // Left wing
        mvwaddch(myBird->win->window, myBird->y, myBird->x, bodyPart); // Center
        mvwaddch(myBird->win->window, myBird->y, myBird->x +1, rightPart); // Right wing
        wattroff(myBird->win->window, COLOR_PAIR(colorPair) | A_BOLD); // Deactivate color and bold attribute
}