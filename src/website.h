#ifndef WEBSITE_H
#define WEBSITE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "commonFS.h"
#include "api.h"
#include <ArduinoJson.h>
#include <Update.h>
#include <AsyncTCP.h>
#include "bambu.h"
#include "nfc.h"
#include "scale.h"
#include "esp_task_wdt.h"

extern String spoolmanUrl;
extern AsyncWebServer server;
extern AsyncWebSocket ws;

// Server initialization and handler
void initWebServer();
void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void setupWebserver(AsyncWebServer &server);

// Web socket functions
void sendAmsData(AsyncWebSocketClient *client);
void sendNfcData(AsyncWebSocketClient *client);
void foundNfcTag(AsyncWebSocketClient *client, uint8_t success);
void sendWriteResult(AsyncWebSocketClient *client, uint8_t success);

#endif
