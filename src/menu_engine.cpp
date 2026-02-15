#include "menu_engine.h"
#include <Arduino.h>

static Adafruit_SSD1306* display;
static int touchNav;
static int touchSelect;
static int* gameState;

static int selectedGame = 0;
static const int numGames = 4;
static const char* gameNames[] = {"Pong", "Dodger", "Reaction", "Catch"};

static bool lastNavState = false;
static bool lastSelectState = false;
static unsigned long lastNavTime = 0;
static unsigned long lastSelectTime = 0;
static const unsigned long debounceDelay = 200;

void initMenu(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr) {
    display = disp;
    touchNav = navPin;
    touchSelect = selectPin;
    gameState = (int*)statePtr;
    selectedGame = 0;
}

void updateMenu() {
    unsigned long currentTime = millis();
    
    // Read touch sensors (active HIGH when touched)
    bool navTouched = digitalRead(touchNav);
    bool selectTouched = digitalRead(touchSelect);
    
    // Navigation - cycle through games
    if (navTouched && !lastNavState && (currentTime - lastNavTime > debounceDelay)) {
        selectedGame = (selectedGame + 1) % numGames;
        lastNavTime = currentTime;
        Serial.print("Selected: ");
        Serial.println(gameNames[selectedGame]);
    }
    lastNavState = navTouched;
    
    // Selection - launch game
    if (selectTouched && !lastSelectState && (currentTime - lastSelectTime > debounceDelay)) {
        *gameState = selectedGame + 1; // +1 because MENU is 0
        lastSelectTime = currentTime;
        Serial.print("=== LAUNCHING GAME: ");
        Serial.print(gameNames[selectedGame]);
        Serial.print(" (state = ");
        Serial.print(*gameState);
        Serial.println(") ===");
        Serial.print("SELECT button state at launch: ");
        Serial.println(selectTouched ? "PRESSED" : "RELEASED");
    }
    lastSelectState = selectTouched;
}

void renderMenu() {
    display->clearDisplay();
    
    // Title
    display->setTextSize(2);
    display->setCursor(35, 0);
    display->print("GAMES");
    
    // Menu items
    display->setTextSize(1);
    for (int i = 0; i < numGames; i++) {
        int y = 20 + (i * 11);
        
        if (i == selectedGame) {
            display->fillRect(0, y - 1, 128, 10, SSD1306_WHITE);
            display->setTextColor(SSD1306_BLACK);
        } else {
            display->setTextColor(SSD1306_WHITE);
        }
        
        display->setCursor(10, y);
        display->print(gameNames[i]);
    }
    
    display->setTextColor(SSD1306_WHITE);
    
    display->display();
}