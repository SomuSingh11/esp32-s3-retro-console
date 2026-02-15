#include "reaction_engine.h"
#include <Arduino.h>

static Adafruit_SSD1306* display;
static int touchNav;
static int touchSelect;
static int* gameState;

enum GamePhase {
    WAITING_TO_START,
    WAITING_FOR_SIGNAL,
    SIGNAL_SHOWN,
    RESULT_SHOWN,
    TOO_EARLY
};

static GamePhase phase;
static unsigned long startTime;
static unsigned long reactionTime;
static unsigned long bestTime;
static int currentRound;
static const int maxRounds = 5;
static bool lastSelectState;
static bool lastNavState = false;
static unsigned long lastNavTime = 0;

void initReaction(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr) {
    display = disp;
    touchNav = navPin;
    touchSelect = selectPin;
    gameState = (int*)statePtr;
    
    phase = WAITING_TO_START;
    bestTime = 999999;
    currentRound = 0;
    lastSelectState = false;
}

void updateReaction() {
    unsigned long currentTime = millis();
    bool selectTouched = digitalRead(touchSelect);
    bool navTouched = digitalRead(touchNav);
    
    // Exit with nav button - debounced
    if (navTouched && !lastNavState && (currentTime - lastNavTime > 200)) {
        *gameState = 0;
        lastNavTime = currentTime;
        return;
    }
    lastNavState = navTouched;
    
    switch (phase) {
        case WAITING_TO_START:
            if (selectTouched && !lastSelectState) {
                phase = WAITING_FOR_SIGNAL;
                startTime = currentTime + random(1000, 4000); // Random delay
                currentRound = 1;
            }
            break;
            
        case WAITING_FOR_SIGNAL:
            // Too early press
            if (selectTouched && !lastSelectState) {
                phase = TOO_EARLY;
                startTime = currentTime;
            }
            // Show signal after random delay
            else if (currentTime >= startTime) {
                phase = SIGNAL_SHOWN;
                startTime = currentTime;
            }
            break;
            
        case SIGNAL_SHOWN:
            if (selectTouched && !lastSelectState) {
                reactionTime = currentTime - startTime;
                if (reactionTime < bestTime) {
                    bestTime = reactionTime;
                }
                phase = RESULT_SHOWN;
                startTime = currentTime;
            }
            break;
            
        case RESULT_SHOWN:
            if (currentTime - startTime > 2000) {
                currentRound++;
                if (currentRound <= maxRounds) {
                    phase = WAITING_FOR_SIGNAL;
                    startTime = currentTime + random(1000, 4000);
                } else {
                    phase = WAITING_TO_START;
                    currentRound = 0;
                }
            }
            break;
            
        case TOO_EARLY:
            if (currentTime - startTime > 2000) {
                currentRound++;
                if (currentRound <= maxRounds) {
                    phase = WAITING_FOR_SIGNAL;
                    startTime = currentTime + random(1000, 4000);
                } else {
                    phase = WAITING_TO_START;
                    currentRound = 0;
                }
            }
            break;
    }
    
    lastSelectState = selectTouched;
}

void renderReaction() {
    display->clearDisplay();
    
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->print("Reaction Test");
    
    if (currentRound > 0) {
        display->setCursor(100, 0);
        display->print(currentRound);
        display->print("/");
        display->print(maxRounds);
    }
    
    switch (phase) {
        case WAITING_TO_START:
            display->setTextSize(1);
            display->setCursor(10, 20);
            display->print("Touch SELECT");
            display->setCursor(10, 30);
            display->print("to start!");
            
            if (bestTime < 999999) {
                display->setCursor(10, 45);
                display->print("Best: ");
                display->print(bestTime);
                display->print("ms");
            }
            break;
            
        case WAITING_FOR_SIGNAL:
            display->setTextSize(2);
            display->setCursor(20, 25);
            display->print("WAIT...");
            break;
            
        case SIGNAL_SHOWN:
            display->fillCircle(64, 32, 20, SSD1306_WHITE);
            display->setTextSize(2);
            display->setTextColor(SSD1306_BLACK);
            display->setCursor(50, 26);
            display->print("GO!");
            display->setTextColor(SSD1306_WHITE);
            break;
            
        case RESULT_SHOWN:
            display->setTextSize(1);
            display->setCursor(20, 20);
            display->print("Your time:");
            display->setTextSize(2);
            display->setCursor(30, 35);
            display->print(reactionTime);
            display->setTextSize(1);
            display->print("ms");
            break;
            
        case TOO_EARLY:
            display->setTextSize(2);
            display->setCursor(10, 20);
            display->print("TOO EARLY!");
            display->setTextSize(1);
            display->setCursor(30, 45);
            display->print("Wait for GO!");
            break;
    }
    
    display->display();
}