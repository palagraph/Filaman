#ifndef BAMBU_H
#define BAMBU_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TrayData {
    uint8_t id;
    String tray_info_idx;
    String tray_type;
    String tray_sub_brands;
    String tray_color;
    int nozzle_temp_min;
    int nozzle_temp_max;
    String setting_id;
};

#define MAX_AMS 17  // 16 normale AMS + 1 externe Spule
extern String amsJsonData;  // Für die vorbereiteten JSON-Daten

struct AMSData {
    uint8_t ams_id;
    TrayData trays[4]; // Annahme: Maximal 4 Trays pro AMS
};

extern bool bambu_connected;

extern int ams_count;
extern AMSData ams_data[MAX_AMS];

bool loadBambuCredentials();
bool saveBambuCredentials(const String& bambu_ip, const String& bambu_serialnr, const String& bambu_accesscode);
bool setupMqtt();
void mqtt_loop(void * parameter);
bool setBambuSpool(String payload);

#endif
