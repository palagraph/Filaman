#include "commonFS.h"
#include <LittleFS.h>

bool saveJsonValue(const char* filename, const JsonDocument& doc) {
    File file = LittleFS.open(filename, "w");
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
    File file = LittleFS.open(filename, "r");
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

void initializeFileSystem() {
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    Serial.printf("LittleFS Total: %u bytes\n", LittleFS.totalBytes());
    Serial.printf("LittleFS Used: %u bytes\n", LittleFS.usedBytes());
    Serial.printf("LittleFS Free: %u bytes\n", LittleFS.totalBytes() - LittleFS.usedBytes());
}