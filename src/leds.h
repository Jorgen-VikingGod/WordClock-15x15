/*
 * WordClock-15x15: https://github.com/Jorgen-VikingGod/WordClock-15x15
 * Copyright (C) 2019 Juergen Skrotzky alias Jorgen (JorgenVikingGod@gmail.com)
 *
 * This sketch lights up a 15x15 RGB LED wordclock.
 * It shows the time and some special words / symbols
 * you can also use differnet text color and background animations (like plasma,
 * fire, ...) The wordclock is fully controllable over WiFi by MQTT.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "layout.h"
#include <cstdarg>
#include <vector>

uint8_t gHue = 0; // rotating "base color"
uint8_t mHue = 0;

uint8_t currentColorEffectIndex = 2;
uint8_t lastColorEffectIndex = currentColorEffectIndex;

const String colorEffects[] = {
    "Solid",
    "Random",
    "Hue",
};
const uint8_t colorEffectCount = ARRAY_SIZE(colorEffects);

CRGB backgroundColor = CRGB(0, 0, 0);
CRGB foregroundColor = CRGB(255, 255, 255);
CRGB lastForegroundColor = foregroundColor;

void resetDots() { fill_solid(minuteLEDs, 16, CRGB(0, 0, 0)); }
void resetFront() { fill_solid(ledsFront[0], ledsFront.Size(), CRGB(0, 0, 0)); }
void resetBack() { fill_solid(ledsBack[0], ledsBack.Size(), CRGB(0, 0, 0)); }

void showSolidColor() {
  fill_solid(ledsBack[0], ledsBack.Size(), backgroundColor);
}

CRGB brighter(const CRGB &color, fract8 offset) {
  CRGB incrementalColor = color;
  incrementalColor.nscale8(offset);
  return color + incrementalColor;
}

CRGB darker(const CRGB &color, fract8 offset) {
  CRGB newcolor = color;
  newcolor.nscale8(255 - offset);
  return newcolor;
}

void blitLeds() {
  for (int16_t idx = 0; idx < leds.Size(); idx++) {
    yield();
    if (ledsFront(idx) != CRGB(0, 0, 0)) {
      leds(idx) = ledsFront(idx);
    } else {
      leds(idx) = ledsBack(idx);
    }
  }
}

void drawDots(int dots, CRGB customColor = CRGB(0, 0, 0)) {
  CRGB color;
  String colorEffect = colorEffects[currentColorEffectIndex];
  for (int dot = 0; dot < dots - 1; dot++) {
    yield();
    mHue += 20;
    if (customColor != CRGB(0, 0, 0)) {
      color = customColor;
    } else if (colorEffect == "Solid") {
      color = foregroundColor;
    } else if (colorEffect == "Random") {
      color = CHSV(random(0, 255), 255, 255);
    } else if (colorEffect == "Hue") {
      color = CHSV(gHue, 255, 255);
    }
    minuteLEDs[dot] = color;
  }
}

CRGB drawItem(vLayoutItems layoutItemList) {
  bool customColor = false;
  CRGB color;
  String colorEffect = colorEffects[currentColorEffectIndex];
  for (int i = 0; i < layoutItemList.size(); i++) {
    yield();
    sLayoutItem item = layoutItemList[i];
    // cycle hue for every new word
    if (item.customColor) {
      customColor = true;
      color.r = item.r;
      color.g = item.g;
      color.b = item.b;
      continue;
    }
    if (item.newWord) {
      gHue += 40;
      continue;
    }
    // handle color
    if (!customColor && colorEffect == "Solid") {
      color = foregroundColor;
    } else if (!customColor && colorEffect == "Random") {
      color = CHSV(random(0, 255), 255, 255);
    } else if (!customColor && colorEffect == "Hue") {
      color = CHSV(gHue, 255, 255);
    }
    // draw pixel or line
    if (item.pixel) {
      ledsFront(item.x, item.y) = color;
    } else if (item.line) {
      ledsFront.DrawLine(item.x1, item.y1, item.x2, item.y2, color);
    }
  } // for(int i = 0; i < size; i++)
  return color;
}

void showWiFiSymbol() {
  Serial.println("showWiFiSymbol()");
  resetFront();
  drawItem(GRAPHIC_WIFI_SYMBOL);
  blitLeds();
}