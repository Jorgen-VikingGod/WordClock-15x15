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
#include "leds.h"

uint8_t randomTouretteMode = 0;
uint8_t cycleTouretteMode = 0;
uint8_t touretteCycleDuration = 60;
uint8_t currentTouretteModeIndex = 0;
unsigned long touretteCycleTimeout = 0;

uint8_t randomTouretteStart = 0;
uint8_t cycleTouretteStart = 0;
uint8_t currentTouretteStartIndex = 0;

uint8_t randomTouretteMiddle = 0;
uint8_t cycleTouretteMiddle = 0;
uint8_t currentTouretteMiddleIndex = 0;

uint8_t randomTouretteEnd = 0;
uint8_t cycleTouretteEnd = 0;
uint8_t currentTouretteEndIndex = 0;

void drawWordSau() { drawItem(WORD_SAU); }
void drawWordBazi() { drawItem(WORD_BAZI); }
void drawWordSaubazi() { drawItem(WORD_SAUBAZI); }
void drawWordPenner() { drawItem(WORD_PENNER); }
void drawWordSaupenner() { drawItem(WORD_SAUPENNER); }
void drawWordArsch() { drawItem(WORD_ARSCH); }
void drawWordFettarsch() { drawItem(WORD_FETTARSCH); }
void drawWordArschsaft() { drawItem(WORD_ARSCHSAFT); }
void drawWordSack() { drawItem(WORD_SACK); }
void drawWordSaftsack() { drawItem(WORD_SAFTSACK); }
void drawWordFettsack() { drawItem(WORD_FETTSACK); }
void drawWordTrottel() { drawItem(WORD_TROTTEL); }
void drawWordPenis() { drawItem(WORD_PENIS); }
void drawWordArschpenis() { drawItem(WORD_ARSCHPENIS); }
void drawWordSaftpenis() { drawItem(WORD_SAFTPENIS); }

void drawWordVerdammte() { drawItem(WORD_VERDAMMTE); }
void drawWordKack() { drawItem(WORD_KACK); }
void drawWordVerkackte() { drawItem(WORD_VERKACKTE); }
void drawWordFick() { drawItem(WORD_FICK); }
void drawWordVerfickte() { drawItem(WORD_VERFICKTE); }
void drawWordScheiss() { drawItem(WORD_SCHEISS); }
void drawWordFickScheiss() { drawItem(WORD_FICK_SCHEISS); }
void drawWordKackScheiss() { drawItem(WORD_KACK_SCHEISS); }
void drawWordVerdammteScheiss() { drawItem(WORD_VERDAMMTE_SCHEISS); }
void drawWordVerkackteScheiss() { drawItem(WORD_VERKACKTE_SCHEISS); }
void drawWordVerfickteScheiss() { drawItem(WORD_VERFICKTE_SCHEISS); }

void drawWordSau2() { drawItem(WORD_SAU2); }
void drawWordBazi2() { drawItem(WORD_BAZI2); }
void drawWordDrecksau() { drawItem(WORD_DRECKSAU); }
void drawWordDrecksBazi() { drawItem(WORD_DRECKS_BAZI); }
void drawWordDrecksaubazi() { drawItem(WORD_DRECKSAUBAZI); }
void drawWordArsch2() { drawItem(WORD_ARSCH2); }
void drawWordArschloch() { drawItem(WORD_ARSCHLOCH); }
void drawWordPenner2() { drawItem(WORD_PENNER2); }
void drawWordDrecksPenner() { drawItem(WORD_DRECKS_PENNER); }
void drawWordArschlochpenner() { drawItem(WORD_ARSCHLOCHPENNER); }
void drawWordKobold() { drawItem(WORD_KOBOLD); }
void drawWordHodenkobold() { drawItem(WORD_HODENKOBOLD); }
void drawWordSaukobold() { drawItem(WORD_SAUKOBOLD); }
void drawWordDrecksKobold() { drawItem(WORD_DRECKS_KOBOLD); }
void drawWordDrecksaukobold() { drawItem(WORD_DRECKSAUKOBOLD); }
void drawWordArschkobold() { drawItem(WORD_ARSCHKOBOLD); }
void drawWordDummy() { drawItem(WORD_DUMMY); }
void drawWordDrecksDummy() { drawItem(WORD_DRECKS_DUMMY); }
void drawWordArschdummy() { drawItem(WORD_ARSCHDUMMY); }
void drawWordHodendummy() { drawItem(WORD_HODENDUMMY); }

typedef void (*TouretteWord)();
typedef TouretteWord TouretteWordList[];
typedef struct {
  TouretteWord drawWord;
  String name;
} WordAndName;
typedef WordAndName WordAndNameList[];

WordAndNameList touretteStartWords = {
    {drawWordSau, "sau"},
    {drawWordBazi, "bazi"},
    {drawWordSaubazi, "saubazi"},
    {drawWordPenner, "penner"},
    {drawWordSaupenner, "saupenner"},
    {drawWordArsch, "arsch"},
    {drawWordFettarsch, "fettarsch"},
    {drawWordArschsaft, "arschsaft"},
    {drawWordSack, "sack"},
    {drawWordSaftsack, "saftsack"},
    {drawWordFettsack, "fettsack"},
    {drawWordTrottel, "trottel"},
    {drawWordPenis, "penis"},
    {drawWordArschpenis, "arschpenis"},
    {drawWordSaftpenis, "saftpenis"},
};
const uint8_t touretteStartWordCount = ARRAY_SIZE(touretteStartWords);

WordAndNameList touretteMiddleWords = {
    {drawWordVerdammte, "verdammte"},
    {drawWordKack, "kack"},
    {drawWordVerkackte, "verkackte"},
    {drawWordFick, "fick"},
    {drawWordVerfickte, "verfickte"},
    {drawWordScheiss, "scheiss"},
    {drawWordFickScheiss, "fick scheiss"},
    {drawWordKackScheiss, "kack scheiss"},
    {drawWordVerdammteScheiss, "verdammte scheiss"},
    {drawWordVerkackteScheiss, "verkackte scheiss"},
    {drawWordVerfickteScheiss, "verfickte scheiss"},
};
const uint8_t touretteMiddleWordCount = ARRAY_SIZE(touretteMiddleWords);

WordAndNameList touretteEndWords = {
    {drawWordSau2, "sau"},
    {drawWordBazi2, "bazi"},
    {drawWordDrecksau, "drecksau"},
    {drawWordDrecksBazi, "drecks bazi"},
    {drawWordDrecksaubazi, "drecksaubazi"},
    {drawWordArsch2, "arsch"},
    {drawWordArschloch, "arschloch"},
    {drawWordPenner2, "penner"},
    {drawWordDrecksPenner, "drecks penner"},
    {drawWordArschlochpenner, "arschlochpenner"},
    {drawWordKobold, "kobold"},
    {drawWordHodenkobold, "hodenkobold"},
    {drawWordSaukobold, "saukobold"},
    {drawWordDrecksKobold, "drecks kobold"},
    {drawWordDrecksaukobold, "drecksaukobold"},
    {drawWordArschkobold, "arschkobold"},
    {drawWordDummy, "dummy"},
    {drawWordDrecksDummy, "drecks dummy"},
    {drawWordArschdummy, "arschdummy"},
    {drawWordHodendummy, "hodendummy"},
};
const uint8_t touretteEndWordCount = ARRAY_SIZE(touretteEndWords);

const String touretteModes[] = {"Start", "End"};
const uint8_t touretteModeCount = ARRAY_SIZE(touretteModes);
