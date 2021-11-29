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

#define HOSTNAME "ESP32-"  ///< Hostname. The setup function adds the Chip ID at the end.

void gotIP(system_event_id_t event) {
  Serial.print("Got IP event triggered. Local IP address: ");
  Serial.println(WiFi.localIP().toString());
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Tourette Wordclock", 5, 20, 4);
  tft.drawString("IP: " + WiFi.localIP().toString(), 5, tft.height() / 2 - 10, 4);
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Wifi is disconnected event has been triggered");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.setAutoReconnect(true);
  WiFi.begin();
}

void setupWifi() {
  // Set Hostname.
  String hostname(HOSTNAME);

  uint64_t chipid = ESP.getEfuseMac();
  uint16_t long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16);
  uint16_t long2 = (unsigned long)((chipid & 0x0000FFFF));
  String hex = String(long1, HEX) + String(long2, HEX);  // six octets
  hostname += hex;

  char hostnameChar[hostname.length() + 1];
  memset(hostnameChar, 0, hostname.length() + 1);

  for (uint8_t i = 0; i < hostname.length(); i++) hostnameChar[i] = hostname.charAt(i);

  WiFi.setHostname(hostnameChar);

  WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.onEvent(gotIP, SYSTEM_EVENT_STA_GOT_IP);

  // Print hostname.
  Serial.println("Hostname: " + hostname);
  Serial.print(F("\nStarting Tourette Wordclock on "));
  Serial.println(ARDUINO_BOARD);
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) {
    Serial.println(F("DRD"));
    initialConfig = true;
  }
  btn1.loop();
  if (btn1.isPressed()) {
    Serial.println(F("BTN1"));
    initialConfig = true;
  }
  ESPAsync_WiFiManager espAsync_wifiManager(&webServerConfig, &dnsServerConfig, hostname.c_str());
  if (espAsync_wifiManager.WiFi_SSID() == "") {
    Serial.println(F("No AP credentials"));
    initialConfig = true;
  }
  if (initialConfig) {
    showWiFiSymbol();
    blitLeds();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
    Serial.println(F("Starting Config Portal"));
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Tourette Wordclock", 5, 20, 4);
    tft.drawString("AP: " + hostname, 5, tft.height() / 2 - 10, 4);
    tft.drawString("IP: 192.168.4.1", 5, tft.height() - 40, 4);
    if (!espAsync_wifiManager.startConfigPortal(hostname.c_str(), apPassword)) {
      Serial.println(F("Not connected to WiFi"));
    } else {
      Serial.println(F("connected"));
    }
    webServerConfig.end();
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }
  unsigned long startedAt = millis();
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis() - startedAt);
  Serial.print(F("After waiting "));
  Serial.print(waited / 1000);
  Serial.print(F(" secs , Connection result is "));
  Serial.println(connRes);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Failed to connect"));
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Failed to connect", 5, tft.height() / 2 - 10, 4);
    delay(3000);
    ESP.restart();
    delay(5000);
  }
}
