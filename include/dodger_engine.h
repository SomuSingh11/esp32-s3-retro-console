#ifndef DODGER_ENGINE_H
#define DODGER_ENGINE_H

#include <Adafruit_SSD1306.h>

void initDodger(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr);
void updateDodger();
void renderDodger();

#endif