#include "display.h"
#include "api.h"
#include <vector>
#include "icons.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool wifiOn = false;

void setupDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Stop the program if the display cannot be initialized

    }
    display.setTextColor(WHITE);
    display.clearDisplay();
    display.display();

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.

    display.setTextColor(WHITE);
    display.display();
    oledShowTopRow();
    oledShowMessage("FilaMan v" + String(VERSION));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

void oledclearline() {
    int x, y;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            display.drawPixel(x, y, BLACK);
        }
    }
    //Display.display ();

}

void oledcleardata() {
    int x, y;
    for (y = OLED_DATA_START; y < OLED_DATA_END; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            display.drawPixel(x, y, BLACK);
        }
    }
    //Display.display ();

}

int oled_center_h(String text) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    return (SCREEN_WIDTH - w) / 2;
}

int oled_center_v(String text) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, OLED_DATA_START, &x1, &y1, &w, &h);
    // Centation only in the data area between OLED_DATA_START and OLED_DATA_END

    return OLED_DATA_START + ((OLED_DATA_END - OLED_DATA_START - h) / 2);
}

std::vector<String> splitTextIntoLines(String text, uint8_t textSize) {
    std::vector<String> lines;
    display.setTextSize(textSize);
    
    // Divide text into words

    std::vector<String> words;
    int pos = 0;
    while (pos < text.length()) {
        // Spring spaces at the beginning

        while (pos < text.length() && text[pos] == ' ') pos++;
        
        // Find the next spaces

        int nextSpace = text.indexOf(' ', pos);
        if (nextSpace == -1) {
            // Last word

            if (pos < text.length()) {
                words.push_back(text.substring(pos));
            }
            break;
        }
        // Add word

        words.push_back(text.substring(pos, nextSpace));
        pos = nextSpace + 1;
    }
    
    // Combine words into lines

    String currentLine = "";
    for (size_t i = 0; i < words.size(); i++) {
        String testLine = currentLine;
        if (currentLine.length() > 0) testLine += " ";
        testLine += words[i];
        
        // Check whether this combination fits the line

        int16_t x1, y1;
        uint16_t lineWidth, h;
        display.getTextBounds(testLine, 0, OLED_DATA_START, &x1, &y1, &lineWidth, &h);
        
        if (lineWidth <= SCREEN_WIDTH) {
            // Still fits into this line

            currentLine = testLine;
        } else {
            // Start new line

            if (currentLine.length() > 0) {
                lines.push_back(currentLine);
                currentLine = words[i];
            } else {
                // A single word is too long

                lines.push_back(words[i]);
            }
        }
    }
    
    // Add last line

    if (currentLine.length() > 0) {
        lines.push_back(currentLine);
    }
    
    return lines;
}

void oledShowMultilineMessage(String message, uint8_t size) {
    std::vector<String> lines;
    int maxLines = 3;  // Maximum number of lines for Size 2

    
    // First test with current size

    lines = splitTextIntoLines(message, size);
    
    // If more than Maxlines lines, reduce text size

    if (lines.size() > maxLines && size > 1) {
        size = 1;
        lines = splitTextIntoLines(message, size);
    }
    
    // output

    display.setTextSize(size);
    int lineHeight = size * 8;
    int totalHeight = lines.size() * lineHeight;
    int startY = OLED_DATA_START + ((OLED_DATA_END - OLED_DATA_START - totalHeight) / 2);
    
    uint8_t lineDistance = (lines.size() == 2) ? 5 : 0;
    for (size_t i = 0; i < lines.size(); i++) {
        display.setCursor(oled_center_h(lines[i]), startY + (i * lineHeight) + (i == 1 ? lineDistance : 0));
        display.print(lines[i]);
    }
    
    display.display();
}

void oledShowMessage(String message, uint8_t size) {
    oledcleardata();
    display.setTextSize(size);
    display.setTextWrap(false);
    
    // Check whether text fits into a line

    int16_t x1, y1;
    uint16_t textWidth, h;
    display.getTextBounds(message, 0, 0, &x1, &y1, &textWidth, &h);
   
    // Text fits into a line?

    if (textWidth <= SCREEN_WIDTH) {
        display.setCursor(oled_center_h(message), oled_center_v(message));
        display.print(message);
        display.display();
    } else {
        oledShowMultilineMessage(message, size);
    }
}

void oledShowTopRow() {
    oledclearline();

    if (bambu_connected == 1) {
        display.drawBitmap(50, 0, bitmap_bambu_on , 16, 16, WHITE);
    } else {
        display.drawBitmap(50, 0, bitmap_off , 16, 16, WHITE);
    }

    if (spoolman_connected == 1) {
        display.drawBitmap(80, 0, bitmap_spoolman_on , 16, 16, WHITE);
    } else {
        display.drawBitmap(80, 0, bitmap_off , 16, 16, WHITE);
    }

    if (wifiOn == 1) {
        display.drawBitmap(107, 0, wifi_on , 16, 16, WHITE);
    } else {
        display.drawBitmap(107, 0, wifi_off , 16, 16, WHITE);
    }
    
    display.display();
}

void oledShowIcon(const char* icon) {
    oledcleardata();

    uint16_t iconSize = OLED_DATA_END-OLED_DATA_START;
    uint16_t iconStart = (SCREEN_WIDTH - iconSize) / 2;

    if (icon == "failed") {
        display.drawBitmap(iconStart, OLED_DATA_START, icon_failed , iconSize, iconSize, WHITE);
    }
    else if (icon == "success") {
        display.drawBitmap(iconStart, OLED_DATA_START, icon_success , iconSize, iconSize, WHITE);
    }
    else if (icon == "transfer") {
        display.drawBitmap(iconStart, OLED_DATA_START, icon_transfer , iconSize, iconSize, WHITE);
    }
    else if (icon == "loading") {
        display.drawBitmap(iconStart, OLED_DATA_START, icon_loading , iconSize, iconSize, WHITE);
    }

    display.display();
}

void oledShowWeight(uint16_t weight) {
    // Display weight

    oledcleardata();
    display.setTextSize(3);
    display.setCursor(oled_center_h(String(weight)+" g"), OLED_DATA_START+10);
    display.print(weight);
    display.print(" g");
    display.display();
}
