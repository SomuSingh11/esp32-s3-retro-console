#include "dodger_engine.h"
#include <Arduino.h>

static Adafruit_SSD1306* display;
static int touchNav;
static int touchSelect;
static int* gameState;

// Game variables
static int playerX;
static const int playerY = 54;
static const int playerWidth = 8;
static const int playerHeight = 8;
static bool justEntered = true;
static bool lastSelectState = false;

struct Obstacle {
    int x;
    float y;
    float speed;
    bool active;
    int width;
};

static const int maxObstacles = 6;
static Obstacle obstacles[maxObstacles];
static int score;
static bool gameOver;
static unsigned long lastSpawn = 0;
static unsigned long gameOverTime = 0;
static unsigned long gameStartTime = 0;

void spawnObstacle() {
    for (int i = 0; i < maxObstacles; i++) {
        if (!obstacles[i].active) {
            obstacles[i].x = random(0, 120);
            obstacles[i].y = 0;
            obstacles[i].speed = 1.5 + (score / 20.0); // Speed increases with score
            obstacles[i].active = true;
            obstacles[i].width = random(6, 16);
            break;
        }
    }
}

void initDodger(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr) {
    display = disp;
    touchNav = navPin;
    touchSelect = selectPin;
    gameState = (int*)statePtr;
    
    playerX = 60;
    score = 0;
    gameOver = false;
    gameStartTime = millis();
    justEntered = true;
    lastSelectState = false;
    
    for (int i = 0; i < maxObstacles; i++) {
        obstacles[i].active = false;
    }
    
    Serial.println(">>> Dodger game initialized");
}

void updateDodger() {
    unsigned long currentTime = millis();
    bool selectTouched = digitalRead(touchSelect);
    
    // Wait for button to be released after entering game
    if (justEntered) {
        if (!selectTouched) {
            justEntered = false;
            lastSelectState = false;
        }
        // Don't process any input while waiting for release
        lastSelectState = selectTouched;
        return;
    }
    
    // Exit game - only on new press
    if (selectTouched && !lastSelectState) {
        *gameState = 0; // Back to menu
        return;
    }
    lastSelectState = selectTouched;
    
    if (gameOver) {
        if (currentTime - gameOverTime > 3000) {
            // Restart game
            playerX = 60;
            score = 0;
            gameOver = false;
            gameStartTime = currentTime;
            for (int i = 0; i < maxObstacles; i++) {
                obstacles[i].active = false;
            }
        }
        return;
    }
    
    // Update score based on survival time
    score = (currentTime - gameStartTime) / 100;
    
    // Player movement - hold to move left
    if (digitalRead(touchNav)) {
        playerX -= 4;
    } else {
        playerX += 4;
    }
    playerX = constrain(playerX, 0, 128 - playerWidth);
    
    // Spawn obstacles
    if (currentTime - lastSpawn > 800) {
        spawnObstacle();
        lastSpawn = currentTime;
    }
    
    // Update obstacles
    for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].active) {
            obstacles[i].y += obstacles[i].speed;
            
            // Check collision with player
            if (obstacles[i].y + 4 >= playerY && obstacles[i].y <= playerY + playerHeight) {
                if (obstacles[i].x < playerX + playerWidth && 
                    obstacles[i].x + obstacles[i].width > playerX) {
                    gameOver = true;
                    gameOverTime = currentTime;
                }
            }
            
            // Remove if off screen
            if (obstacles[i].y > 64) {
                obstacles[i].active = false;
            }
        }
    }
}

void renderDodger() {
    display->clearDisplay();
    
    // Player (triangle/arrow shape)
    display->fillTriangle(
        playerX + playerWidth/2, playerY,           // top point
        playerX, playerY + playerHeight,            // bottom left
        playerX + playerWidth, playerY + playerHeight, // bottom right
        SSD1306_WHITE
    );
    
    // Obstacles (rectangles)
    for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].active) {
            display->fillRect(obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, 4, SSD1306_WHITE);
        }
    }
    
    // Score
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->print("Score: ");
    display->print(score);
    
    // Game over
    if (gameOver) {
        display->fillRect(10, 20, 108, 30, SSD1306_BLACK);
        display->drawRect(10, 20, 108, 30, SSD1306_WHITE);
        display->setTextSize(1);
        display->setCursor(25, 25);
        display->print("GAME OVER!");
        display->setCursor(30, 38);
        display->print("Score: ");
        display->print(score);
    }
    
    display->display();
}