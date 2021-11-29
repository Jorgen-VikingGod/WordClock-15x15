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

NTPClient ntpClient("de.pool.ntp.org", 3600);  // 3600);

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data,
               size_t len) {
  if (type == WS_EVT_CONNECT) {
    IPAddress ip = client->remoteIP();
    Serial.printf("ws[%s][%u] Connected from %d.%d.%d.%d\n", server->url(), client->id(), ip[0], ip[1], ip[2], ip[3]);
    client->ping();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len) {
      // the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(),
                    (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char)data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());
      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      // message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0) {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(),
                        (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }
      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num,
                    (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < len; i++) {
          msg += (char)data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());
      if ((info->index + len) == info->len) {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final) {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(),
                        (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void setupWeb() {
  ws.onEvent(onWsEvent);
  webServer.addHandler(&ws);
  webServer.addHandler(new SPIFFSEditor(SPIFFS, editUser, editPassword));

  webServer.on("/all", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String json = getFieldsJson(fields, fieldCount);
    request->send(200, "text/json", json);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String name = request->getParam("name")->value();
    String value = getFieldValue(name, fields, fieldCount);
    request->send(200, "text/json", value);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_BUILTIN, HIGH);
    String name = request->getParam("name")->value();

    Field field = getField(name, fields, fieldCount);
    String value;
    if (field.type == ColorFieldType) {
      String r = request->getParam("r")->value();
      String g = request->getParam("g")->value();
      String b = request->getParam("b")->value();
      value = r + "," + g + "," + b;
    } else {
      value = request->getParam("value")->value();
    }

    String newValue = setFieldValue(name, value, fields, fieldCount);
    request->send(200, "text/json", newValue);
    digitalWrite(LED_BUILTIN, LOW);
  });

  webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

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
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

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
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  webServer.onFileUpload(
      [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final) Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
      });
  webServer.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!index) Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char *)data);
    if (index + len == total) Serial.printf("BodyEnd: %u\n", total);
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  webServer.begin();
  Serial.println("HTTP server started");
}

bool wifiConnected() {
  // turn off the board's LED when connected to wifi
  digitalWrite(LED_BUILTIN, HIGH);
  saveConfigToJSON();
  digitalWrite(LED_BUILTIN, LOW);
  ntpClient.begin();
  return true;
}

void handleWeb() {
  static bool webServerStarted = false;

  // check for connection
  if (WiFi.status() == WL_CONNECTED) {
    if (!webServerStarted) {
      webServerStarted = wifiConnected();
    }
    ws.cleanupClients();
    ntpClient.setDaylightSaving(daylightSaving == 1);
    ntpClient.update();
    EVERY_N_MILLIS(1000) {
      String json = "{\"name\":\"time\",\"value\":\"" + ntpClient.getFormattedTime() + "\"}";
      ws.textAll(json);
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
