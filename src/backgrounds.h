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

#include "leds.h"
#include "palettes.h"

#define TARGET_FRAME_TIME 25

uint16_t plasmaTime;
uint16_t plasmaShift;
uint32_t loopDelayMS = 60;
uint32_t lastLoop;

// The 16 bit version of our coordinates
uint16_t noisex;
uint16_t noisey;
uint16_t noisez;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.
// We'll use the z-axis for "time".  speed determines how fast time moves
// forward.  Try 1 for a very slow moving effect, or 60 for something that
// ends up looking like water.
uint32_t noisespeedx;
uint32_t noisespeedy;
uint32_t noisespeedz;

uint8_t noisePaletteSpeedX = 9;
uint8_t noisePaletteSpeedY = 0;
uint8_t noisePaletteSpeedZ = 0;

uint8_t backgroundBrightness = 64;

uint8_t currentBackgroundIndex = 3;
uint8_t lastBackgroundIndex = currentBackgroundIndex;

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the
// display.  The higher the value of scale, the more "zoomed out" the noise
// will be.  A value of 1 will be so zoomed in, you'll mostly see solid
// colors.
uint16_t noisescale;
// This is the array that we keep our computed noise values in
uint8_t noise[maxDimension][maxDimension];
uint8_t colorLoop;
bool initialized;

void plasma() {
  if (abs(millis() - lastLoop) >= loopDelayMS) {
    lastLoop = millis();
    resetBack();
// Fill background with dim plasma
#define PLASMA_X_FACTOR 24
#define PLASMA_Y_FACTOR 24
    for (int16_t x = 0; x < width; x++) {
      for (int16_t y = 0; y < height; y++) {
        int16_t r = sin16(plasmaTime) / 256;
        int16_t h = sin16(x * r * PLASMA_X_FACTOR + plasmaTime) +
                    cos16(y * (-r) * PLASMA_Y_FACTOR + plasmaTime) +
                    sin16(y * x * (cos16(-plasmaTime) / 256) / 2);
        ledsBack.DrawPixel(
            x, y, CHSV((uint8_t)((h / 256) + 128), 255, backgroundBrightness));
      }
    }
    uint16_t OldPlasmaTime = plasmaTime;
    plasmaTime += plasmaShift;
    if (OldPlasmaTime > plasmaTime)
      plasmaShift = (random8(0, 5) * 32) + 64;
  }
}

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  if (!initialized) {
    initialized = true;
    // Initialize our coordinates to some random values
    noisex = random16();
    noisey = random16();
    noisez = random16();
  }

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast
  // data-smoothing. The amount of data smoothing we're doing depends on
  // "speed".
  uint8_t dataSmoothing = 0;
  uint16_t lowestNoise = noisespeedx < noisespeedy ? noisespeedx : noisespeedy;
  lowestNoise = lowestNoise < noisespeedz ? lowestNoise : noisespeedz;
  if (lowestNoise < 8) {
    dataSmoothing = 200 - (lowestNoise * 4);
  }

  for (int i = 0; i < maxDimension; i++) {
    int ioffset = noisescale * i;
    for (int j = 0; j < maxDimension; j++) {
      yield();
      int joffset = noisescale * j;
      uint8_t data = inoise8(noisex + ioffset, noisey + joffset, noisez);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata =
            scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i][j] = data;
    }
  }

  noisex += noisespeedx;
  noisey += noisespeedy;
  noisez += noisespeedz;
}

void mapNoiseToLEDsUsingPalette(CRGBPalette16 palette, uint8_t hueReduce) {
  static uint8_t ihue = 0;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      yield();
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.
      uint8_t index = noise[j][i];
      uint8_t bri = noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if (colorLoop) {
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if (bri > 127) {
        bri = 255;
      } else {
        bri = dim8_raw(bri * 2);
      }
      bri = map(bri, 0, 255, 0, backgroundBrightness);

      if (hueReduce > 0) {
        if (index < hueReduce)
          index = 0;
        else
          index -= hueReduce;
      }

      CRGB color = ColorFromPalette(palette, index, bri);
      // ledsBack(i, j) = color;
      ledsBack.DrawPixel(i, j, color);
      // ledsBack->drawPixel(i, j, color);
    }
  }

  ihue += 1;
}

uint16_t drawNoise(const CRGBPalette16 &palette, uint8_t hueReduce = 0) {
  // generate noise data
  fillnoise8();

  // convert the noise data to colors in the LED array
  // using the current palette
  mapNoiseToLEDsUsingPalette(palette, hueReduce);

  return 10;
}

void paletteNoise() {
  noisespeedx = noisePaletteSpeedX;
  noisespeedy = noisePaletteSpeedY;
  noisespeedz = noisePaletteSpeedZ;
  noisescale = 30;
  colorLoop = 0;
  const CRGBPalette16 palette = palettes[currentPaletteIndex];
  drawNoise(palette);
}

typedef void (*Background)();
typedef Background BackgroundList[];
typedef struct {
  Background background;
  String name;
} BackgroundAndName;
typedef BackgroundAndName BackgroundAndNameList[];

BackgroundAndNameList backgrounds = {
    {resetBack, "Black"},
    {showSolidColor, "Solid Color"},
    {plasma, "Plasma"},
    {paletteNoise, "Palette Noise"},
};

const uint8_t backgroundCount = ARRAY_SIZE(backgrounds);
