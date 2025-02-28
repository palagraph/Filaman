#include "website.h"
#include "commonFS.h"
#include "api.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "bambu.h"
#include "nfc.h"
#include "scale.h"
#include "esp_task_wdt.h"
#include <Update.h>
#include "display.h"
#include "ota.h"

#ifndef VERSION
  #define VERSION "1.1.0"
#endif

// Define Cache Control header
#define CACHE_CONTROL "max-age=604800" // Cache for 1 week

AsyncWebServer server(webserverPort);
AsyncWebSocket ws("/ws");

uint8_t lastSuccess = 0;
uint8_t lastHasReadRfidTag = 0;


void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("New client connected!");
        // Send the AMS data to the new client
        sendAmsData(client);
        sendNfcData(client);
        foundNfcTag(client, 0);
        sendWriteResult(client, 3);
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("Client disconnected.");
    } else if (type == WS_EVT_ERROR) {
        Serial.printf("WebSocket Client #%u error(%u): %s\n", client->id(), *((uint16_t*)arg), (char*)data);
    } else if (type == WS_EVT_PONG) {
        Serial.printf("WebSocket Client #%u pong\n", client->id());
    } else if (type == WS_EVT_DATA) {
        String message = String((char*)data);
        JsonDocument doc;
        deserializeJson(doc, message);

        if (doc["type"] == "heartbeat") {
            // Send Heartbeat response
            ws.text(client->id(), "{"
                "\"type\":\"heartbeat\","
                "\"freeHeap\":" + String(ESP.getFreeHeap()/1024) + ","
                "\"bambu_connected\":" + String(bambu_connected) + ","
                "\"spoolman_connected\":" + String(spoolman_connected) + ""
                "}");
        }

        else if (doc["type"] == "writeNfcTag") {
            if (doc["payload"].is<JsonObject>()) {
                // Attempts to write NFC data
                String payloadString;
                serializeJson(doc["payload"], payloadString);
                startWriteJsonToTag(payloadString.c_str());
            }
        }

        else if (doc["type"] == "scale") {
            uint8_t success = 0;
            if (doc["payload"] == "tare") {
                success = tareScale();
            }

            if (doc["payload"] == "calibrate") {
                success = calibrate_scale();
            }

            if (success) {
                ws.textAll("{\"type\":\"scale\",\"payload\":\"success\"}");
            } else {
                ws.textAll("{\"type\":\"scale\",\"payload\":\"error\"}");
            }
        }

        else if (doc["type"] == "reconnect") {
            if (doc["payload"] == "bambu") {
                bambu_restart();
            }

            if (doc["payload"] == "spoolman") {
                initSpoolman();
            }
        }

        else if (doc["type"] == "setBambuSpool") {
            Serial.println(doc["payload"].as<String>());
            setBambuSpool(doc["payload"]);
        }

        else if (doc["type"] == "setSpoolmanSettings") {
            Serial.println(doc["payload"].as<String>());
            if (updateSpoolBambuData(doc["payload"].as<String>())) {
                ws.textAll("{\"type\":\"setSpoolmanSettings\",\"payload\":\"success\"}");
            } else {
                ws.textAll("{\"type\":\"setSpoolmanSettings\",\"payload\":\"error\"}");
            }
        }

        else {
            Serial.println("Unknown website type: " + doc["type"].as<String>());
        }
    }
}

// Function for charging and replacing the header in an HTML file
String loadHtmlWithHeader(const char* filename) {
    Serial.println("Load HTML file.: " + String(filename));
    if (!SPIFFS.exists(filename)) {
        Serial.println("Error: File not found!");
        return "Fehler: Datei nicht gefunden!";
    }

    File file = SPIFFS.open(filename, "r");
    String html = file.readString();
    file.close();

    return html;
}

void sendWriteResult(AsyncWebSocketClient *client, uint8_t success) {
    // Send success/failure to all clients
    String response = "{\"type\":\"writeNfcTag\",\"success\":" + String(success ? "1" : "0") + "}";
    ws.textAll(response);
}

void foundNfcTag(AsyncWebSocketClient *client, uint8_t success) {
    if (success == lastSuccess) return;
    ws.textAll("{\"type\":\"nfcTag\", \"payload\":{\"found\": " + String(success) + "}}");
    sendNfcData(nullptr);
    lastSuccess = success;
}

void sendNfcData(AsyncWebSocketClient *client) {
    if (lastHasReadRfidTag == hasReadRfidTag) return;
    if (hasReadRfidTag == 0) {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{}}");
    }
    else if (hasReadRfidTag == 1) {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":" + nfcJsonData + "}");
    }
    else if (hasReadRfidTag == 2)
    {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{\"error\":\"Empty Tag or Data not readable\"}}");
    }
    else if (hasReadRfidTag == 3)
    {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{\"info\":\"Write tag ...\"}}");
    }
    else if (hasReadRfidTag == 4)
    {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{\"error\":\"Error writing to tag\"}}");
    }
    else if (hasReadRfidTag == 5)
    {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{\"info\":\"Tag successfully written\"}}");
    }
    else 
    {
        ws.textAll("{\"type\":\"nfcData\", \"payload\":{\"error\":\"Something went wrong\"}}");
    }
    lastHasReadRfidTag = hasReadRfidTag;
}

void sendAmsData(AsyncWebSocketClient *client) {
    if (ams_count > 0) {
        ws.textAll("{\"type\":\"amsData\",\"payload\":" + amsJsonData + "}");
    }
}

void setupWebserver(AsyncWebServer &server) {
    // Disable all debug output.
    Serial.setDebugOutput(false);
    
    // Web socket optimizations
    ws.onEvent(onWsEvent);
    ws.enable(true);

    // Configure servers for large uploads
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){});
    server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){});

    // Load the Spoolman URL at boot.
    spoolmanUrl = loadSpoolmanUrl();
    Serial.print("Geladene Spoolman-URL: ");
    Serial.println(spoolmanUrl);

    // Route for About
    server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Receive request for /about");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
    });

    // Route for scales
    server.on("/scale", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Receive request for /scales");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/scale.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
    });

    // Route for RFID
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Receive request for /RFID");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/rfid.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("RFID page sent");
    });

    server.on("/api/url", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("API-Aufruf: /api/url");
        String jsonResponse = "{\"spoolman_url\": \"" + String(spoolmanUrl) + "\"}";
        request->send(200, "application/json", jsonResponse);
    });

    // Route for WiFi
    server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Receive request for /wifi");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/wifi.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
    });

    // Route for Spoolman Setting
    server.on("/spoolman", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Receive request for /spoolman");
        String html = loadHtmlWithHeader("/spoolman.html");
        html.replace("{{spoolmanUrl}}", spoolmanUrl);

        JsonDocument doc;
        if (loadJsonValue("/bambu_credentials.json", doc) && doc["bambu_ip"].is<String>()) 
        {
            String bambuIp = doc["bambu_ip"].as<String>();
            String bambuSerial = doc["bambu_serialnr"].as<String>();
            String bambuCode = doc["bambu_accesscode"].as<String>();
            autoSendToBambu = doc["autoSendToBambu"].as<bool>();
            bambuIp.trim();
            bambuSerial.trim();
            bambuCode.trim();

            html.replace("{{bambuIp}}", bambuIp ? bambuIp : "");            
            html.replace("{{bambuSerial}}", bambuSerial ? bambuSerial : "");
            html.replace("{{bambuCode}}", bambuCode ? bambuCode : "");
            html.replace("{{autoSendToBambu}}", autoSendToBambu ? "checked" : "");
            html.replace("{{autoSendTime}}", String(autoSetBambuAmsCounter));
        }
        else
        {
            html.replace("{{bambuIp}}", "");
            html.replace("{{bambuSerial}}", "");
            html.replace("{{bambuCode}}", "");
            html.replace("{{autoSendToBambu}}", "");
            html.replace("{{autoSendTime}}", String(autoSetBambuAmsCounter));
        }

        request->send(200, "text/html", html);
    });

    // Route for checking the Spoolman instance
    server.on("/api/checkSpoolman", HTTP_GET, [](AsyncWebServerRequest *request){
        if (!request->hasParam("url")) {
            request->send(400, "application/json", "{\"success\": false, \"error\": \"Missing URL parameter\"}");
            return;
        }

        String url = request->getParam("url")->value();
        url.trim();
        
        bool healthy = saveSpoolmanUrl(url);
        String jsonResponse = "{\"healthy\": " + String(healthy ? "true" : "false") + "}";

        request->send(200, "application/json", jsonResponse);
    });

    // Route for checking the Spoolman instance
    server.on("/api/bambu", HTTP_GET, [](AsyncWebServerRequest *request){
        if (!request->hasParam("bambu_ip") || !request->hasParam("bambu_serialnr") || !request->hasParam("bambu_accesscode")) {
            request->send(400, "application/json", "{\"success\": false, \"error\": \"Missing parameter\"}");
            return;
        }

        String bambu_ip = request->getParam("bambu_ip")->value();
        String bambu_serialnr = request->getParam("bambu_serialnr")->value();
        String bambu_accesscode = request->getParam("bambu_accesscode")->value();
        bool autoSend = (request->getParam("autoSend")->value() == "true") ? true : false;
        String autoSendTime = request->getParam("autoSendTime")->value();
        
        bambu_ip.trim();
        bambu_serialnr.trim();
        bambu_accesscode.trim();
        autoSendTime.trim();

        if (bambu_ip.length() == 0 || bambu_serialnr.length() == 0 || bambu_accesscode.length() == 0) {
            request->send(400, "application/json", "{\"success\": false, \"error\": \"Empty parameter\"}");
            return;
        }

        bool success = saveBambuCredentials(bambu_ip, bambu_serialnr, bambu_accesscode, autoSend, autoSendTime);

        request->send(200, "application/json", "{\"healthy\": " + String(success ? "true" : "false") + "}");
    });

    // Route for checking the Spoolman instance
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
        ESP.restart();
    });

    // Route for charging the CSS file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Load style.css");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/style.css.gz", "text/css");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("style.css sent");
    });

    // Route for the logo
    server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/logo.png.gz", "image/png");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("logo.png sent");
    });

    // Route for Favicon
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/favicon.ico", "image/x-icon");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("favicon.ico sent");
    });

    // Route for spool_in.png
    server.on("/spool_in.png", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/spool_in.png.gz", "image/png");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("spool_in.png sent");
    });

    // Route for set_spoolman.png
    server.on("/set_spoolman.png", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/set_spoolman.png.gz", "image/png");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("set_spoolman.png sent");
    });

    // Route für JavaScript Dateien
    server.on("/spoolman.js", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Received request for /spoolman.js");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/spoolman.js.gz", "text/javascript");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("Spoolman.js sent");
    });

    server.on("/rfid.js", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Received request for /rfid.js");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS,"/rfid.js.gz", "text/javascript");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", CACHE_CONTROL);
        request->send(response);
        Serial.println("RFID.js sent");
    });

    // Simplified update handler
    server.on("/upgrade", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/upgrade.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Cache-Control", "no-store");
        request->send(response);
    });

    // Register update handler
    handleUpdate(server);

    server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest *request){
        String fm_version = VERSION;
        String jsonResponse = "{\"version\": \""+ fm_version +"\"}";
        request->send(200, "application/json", jsonResponse);
    });

    // Error processing for non -found pages
    server.onNotFound([](AsyncWebServerRequest *request){
        Serial.print("404 - not found: ");
        Serial.println(request->url());
        request->send(404, "text/plain", "Page not found");
    });

    // Web Socket Route
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    ws.enable(true);

    // Start the web server
    server.begin();
    Serial.println("Web server started");
}
