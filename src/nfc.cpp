#include "nfc.h"
#include <Arduino.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include "config.h"
#include "website.h"
#include "api.h"
#include "esp_task_wdt.h"
#include "scale.h"

//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

TaskHandle_t RfidReaderTask;

JsonDocument rfidData;
String spoolId = "";
String nfcJsonData = "";
volatile bool pauseBambuMqttTask = false;

volatile uint8_t hasReadRfidTag = 0;
//0 = not read
//1 = successfully read
//2 = error when reading
//3 = write
//4 = error when writing
//5 = successfully written
//6 = reading
//***** PN532


// ##### functions for RFID ######
void payloadToJson(uint8_t *data) {
    const char* startJson = strchr((char*)data, '{');
    const char* endJson = strrchr((char*)data, '}');
  
    if (startJson && endJson && endJson > startJson) {
      String jsonString = String(startJson, endJson - startJson + 1);
      //Serial.print("Cleaned JSON String: ");
      //Serial.println(jsonString);
  
      // Process JSON document
      JsonDocument doc;  // Adjust the size to the JSON content
      DeserializationError error = deserializeJson(doc, jsonString);
  
      if (!error) {
        const char* color_hex = doc["color_hex"];
        const char* type = doc["type"];
        int min_temp = doc["min_temp"];
        int max_temp = doc["max_temp"];
        const char* brand = doc["brand"];
  
        Serial.println();
        Serial.println("-----------------");
        Serial.println("JSON-Parsed Data:");
        Serial.println(color_hex);
        Serial.println(type);
        Serial.println(min_temp);
        Serial.println(max_temp);
        Serial.println(brand);
        Serial.println("-----------------");
        Serial.println();
      } else {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
      }
    } else {
        Serial.println("No valid JSON content found or incorrect formatting.");
        //writeJsonToTag("{\"version\":\"1.0\",\"protocol\":\"NFC\",\"color_hex\":\"#FFFFFF\",\"type\":\"Example\",\"min_temp\":10,\"max_temp\":30,\"brand\":\"BrandName\"}");
    }
  }

bool formatNdefTag() {
    uint8_t ndefInit[] = { 0x03, 0x00, 0xFE }; // NDEF initialization message
    bool success = true;
    int pageOffset = 4; // Start of NDEF data on NTAG2XX
  
    Serial.println();
    Serial.println("Format NDEF tag...");
  
    // Write the initialization message on the first page
    for (int i = 0; i < sizeof(ndefInit); i += 4) {
      if (!nfc.ntag2xx_WritePage(pageOffset + (i / 4), &ndefInit[i])) {
          success = false;
          break;
      }
    }
  
    return success;
  }

uint16_t readTagSize()
{
  uint8_t buffer[4];
  memset(buffer, 0, 4);
  nfc.ntag2xx_ReadPage(3, buffer);
  return buffer[2]*8;
}

uint8_t ntag2xx_WriteNDEF(const char *payload) {
  uint16_t tagSize = readTagSize();
  Serial.print("Tag Size: ");Serial.println(tagSize);

  uint8_t pageBuffer[4] = {0, 0, 0, 0};
  Serial.println("Start writing the NDEF message ...");
  
  // Figure out how long the string is
  uint8_t len = strlen(payload);
  Serial.print("Length of the Payoad: ");
  Serial.println(len);
  
  Serial.print("Payload: ");Serial.println(payload);

  // Setup the record header
  // See NFCForum-TS-Type-2-Tag_1.1.pdf for details
  uint8_t pageHeader[21] = {
    /* NDEF Message TLV - JSON Record */
    0x03, /* Tag Field (0x03 = NDEF Message) */
    (uint8_t)(len+3+16), /* Payload Length (including NDEF header) */
    0xD2, /* NDEF Record Header (TNF=0x2:MIME Media + SR + ME + MB) */
    0x10, /* Type Length for the record type indicator */
    (uint8_t)(len), /* Payload len */
    'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n'
  };

  // Make sure the URI payload will fit in dataLen (include 0xFE trailer)
  if ((len < 1) || (len + 1 > (tagSize - sizeof(pageHeader)))) 
  {
    Serial.println();
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println("Error: The payload does not fit in the data length.");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println();
    return 0;
  }

  // Combine header and payload
  int totalSize = sizeof(pageHeader) + len;
  uint8_t* combinedData = (uint8_t*) malloc(totalSize);
  if (combinedData == NULL) 
  {
    Serial.println("Error: not enough memory.");
    oledShowMessage("Tag too small");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    return 0;
  }

  // Combine Header und Payload
  memcpy(combinedData, pageHeader, sizeof(pageHeader));
  memcpy(&combinedData[sizeof(pageHeader)], payload, len);

  // Write the pages
  uint8_t a = 0;
  uint8_t i = 0;
  while (totalSize > 0) {
    memset(pageBuffer, 0, 4);
    int bytesToWrite = (totalSize < 4) ? totalSize : 4;
    memcpy(pageBuffer, combinedData + a, bytesToWrite);

    //uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    //uint8_t uidLength;
    //nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if (!(nfc.ntag2xx_WritePage(4+i, pageBuffer))) 
    {
      Serial.println("Error when writing the page.");
      free(combinedData);
      return 0;
    }

    yield();
    //esp_task_wdt_reset();

    i++;
    a += 4;
    totalSize -= bytesToWrite;
  }

  // Ensure the NDEF message is properly terminated
  memset(pageBuffer, 0, 4);
  pageBuffer[0] = 0xFE; // NDEF record footer
  if (!(nfc.ntag2xx_WritePage(4+i, pageBuffer))) 
  {
    Serial.println("Error when writing the end bits.");
    free(combinedData);
    return 0;
  }

  Serial.println("NDEF message written successfully.");
  free(combinedData);
  return 1;
}

bool decodeNdefAndReturnJson(const byte* encodedMessage) {
  byte typeLength = encodedMessage[3];
  byte payloadLength = encodedMessage[4];

  nfcJsonData = "";

  for (int i = 2; i < payloadLength+2; i++) 
  {
    nfcJsonData += (char)encodedMessage[3 + typeLength + i];
  }

  // Process JSON document
  JsonDocument doc;  // Adjust the size to the JSON content
  DeserializationError error = deserializeJson(doc, nfcJsonData);
  if (error) 
  {
    nfcJsonData = "";
    Serial.println("Error processing the JSON document");
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return false;
  } 
  else 
  {
    // Send the updated AMS data to all web socket clients
    Serial.println("JSON document successfully processed");
    Serial.println(doc.as<String>());
    if (doc["sm_id"] != "") 
    {
      Serial.println("SPOOL ID found: " + doc["sm_id"].as<String>());
      spoolId = doc["sm_id"].as<String>();
    } 
    else 
    {
      Serial.println("No spool ID found.");
      spoolId = "";
      oledShowMessage("Unknown Spool");
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }

  return true;
}

void writeJsonToTag(void *parameter) {
  const char* payload = (const char*)parameter;

  // Provide the created NDEF message
  Serial.println("Create NDEF Message...");
  Serial.println(payload);

  hasReadRfidTag = 3;
  vTaskSuspend(RfidReaderTask);
  vTaskDelay(50 / portTICK_PERIOD_MS);

  //pauseBambuMqttTask = true;
  // Update the website if the status changes
  sendNfcData(nullptr);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  oledShowMessage("Waiting for NFC-Tag");
  
  // Wait 10sec for tag
  uint8_t success = 0;
  String uidString = "";
  for (uint16_t i = 0; i < 20; i++) {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500);
    if (success) {
      for (uint8_t i = 0; i < uidLength; i++) {
        uidString += String(uid[i], HEX);
        if (i < uidLength - 1) {
            uidString += ":"; // Optional: Add the separator
        }
      }
      foundNfcTag(nullptr, success);
      break;
    }

    if (i == 0) oledShowMessage("Waiting for NFC-Tag");

    yield();
    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  if (success)
  {
    oledShowIcon("transfer");
    // Write the NDEF message on the tag
    success = ntag2xx_WriteNDEF(payload);
    if (success) 
    {
        Serial.println("NDEF Message successfully written on the tag");
        //oledShowMessage("NFC Tag written");
        oledShowIcon("success");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        hasReadRfidTag = 5;
        // Update the website if the status changes
        sendNfcData(nullptr);
        pauseBambuMqttTask = false;
        
        if (updateSpoolTagId(uidString, payload)) {
          uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
          uint8_t uidLength;
          oledShowIcon("success");
          while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
            yield();
          }
        }
          
        vTaskResume(RfidReaderTask);
        vTaskDelay(500 / portTICK_PERIOD_MS);        
    } 
    else 
    {
        Serial.println("Error when writing the NDEF message on the tag");
        oledShowIcon("failed");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        hasReadRfidTag = 4;
    }
  }
  else
  {
    Serial.println("Error: no tag to write.");
    oledShowMessage("No NFC Tag found");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    hasReadRfidTag = 0;
  }
  
  sendWriteResult(nullptr, success);
  sendNfcData(nullptr);

  vTaskResume(RfidReaderTask);
  pauseBambuMqttTask = false;

  vTaskDelete(NULL);
}

void startWriteJsonToTag(const char* payload) {
  char* payloadCopy = strdup(payload);
  
  // Task did not start several times
  if (hasReadRfidTag != 3) {
    // Create the task
    xTaskCreate(
        writeJsonToTag,        // Task function
        "WriteJsonToTagTask",       // Task name
        5115,                       // Stack size in bytes
        (void*)payloadCopy,         // Parameter
        rfidWriteTaskPrio,          // Priority
        NULL                        // Task handle (not required)
    );
  }
}

void scanRfidTask(void * parameter) {
  Serial.println("RFID Task gestartet");
  for(;;) {
    // Suspend loop when writing.
    if (hasReadRfidTag != 3)
    {
      yield();

      uint8_t success;
      uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
      uint8_t uidLength;

      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

      foundNfcTag(nullptr, success);
      
      if (success && hasReadRfidTag != 1)
      {
        // Display some basic information about the card
        Serial.println("Found an ISO14443A card");

        hasReadRfidTag = 6;

        oledShowIcon("transfer");
        vTaskDelay(500 / portTICK_PERIOD_MS);

        if (uidLength == 7)
        {
          uint16_t tagSize = readTagSize();
          if(tagSize > 0)
          {
            // Create a buffer depending on the size of the tag
            uint8_t* data = (uint8_t*)malloc(tagSize);
            memset(data, 0, tagSize);

            // We probably have an NTAG2xx card (though it could be Ultralight as well)
            Serial.println("Seems to be an NTAG2xx tag (7 byte UID)");
            
            uint8_t numPages = readTagSize()/4;
            for (uint8_t i = 4; i < 4+numPages; i++) {
              if (!nfc.ntag2xx_ReadPage(i, data+(i-4) * 4))
              {
                break; // Stop if reading fails
              }
              // Check for NDEF message end
              if (data[(i - 4) * 4] == 0xFE) 
              {
                break; // End of NDEF message
              }

              yield();
              esp_task_wdt_reset();
              vTaskDelay(pdMS_TO_TICKS(1));
            }

            if (!decodeNdefAndReturnJson(data)) 
            {
              oledShowMessage("NFC Tag unknown");
              vTaskDelay(2000 / portTICK_PERIOD_MS);
              hasReadRfidTag = 2;
            }
            else 
            {
              hasReadRfidTag = 1;
            }

            free(data);
          }
          else
          {
            oledShowMessage("NFC Tag read error");
            hasReadRfidTag = 2;
          }
        }
        else
        {
          Serial.println("This doesn't seem to be an NTAG2xx tag (UUID length != 7 bytes)!");
        }
      }

      if (!success && hasReadRfidTag > 0)
      {
        hasReadRfidTag = 0;
        //uidString = "";
        nfcJsonData = "";
        Serial.println("Tag removed");
        if (!autoSendToBambu) oledShowWeight(weight);
      }

      // Update the website if the status changes
      sendNfcData(nullptr);
    }
    yield();
  }
}

void startNfc() {
  nfc.begin();                                           // Start communication with RFID reader
  delay(1000);
  unsigned long versiondata = nfc.getFirmwareVersion();  // Read version number of the firmware
  if (! versiondata) {                                   // If there is no answer
    Serial.println("Can't find a RFID board!");          // Send text "Can't ..." to serial monitor
    //delay(5000);
    //ESP.restart();
    oledShowMessage("No RFID Board found");
    delay(2000);
  }
  else {
    Serial.print("Chip PN5 found"); Serial.println((versiondata >> 24) & 0xFF, HEX); // Send text and version information on serial
    Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);   // monitor when answer comes from the board
    Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);               // 

    nfc.SAMConfig();
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    //nfc.setPassiveActivationRetries(0x7F);
    //nfc.setPassiveActivationRetries(0xFF);

    BaseType_t result = xTaskCreatePinnedToCore(
      scanRfidTask, /* Function to implement the task */
      "RfidReader", /* Name of the task */
      5115,  /* Stack size in words */
      NULL,  /* Task input parameter */
      rfidTaskPrio,  /* Priority of the task */
      &RfidReaderTask,  /* Task handle. */
      rfidTaskCore); /* Core where the task should run */

      if (result != pdPASS) {
        Serial.println("Error creating the RFID tasks");
    } else {
        Serial.println("RFID Task successfully created");
    }
  }
}