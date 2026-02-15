#ifndef MENU_ENGINE_H
#define MENU_ENGINE_H

#include <Adafruit_SSD1306.h>

void initMenu(Adafruit_SSD1306* disp, int navPin, int selectPin, void* statePtr);
void updateMenu();
void renderMenu();

#endif