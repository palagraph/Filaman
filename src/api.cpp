#include "api.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "commonFS.h"

bool spoolman_connected = false;
String spoolmanUrl = "";

struct SendToApiParams {
    String httpType;
    String spoolsUrl;
    String updatePayload;
};

/*
    // Spoolman Data
    {
        "version":"1.0",
        "protocol":"openspool",
        "color_hex":"AF7933",
        "type":"ABS",
        "min_temp":175,
        "max_temp":275,
        "brand":"Overture"
    }

    // FilaMan Data
    {
        "version":"1.0",
        "protocol":"openspool",
        "color_hex":"AF7933",
        "type":"ABS",
        "min_temp":175,
        "max_temp":275,
        "brand":"Overture",
        "sm_id": 
    }
*/

JsonDocument fetchSingleSpoolInfo(int spoolId) {
    HTTPClient http;
    String spoolsUrl = spoolmanUrl + apiUrl + "/spool/" + spoolId;

    Serial.print("Call spool data from: ");
    Serial.println(spoolsUrl);

    http.begin(spoolsUrl);
    int httpCode = http.GET();

    JsonDocument filteredDoc;
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.print("Errors when parsing the JSON response: ");
            Serial.println(error.c_str());
        } else {
            String filamentType = doc["filament"]["material"].as<String>();
            String filamentBrand = doc["filament"]["vendor"]["name"].as<String>();

            int nozzle_temp_min = 0;
            int nozzle_temp_max = 0;
            if (doc["filament"]["extra"]["nozzle_temperature"].is<String>()) {
                String tempString = doc["filament"]["extra"]["nozzle_temperature"].as<String>();
                tempString.replace("[", "");
                tempString.replace("]", "");
                int commaIndex = tempString.indexOf(',');
                
                if (commaIndex != -1) {
                    nozzle_temp_min = tempString.substring(0, commaIndex).toInt();
                    nozzle_temp_max = tempString.substring(commaIndex + 1).toInt();
                }
            } 

            String filamentColor = doc["filament"]["color_hex"].as<String>();
            filamentColor.toUpperCase();

            String tray_info_idx = doc["filament"]["extra"]["bambu_idx"].as<String>();
            tray_info_idx.replace("\"", "");
            
            String cali_idx = doc["filament"]["extra"]["bambu_cali_id"].as<String>(); // "\"153\""
            cali_idx.replace("\"", "");
            
            String bambu_setting_id = doc["filament"]["extra"]["bambu_setting_id"].as<String>(); // "\"PFUSf40e9953b40d3d\""
            bambu_setting_id.replace("\"", "");

            doc.clear();

            filteredDoc["color"] = filamentColor;
            filteredDoc["type"] = filamentType;
            filteredDoc["nozzle_temp_min"] = nozzle_temp_min;
            filteredDoc["nozzle_temp_max"] = nozzle_temp_max;
            filteredDoc["brand"] = filamentBrand;
            filteredDoc["tray_info_idx"] = tray_info_idx;
            filteredDoc["cali_idx"] = cali_idx;
            filteredDoc["bambu_setting_id"] = bambu_setting_id;
        }
    } else {
        Serial.print("Error when retrieving the spool data. HTTP code: ");
        Serial.println(httpCode);
    }

    http.end();
    return filteredDoc;
}

void sendToApi(void *parameter) {
    SendToApiParams* params = (SendToApiParams*)parameter;

    // Extrahiere die Werte
    String httpType = params->httpType;
    String spoolsUrl = params->spoolsUrl;
    String updatePayload = params->updatePayload;
    

    HTTPClient http;
    http.begin(spoolsUrl);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(updatePayload);
    if (httpType == "PATCH") httpCode = http.PATCH(updatePayload);

    if (httpCode == HTTP_CODE_OK) {
        Serial.println("Spoolman successfully updated");
    } else {
        Serial.println("Error sending to Spoolman!");
        oledShowMessage("Spoolman Update Failed");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    http.end();

    // Share memory
    delete params;
    vTaskDelete(NULL);
}

bool updateSpoolTagId(String uidString, const char* payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.print("Errors in JSON PARSING: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // Check whether the required fields are available
    if (!doc["sm_id"].is<String>() || doc["sm_id"].as<String>() == "") {
        Serial.println("No spoolman ID found.");
        return false;
    }

    String spoolsUrl = spoolmanUrl + apiUrl + "/spool/" + doc["sm_id"].as<String>();
    Serial.print("Update spool with URL: ");
    Serial.println(spoolsUrl);
    
    // Create update payload
    JsonDocument updateDoc;
    updateDoc["extra"]["nfc_id"] = "\""+uidString+"\"";
    
    String updatePayload;
    serializeJson(updateDoc, updatePayload);
    Serial.print("Update Payload: ");
    Serial.println(updatePayload);

    SendToApiParams* params = new SendToApiParams();
    if (params == nullptr) {
        Serial.println("Error: Can not allocate memory for task parameters.");
        return false;
    }
    params->httpType = "PATCH";
    params->spoolsUrl = spoolsUrl;
    params->updatePayload = updatePayload;

    // Create the task
    BaseType_t result = xTaskCreate(
        sendToApi,                // Task function
        "SendToApiTask",          // Task name
        4096,                     // Stack size in bytes
        (void*)params,            // Parameter
        0,                        // Priority
        NULL                      // Task handle (not required)
    );

    return true;
}

uint8_t updateSpoolWeight(String spoolId, uint16_t weight) {
    String spoolsUrl = spoolmanUrl + apiUrl + "/spool/" + spoolId + "/measure";
    Serial.print("Update spool with URL: ");
    Serial.println(spoolsUrl);

    // Create update Payload
    JsonDocument updateDoc;
    updateDoc["weight"] = weight;
    
    String updatePayload;
    serializeJson(updateDoc, updatePayload);
    Serial.print("Update Payload: ");
    Serial.println(updatePayload);

    SendToApiParams* params = new SendToApiParams();
    if (params == nullptr) {
        Serial.println("Error: Can not allocate memory for task parameters.");
        return 0;
    }
    params->httpType = "PUT";
    params->spoolsUrl = spoolsUrl;
    params->updatePayload = updatePayload;

    // Create the task
    BaseType_t result = xTaskCreate(
        sendToApi,                // Task function
        "SendToApiTask",          // Task name
        4096,                     // Stack size in bytes
        (void*)params,            // Parameter
        0,                        // Priority
        NULL                      // Task handle (not required)
    );

    return 1;
}

bool updateSpoolBambuData(String payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("Errors in JSON PARSING: ");
        Serial.println(error.c_str());
        return false;
    }

    String spoolsUrl = spoolmanUrl + apiUrl + "/filament/" + doc["filament_id"].as<String>();
    Serial.print("Update spool with URL: ");
    Serial.println(spoolsUrl);

    JsonDocument updateDoc;
    updateDoc["extra"]["bambu_setting_id"] = "\"" + doc["setting_id"].as<String>() + "\"";
    updateDoc["extra"]["bambu_cali_id"] = "\"" + doc["cali_idx"].as<String>() + "\"";
    updateDoc["extra"]["bambu_idx"] = "\"" + doc["tray_info_idx"].as<String>() + "\"";
    updateDoc["extra"]["nozzle_temperature"] = "[" + doc["temp_min"].as<String>() + "," + doc["temp_max"].as<String>() + "]";

    String updatePayload;
    serializeJson(updateDoc, updatePayload);
    Serial.print("Update Payload: ");
    Serial.println(updatePayload);

    SendToApiParams* params = new SendToApiParams();
    if (params == nullptr) {
        Serial.println("Error: Can not allocate memory for task parameters.");
        return false;
    }
    params->httpType = "PATCH";
    params->spoolsUrl = spoolsUrl;
    params->updatePayload = updatePayload;

    // Create the task

    BaseType_t result = xTaskCreate(
        sendToApi,                // Task function
        "SendToApiTask",          // Task name
        4096,                     // Stack size in bytes
        (void*)params,            // Parameter
        0,                        // Priority
        NULL                      // Task handle (not required)
    );

    return true;
}

// #### Spoolman init
bool checkSpoolmanExtraFields() {
    HTTPClient http;
    String checkUrls[] = {
        spoolmanUrl + apiUrl + "/field/spool",
        spoolmanUrl + apiUrl + "/field/filament"
    };

    String spoolExtra[] = {
        "nfc_id"
    };

    String filamentExtra[] = {
        "nozzle_temperature",
        "price_meter",
        "price_gramm",
        "bambu_setting_id",
        "bambu_cali_id",
        "bambu_idx",
        "bambu_k",
        "bambu_flow_ratio",
        "bambu_max_volspeed"
    };

    String spoolExtraFields[] = {
        "{\"name\": \"NFC ID\","
        "\"key\": \"nfc_id\","
        "\"field_type\": \"text\"}"
    };

    String filamentExtraFields[] = {
        "{\"name\": \"Nozzle Temp\","
        "\"unit\": \"°C\","
        "\"field_type\": \"integer_range\","
        "\"default_value\": \"[190,230]\","
        "\"key\": \"nozzle_temperature\"}",

        "{\"name\": \"Price/m\","
        "\"unit\": \"€\","
        "\"field_type\": \"float\","
        "\"key\": \"price_meter\"}",
        
        "{\"name\": \"Price/g\","
        "\"unit\": \"€\","
        "\"field_type\": \"float\","
        "\"key\": \"price_gramm\"}",

        "{\"name\": \"Bambu Setting ID\","
        "\"field_type\": \"text\","
        "\"key\": \"bambu_setting_id\"}",

        "{\"name\": \"Bambu Cali ID\","
        "\"field_type\": \"text\","
        "\"key\": \"bambu_cali_id\"}",

        "{\"name\": \"Bambu Filament IDX\","
        "\"field_type\": \"text\","
        "\"key\": \"bambu_idx\"}",

        "{\"name\": \"Bambu k\","
        "\"field_type\": \"float\","
        "\"key\": \"bambu_k\"}",

        "{\"name\": \"Bambu Flow Ratio\","
        "\"field_type\": \"float\","
        "\"key\": \"bambu_flow_ratio\"}",

        "{\"name\": \"Bambu Max Vol. Speed\","
        "\"unit\": \"mm3/s\","
        "\"field_type\": \"integer\","
        "\"default_value\": \"12\","
        "\"key\": \"bambu_max_volspeed\"}"
    };

    Serial.println("Check extra fields ...");

    int urlLength = sizeof(checkUrls) / sizeof(checkUrls[0]);

    for (uint8_t i = 0; i < urlLength; i++) {
        Serial.println();
        Serial.println("-------- Check fields for "+checkUrls[i]+" --------");
        http.begin(checkUrls[i]);
        int httpCode = http.GET();
    
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (!error) {
                String* extraFields;
                String* extraFieldData;
                u16_t extraLength;

                if (i == 0) {
                    extraFields = spoolExtra;
                    extraFieldData = spoolExtraFields;
                    extraLength = sizeof(spoolExtra) / sizeof(spoolExtra[0]);
                } else {
                    extraFields = filamentExtra;
                    extraFieldData = filamentExtraFields;
                    extraLength = sizeof(filamentExtra) / sizeof(filamentExtra[0]);
                }

                for (uint8_t s = 0; s < extraLength; s++) {
                    bool found = false;
                    for (JsonObject field : doc.as<JsonArray>()) {
                        if (field["key"].is<String>() && field["key"] == extraFields[s]) {
                            Serial.println("Field found: " + extraFields[s]);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        Serial.println("Field not found: " + extraFields[s]);

                        // Add extra field
                        http.begin(checkUrls[i] + "/" + extraFields[s]);
                        http.addHeader("Content-Type", "application/json");
                        int httpCode = http.POST(extraFieldData[s]);

                         if (httpCode > 0) {
                            // Call the response code and message
                            String response = http.getString();
                            //Serial.println("HTTP-Code: " + String(httpCode));
                            //Serial.println("Answer: " + response);
                            if (httpCode != HTTP_CODE_OK) {

                                return false;
                            }
                        } else {
                            // Error when sending the request
                            Serial.println("Error when sending the request: " + String(http.errorToString(httpCode)));
                            return false;
                        }
                        //http.end();
                    }
                    yield();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
            }
        }
        http.end();  
    }
    
    Serial.println("-------- End review fields --------");
    Serial.println();

    return true;
}

bool checkSpoolmanInstance(const String& url) {
    HTTPClient http;
    String healthUrl = url + apiUrl + "/health";

    Serial.print("Check Spoolman instance at: ");
    Serial.println(healthUrl);

    http.begin(healthUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            oledShowMessage("Spoolman available");
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (!error && doc["status"].is<String>()) {
                const char* status = doc["status"];
                http.end();

                if (!checkSpoolmanExtraFields()) {
                    Serial.println("Error checking the extra fields.");

                    oledShowMessage("Spoolman Error creating Extrafields");
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                    
                    return false;
                }

                spoolman_connected = true;
                return strcmp(status, "healthy") == 0;
            }
        }
    }
    http.end();
    return false;
}

bool saveSpoolmanUrl(const String& url) {
    if (!checkSpoolmanInstance(url)) return false;

    JsonDocument doc;
    doc["url"] = url;
    Serial.print("Save URL in file: ");
    Serial.println(url);
    if (!saveJsonValue("/spoolman_url.json", doc)) {
        Serial.println("Error when saving the Spoolman URL.");
    }
    spoolmanUrl = url;

    return true;
}

String loadSpoolmanUrl() {
    JsonDocument doc;
    if (loadJsonValue("/spoolman_url.json", doc) && doc["url"].is<String>()) {
        return doc["url"].as<String>();
    }
    Serial.println("No valid Spoolman URL found.");
    return "";
}

bool initSpoolman() {
    spoolmanUrl = loadSpoolmanUrl();
    spoolmanUrl.trim();
    if (spoolmanUrl == "") {
        Serial.println("No spoolman url found.");
        return false;
    }

    bool success = checkSpoolmanInstance(spoolmanUrl);
    if (!success) {
        Serial.println("Spoolman cannot be reached.");
        return false;
    }

    oledShowTopRow();
    return true;
}