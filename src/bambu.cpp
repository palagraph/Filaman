#include "bambu.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <SSLClient.h>
#include "bambu_cert.h"
#include "website.h"
#include "nfc.h"
#include "commonFS.h"
#include "esp_task_wdt.h"
#include "config.h"
#include "display.h"

WiFiClient espClient;
SSLClient sslClient(&espClient);
PubSubClient client(sslClient);

TaskHandle_t BambuMqttTask;

String report_topic = "";
//String request_topic = "";

const char* bambu_username = "bblp";
const char* bambu_ip = nullptr;
const char* bambu_accesscode = nullptr;
const char* bambu_serialnr = nullptr;
bool bambu_connected = false;
bool autoSendToBambu = false;
int autoSetToBambuSpoolId = 0;

// Global variables for AMS data

int ams_count = 0;
String amsJsonData;  // Saves the finished JSON for webocket clients
AMSData ams_data[MAX_AMS];  // Definition of the array;


bool saveBambuCredentials(const String& ip, const String& serialnr, const String& accesscode, bool autoSend, const String& autoSendTime) {
    if (BambuMqttTask) {
        vTaskDelete(BambuMqttTask);
    }
    
    JsonDocument doc;
    doc["bambu_ip"] = ip;
    doc["bambu_accesscode"] = accesscode;
    doc["bambu_serialnr"] = serialnr;
    doc["autoSendToBambu"] = autoSend;
    doc["autoSendTime"] = (autoSendTime != "") ? autoSendTime.toInt() : autoSetBambuAmsCounter;

    if (!saveJsonValue("/bambu_credentials.json", doc)) {
        Serial.println("Errors when saving the Bambu credentials.");
        return false;
    }

    // Dynamic memory allocation for global pointers

    bambu_ip = ip.c_str();
    bambu_accesscode = accesscode.c_str();
    bambu_serialnr = serialnr.c_str();
    autoSendToBambu = autoSend;
    autoSetBambuAmsCounter = autoSendTime.toInt();

    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (!setupMqtt()) return false;

    return true;
}

bool loadBambuCredentials() {
    JsonDocument doc;
    if (loadJsonValue("/bambu_credentials.json", doc) && doc["bambu_ip"].is<String>()) {
        // Temporary strings for the values

        String ip = doc["bambu_ip"].as<String>();
        String code = doc["bambu_accesscode"].as<String>();
        String serial = doc["bambu_serialnr"].as<String>();
        if (doc["autoSendToBambu"].is<bool>()) autoSendToBambu = doc["autoSendToBambu"].as<bool>();
        if (doc["autoSendTime"].is<int>()) autoSetBambuAmsCounter = doc["autoSendTime"].as<int>();

        ip.trim();
        code.trim();
        serial.trim();

        // Dynamic memory allocation for global pointers

        bambu_ip = strdup(ip.c_str());
        bambu_accesscode = strdup(code.c_str());
        bambu_serialnr = strdup(serial.c_str());

        report_topic = "device/" + String(bambu_serialnr) + "/report";
        //request_topic = "device/" + String(bambu_serialnr) + "/request";

        return true;
    }
    Serial.println("Keine gültigen Bambu-Credentials gefunden.");
    return false;
}

struct FilamentResult {
    String key;
    String type;
};

FilamentResult findFilamentIdx(String brand, String type) {
    // Create the JSON document for the filament data

    JsonDocument doc;
    
    // Load the own_filaments.json

    String ownFilament = "";
    if (!loadJsonValue("/own_filaments.json", doc)) 
    {
        Serial.println("Fehler beim Laden der eigenen Filament-Daten");
    }
    else
    {
        // Search through the type as a key

        if (doc[type].is<String>()) {
            ownFilament = doc[type].as<String>();
        }
        doc.clear();
    }

    // Load the bambu_filaments.json

    if (!loadJsonValue("/bambu_filaments.json", doc)) 
    {
        Serial.println("Fehler beim Laden der Filament-Daten");
        return {"GFL99", "PLA"}; // Fallback on Generic Pla

    }

    // If your own type

    if (ownFilament != "")
    {
        if (doc[ownFilament].is<String>()) 
        {
            return {ownFilament, doc[ownFilament].as<String>()};
        }
    }

    // 1. First we try to find the exact brand + type combination

    String searchKey;
    if (brand == "Bambu" || brand == "Bambulab") {
        searchKey = "Bambu " + type;
    } else if (brand == "PolyLite") {
        searchKey = "PolyLite " + type;
    } else if (brand == "eSUN") {
        searchKey = "eSUN " + type;
    } else if (brand == "Overture") {
        searchKey = "Overture " + type;
    } else if (brand == "PolyTerra") {
        searchKey = "PolyTerra " + type;
    }

    // Search through all entries according to the Brand + Type combination

    for (JsonPair kv : doc.as<JsonObject>()) {
        if (kv.value().as<String>() == searchKey) {
            return {kv.key().c_str(), kv.value().as<String>()};
        }
    }

    // 2. If not found, disassemble the Type String into words and search for every word
    // Collect all existing filament types from the JSON

    std::vector<String> knownTypes;
    for (JsonPair kv : doc.as<JsonObject>()) {
        String value = kv.value().as<String>();
        // Extract the type without a brand name

        if (value.indexOf(" ") != -1) {
            value = value.substring(value.indexOf(" ") + 1);
        }
        if (!value.isEmpty()) {
            knownTypes.push_back(value);
        }
    }

    // Dismantle the input type into words

    String typeStr = type;
    typeStr.trim();
    
    // Search for every known filament, whether it occurs in the input

    for (const String& knownType : knownTypes) {
        if (typeStr.indexOf(knownType) != -1) {
            // Search for this type in the original Json

            for (JsonPair kv : doc.as<JsonObject>()) {
                String value = kv.value().as<String>();
                if (value.indexOf(knownType) != -1) {
                    return {kv.key().c_str(), knownType};
                }
            }
        }
    }

    // 3. If nothing still found, return GFL99 (Generic Pla)

    return {"GFL99", "PLA"};
}

bool sendMqttMessage(String payload) {
    Serial.println("Sending MQTT message");
    Serial.println(payload);
    if (client.publish(report_topic.c_str(), payload.c_str())) 
    {
        return true;
    }
    
    return false;
}

bool setBambuSpool(String payload) {
    Serial.println("Spool settings in");
    Serial.println(payload);

    // PARSE THE JSON

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
        return false;
    }

    int amsId = doc["amsId"];
    int trayId = doc["trayId"];
    String color = doc["color"].as<String>();
    color.toUpperCase();
    int minTemp = doc["nozzle_temp_min"];
    int maxTemp = doc["nozzle_temp_max"];
    String type = doc["type"].as<String>();
    (type == "PLA+") ? type = "PLA" : type;
    String brand = doc["brand"].as<String>();
    String tray_info_idx = (doc["tray_info_idx"].as<String>() != "-1") ? doc["tray_info_idx"].as<String>() : "";
    if (tray_info_idx == "") {
        if (brand != "" && type != "") {
            FilamentResult result = findFilamentIdx(brand, type);
            tray_info_idx = result.key;
            type = result.type;  // Update the type with the basic type found

        }
    }
    String setting_id = doc["bambu_setting_id"].as<String>();
    String cali_idx = doc["cali_idx"].as<String>();

    doc.clear();

    doc["print"]["sequence_id"] = "0";
    doc["print"]["command"] = "ams_filament_setting";
    doc["print"]["ams_id"] = amsId < 200 ? amsId : 255;
    doc["print"]["tray_id"] = trayId < 200 ? trayId : 254;
    doc["print"]["tray_color"] = color.length() == 8 ? color : color+"FF";
    doc["print"]["nozzle_temp_min"] = minTemp;
    doc["print"]["nozzle_temp_max"] = maxTemp;
    doc["print"]["tray_type"] = type;
    //doc ["print"] ["Cali_idx"] = (Cali_idx! = "")? Cali_idx: "";

    doc["print"]["tray_info_idx"] = tray_info_idx;
    doc["print"]["setting_id"] = setting_id;
    
    // Serialize the JSON

    String output;
    serializeJson(doc, output);

    if (sendMqttMessage(output)) {
        Serial.println("Spool successfully set");
    }
    else
    {
        Serial.println("Failed to set spool");
        return false;
    }
    
    doc.clear();
    yield();

    if (cali_idx != "") {
        yield();
        doc["print"]["sequence_id"] = "0";
        doc["print"]["command"] = "extrusion_cali_sel";
        doc["print"]["filament_id"] = tray_info_idx;
        doc["print"]["nozzle_diameter"] = "0.4";
        doc["print"]["cali_idx"] = cali_idx.toInt();
        doc["print"]["tray_id"] = trayId < 200 ? trayId : 254;
        //doc ["print"] ["ams_id"] = amsid <200? AMSID: 255;


        // Serialize the JSON

        String output;
        serializeJson(doc, output);

        if (sendMqttMessage(output)) {
            Serial.println("Extrusion calibration successfully set");
        }
        else
        {
            Serial.println("Failed to set extrusion calibration");
            return false;
        }

        doc.clear();
        yield();
    }

    return true;
}

void autoSetSpool(int spoolId, uint8_t trayId) {
    // When new spools recognized and autosettobambu> 0

    JsonDocument spoolInfo = fetchSingleSpoolInfo(spoolId);

    if (!spoolInfo.isNull())
    {
        // AMS and Tray ID complement

        spoolInfo["amsId"] = 0;
        spoolInfo["trayId"] = trayId;

        Serial.println("Auto set spool");
        Serial.println(spoolInfo.as<String>());

        setBambuSpool(spoolInfo.as<String>());
    }

    // Reset the ID with it completed

    autoSetToBambuSpoolId = 0;
}

// Init

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String message;

    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    // JSON document Parsen

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) 
    {
        Serial.print("Fehler beim Parsen des JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // When Bambu Auto Set Spool is actively recognized and a spool and MQTT report the new spool in the AMS

    if (autoSendToBambu && autoSetToBambuSpoolId > 0 && 
        doc["print"]["command"].as<String>() == "push_status" && doc["print"]["ams"]["tray_pre"].as<uint8_t>()
        && !doc["print"]["ams"]["ams"].as<JsonArray>())
    {
        autoSetSpool(autoSetToBambuSpoolId, doc["print"]["ams"]["tray_pre"].as<uint8_t>());
    }

    // Check whether "print-> upgrade_state" and "print.ams.ams" exist

    if (doc["print"]["upgrade_state"].is<JsonObject>()) 
    {
        // Check whether there are AMS data

        if (!doc["print"]["ams"].is<JsonObject>() || !doc["print"]["ams"]["ams"].is<JsonArray>()) 
        {
            return;
        }

        JsonArray amsArray = doc["print"]["ams"]["ams"].as<JsonArray>();
        
        // Check whether the AMS data have changed

        bool hasChanges = false;
        
        // Compare every AMS and its trays

        for (int i = 0; i < amsArray.size() && !hasChanges; i++) {
            JsonObject amsObj = amsArray[i];
            int amsId = amsObj["id"].as<uint8_t>();
            JsonArray trayArray = amsObj["tray"].as<JsonArray>();
            
            // Find the corresponding AMS in our data

            int storedIndex = -1;
            for (int k = 0; k < ams_count; k++) {
                if (ams_data[k].ams_id == amsId) {
                    storedIndex = k;
                    break;
                }
            }
            
            if (storedIndex == -1) {
                hasChanges = true;
                break;
            }

            // Compare the trays

            for (int j = 0; j < trayArray.size() && j < 4 && !hasChanges; j++) {
                JsonObject trayObj = trayArray[j];
                if (trayObj["tray_info_idx"].as<String>() != ams_data[storedIndex].trays[j].tray_info_idx ||
                    trayObj["tray_type"].as<String>() != ams_data[storedIndex].trays[j].tray_type ||
                    trayObj["tray_color"].as<String>() != ams_data[storedIndex].trays[j].tray_color ||
                    trayObj["cali_idx"].as<String>() != ams_data[storedIndex].trays[j].cali_idx) {
                    hasChanges = true;
                    break;
                }
            }
        }

        // Check the external spool

        if (!hasChanges && doc["print"]["vt_tray"].is<JsonObject>()) {
            JsonObject vtTray = doc["print"]["vt_tray"];
            bool foundExternal = false;
            
            for (int i = 0; i < ams_count; i++) {
                if (ams_data[i].ams_id == 255) {
                    foundExternal = true;
                    if (vtTray["tray_info_idx"].as<String>() != ams_data[i].trays[0].tray_info_idx ||
                        vtTray["tray_type"].as<String>() != ams_data[i].trays[0].tray_type ||
                        vtTray["tray_color"].as<String>() != ams_data[i].trays[0].tray_color ||
                        (vtTray["tray_type"].as<String>() != "" && vtTray["cali_idx"].as<String>() != ams_data[i].trays[0].cali_idx)) {
                        hasChanges = true;
                    }
                    break;
                }
            }
            //If (! FOUNDEXTernal) Haschanges = True;

        }

        if (!hasChanges) return;

        // Continue with the existing workmanship, since changes were found

        ams_count = amsArray.size();
        
        for (int i = 0; i < ams_count && i < 16; i++) {
            JsonObject amsObj = amsArray[i];
            JsonArray trayArray = amsObj["tray"].as<JsonArray>();

            ams_data[i].ams_id = i; // Set the AMS ID

            for (int j = 0; j < trayArray.size() && j < 4; j++) { // Acceptance: a maximum of 4 trays per AMS

                JsonObject trayObj = trayArray[j];

                ams_data[i].trays[j].id = trayObj["id"].as<uint8_t>();
                ams_data[i].trays[j].tray_info_idx = trayObj["tray_info_idx"].as<String>();
                ams_data[i].trays[j].tray_type = trayObj["tray_type"].as<String>();
                ams_data[i].trays[j].tray_sub_brands = trayObj["tray_sub_brands"].as<String>();
                ams_data[i].trays[j].tray_color = trayObj["tray_color"].as<String>();
                ams_data[i].trays[j].nozzle_temp_min = trayObj["nozzle_temp_min"].as<int>();
                ams_data[i].trays[j].nozzle_temp_max = trayObj["nozzle_temp_max"].as<int>();
                ams_data[i].trays[j].setting_id = trayObj["setting_id"].as<String>();
                ams_data[i].trays[j].cali_idx = trayObj["cali_idx"].as<String>();
            }
        }
        
        // Put AMS_COUNT on the number of normal AMS

        ams_count = amsArray.size();

        // If there are external spools, add them

        if (doc["print"]["vt_tray"].is<JsonObject>()) {
            JsonObject vtTray = doc["print"]["vt_tray"];
            int extIdx = ams_count;  // Index for external spool

            ams_data[extIdx].ams_id = 255;  // Special ID for external spool

            ams_data[extIdx].trays[0].id = 254;  // Special ID for external tray

            ams_data[extIdx].trays[0].tray_info_idx = vtTray["tray_info_idx"].as<String>();
            ams_data[extIdx].trays[0].tray_type = vtTray["tray_type"].as<String>();
            ams_data[extIdx].trays[0].tray_sub_brands = vtTray["tray_sub_brands"].as<String>();
            ams_data[extIdx].trays[0].tray_color = vtTray["tray_color"].as<String>();
            ams_data[extIdx].trays[0].nozzle_temp_min = vtTray["nozzle_temp_min"].as<int>();
            ams_data[extIdx].trays[0].nozzle_temp_max = vtTray["nozzle_temp_max"].as<int>();

            if (doc["print"]["vt_tray"]["tray_type"].as<String>() != "")
            {
                ams_data[extIdx].trays[0].setting_id = vtTray["setting_id"].as<String>();
                ams_data[extIdx].trays[0].cali_idx = vtTray["cali_idx"].as<String>();
            }
            else
            {
                ams_data[extIdx].trays[0].setting_id = "";
                ams_data[extIdx].trays[0].cali_idx = "";
            }
            ams_count++;  // Increase ams_count for the external spool

        }

        // Create JSON for web socket clients

        JsonDocument wsDoc;
        JsonArray wsArray = wsDoc.to<JsonArray>();

        for (int i = 0; i < ams_count; i++) {
            JsonObject amsObj = wsArray.add<JsonObject>();
            amsObj["ams_id"] = ams_data[i].ams_id;

            JsonArray trays = amsObj["tray"].to<JsonArray>();
            int maxTrays = (ams_data[i].ams_id == 255) ? 1 : 4;
            
            for (int j = 0; j < maxTrays; j++) {
                JsonObject trayObj = trays.add<JsonObject>();
                trayObj["id"] = ams_data[i].trays[j].id;
                trayObj["tray_info_idx"] = ams_data[i].trays[j].tray_info_idx;
                trayObj["tray_type"] = ams_data[i].trays[j].tray_type;
                trayObj["tray_sub_brands"] = ams_data[i].trays[j].tray_sub_brands;
                trayObj["tray_color"] = ams_data[i].trays[j].tray_color;
                trayObj["nozzle_temp_min"] = ams_data[i].trays[j].nozzle_temp_min;
                trayObj["nozzle_temp_max"] = ams_data[i].trays[j].nozzle_temp_max;
                trayObj["setting_id"] = ams_data[i].trays[j].setting_id;
                trayObj["cali_idx"] = ams_data[i].trays[j].cali_idx;
            }
        }

        serializeJson(wsArray, amsJsonData);
        Serial.println("AMS data updated");
        sendAmsData(nullptr);
    }
    
    // New condition for ams_filament_setting

    if (doc["print"]["command"] == "ams_filament_setting") {
        int amsId = doc["print"]["ams_id"].as<int>();
        int trayId = doc["print"]["tray_id"].as<int>();
        String settingId = doc["print"]["setting_id"].as<String>();

        // Find the corresponding AMS and Tray

        for (int i = 0; i < ams_count; i++) {
            if (ams_data[i].ams_id == amsId) {
                // Update Setting_ID in the corresponding tray

                ams_data[i].trays[trayId].setting_id = settingId;
                
                // Create new JSON for website clients

                JsonDocument wsDoc;
                JsonArray wsArray = wsDoc.to<JsonArray>();

                for (int j = 0; j < ams_count; j++) {
                    JsonObject amsObj = wsArray.add<JsonObject>();
                    amsObj["ams_id"] = ams_data[j].ams_id;

                    JsonArray trays = amsObj["tray"].to<JsonArray>();
                    int maxTrays = (ams_data[j].ams_id == 255) ? 1 : 4;
                    
                    for (int k = 0; k < maxTrays; k++) {
                        JsonObject trayObj = trays.add<JsonObject>();
                        trayObj["id"] = ams_data[j].trays[k].id;
                        trayObj["tray_info_idx"] = ams_data[j].trays[k].tray_info_idx;
                        trayObj["tray_type"] = ams_data[j].trays[k].tray_type;
                        trayObj["tray_sub_brands"] = ams_data[j].trays[k].tray_sub_brands;
                        trayObj["tray_color"] = ams_data[j].trays[k].tray_color;
                        trayObj["nozzle_temp_min"] = ams_data[j].trays[k].nozzle_temp_min;
                        trayObj["nozzle_temp_max"] = ams_data[j].trays[k].nozzle_temp_max;
                        trayObj["setting_id"] = ams_data[j].trays[k].setting_id;
                        trayObj["cali_idx"] = ams_data[j].trays[k].cali_idx;
                    }
                }

                // Update the global Amsjsondata

                amsJsonData = "";
                serializeJson(wsArray, amsJsonData);
                
                // Send to webocket clients

                Serial.println("Filament setting updated");
                sendAmsData(nullptr);
                break;
            }
        }
    }
}

void reconnect() {
    // Loop unstil we're reconnected

    while (!client.connected()) {
        Serial.println("Attempting MQTT connection...");
        bambu_connected = false;
        oledShowTopRow();

        // Attempt to connect

        if (client.connect(bambu_serialnr, bambu_username, bambu_accesscode)) {
            Serial.println("... re-connected");
            // ... and resubscribe

            client.subscribe(report_topic.c_str());
            bambu_connected = true;
            oledShowTopRow();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            bambu_connected = false;
            oledShowTopRow();
            // Wait 5 seconds before retrying

            yield();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

void mqtt_loop(void * parameter) {
    for(;;) {
        if (pauseBambuMqttTask) {
            vTaskDelay(10000);
        }

        if (!client.connected()) {
            reconnect();
            yield();
            esp_task_wdt_reset();
            vTaskDelay(100);
        }
        client.loop();
        yield();
        vTaskDelay(100);
    }
}

bool setupMqtt() {
    // If BAMBU data is available

    bool success = loadBambuCredentials();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (!success) {
        Serial.println("Failed to load Bambu credentials");
        oledShowMessage("Bambu Credentials Missing");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return false;
    }

    if (success && bambu_ip != "" && bambu_accesscode != "" && bambu_serialnr != "") 
    {
        sslClient.setCACert(root_ca);
        sslClient.setInsecure();
        client.setServer(bambu_ip, 8883);

        // Connect to the MQTT server

        bool connected = true;
        if (client.connect(bambu_serialnr, bambu_username, bambu_accesscode)) 
        {
            client.setCallback(mqtt_callback);
            client.setBufferSize(5120);
            // Optional: Subscribe to Topic

            client.subscribe(report_topic.c_str());
            //Client.subscribe(request topic.c str());

            Serial.println("MQTT-Client initialisiert");

            oledShowMessage("Bambu Connected");
            bambu_connected = true;
            oledShowTopRow();

            xTaskCreatePinnedToCore(
                mqtt_loop, /* Function to implement the task */
                "BambuMqtt", /* Name of the task */
                10000,  /* Stack size in words */
                NULL,  /* Task Input parameter */
                mqttTaskPrio,  /* Priority of the task */
                &BambuMqttTask,  /* Task handle. */
                mqttTaskCore); /* Core Where the Task Should Run */
        } 
        else 
        {
            Serial.println("Fehler: Konnte sich nicht beim MQTT-Server anmelden");
            oledShowMessage("Bambu Connection Failed");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            connected = false;
            oledShowTopRow();
        }

        if (!connected) return false;
    } 
    else 
    {
        Serial.println("Fehler: Keine MQTT-Daten vorhanden");
        oledShowMessage("Bambu Credentials Missing");
        oledShowTopRow();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return false;
    }
    return true;
}

void bambu_restart() {
    if (BambuMqttTask) {
        vTaskDelete(BambuMqttTask);
    }
    setupMqtt();
}