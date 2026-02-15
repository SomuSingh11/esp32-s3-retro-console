#ifndef CATCH_ENGINE_H
#define CATCH_ENGINE_H

#include <Adafruit_SSD1306.h>

void initCatch(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr);
void updateCatch();
void renderCatch();

#endif