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

#include <WiFi.h>
#include <WiFiUdp.h>

#include "Arduino.h"
#include "TimeLib.h"

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48

class NTPClient {
 private:
  WiFiUDP _udp;

  const char *_poolServerName = "time.nist.gov";  // Default time server
  int _port = 1337;
  int _timeOffset;
  bool _daylightSaving = false;

  unsigned int _updateInterval = 60000;  // In ms

  unsigned long _currentEpoc;     // In s
  unsigned long _lastUpdate = 0;  // In ms

  byte _packetBuffer[NTP_PACKET_SIZE];

  void sendNTPPacket(IPAddress _timeServerIP);

 public:
  NTPClient(int timeOffset);
  NTPClient(const char *poolServerName);
  NTPClient(const char *poolServerName, int timeOffset);
  NTPClient(const char *poolServerName, int timeOffset, int updateInterval);

  void setTimeOffset(int timeOffset);
  void setDaylightSaving(bool saving);

  void begin();
  void update();
  void forceUpdate();

  tmElements_t timeComponents();

  unsigned long hours();
  unsigned long minutes();
  unsigned long seconds();

  String getFormattedHours();
  String getFormattedMinutes();
  String getFormattedSeconds();
  String getFormattedTime();

  unsigned long getRawTime();
};