# ESP32-S3 OLED Retro Game Console
![esp32](https://github.com/user-attachments/assets/32f21360-959e-473a-9ea4-dfb8cc68955a)

A simple handheld game console built while learning ESP32-S3 and SSD1306 OLED displays. Features 4 classic games with 2-button controls.

## 🎮 Games

1. **Pong** - Classic paddle game vs CPU
2. **Dodger** - Avoid falling obstacles
3. **Reaction Test** - Test your reflexes
4. **Catch** - Catch good items, avoid bad ones

## 🛠️ Hardware

- **ESP32-S3 DevKit** (ESP32-S3-DevKitM-1)
- **SSD1306 OLED Display** (128x64, I2C)
- **2x Touch Sensors** or Push Buttons
- Breadboard and jumper wires

## 🔌 Connections

```
ESP32-S3          Component
─────────────────────────────
GPIO 8 (SDA)  →   OLED SDA
GPIO 9 (SCL)  →   OLED SCL
GPIO 4        →   NAV Button
GPIO 5        →   SELECT Button
3.3V          →   OLED VCC
GND           →   OLED GND, Buttons GND
```

## 🎯 Controls

**Menu:**
- NAV Button: Cycle through games
- SELECT Button: Launch game

**In Games:**
- NAV: Move player/paddle
- SELECT: Exit to menu (or fire/start depending on game)

## 📦 Setup

1. Install [PlatformIO](https://platformio.org/)
2. Clone this repository
3. Open in PlatformIO
4. Upload to ESP32-S3

**Libraries used:**
- Adafruit SSD1306
- Adafruit GFX Library

## 🤖 Credits

This project was created as a learning experience with ESP32-S3 and OLED displays. Code and game logic were developed with AI assistance.

## 📝 License

MIT License - Free to use and modify
