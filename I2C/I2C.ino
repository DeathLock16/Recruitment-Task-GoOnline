#include <Arduino.h>
#include <PCA9633.h> // https://github.com/HendrikVE/Arduino-PCA9633

#define PCA9633_ADDRESS 0x62 // I2C address of PCA9633

const uint8_t colorSteps = 100;  // Number of steps for the color transition
const uint16_t transitionDelay = 10;  // Delay in milliseconds between each step

PCA9633 pca9633 = PCA9633(REG_PWM2, REG_PWM1, REG_PWM0);

float lightness = 1.00;

const uint32_t rainbowColors[][4] = {
  0xFF0000FF, // Red
  0xFFFF00FF, // Yellow
  0x00FF00FF, // Green
  0x00FFFFFF, // Cyan
  0x0000FFFF, // Blue
  0xFF00FFFF  // Magenta
};

void setLEDColor(uint32_t _color) {
  uint8_t red = (_color >> 24) & 0xFF;
  uint8_t green = (_color >> 16) & 0xFF;
  uint8_t blue = (_color >> 8) & 0xFF;
  uint8_t white = _color & 0xFF;

  white = static_cast<uint8_t>(white * lightness);

  pca9633.setRGBW(red, green, blue, white);
}

void smoothColorTransition(uint32_t _current, uint32_t _transition) {
  uint8_t rCurr = (_current >> 24) & 0xFF;
  uint8_t gCurr = (_current >> 16) & 0xFF;
  uint8_t bCurr = (_current >> 8) & 0xFF;
  uint8_t wCurr = _current & 0xFF;

  uint8_t rTran = (_transition >> 24) & 0xFF;
  uint8_t gTran = (_transition >> 16) & 0xFF;
  uint8_t bTran = (_transition >> 8) & 0xFF;
  uint8_t wTran = _transition & 0xFF;

  for (uint8_t step = 0; step <= colorSteps; step++) {
    uint8_t r = map(step, 0, colorSteps, rCurr, rTran);
    uint8_t g = map(step, 0, colorSteps, gCurr, gTran);
    uint8_t b = map(step, 0, colorSteps, bCurr, bTran);
    uint8_t w = map(step, 0, colorSteps, wCurr, wTran);

    pca9633.setRGBW(r, g, b, w);
    lightness > 0.00 ? lightness -= 0.01 : lightness = 1.00; // Gradually change lightness from 1 to 0.0 resets to 1.0
    delay(transitionDelay);
  }
}

void setup() {
  Serial.begin(115200);
  pca9633.begin(PCA9633_ADDRESS, &Wire);
  pca9633.setRGBW(255, 0, 0, 255);
}

void loop() {
  for (uint8_t i = 0; i < sizeof(rainbowColors) / sizeof(rainbowColors[0]) - 1; i++) {
    uint32_t color1 = rainbowColors[i];
    uint32_t color2 = rainbowColors[i + 1];
    smoothColorTransition(color1, color2);
  }
}