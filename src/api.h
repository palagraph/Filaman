#ifndef API_H
#define API_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h> // Include for AsyncWebServerRequest

#include "website.h"
#include "display.h"
#include <ArduinoJson.h>

extern bool spoolman_connected;
extern String spoolmanUrl;
extern bool octoEnabled;
extern String octoUrl;
extern String octoToken;

bool checkSpoolmanInstance(const String& url);
bool saveSpoolmanUrl(const String& url, bool octoOn, const String& octoWh, const String& octoTk);
String loadSpoolmanUrl(); // New function for loading the URL
bool checkSpoolmanExtraFields(); // New function to check the extrafelder
JsonDocument fetchSingleSpoolInfo(int spoolId); // API function for the website

bool updateSpoolTagId(String uidString, const char* payload); // New function to update a spool
uint8_t updateSpoolWeight(String spoolId, uint16_t weight); // New function for updating the weight
bool initSpoolman(); // New function to initialize Spoolman
bool updateSpoolBambuData(String payload); // New function to update the BAMBU data

bool updateSpoolOcto(int spoolId); // Neue Funktion zum Aktualisieren der Octo-Daten

#endif
