#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "menu_engine.h"
#include "pong_engine.h"
#include "dodger_engine.h"
#include "reaction_engine.h"
#include "catch_engine.h"

// --- Pins ---
#define SDA_PIN 8
#define SCL_PIN 9
#define TOUCH_NAV 4
#define TOUCH_SELECT 5

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- States ---
enum State {
    MENU,
    PONG,
    DODGER,
    REACTION,
    CATCH
};

State currentState = MENU;
State lastState = MENU;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-S3 Game Console Starting...");

    pinMode(TOUCH_NAV, INPUT);
    pinMode(TOUCH_SELECT, INPUT);

    Wire.begin(SDA_PIN, SCL_PIN);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Game Console"));
    display.println(F("Ready!"));
    display.display();
    delay(1000);

    initMenu(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
    initPong(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
    initDodger(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
    initReaction(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
    initCatch(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
    
    Serial.println("Setup complete!");
}

void loop() {
    // Detect state change and reinitialize games
    if (currentState != lastState) {
        switch (currentState) {
            case PONG:
                initPong(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
                break;
            case DODGER:
                initDodger(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
                break;
            case REACTION:
                initReaction(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
                break;
            case CATCH:
                initCatch(&display, TOUCH_NAV, TOUCH_SELECT, (void*)&currentState);
                break;
        }
        lastState = currentState;
    }
    
    switch (currentState) {
        case MENU:
            updateMenu();
            renderMenu();
            break;

        case PONG:
            updatePong();
            renderPong();
            break;

        case DODGER:
            updateDodger();
            renderDodger();
            break;

        case REACTION:
            updateReaction();
            renderReaction();
            break;

        case CATCH:
            updateCatch();
            renderCatch();
            break;
    }

    delay(20);
}