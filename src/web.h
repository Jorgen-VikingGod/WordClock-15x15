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

#include "NTPClient.h"
#include "tft.h"
#include <SPIFFSEditor.h>
#include <wifi.h>

uint8_t connectTry = 0;

NTPClient ntpClient("de.pool.ntp.org", 3600); // 3600);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;

  case WStype_CONNECTED: {
    IPAddress ip = webSocketsServer.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0],
                  ip[1], ip[2], ip[3], payload);

    // send message to client
    // webSocketsServer.sendTXT(num, "Connected");
  } break;

  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);

    // send message to client
    // webSocketsServer.sendTXT(num, "message here");

    // send data to all connected clients
    // webSocketsServer.broadcastTXT("someone else connected");
    break;

  case WStype_BIN:
    Serial.printf("[%u] get binary length: %u\n", num, length);
    //  hexdump(payload, length);

    // send message to client
    // webSocketsServer.sendBIN(num, payload, lenght);
    break;
  }
}

void setupWeb() {
  webServer.on("/all", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String json = getFieldsJson(fields, fieldCount);
    request->send(200, "text/json", json);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.on("/fieldValue", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String name = request->getParam("name")->value();
    String value = getFieldValue(name, fields, fieldCount);
    request->send(200, "text/json", value);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.on("/fieldValue", HTTP_POST, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String name = request->getParam("name", true)->value();

    Field field = getField(name, fields, fieldCount);
    String value;
    if (field.type == ColorFieldType) {
      String r = request->getParam("r", true)->value();
      String g = request->getParam("g", true)->value();
      String b = request->getParam("b", true)->value();
      value = r + "," + g + "," + b;
    } else {
      value = request->getParam("value", true)->value();
    }

    String newValue = setFieldValue(name, value, fields, fieldCount);
    request->send(200, "text/json", newValue);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
  webServer.addHandler(new SPIFFSEditor(SPIFFS, editUser, editPassword));

  webServer.onNotFound([](AsyncWebServerRequest *request) {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      Serial.printf("GET");
    else if (request->method() == HTTP_POST)
      Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(),
                  request->url().c_str());

    if (request->contentLength()) {
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for (i = 0; i < headers; i++) {
      AsyncWebHeader *h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for (i = 0; i < params; i++) {
      AsyncWebParameter *p = request->getParam(i);
      if (p->isFile()) {
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(),
                      p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  webServer.onFileUpload([](AsyncWebServerRequest *request,
                            const String &filename, size_t index, uint8_t *data,
                            size_t len, bool final) {
    if (!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char *)data);
    if (final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  });
  webServer.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data,
                             size_t len, size_t index, size_t total) {
    if (!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char *)data);
    if (index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  webServer.begin();
  Serial.println("HTTP server started");

  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketEvent);
  Serial.println("WebSockets server started");
}

bool wifiConnected() {
  // turn off the board's LED when connected to wifi
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.fillScreen(TFT_BLACK);
  tft.drawString("IP: " + WiFi.localIP().toString(), 5, tft.height() / 2, 4);
  saveConfigToJSON();
  digitalWrite(LED_BUILTIN, LOW);
  drawItem(GRAPHIC_WIFI_SYMBOL);
  // showWiFiSymbol();
  ntpClient.begin();
  return true;
}

void configPortal() {
  AsyncWiFiManager wifiManager(&webServer, &dns);
  // reset settings - for testing
  // wifiManager.resetSettings();
  if (!wifiManager.startConfigPortal("OnDemandAP")) {
    Serial.println("failed to connect and hit timeout");
    tft.fillScreen(TFT_BLACK);
    tft.drawString("failed to connect and hit timeout", 5, tft.height() / 2, 4);
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
}

void handleWeb() {
  static bool webServerStarted = false;
  if (btnPortal) {
    configPortal();
    webServerStarted = wifiConnected();
    btnPortal = false;
  } else {
    // check for connection
    if (apMode == true || (apMode == false && WiFi.status() == WL_CONNECTED)) {
      if (!webServerStarted) {
        webServerStarted = wifiConnected();
      }
      webSocketsServer.loop();
      ntpClient.setDaylightSaving(daylightSaving == 1);
      ntpClient.update();
      EVERY_N_MILLIS(1000) {
        // Serial.println(ntpClient.getFormattedTime());
        String json = "{\"name\":\"time\",\"value\":\"" +
                      ntpClient.getFormattedTime() + "\"}";
        webSocketsServer.broadcastTXT(json);
        // Serial.println(ntpClient.seconds());
      }
    } else {
      // blink the board's LED while connecting to wifi
      // static uint8_t ledState = 0;
      EVERY_N_MILLIS(125) {
        // ledState = ;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        connectTry = connectTry + 1;
        if (connectTry > 30) {
          Serial.println("");
          Serial.println("could not connect to wifi, restarting...");
          tft.fillScreen(TFT_BLACK);
          tft.drawString("could not connect to wifi, restarting...", 5, tft.height() / 2, 4);
          delay(3000);
          ESP.restart();
          delay(5000);
        }
        Serial.print(".");
      }
    }
  }
  btn1.loop();
}
