#include "catch_engine.h"
#include <Arduino.h>

static Adafruit_SSD1306* display;
static int touchNav;
static int touchSelect;
static int* gameState;

// Game variables
static int basketX;
static const int basketY = 56;
static const int basketWidth = 16;
static const int basketHeight = 6;
static bool justEntered = true;
static bool lastSelectState = false;

struct FallingItem {
    int x;
    float y;
    float speed;
    bool active;
    bool isGood; // true = good item (circle), false = bad item (X)
};

static const int maxItems = 5;
static FallingItem items[maxItems];
static int score;
static int lives;
static bool gameOver;
static unsigned long lastSpawn = 0;
static unsigned long gameOverTime = 0;

void spawnItem() {
    for (int i = 0; i < maxItems; i++) {
        if (!items[i].active) {
            items[i].x = random(0, 112);
            items[i].y = 0;
            items[i].speed = 1.0 + (score / 10.0); // Speed increases with score
            items[i].active = true;
            items[i].isGood = random(0, 10) < 7; // 70% good items
            break;
        }
    }
}

void initCatch(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr) {
    display = disp;
    touchNav = navPin;
    touchSelect = selectPin;
    gameState = (int*)statePtr;
    
    basketX = 56;
    score = 0;
    lives = 3;
    gameOver = false;
    justEntered = true;
    lastSelectState = false;
    
    for (int i = 0; i < maxItems; i++) {
        items[i].active = false;
    }
}

void updateCatch() {
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
            basketX = 56;
            score = 0;
            lives = 3;
            gameOver = false;
            for (int i = 0; i < maxItems; i++) {
                items[i].active = false;
            }
        }
        return;
    }
    
    // Basket movement - hold to move left
    if (digitalRead(touchNav)) {
        basketX -= 3;
    } else {
        basketX += 3;
    }
    basketX = constrain(basketX, 0, 128 - basketWidth);
    
    // Spawn items
    if (currentTime - lastSpawn > 1000) {
        spawnItem();
        lastSpawn = currentTime;
    }
    
    // Update items
    for (int i = 0; i < maxItems; i++) {
        if (items[i].active) {
            items[i].y += items[i].speed;
            
            // Check if caught
            if (items[i].y >= basketY - 4 && items[i].y <= basketY + basketHeight) {
                if (items[i].x + 4 >= basketX && items[i].x <= basketX + basketWidth) {
                    items[i].active = false;
                    
                    if (items[i].isGood) {
                        score++;
                    } else {
                        lives--;
                        if (lives <= 0) {
                            gameOver = true;
                            gameOverTime = currentTime;
                        }
                    }
                }
            }
            
            // Missed good item
            if (items[i].y > 64) {
                if (items[i].isGood) {
                    lives--;
                    if (lives <= 0) {
                        gameOver = true;
                        gameOverTime = currentTime;
                    }
                }
                items[i].active = false;
            }
        }
    }
}

void renderCatch() {
    display->clearDisplay();
    
    // Basket
    display->drawRect(basketX, basketY, basketWidth, basketHeight, SSD1306_WHITE);
    display->drawLine(basketX, basketY, basketX + basketWidth - 1, basketY, SSD1306_WHITE);
    
    // Falling items
    for (int i = 0; i < maxItems; i++) {
        if (items[i].active) {
            if (items[i].isGood) {
                // Good item (circle)
                display->fillCircle(items[i].x + 2, (int)items[i].y + 2, 3, SSD1306_WHITE);
            } else {
                // Bad item (X)
                display->drawLine(items[i].x, (int)items[i].y, items[i].x + 4, (int)items[i].y + 4, SSD1306_WHITE);
                display->drawLine(items[i].x + 4, (int)items[i].y, items[i].x, (int)items[i].y + 4, SSD1306_WHITE);
            }
        }
    }
    
    // Score and Lives
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->print("Score:");
    display->print(score);
    
    display->setCursor(80, 0);
    display->print("Lives:");
    display->print(lives);
    
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