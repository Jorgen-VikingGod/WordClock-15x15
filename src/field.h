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

typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();

const String NumberFieldType = "Number";
const String BooleanFieldType = "Boolean";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";
const String SectionFieldType = "Section";
const String ButtonsFieldType = "Buttons";

typedef struct SectionField {
public:
  String name;
  String layout;
  String color;
  SectionField(String sName = "", String sLayout = "", String sColor = "") {
    name = sName;
    layout = sLayout;
    color = sColor;
  }
};

typedef struct Field {
public:
  SectionField section;
  String name;
  String label;
  String type;
  uint8_t min;
  uint8_t max;
  FieldGetter getValue;
  FieldGetter getOptions;
  FieldSetter setValue;
};

typedef Field FieldList[];

Field getField(String name, FieldList fields, uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];
    if (field.name == name) {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name, FieldList fields, uint8_t count) {
  Field field = getField(name, fields, count);
  if (field.getValue) {
    return field.getValue();
  }
  return String();
}

CRGB parseColor(String value) {
  uint8_t ri = value.indexOf(",");
  uint8_t gi = value.indexOf(",", ri + 1);

  String rs = value.substring(0, ri);
  String gs = value.substring(ri + 1, gi);
  String bs = value.substring(gi + 1);

  uint8_t r = rs.toInt();
  uint8_t g = gs.toInt();
  uint8_t b = bs.toInt();

  return CRGB(r, g, b);
}

void writeFieldsToEEPROM(FieldList fields, uint8_t count) {
  uint8_t index = 0;

  EEPROM.write(index, 0);

  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];
    if (!field.getValue && !field.setValue)
      continue;

    String value = field.getValue();

    if (field.type == ColorFieldType) {
      CRGB color = parseColor(value);
      EEPROM.write(index++, color.r);
      EEPROM.write(index++, color.g);
      EEPROM.write(index++, color.b);
    } else {
      byte v = value.toInt();
      EEPROM.write(index++, v);
    }
  }

  EEPROM.commit();
}

String setFieldValue(String name, String value, FieldList fields,
                     uint8_t count) {
  Field field = getField(name, fields, count);
  if (!field.setValue) {
    return "";
  }
  String result = field.setValue(value);

  String json = "{\"name\":\"" + name + "\",\"value\":" + result + "}";
  webSocketsServer.broadcastTXT(json);

  writeFieldsToEEPROM(fields, count);

  return result;
}

void loadFieldsFromEEPROM(FieldList fields, uint8_t count) {
  uint8_t byteCount = 1;

  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];
    if (!field.setValue)
      continue;

    if (field.type == ColorFieldType) {
      byteCount += 3;
    } else {
      byteCount++;
    }
  }

  EEPROM.begin(count);
  // if () {
  //   Serial.println("Failed to initialize EEPROM!");
  //   return;
  // }

  if (EEPROM.read(0) == 255) {
    Serial.println("First run, or EEPROM erased, skipping settings load!");
    return;
  }

  uint8_t index = 0;

  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];
    if (!field.setValue)
      continue;

    if (field.type == ColorFieldType) {
      String r = String(EEPROM.read(index++));
      String g = String(EEPROM.read(index++));
      String b = String(EEPROM.read(index++));
      field.setValue(r + "," + g + "," + b);
    } else {
      byte v = EEPROM.read(index++);
      field.setValue(String(v));
    }
  }
}

String getFieldsJson(FieldList fields, uint8_t count) {
  String json = "[";

  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];

    json += "{\"name\":\"" + field.name + "\",\"label\":\"" + field.label +
            "\",\"type\":\"" + field.type + "\"";

    if (field.section.name != "") {
      json += ",\"section\": {\"name\": \"" + field.section.name +
              "\", \"layout\": \"" + field.section.layout +
              "\", \"color\": \"" + field.section.color + "\"}";
    }

    if (field.getValue) {
      if (field.type == ColorFieldType || field.type == "String") {
        json += ",\"value\":\"" + field.getValue() + "\"";
      } else {
        json += ",\"value\":" + field.getValue();
      }
    }

    if (field.type == NumberFieldType) {
      json += ",\"min\":" + String(field.min);
      json += ",\"max\":" + String(field.max);
    }

    if (field.getOptions) {
      json += ",\"options\":[";
      json += field.getOptions();
      json += "]";
    }

    json += "}";

    if (i < count - 1)
      json += ",";
  }

  json += "]";

  return json;
}

void saveConfigToJSON() {
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove("/js/config.js");
  // Open file for writing
  File file = SPIFFS.open("/js/config.js", FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  file.print("var ip = '" + WiFi.localIP().toString() + "';");
  // Close the file
  file.close();
}