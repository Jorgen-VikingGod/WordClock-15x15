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

String getPower() { return String(power); }

String setPower(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (power != newValue) {
    power = newValue;
    if (power == 0) {
      lastMode = mode;
      mode = MODE_RESET;
    } else {
      mode = lastMode;
      if (mode == MODE_TIME) timeMode = TIME_UPDATE;
    }
  }
  return String(power);
}

String getDaylightSaving() { return String(daylightSaving); }

String setDaylightSaving(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (daylightSaving != newValue) {
    daylightSaving = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(daylightSaving);
}

String getMode() { return String(currentModeIndex); }

void setMode(uint8_t value) {
  uint8_t newValue = value >= modeCount ? modeCount - 1 : value;
  if (currentModeIndex != newValue) {
    currentModeIndex = newValue;
    mode = modes[currentModeIndex].value;
    if (mode == MODE_TIME) timeMode = TIME_UPDATE;
  }
}

String setMode(String value) {
  setMode(value.toInt());
  return String(currentModeIndex);
}

String getModes() {
  String json = "";

  for (uint8_t i = 0; i < modeCount; i++) {
    json += "\"" + modes[i].name + "\"";
    if (i < modeCount - 1) json += ",";
  }

  return json;
}

String getTourette() { return String(tourette); }

String setTourette(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (tourette != newValue) {
    tourette = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(tourette);
}

String getForegroundColor() {
  return String(foregroundColor.r) + "," + String(foregroundColor.g) + "," + String(foregroundColor.b);
}

String setForegroundColor(uint8_t r, uint8_t g, uint8_t b) {
  CRGB newValue = CRGB(r, g, b);
  if (foregroundColor != newValue) {
    foregroundColor = newValue;
    timeMode = TIME_UPDATE;
  }
  return "\"" + String(foregroundColor.r) + "," + String(foregroundColor.g) + "," + String(foregroundColor.b) + "\"";
}

String setForegroundColor(CRGB color) { return setForegroundColor(color.r, color.g, color.b); }

String setForegroundColor(String value) {
  CRGB color = parseColor(value);
  return setForegroundColor(color);
}

String getBrightness() { return String(brightness); }

String setBrightness(String value) {
  uint8_t newValue = value.toInt();
  if (brightness != newValue) {
    brightness = newValue;
    FastLED.setBrightness(brightness);
    timeMode = TIME_UPDATE;
  }
  return String(brightness);
}

String getAutoBrightness() { return String(autoBrightness); }

String setAutoBrightness(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (autoBrightness != newValue) {
    autoBrightness = newValue;
    if (autoBrightness) {
      lastBrightness = brightness;
    } else {
      brightness = lastBrightness;
      FastLED.setBrightness(brightness);
    }
    timeMode = TIME_UPDATE;
  }
  return String(autoBrightness);
}

String getBackgroundBrightness() { return String(backgroundBrightness); }

String setBackgroundBrightness(String value) {
  uint8_t newValue = value.toInt();
  if (backgroundBrightness != newValue) {
    backgroundBrightness = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(backgroundBrightness);
}

String getBackground() { return String(currentBackgroundIndex); }

void setBackground(uint8_t value) {
  uint8_t newValue = value >= backgroundCount ? backgroundCount - 1 : value;
  if (currentBackgroundIndex != newValue) {
    currentBackgroundIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setBackground(String value) {
  setBackground(value.toInt());
  return String(currentBackgroundIndex);
}

String getBackgrounds() {
  String json = "";

  for (uint8_t i = 0; i < backgroundCount; i++) {
    json += "\"" + backgrounds[i].name + "\"";
    if (i < backgroundCount - 1) json += ",";
  }

  return json;
}

String getNoisePaletteSpeedX() { return String(noisePaletteSpeedX); }

String setNoisePaletteSpeedX(String value) {
  uint8_t newValue = constrain(value.toInt(), 0, 24);
  if (noisePaletteSpeedX != newValue) {
    noisePaletteSpeedX = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(noisePaletteSpeedX);
}

String getNoisePaletteSpeedY() { return String(noisePaletteSpeedY); }

String setNoisePaletteSpeedY(String value) {
  uint8_t newValue = constrain(value.toInt(), 0, 24);
  if (noisePaletteSpeedY != newValue) {
    noisePaletteSpeedY = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(noisePaletteSpeedY);
}

String getNoisePaletteSpeedZ() { return String(noisePaletteSpeedZ); }

String setNoisePaletteSpeedZ(String value) {
  uint8_t newValue = constrain(value.toInt(), 0, 24);
  if (noisePaletteSpeedZ != newValue) {
    noisePaletteSpeedZ = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(noisePaletteSpeedZ);
}

String getBackgroundColor() {
  return String(backgroundColor.r) + "," + String(backgroundColor.g) + "," + String(backgroundColor.b);
}

String setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
  CRGB newValue = CRGB(r, g, b);
  if (backgroundColor != newValue) {
    backgroundColor = newValue;
    timeMode = TIME_UPDATE;
  }
  return "\"" + String(backgroundColor.r) + "," + String(backgroundColor.g) + "," + String(backgroundColor.b) + "\"";
}

String setBackgroundColor(CRGB color) { return setBackgroundColor(color.r, color.g, color.b); }

String setBackgroundColor(String value) {
  CRGB color = parseColor(value);

  return setBackgroundColor(color);
}

String getPalette() { return String(currentPaletteIndex); }

String setPalette(String value) {
  uint8_t newValue = constrain(value.toInt(), 0, paletteCount);
  if (newValue >= paletteCount) newValue = paletteCount - 1;
  if (currentPaletteIndex != newValue) {
    currentPaletteIndex = newValue;
    timeMode = TIME_UPDATE;
  }
  targetPalette = palettes[currentPaletteIndex];
  return String(currentPaletteIndex);
}

String getPalettes() {
  String json = "";

  for (uint8_t i = 0; i < paletteCount; i++) {
    json += "\"" + paletteNames[i] + "\"";
    if (i < paletteCount - 1) json += ",";
  }

  return json;
}

String getCyclePalettes() { return String(cyclePalettes); }

String setCyclePalettes(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (cyclePalettes != newValue) {
    cyclePalettes = newValue;
    paletteTimeout = millis() + (paletteDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(cyclePalettes);
}

String getPaletteDuration() { return String(paletteDuration); }

String setPaletteDuration(String value) {
  uint8_t newValue = constrain(value.toInt(), 1, 255);
  if (paletteDuration != newValue) {
    paletteDuration = newValue;
    paletteTimeout = millis() + (paletteDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(paletteDuration);
}

String getColorEffect() { return String(currentColorEffectIndex); }

void setColorEffect(uint8_t value) {
  uint8_t newValue = value >= colorEffectCount ? colorEffectCount - 1 : value;
  if (currentColorEffectIndex != newValue) {
    currentColorEffectIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setColorEffect(String value) {
  setColorEffect(value.toInt());
  return String(currentColorEffectIndex);
}

String getColorEffects() {
  String json = "";

  for (uint8_t i = 0; i < colorEffectCount; i++) {
    json += "\"" + colorEffects[i] + "\"";
    if (i < colorEffectCount - 1) json += ",";
  }

  return json;
}

String getTouretteMode() { return String(currentTouretteModeIndex); }

void setTouretteMode(uint8_t value) {
  uint8_t newValue = value >= touretteModeCount ? touretteModeCount - 1 : value;
  if (currentTouretteModeIndex != newValue) {
    currentTouretteModeIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setTouretteMode(String value) {
  setTouretteMode(value.toInt());
  return String(currentTouretteModeIndex);
}

String getTouretteModes() {
  String json = "";
  for (uint8_t i = 0; i < touretteModeCount; i++) {
    json += "\"" + touretteModes[i] + "\"";
    if (i < touretteModeCount - 1) json += ",";
  }
  return json;
}

String getRandomTouretteMode() { return String(randomTouretteMode); }

String setRandomTouretteMode(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (randomTouretteMode != newValue) {
    randomTouretteMode = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(randomTouretteMode);
}

String getCycleTouretteMode() { return String(cycleTouretteMode); }

String setCycleTouretteMode(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (cycleTouretteMode != newValue) {
    cycleTouretteMode = newValue;
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(cycleTouretteMode);
}

String getTouretteCycleDuration() { return String(touretteCycleDuration); }

String setTouretteCycleDuration(String value) {
  uint8_t newValue = constrain(value.toInt(), 1, 255);
  if (touretteCycleDuration != newValue) {
    touretteCycleDuration = newValue;
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(touretteCycleDuration);
}

String getTouretteStartWord() { return String(currentTouretteStartIndex); }

void setTouretteStartWord(uint8_t value) {
  uint8_t newValue = value >= touretteStartWordCount ? touretteStartWordCount - 1 : value;
  if (currentTouretteStartIndex != newValue) {
    currentTouretteStartIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setTouretteStartWord(String value) {
  setTouretteStartWord(value.toInt());
  return String(currentTouretteStartIndex);
}

String getTouretteStartWords() {
  String json = "";
  for (uint8_t i = 0; i < touretteStartWordCount; i++) {
    json += "\"" + touretteStartWords[i].name + "\"";
    if (i < touretteStartWordCount - 1) json += ",";
  }
  return json;
}

String getRandomTouretteStart() { return String(randomTouretteStart); }

String setRandomTouretteStart(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (randomTouretteStart != newValue) {
    randomTouretteStart = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(randomTouretteStart);
}

String getCycleTouretteStart() { return String(cycleTouretteStart); }

String setCycleTouretteStart(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (cycleTouretteStart != newValue) {
    cycleTouretteStart = newValue;
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(cycleTouretteStart);
}

String getTouretteMiddleWord() { return String(currentTouretteMiddleIndex); }

void setTouretteMiddleWord(uint8_t value) {
  uint8_t newValue = value >= touretteMiddleWordCount ? touretteMiddleWordCount - 1 : value;
  if (currentTouretteMiddleIndex != newValue) {
    currentTouretteMiddleIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setTouretteMiddleWord(String value) {
  setTouretteMiddleWord(value.toInt());
  return String(currentTouretteMiddleIndex);
}

String getTouretteMiddleWords() {
  String json = "";

  for (uint8_t i = 0; i < touretteMiddleWordCount; i++) {
    json += "\"" + touretteMiddleWords[i].name + "\"";
    if (i < touretteMiddleWordCount - 1) json += ",";
  }

  return json;
}

String getRandomTouretteMiddle() { return String(randomTouretteMiddle); }

String setRandomTouretteMiddle(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (randomTouretteMiddle != newValue) {
    randomTouretteMiddle = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(randomTouretteMiddle);
}

String getCycleTouretteMiddle() { return String(cycleTouretteMiddle); }

String setCycleTouretteMiddle(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (cycleTouretteMiddle != newValue) {
    cycleTouretteMiddle = newValue;
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(cycleTouretteMiddle);
}

String getTouretteEndWord() { return String(currentTouretteEndIndex); }

void setTouretteEndWord(uint8_t value) {
  uint8_t newValue = value >= touretteEndWordCount ? touretteEndWordCount - 1 : value;
  if (currentTouretteEndIndex != newValue) {
    currentTouretteEndIndex = newValue;
    timeMode = TIME_UPDATE;
  }
}

String setTouretteEndWord(String value) {
  setTouretteEndWord(value.toInt());
  return String(currentTouretteEndIndex);
}

String getTouretteEndWords() {
  String json = "";

  for (uint8_t i = 0; i < touretteEndWordCount; i++) {
    json += "\"" + touretteEndWords[i].name + "\"";
    if (i < touretteEndWordCount - 1) json += ",";
  }

  return json;
}

String getRandomTouretteEnd() { return String(randomTouretteEnd); }

String setRandomTouretteEnd(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (randomTouretteEnd != newValue) {
    randomTouretteEnd = newValue;
    timeMode = TIME_UPDATE;
  }
  return String(randomTouretteEnd);
}

String getCycleTouretteEnd() { return String(cycleTouretteEnd); }

String setCycleTouretteEnd(String value) {
  uint8_t newValue = value.toInt() == 0 ? 0 : 1;
  if (cycleTouretteEnd != newValue) {
    cycleTouretteEnd = newValue;
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
    timeMode = TIME_UPDATE;
  }
  return String(cycleTouretteEnd);
}

const SectionField sectionGlobal{"Global", "col-sm-12", "shadow-sm"};
const SectionField sectionForeground{"Foreground", "col-xl-4 col-lg-6", "border-primary shadow-sm"};
const SectionField sectionBackground{"Background", "col-xl-4 col-lg-6", "border-success shadow-sm"};
const SectionField sectionTourette{"Tourette", "col-xl-4 col-lg-6", "border-secondary shadow-sm"};

FieldList fields = {
    {sectionGlobal, "power", "Power", BooleanFieldType, 0, 1, getPower, NULL, setPower},
    {sectionGlobal, "tourette", "Tourette", BooleanFieldType, 0, 1, getTourette, NULL, setTourette},
    {sectionGlobal, "daylight", "Daylight Saving", BooleanFieldType, 0, 1, getDaylightSaving, NULL, setDaylightSaving},
    {sectionGlobal, "autoBrightness", "Auto Brightness", BooleanFieldType, 0, 1, getAutoBrightness, NULL,
     setAutoBrightness},
    {sectionGlobal, "brightness", "Brightness", NumberFieldType, 1, 255, getBrightness, NULL, setBrightness},

    {sectionForeground, "colorEffect", "Color Effect", SelectFieldType, 0, colorEffectCount, getColorEffect,
     getColorEffects, setColorEffect},
    {sectionForeground, "foregroundColor", "Color", ColorFieldType, 0, 255, getForegroundColor, NULL,
     setForegroundColor},

    {sectionBackground, "background", "Background", SelectFieldType, 0, backgroundCount, getBackground, getBackgrounds,
     setBackground},
    {sectionBackground, "backgroundBrightness", "Brightness", NumberFieldType, 1, 255, getBackgroundBrightness, NULL,
     setBackgroundBrightness},

    {sectionBackground, "noisePaletteSpeedX", "Speed X", NumberFieldType, 0, 24, getNoisePaletteSpeedX, NULL,
     setNoisePaletteSpeedX},
    {sectionBackground, "noisePaletteSpeedY", "Speed Y", NumberFieldType, 0, 24, getNoisePaletteSpeedY, NULL,
     setNoisePaletteSpeedY},
    {sectionBackground, "noisePaletteSpeedZ", "Speed Z", NumberFieldType, 0, 24, getNoisePaletteSpeedZ, NULL,
     setNoisePaletteSpeedZ},

    {sectionBackground, "backgroundColor", "Color", ColorFieldType, 0, 255, getBackgroundColor, NULL,
     setBackgroundColor},

    {sectionBackground, "palette", "Palette", SelectFieldType, 0, paletteCount, getPalette, getPalettes, setPalette},
    {sectionBackground, "cyclePalettes", "Cycle Palettes", BooleanFieldType, 0, 1, getCyclePalettes, NULL,
     setCyclePalettes},
    {sectionBackground, "paletteDuration", "Palette Duration", NumberFieldType, 1, 255, getPaletteDuration, NULL,
     setPaletteDuration},

    {sectionTourette, "touretteModes", "Tourette Mode", SelectFieldType, 0, touretteModeCount, getTouretteMode,
     getTouretteModes, setTouretteMode},
    {sectionTourette, "randomTouretteMode", "Random Cycle", BooleanFieldType, 0, 1, getRandomTouretteMode, NULL,
     setRandomTouretteMode},
    {sectionTourette, "cycleTouretteMode", "Cycle Mode", BooleanFieldType, 0, 1, getCycleTouretteMode, NULL,
     setCycleTouretteMode},
    {sectionTourette, "touretteCycleDuration", "Cycle Duration", NumberFieldType, 1, 255, getTouretteCycleDuration,
     NULL, setTouretteCycleDuration},

    {sectionTourette, "touretteStartWords", "Start Word", SelectFieldType, 0, touretteStartWordCount,
     getTouretteStartWord, getTouretteStartWords, setTouretteStartWord},
    {sectionTourette, "randomTouretteStart", "Random Start", BooleanFieldType, 0, 1, getRandomTouretteStart, NULL,
     setRandomTouretteStart},
    {sectionTourette, "cycleTouretteStart", "Cycle Start", BooleanFieldType, 0, 1, getCycleTouretteStart, NULL,
     setCycleTouretteStart},

    {sectionTourette, "touretteMiddleWords", "Middle Word", SelectFieldType, 0, touretteMiddleWordCount,
     getTouretteMiddleWord, getTouretteMiddleWords, setTouretteMiddleWord},
    {sectionTourette, "randomTouretteMiddle", "Random Middle", BooleanFieldType, 0, 1, getRandomTouretteMiddle, NULL,
     setRandomTouretteMiddle},
    {sectionTourette, "cycleTouretteMiddle", "Cycle Middle", BooleanFieldType, 0, 1, getCycleTouretteMiddle, NULL,
     setCycleTouretteMiddle},

    {sectionTourette, "touretteEndWords", "End Word", SelectFieldType, 0, touretteEndWordCount, getTouretteEndWord,
     getTouretteEndWords, setTouretteEndWord},
    {sectionTourette, "randomTouretteEnd", "Random End", BooleanFieldType, 0, 1, getRandomTouretteEnd, NULL,
     setRandomTouretteEnd},
    {sectionTourette, "cycleTouretteEnd", "Cycle End", BooleanFieldType, 0, 1, getCycleTouretteEnd, NULL,
     setCycleTouretteEnd},
};

uint8_t fieldCount = ARRAY_SIZE(fields);
