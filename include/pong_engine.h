#ifndef PONG_ENGINE_H
#define PONG_ENGINE_H

#include <Adafruit_SSD1306.h>

void initPong(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr);
void updatePong();
void renderPong();

#endif