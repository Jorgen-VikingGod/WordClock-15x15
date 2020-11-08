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

#include <vector>

struct sLayoutItem {
  bool newWord;
  bool customColor;
  bool pixel;
  bool line;
  int r;
  int g;
  int b;
  int x;
  int y;
  int x1;
  int y1;
  int x2;
  int y2;
  sLayoutItem(bool bNew = true) {
    newWord = bNew;
    customColor = false;
    pixel = false;
    line = false;
  }
  sLayoutItem(int iR, int iG, int iB) {
    newWord = false;
    customColor = true;
    pixel = false;
    line = false;
    r = iR;
    g = iG;
    b = iB;
  }
  sLayoutItem(int iX, int iY) {
    newWord = false;
    customColor = false;
    pixel = true;
    line = false;
    x = iX;
    y = iY;
  }
  sLayoutItem(int iX1, int iY1, int iX2, int iY2) {
    newWord = false;
    customColor = false;
    pixel = false;
    line = true;
    x1 = iX1;
    x2 = iX2;
    y1 = iY1;
    y2 = iY2;
  }
};
typedef std::vector<sLayoutItem> vLayoutItems;

// prefix "hey du" or "es ist"
const vLayoutItems WORD_HEY_DU{
    {},
    {0, 0, 2, 0},
    {},
    {4, 0, 5, 0},
};
const vLayoutItems WORD_ES_IST{
    {},
    {6, 0, 7, 0},
    {},
    {3, 3, 5, 3},
};
const vLayoutItems WORD_WIR_HABEN{
    {},
    {6, 3, 8, 3},
    {},
    {10, 3, 14, 3},
};
// time prefix & suffix
const vLayoutItems WORD_VOR{
    {},
    {12, 7, 14, 7},
};
const vLayoutItems WORD_HALB{
    {},
    {3, 8, 6, 8},
};
const vLayoutItems WORD_NACH{
    {},
    {0, 8, 3, 8},
};
// numbers
const vLayoutItems WORD_M_05{
    {},
    {0, 6, 3, 6},
};
const vLayoutItems WORD_M_10{
    {},
    {4, 6, 7, 6},
};
const vLayoutItems WORD_M_15{
    {},
    {4, 7, 10, 7},
};
const vLayoutItems WORD_M_20{
    {},
    {8, 6, 14, 6},
};
// minutes
const vLayoutItems WORD_M_05_NACH{
    {},
    {0, 6, 3, 6},
    {},
    {0, 8, 3, 8},
};
const vLayoutItems WORD_M_10_NACH{
    {},
    {4, 6, 7, 6},
    {},
    {0, 8, 3, 8},
};
const vLayoutItems WORD_M_15_NACH{
    {},
    {4, 7, 10, 7},
    {},
    {0, 8, 3, 8},
};
const vLayoutItems WORD_M_20_NACH{
    {},
    {8, 6, 14, 6},
    {},
    {0, 8, 3, 8},
};
const vLayoutItems WORD_M_25{
    {}, {0, 6, 3, 6}, {}, {12, 7, 14, 7}, {}, {3, 8, 6, 8},
};
const vLayoutItems WORD_M_30{
    {},
    {3, 8, 6, 8},
};
const vLayoutItems WORD_M_35{
    {}, {0, 6, 3, 6}, {}, {0, 8, 3, 8}, {}, {3, 8, 6, 8},
};
const vLayoutItems WORD_M_40{
    {},
    {8, 6, 14, 6},
    {},
    {12, 7, 14, 7},
};
const vLayoutItems WORD_M_45{
    {},
    {0, 7, 10, 7},
};
const vLayoutItems WORD_M_50{
    {},
    {4, 6, 7, 6},
    {},
    {12, 7, 14, 7},
};
const vLayoutItems WORD_M_55{
    {},
    {0, 6, 3, 6},
    {},
    {12, 7, 14, 7},
};
// hours
const vLayoutItems WORD_H_EIN{
    {},
    {2, 11, 4, 11},
};
const vLayoutItems WORD_H_EINS{
    {},
    {2, 11, 5, 11},
};
const vLayoutItems WORD_H_ZWEI{
    {},
    {0, 10, 3, 10},
};
const vLayoutItems WORD_H_DREI{
    {},
    {0, 11, 3, 11},
};
const vLayoutItems WORD_H_VIER{
    {},
    {4, 10, 7, 10},
};
const vLayoutItems WORD_H_FUENF{
    {},
    {10, 8, 13, 8},
};
const vLayoutItems WORD_H_SECHS{
    {},
    {0, 9, 4, 9},
};
const vLayoutItems WORD_H_SIEBEN{
    {},
    {5, 11, 10, 11},
};
const vLayoutItems WORD_H_ACHT{
    {},
    {11, 9, 14, 9},
};
const vLayoutItems WORD_H_NEUN{
    {},
    {11, 10, 14, 10},
};
const vLayoutItems WORD_H_ZEHN{
    {},
    {8, 10, 11, 10},
};
const vLayoutItems WORD_H_ELF{
    {},
    {8, 8, 10, 8},
};
const vLayoutItems WORD_H_ZWOELF{
    {},
    {5, 9, 9, 9},
};
// time
const vLayoutItems WORD_UHR{
    {},
    {12, 11, 14, 11},
};
// Tourette start
const vLayoutItems WORD_SAU{
    {},
    {7, 0, 9, 0},
};
const vLayoutItems WORD_BAZI{
    {},
    {10, 0, 13, 0},
};
const vLayoutItems WORD_SAUBAZI{
    {},
    {7, 0, 13, 0},
};
const vLayoutItems WORD_PENNER{
    {},
    {0, 1, 5, 1},
};
const vLayoutItems WORD_SAUPENNER{
    {},
    {7, 0, 9, 0},
    {0, 1, 5, 1},
};
const vLayoutItems WORD_ARSCH{
    {},
    {10, 1, 14, 1},
};
const vLayoutItems WORD_FETTARSCH{
    {},
    {6, 1, 14, 1},
};
const vLayoutItems WORD_ARSCHSAFT{
    {10, 1, 14, 1},
    {0, 2, 3, 2},
};
const vLayoutItems WORD_SACK{
    {},
    {4, 2, 7, 2},
};
const vLayoutItems WORD_SAFTSACK{
    {},
    {0, 2, 7, 2},
};
const vLayoutItems WORD_FETTSACK{
    {},
    {6, 1, 9, 1},
    {4, 2, 7, 2},
};
const vLayoutItems WORD_TROTTEL{
    {},
    {8, 2, 14, 2},
};
const vLayoutItems WORD_PENIS{
    {},
    {0, 3, 4, 3},
};
const vLayoutItems WORD_ARSCHPENIS{
    {},
    {10, 1, 14, 1},
    {0, 3, 4, 3},
};
const vLayoutItems WORD_SAFTPENIS{
    {},
    {0, 2, 3, 2},
    {0, 3, 4, 3},
};
// Tourette middle
const vLayoutItems WORD_VERDAMMTE{
    {},
    {0, 4, 8, 4},
};
const vLayoutItems WORD_KACK{
    {},
    {9, 4, 12, 4},
};
const vLayoutItems WORD_VERFICKTE{
    {},
    {0, 4, 2, 4},
    {0, 5, 5, 5},
};
const vLayoutItems WORD_VERKACKTE{
    {},
    {0, 4, 2, 4},
    {9, 4, 14, 4},
};
const vLayoutItems WORD_FICK{
    {},
    {0, 5, 3, 5},
};
const vLayoutItems WORD_SCHEISS{
    {},
    {6, 5, 12, 5},
};
const vLayoutItems WORD_FICK_SCHEISS{
    {},
    {0, 5, 3, 5},
    {6, 5, 12, 5},
};
const vLayoutItems WORD_KACK_SCHEISS{
    {},
    {9, 4, 12, 4},
    {6, 5, 12, 5},
};
const vLayoutItems WORD_VERDAMMTE_SCHEISS{
    {},
    {0, 4, 8, 4},
    {6, 5, 12, 5},
};
const vLayoutItems WORD_VERKACKTE_SCHEISS{
    {},
    {0, 4, 2, 4},
    {9, 4, 14, 4},
    {6, 5, 12, 5},
};
const vLayoutItems WORD_VERFICKTE_SCHEISS{
    {},
    {0, 4, 2, 4},
    {0, 5, 12, 5},
};
// Tourette end
const vLayoutItems WORD_DU{
    {},
    {0, 12, 1, 12},
};
const vLayoutItems WORD_SAU2{
    {},
    {8, 12, 10, 12},
};
const vLayoutItems WORD_BAZI2{
    {},
    {11, 12, 14, 12},
};
const vLayoutItems WORD_DRECKSAU{
    {},
    {3, 12, 10, 12},
};
const vLayoutItems WORD_DRECKS{
    {},
    {3, 12, 8, 12},
};
const vLayoutItems WORD_DRECKS_BAZI{
    {},
    {3, 12, 8, 12},
    {11, 12, 14, 12},
};
const vLayoutItems WORD_DRECKSAUBAZI{
    {},
    {3, 12, 14, 12},
};
const vLayoutItems WORD_ARSCH2{
    {},
    {0, 13, 4, 13},
};
const vLayoutItems WORD_ARSCHLOCH{
    {},
    {0, 13, 8, 13},
};
const vLayoutItems WORD_PENNER2{
    {},
    {9, 13, 14, 13},
};
const vLayoutItems WORD_DRECKS_PENNER{
    {},
    {3, 12, 8, 12},
    {9, 13, 14, 13},
};
const vLayoutItems WORD_ARSCHLOCHPENNER{
    {},
    {0, 13, 14, 13},
};
const vLayoutItems WORD_KOBOLD{
    {},
    {5, 14, 10, 14},
};
const vLayoutItems WORD_HODENKOBOLD{
    {},
    {0, 14, 10, 14},
};
const vLayoutItems WORD_SAUKOBOLD{
    {},
    {8, 12, 10, 12},
    {5, 14, 10, 14},
};
const vLayoutItems WORD_DRECKS_KOBOLD{
    {},
    {3, 12, 8, 12},
    {5, 14, 10, 14},
};
const vLayoutItems WORD_DRECKSAUKOBOLD{
    {},
    {3, 12, 10, 12},
    {5, 14, 10, 14},
};
const vLayoutItems WORD_ARSCHKOBOLD{
    {},
    {0, 13, 4, 13},
    {5, 14, 10, 14},
};
const vLayoutItems WORD_DUMMY{
    {},
    {10, 14, 14, 14},
};
const vLayoutItems WORD_DRECKS_DUMMY{
    {},
    {3, 12, 8, 12},
    {10, 14, 14, 14},
};
const vLayoutItems WORD_ARSCHDUMMY{
    {},
    {0, 13, 4, 13},
    {10, 14, 14, 14},
};
const vLayoutItems WORD_HODENDUMMY{
    {},
    {0, 14, 4, 14},
    {10, 14, 14, 14},
};
// wifi symbol
const vLayoutItems GRAPHIC_WIFI_SYMBOL{
    {255, 0, 0},    {6, 13, 8, 13}, {7, 14},        {255, 80, 0},
    {6, 9, 8, 9},   {5, 10, 9, 10}, {4, 11, 5, 11}, {9, 11, 10, 11},
    {255, 120, 0},  {5, 5, 9, 5},   {3, 6, 11, 6},  {2, 7, 4, 7},
    {10, 7, 12, 7}, {255, 180, 0},  {4, 1, 10, 1},  {2, 2, 12, 2},
    {0, 3, 4, 3},   {10, 3, 14, 3}, {0, 4, 1, 4},   {13, 4, 14, 4},
};
