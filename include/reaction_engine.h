#ifndef REACTION_ENGINE_H
#define REACTION_ENGINE_H

#include <Adafruit_SSD1306.h>

void initReaction(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr);
void updateReaction();
void renderReaction();

#endif