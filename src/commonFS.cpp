#include "commonFS.h"
#include <Spiffs.h>

bool saveJsonValue(const char* filename, const JsonDocument& doc) {
    File file = SPIFFS.open(filename, "W");
    if (!file) {
        Serial.print("Error when opening the file for writing: ");
        Serial.println(filename);
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Ersal error in JSON.");
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool loadJsonValue(const char* filename, JsonDocument& doc) {
    File file = SPIFFS.open(filename, "R");
    if (!file) {
        Serial.print("Error when opening the file for reading: ");
        Serial.println(filename);
        return false;
    }
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.print("Errors when deserializing JSON: ");
        Serial.println(error.f_str());
        return false;
    }
    return true;
}

void initializeSPIFFS() {
    if (!SPIFFS.begin(true, "/Spiff", 10, "Spiff")) {
        Serial.println("Spiffs Mount Failed");
        return;
    }
    Serial.printf("Spiffs totally: %u Bytes \n", SPIFFS.totalBytes());
    Serial.printf("Spiffs used: %u Bytes \n", SPIFFS.usedBytes());
    Serial.printf("Spiffs Free: %u Bytes \n", SPIFFS.totalBytes() - SPIFFS.usedBytes());
}