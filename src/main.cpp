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

#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FS.h>

#include <SPI.h>
#include <SPIFFS.h>

#include <Arduino.h>

//#include "esp_adc_cal.h"
#include <WiFi.h>

#include <Button2.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <FastLED.h>
#include <LEDMatrix.h>
#include <WebSocketsServer.h>

#include <BH1750.h> //https://github.com/claws/BH1750
#include <Wire.h>

#define I2C_SCL 22//SCL
#define I2C_SDA 21//SDA

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001008)
#warning "Requires FastLED 3.1.8 or later; check github for latest code."
#endif

Button2 btn1(BUTTON_1);
int btnPortal = false;

BH1750 lightSensor;

AsyncWebServer webServer(80);
WebSocketsServer webSocketsServer = WebSocketsServer(81);
DNSServer dns;

// const int LED_BUILTIN = 2;

uint8_t pHue = 0; // rotating "base color" used by many of the patterns

uint8_t power = 1;
uint8_t lastPower = power;

uint8_t brightness = 150;
uint8_t lastBrightness = brightness;

uint8_t daylightSaving = 0;
uint8_t lastDaylightSaving = daylightSaving;

uint8_t autoBrightness = 1;
uint8_t lastAutoBrightness = autoBrightness;

uint8_t tourette = 0;
uint8_t lastTourette = tourette;

uint8_t timeMinute = 0;
uint8_t timeHour = 0;
CRGB minuteColor = CRGB::White;


#define MATRIX_TILE_WIDTH   3 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  3 // height of each matrix
#define MATRIX_TILE_H       5  // number of matrices arranged horizontally
#define MATRIX_TILE_V       5  // number of matrices arranged vertically
#define MATRIX_WIDTH        (MATRIX_TILE_WIDTH*MATRIX_TILE_H)
#define MATRIX_HEIGHT       (MATRIX_TILE_HEIGHT*MATRIX_TILE_V)
#define MATRIX_SIZE         (MATRIX_WIDTH*MATRIX_HEIGHT)
#define MATRIX_PANEL        (MATRIX_WIDTH*MATRIX_HEIGHT)
#define NUM_LEDS            (MATRIX_WIDTH*MATRIX_HEIGHT)

// create our matrix based on matrix definition
cLEDMatrix<MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> leds;
cLEDMatrix<MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> ledsFront;
cLEDMatrix<MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> ledsBack;


static const int width = 15;
static const int height = 15;
static const MatrixType_t matrixType = HORIZONTAL_ZIGZAG_MATRIX;
static const int size = (width * height);
static const int numPixels = size;
static const int maxDimension = ((width > height) ? width : height);

CRGB minuteLEDs[4];

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// pins tested so far on the Feather ESP32: 13, 12, 27, 33, 15, 32, 14, SCL
#define DATA_PIN 12
#define DATA_PIN_2 13
//#define CLK_PIN   4
#define CHIPSET WS2812B
#define COLOR_ORDER GRB
#define NUM_STRIPS 1
#define NUM_LEDS_PER_STRIP 16
#define NUM_LEDS NUM_LEDS_PER_STRIP *NUM_STRIPS
// CRGB leds[NUM_LEDS];

#define MILLI_AMPS                                                             \
  1000 // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define FRAMES_PER_SECOND 120

// -- The core to run FastLED.show()
#define FASTLED_SHOW_CORE 0

#include "backgrounds.h"
#include "words.h"

#include "field.h"
#include "fields.h"

#include "secrets.h"
#include "tft.h"
#include "web.h"
#include "wifi.h"

// wifi ssid and password should be added to a file in the sketch named
// secrets.h the secrets.h file should be added to the .gitignore file and never
// committed or pushed to public source control (GitHub). const char* ssid =
// "........"; const char* password = "........";

// -- Task handles for use in the notifications
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

/** show() for ESP32
    Call this function instead of FastLED.show(). It signals core 0 to issue a
   show, then waits for a notification that it is done.
*/
void FastLEDshowESP32() {
  if (userTaskHandle == 0) {
    // -- Store the handle of the current task, so that the show task can
    //    notify it when it's done
    userTaskHandle = xTaskGetCurrentTaskHandle();

    // -- Trigger the show task
    xTaskNotifyGive(FastLEDshowTaskHandle);

    // -- Wait to be notified that it's done
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200);
    ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    userTaskHandle = 0;
  }
}

/** show Task
    This function runs on core 0 and just waits for requests to call
   FastLED.show()
*/
void FastLEDshowTask(void *pvParameters) {
  // -- Run forever...
  for (;;) {
    // -- Wait for the trigger
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // -- Do the show (synchronously)
    FastLED.show();

    // -- Notify the calling task
    xTaskNotifyGive(userTaskHandle);
  }
}

float fscale(float originalMin, float originalMax, float newBegin, float newEnd,
             float inputValue, float curve) {
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  bool invFlag = 0;
  // condition curve parameter
  // limit range
  if (curve > 10) {
    curve = 10;
  }
  if (curve < -10) {
    curve = -10;
  }
  // - invert and scale - this seems more intuitive - postive
  // numbers give more weight to high end on output
  curve = (curve * -.1);
  // convert linear scale into lograthimic exponent for
  // other pow function
  curve = pow(10, curve);
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
  // Zero Refference the values
  OriginalRange = originalMax - originalMin;
  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  } else {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0 - 1 float
  // Check for originalMin > originalMax  - the math for all other cases i.e.
  // negative numbers seems to work out fine
  if (originalMin > originalMax) {
    return 0;
  }
  if (invFlag == 0) {
    rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
  } else {
    // invert the ranges
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }
  return rangedValue;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void nextPalette() {
  if (cyclePalettes == 1 && (millis() > paletteTimeout)) {
    currentPaletteIndex = (currentPaletteIndex + 1) % paletteCount;
    targetPalette = palettes[currentPaletteIndex];

    String json = "{\"name\":\"palette\",\"value\":\"" +
                  String(currentPaletteIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);

    paletteTimeout = millis() + (paletteDuration * 1000);
  }
}

void nextTouretteMode() {
  if (tourette == 0) {
    return;
  }
  if (cycleTouretteMode == 1 && (millis() > touretteCycleTimeout)) {
    // add one to the current mode number, and wrap around at the end
    if (randomTouretteMode) {
      currentTouretteModeIndex = random(touretteModeCount);
    } else {
      currentTouretteModeIndex =
          (currentTouretteModeIndex + 1) % touretteModeCount;
    }
    String json = "{\"name\":\"touretteModes\",\"value\":\"" +
                  String(currentTouretteModeIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
  }
}

void nextTouretteStart() {
  if (touretteModes[currentTouretteModeIndex] != "Start" || tourette == 0) {
    return;
  }
  if (cycleTouretteStart == 1 && (millis() > touretteCycleTimeout)) {
    // add one to the current start word number, and wrap around at the end
    if (randomTouretteStart) {
      currentTouretteStartIndex = random(touretteStartWordCount);
    } else {
      currentTouretteStartIndex =
          (currentTouretteStartIndex + 1) % touretteStartWordCount;
    }
    String json = "{\"name\":\"touretteStartWords\",\"value\":\"" +
                  String(currentTouretteStartIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
  }
}

void nextTouretteMiddle() {
  if (tourette == 0) {
    return;
  }
  if (cycleTouretteMiddle == 1 && (millis() > touretteCycleTimeout)) {
    // add one to the current middle word number, and wrap around at the end
    if (randomTouretteMiddle) {
      currentTouretteMiddleIndex = random(touretteMiddleWordCount);
    } else {
      currentTouretteMiddleIndex =
          (currentTouretteMiddleIndex + 1) % touretteMiddleWordCount;
    }
    String json = "{\"name\":\"touretteMiddleWords\",\"value\":\"" +
                  String(currentTouretteMiddleIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
  }
}

void nextTouretteEnd() {
  if (touretteModes[currentTouretteModeIndex] != "End" || tourette == 0) {
    return;
  }
  if (cycleTouretteEnd == 1 && (millis() > touretteCycleTimeout)) {
    // add one to the current end word number, and wrap around at the end
    if (randomTouretteEnd) {
      currentTouretteEndIndex = random(touretteEndWordCount);
    } else {
      currentTouretteEndIndex =
          (currentTouretteEndIndex + 1) % touretteEndWordCount;
    }
    String json = "{\"name\":\"touretteEndWords\",\"value\":\"" +
                  String(currentTouretteEndIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
    touretteCycleTimeout = millis() + (touretteCycleDuration * 1000);
  }
}

void setup() {
  delay(5000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  SPIFFS.begin();
  listDir(SPIFFS, "/", 1);

  /*
  Serial.println("clear EEPROM!");
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM cleared!");
  */
  // restore from memory
  loadFieldsFromEEPROM(fields, fieldCount);

  btn1.setLongClickHandler([](Button2 &b) { btnPortal = true; });

  setupTft();
  setupWifi();
  setupWeb();

  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, DATA_PIN_2, COLOR_ORDER>(minuteLEDs, 4).setCorrection(TypicalSMD5050);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);

  Serial.println("initial I2C BH1750 light sensor");
  Wire.begin(I2C_SDA, I2C_SCL);
  lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // set master brightness control
  FastLED.setBrightness(brightness);

  loopDelayMS = TARGET_FRAME_TIME;
  lastLoop = millis() - loopDelayMS;
  plasmaShift = (random8(0, 5) * 32) + 64;
  plasmaTime = 0;

  int core = xPortGetCoreID();
  Serial.print("Main code running on core ");
  Serial.println(core);

  // -- Create the FastLED show task
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2,
                          &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);
}

void handleMinuteDots(uint8_t timeMinute) {
  int dots = timeMinute % 5;
  Serial.print("dots: ");
  resetDots();
  if (dots > 0) {
    drawDots(dots, minuteColor);
    Serial.print("+");
    Serial.println(dots);
  } else {
    resetDots();
    Serial.println("0");
  }
}

bool updateTime = false;

bool updateFront() {
  return daylightSaving != lastDaylightSaving || power != lastPower ||
         currentColorEffectIndex != lastColorEffectIndex ||
         foregroundColor != lastForegroundColor || tourette != lastTourette ||
         currentBackgroundIndex != lastBackgroundIndex ||
         currentTouretteModeIndex != lastTouretteModeIndex ||
         currentTouretteStartIndex != lastTouretteStartIndex ||
         currentTouretteMiddleIndex != lastTouretteMiddleIndex ||
         currentTouretteEndIndex != lastTouretteEndIndex;
}

void handleTouretteMode() {
  if (randomTouretteMode && cycleTouretteMode == 0) {
    currentTouretteModeIndex = random(touretteModeCount);
    String json = "{\"name\":\"touretteModes\",\"value\":\"" +
                  String(currentTouretteModeIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
  }
  Serial.print("mode: ");
  Serial.println(touretteModes[currentTouretteModeIndex]);
}

void handleTouretteStart() {
  if (touretteModes[currentTouretteModeIndex] == "Start") {
    drawItem(WORD_HEY_DU);
    Serial.println("hey du");
    if (randomTouretteStart && cycleTouretteStart == 0) {
      currentTouretteStartIndex = random(touretteStartWordCount);
      String json = "{\"name\":\"touretteStartWords\",\"value\":\"" +
                    String(currentTouretteStartIndex) + "\"}";
      webSocketsServer.broadcastTXT(json);
    }
    touretteStartWords[currentTouretteStartIndex].drawWord();
    Serial.print("start: ");
    Serial.println(touretteStartWords[currentTouretteStartIndex].name);
  }
}

void handleTouretteMiddle() {
  if (randomTouretteMiddle && cycleTouretteMiddle == 0) {
    currentTouretteMiddleIndex = random(touretteMiddleWordCount);
    String json = "{\"name\":\"touretteMiddleWords\",\"value\":\"" +
                  String(currentTouretteMiddleIndex) + "\"}";
    webSocketsServer.broadcastTXT(json);
  }
  touretteMiddleWords[currentTouretteMiddleIndex].drawWord();
  Serial.print("middle: ");
  Serial.println(touretteMiddleWords[currentTouretteMiddleIndex].name);
}

void handleTouretteEnd() {
  if (touretteModes[currentTouretteModeIndex] == "End") {
    drawItem(WORD_DU);
    Serial.println("du");
    if (randomTouretteEnd && cycleTouretteEnd == 0) {
      currentTouretteEndIndex = random(touretteEndWordCount);
      String json = "{\"name\":\"touretteEndWords\",\"value\":\"" +
                    String(currentTouretteEndIndex) + "\"}";
      webSocketsServer.broadcastTXT(json);
    }
    touretteEndWords[currentTouretteEndIndex].drawWord();
    Serial.print("end: ");
    Serial.println(touretteEndWords[currentTouretteEndIndex].name);
  }
}

void handleMinutes(uint8_t timeMin) {
  Serial.print("minute: ");
  Serial.print(timeMin);
  if (timeMin >= 55) {
    minuteColor = drawItem(WORD_M_55);
    Serial.println(" - fünf vor");
  } else if (timeMin >= 50) {
    minuteColor = drawItem(WORD_M_50);
    Serial.println(" - zehn vor");
  } else if (timeMin >= 45) {
    minuteColor = drawItem(WORD_M_45);
    Serial.println(" - dreiviertel");
  } else if (timeMin >= 40) {
    minuteColor = drawItem(WORD_M_40);
    Serial.println(" - zwanzig vor");
  } else if (timeMin >= 35) {
    minuteColor = drawItem(WORD_M_35);
    Serial.println(" - fünf nach halb");
  } else if (timeMin >= 30) {
    minuteColor = drawItem(WORD_M_30);
    Serial.println(" - halb");
  } else if (timeMin >= 25) {
    minuteColor = drawItem(WORD_M_25);
    Serial.println(" - fünf vor halb");
  } else if (timeMin >= 20) {
    minuteColor = drawItem(WORD_M_20_NACH);
    Serial.println(" - zwanzig nach");
  } else if (timeMin >= 15) {
    minuteColor = drawItem(WORD_M_15_NACH);
    Serial.println(" - viertel nach");
  } else if (timeMin >= 10) {
    minuteColor = drawItem(WORD_M_10_NACH);
    Serial.println(" - zehn nach");
  } else if (timeMin >= 5) {
    minuteColor = drawItem(WORD_M_05_NACH);
    Serial.println(" - fünf nach");
  } else {
    Serial.println("");
  }
}

void handleHours(uint8_t timeMin, uint8_t timeHrs) {
  Serial.print("hour: ");
  Serial.print(timeHrs);
  switch (timeHrs) {
  case 0:
    drawItem(WORD_H_ZWOELF);
    Serial.println(" - zwoelf");
    break;
  case 1:
    if (timeMin < 5) {
      drawItem(WORD_H_EIN);
      Serial.println(" - ein");
    } else {
      drawItem(WORD_H_EINS);
      Serial.println(" - eins");
    }
    break;
  case 2:
    drawItem(WORD_H_ZWEI);
    Serial.println(" - zwei");
    break;
  case 3:
    drawItem(WORD_H_DREI);
    Serial.println(" - drei");
    break;
  case 4:
    drawItem(WORD_H_VIER);
    Serial.println(" - vier");
    break;
  case 5:
    drawItem(WORD_H_FUENF);
    Serial.println(" - fünf");
    break;
  case 6:
    drawItem(WORD_H_SECHS);
    Serial.println(" - sechs");
    break;
  case 7:
    drawItem(WORD_H_SIEBEN);
    Serial.println(" - sieben");
    break;
  case 8:
    drawItem(WORD_H_ACHT);
    Serial.println(" - acht");
    break;
  case 9:
    drawItem(WORD_H_NEUN);
    Serial.println(" - neun");
    break;
  case 10:
    drawItem(WORD_H_ZEHN);
    Serial.println(" - zehn");
    break;
  case 11:
    drawItem(WORD_H_ELF);
    Serial.println(" - elf");
    break;
  } // switch(timeHrs) {
}

void loop() {
  handleWeb();

  if (power == 0) {
    resetFront();
    resetBack();
  } else {

    if (autoBrightness) {
      EVERY_N_MILLISECONDS(5000) {
        uint16_t lux = lightSensor.readLightLevel();
        Serial.print("lux: ");
        Serial.print(lux);
        Serial.print(" lux log: ");
        brightness = fscale(0, 3000, 20, 255, lux, 5);
        Serial.println(brightness);
        FastLED.setBrightness(brightness);
      }
    }

    // Call the current pattern function once, updating the 'leds' array
    backgrounds[currentBackgroundIndex].background();

    uint8_t currentMinute = ntpClient.minutes();
    uint8_t currentHour = ntpClient.hours();
    if (timeMinute != currentMinute && timeHour != currentHour) {
      timeMinute = currentMinute;
      timeHour = currentHour;
      updateTime = true;
    }
    if (updateFront()) {
      updateTime = true;
      timeMinute = currentMinute;
      timeHour = currentHour;
    }
    if (updateTime) {
      resetFront();
      if (tourette == 0) {
        drawItem(WORD_ES_IST);
        Serial.println("es ist");
      } else {
        handleTouretteMode();
        handleTouretteStart();
        drawItem(WORD_WIR_HABEN);
        Serial.println("wir haben");
        handleTouretteMiddle();
      }
      // show minutes
      handleMinutes(timeMinute);
      // time logic (modify hours depending on current time)
      if (timeMinute > 24) {
        timeHour++;
      }
      if (timeHour >= 12) {
        timeHour -= 12;
      }
      if (timeHour == 12) {
        timeHour = 0;
      }
      // show hours
      handleHours(timeMinute, timeHour);
      // show 'UHR' (o' clock)
      if (timeMinute < 5) {
        drawItem(WORD_UHR);
        Serial.println("uhr");
      }
      if (tourette == 1) {
        handleTouretteEnd();
      }
      // calculate minutes (=dots) from 1-4 (one for each corner)
      handleMinuteDots(timeMinute);
      // store last index values
      lastBackgroundIndex = currentBackgroundIndex;
      lastTouretteModeIndex = currentTouretteModeIndex;
      lastTouretteStartIndex = currentTouretteStartIndex;
      lastTouretteMiddleIndex = currentTouretteMiddleIndex;
      lastTouretteEndIndex = currentTouretteEndIndex;
      lastPower = power;
      lastDaylightSaving = daylightSaving;
      lastTourette = tourette;
      lastColorEffectIndex = currentColorEffectIndex;
      lastForegroundColor = foregroundColor;
      updateTime = false;
    }

    EVERY_N_MILLISECONDS(40) {
      // slowly blend the current palette to the next
      nblendPaletteTowardPalette(currentPalette, targetPalette, 8);
      pHue++; // slowly cycle the "base color" through the rainbow
    }
    // cycle fields
    nextPalette();
    nextTouretteMode();
    nextTouretteStart();
    nextTouretteMiddle();
    nextTouretteEnd();
  }
  blitLeds();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}
