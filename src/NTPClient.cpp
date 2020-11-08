/*
 * WordClock-15x15: https://github.com/Jorgen-VikingGod/WordClock-15x15
 * Copyright (C) 2019 Juergen Skrotzky alias Jorgen (JorgenVikingGod@gmail.com)
 * NTP Copyright (c) 2015 by Fabrice Weinberg
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

#include "NTPClient.h"

NTPClient::NTPClient(int timeOffset) { this->_timeOffset = timeOffset; }

NTPClient::NTPClient(const char *poolServerName) {
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(const char *poolServerName, int timeOffset) {
  this->_timeOffset = timeOffset;
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(const char *poolServerName, int timeOffset,
                     int updateInterval) {
  this->_timeOffset = timeOffset;
  this->_poolServerName = poolServerName;
  this->_updateInterval = updateInterval;
}

void NTPClient::setTimeOffset(int timeOffset) {
  this->_timeOffset = timeOffset;
}

void NTPClient::setDaylightSaving(bool saving) {
  this->_daylightSaving = saving;
}

void NTPClient::begin() {
#ifdef DEBUG_NTPClient
  Serial.println("Begin NTPClient");
  Serial.print("Start udp connection on port: ");
  Serial.println(this->_port);
#endif
  this->_udp.begin(this->_port);
  this->forceUpdate();
}

void NTPClient::forceUpdate() {
#ifdef DEBUG_NTPClient
  Serial.println("Update from NTP Server");
#endif

  IPAddress address;
  WiFi.hostByName(this->_poolServerName, address);

  this->sendNTPPacket(address);

  // Wait till data is there or timeout...
  byte timeout = 0;
  int cb = 0;
  do {
    delay(10);
    cb = this->_udp.parsePacket();
    if (timeout > 100)
      return; // timeout after 1000 ms
    timeout++;
  } while (cb == 0);

  this->_lastUpdate =
      millis() - (10 * (timeout + 1)); // Account for delay in reading the time

  this->_udp.read(this->_packetBuffer, NTP_PACKET_SIZE);

  unsigned long highWord =
      word(this->_packetBuffer[40], this->_packetBuffer[41]);
  unsigned long lowWord =
      word(this->_packetBuffer[42], this->_packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  this->_currentEpoc = secsSince1900 - SEVENZYYEARS;
}

void NTPClient::update() {
  unsigned long runtime = millis();
  if (runtime - this->_lastUpdate >= this->_updateInterval &&
      this->_updateInterval != 0) {
    this->forceUpdate();
  }
}

unsigned long NTPClient::getRawTime() {
  return this->_timeOffset + // User offset
         (this->_daylightSaving ? 3600 : 0) +
         this->_currentEpoc + // Epoc returned by the NTP server
         ((millis() - this->_lastUpdate) / 1000); // Time since last update
}

unsigned long NTPClient::hours() {
  return (this->getRawTime() % 86400L) / 3600;
}
unsigned long NTPClient::minutes() { return (this->getRawTime() % 3600) / 60; }
unsigned long NTPClient::seconds() { return this->getRawTime() % 60; }

String NTPClient::getFormattedHours() {
  unsigned long hours = this->hours();
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);
  return hoursStr;
}
String NTPClient::getFormattedMinutes() {
  unsigned long minutes = this->minutes();
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);
  return minuteStr;
}
String NTPClient::getFormattedSeconds() {
  unsigned long seconds = this->seconds();
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);
  return secondStr;
}

String NTPClient::getFormattedTime() {
  String hoursStr = this->getFormattedHours();
  String minuteStr = this->getFormattedMinutes();
  String secondStr = this->getFormattedSeconds();
  return hoursStr + ":" + minuteStr + ":" + secondStr;
}

void NTPClient::sendNTPPacket(IPAddress ip) {
  // set all bytes in the buffer to 0
  memset(this->_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  this->_packetBuffer[0] = 0b11100011; // LI, Version, Mode
  this->_packetBuffer[1] = 0;          // Stratum, or type of clock
  this->_packetBuffer[2] = 6;          // Polling Interval
  this->_packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  this->_packetBuffer[12] = 49;
  this->_packetBuffer[13] = 0x4E;
  this->_packetBuffer[14] = 49;
  this->_packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  this->_udp.beginPacket(ip, 123); // NTP requests are to port 123
  this->_udp.write(this->_packetBuffer, NTP_PACKET_SIZE);
  this->_udp.endPacket();
}