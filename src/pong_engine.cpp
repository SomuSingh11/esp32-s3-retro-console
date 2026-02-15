#include "pong_engine.h"
#include <Arduino.h>

static Adafruit_SSD1306* display;
static int touchNav;
static int touchSelect;
static int* gameState;

// Game variables
static float ballX, ballY;
static float ballVX, ballVY;
static int paddleY;
static int cpuPaddleY;
static int playerScore;
static int cpuScore;
static const int paddleHeight = 12;
static const int paddleWidth = 2;
static bool gameActive;
static bool lastSelectState = false;
static unsigned long lastSelectTime = 0;
static bool justEntered = true;

static unsigned long lastUpdate = 0;

void resetBall() {
    ballX = 64;
    ballY = 32;
    ballVX = random(0, 2) == 0 ? -1.5 : 1.5;
    ballVY = random(-10, 10) / 10.0;
}

void initPong(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr) {
    display = disp;
    touchNav = navPin;
    touchSelect = selectPin;
    gameState = (int*)statePtr;
    
    paddleY = 26;
    cpuPaddleY = 26;
    playerScore = 0;
    cpuScore = 0;
    gameActive = true;
    justEntered = true;
    lastSelectState = false;
    resetBall();
}

void updatePong() {
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
    
    // Exit game - debounced
    if (selectTouched && !lastSelectState && (currentTime - lastSelectTime > 200)) {
        *gameState = 0; // Back to menu
        playerScore = 0;
        cpuScore = 0;
        gameActive = true;
        resetBall();
        lastSelectTime = currentTime;
        return;
    }
    lastSelectState = selectTouched;
    
    if (!gameActive) {
        if (currentTime - lastUpdate > 2000) {
            gameActive = true;
            resetBall();
            lastUpdate = currentTime;
        }
        return;
    }
    
    // Player paddle control
    if (digitalRead(touchNav)) {
        paddleY -= 3;
    } else {
        paddleY += 3;
    }
    paddleY = constrain(paddleY, 0, 64 - paddleHeight);
    
    // CPU AI - simple tracking
    if (ballX > 64) { // Only move when ball is on CPU side
        if (cpuPaddleY + paddleHeight/2 < ballY) {
            cpuPaddleY += 2;
        } else if (cpuPaddleY + paddleHeight/2 > ballY) {
            cpuPaddleY -= 2;
        }
    }
    cpuPaddleY = constrain(cpuPaddleY, 0, 64 - paddleHeight);
    
    // Ball movement
    ballX += ballVX;
    ballY += ballVY;
    
    // Ball collision with top/bottom
    if (ballY <= 0 || ballY >= 63) {
        ballVY = -ballVY;
        ballY = constrain(ballY, 0, 63);
    }
    
    // Ball collision with player paddle
    if (ballX <= 4 && ballY >= paddleY && ballY <= paddleY + paddleHeight) {
        ballVX = -ballVX * 1.05; // Speed up slightly
        ballVY += (ballY - (paddleY + paddleHeight/2)) / 4.0; // Angle based on hit position
        ballX = 4;
    }
    
    // Ball collision with CPU paddle
    if (ballX >= 122 && ballY >= cpuPaddleY && ballY <= cpuPaddleY + paddleHeight) {
        ballVX = -ballVX * 1.05;
        ballVY += (ballY - (cpuPaddleY + paddleHeight/2)) / 4.0;
        ballX = 122;
    }
    
    // Scoring
    if (ballX <= 0) {
        cpuScore++;
        gameActive = false;
        lastUpdate = currentTime;
    }
    if (ballX >= 127) {
        playerScore++;
        gameActive = false;
        lastUpdate = currentTime;
    }
    
    // Limit ball velocity
    ballVY = constrain(ballVY, -3, 3);
}

void renderPong() {
    display->clearDisplay();
    
    // Center line
    for (int y = 0; y < 64; y += 4) {
        display->drawPixel(64, y, SSD1306_WHITE);
    }
    
    // Paddles
    display->fillRect(2, paddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display->fillRect(124, cpuPaddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    
    // Ball
    display->fillCircle((int)ballX, (int)ballY, 2, SSD1306_WHITE);
    
    // Scores
    display->setTextSize(1);
    display->setCursor(50, 2);
    display->print(playerScore);
    display->setCursor(70, 2);
    display->print(cpuScore);
    
    // Game over check
    if (playerScore >= 5 || cpuScore >= 5) {
        display->setTextSize(2);
        display->setCursor(20, 28);
        if (playerScore >= 5) {
            display->print("YOU WIN!");
        } else {
            display->print("CPU WINS");
        }
        
        if (millis() - lastUpdate > 3000) {
            playerScore = 0;
            cpuScore = 0;
            gameActive = true;
            resetBall();
        }
    }
    
    display->display();
}