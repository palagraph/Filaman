#include <Arduino.h>

/*
// Create Icons 
https://javl.github.io/image2cpp/
Size: 47x47
BG Color: Black
Invert: True
Ohters in default
*/

const unsigned char wifi_on [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x78, 0x1e, 0x60, 0x06, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x00, 0x00, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
  
const unsigned char wifi_off [] PROGMEM = {
  0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x1b, 0xf0, 0x3d, 0xfc, 0x7e, 0x1e, 0x63, 0x06, 0x07, 0xa0, 
  0x1f, 0xd8, 0x08, 0x60, 0x01, 0xb0, 0x03, 0xd8, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'off', 16x16px
const unsigned char bitmap_off [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'spoolman_on', 16x16px
const unsigned char bitmap_spoolman_on [] PROGMEM = {
	0x03, 0xc0, 0x08, 0xf0, 0x20, 0xfc, 0x00, 0xfc, 0x40, 0xfe, 0x70, 0xf0, 0xf8, 0xc1, 0xff, 0xc1, 
	0xff, 0xc1, 0xf9, 0xc1, 0x70, 0xf0, 0x40, 0xfe, 0x00, 0xfc, 0x20, 0xfc, 0x08, 0xf0, 0x03, 0xc0
};

// 'bambu_on', 16x16px
const unsigned char bitmap_bambu_on [] PROGMEM = {
  0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x1c, 0x3e, 0x00, 0x3e, 0x40, 
	0x30, 0x78, 0x00, 0x7c, 0x06, 0x7c, 0x1e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c
};

// 'failed', 47x47px
const unsigned char icon_failed [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x1f, 0xfc, 0x7f, 0xf0, 0x00, 0x00, 0x7f, 
  0x80, 0x03, 0xf8, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x3f, 0x00, 
  0x03, 0xf0, 0x00, 0x00, 0x1f, 0x80, 0x07, 0xc0, 0x00, 0x00, 0x0f, 0x80, 0x07, 0x80, 0x00, 0x00, 
  0x07, 0xc0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x1f, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x1e, 0x03, 
  0x80, 0x07, 0x80, 0xf0, 0x3e, 0x03, 0xc0, 0x07, 0x80, 0xf0, 0x3c, 0x03, 0xe0, 0x0f, 0x80, 0x78, 
  0x3c, 0x01, 0xf0, 0x1f, 0x00, 0x78, 0x78, 0x00, 0xf8, 0x3e, 0x00, 0x38, 0x78, 0x00, 0x7c, 0x7c, 
  0x00, 0x3c, 0x78, 0x00, 0x3c, 0xf8, 0x00, 0x3c, 0x78, 0x00, 0x3f, 0xf0, 0x00, 0x3c, 0x70, 0x00, 
  0x1f, 0xf0, 0x00, 0x3c, 0x70, 0x00, 0x0f, 0xe0, 0x00, 0x3c, 0xf0, 0x00, 0x07, 0xc0, 0x00, 0x1c, 
  0xf0, 0x00, 0x0f, 0xe0, 0x00, 0x3c, 0x70, 0x00, 0x1f, 0xf0, 0x00, 0x3c, 0x78, 0x00, 0x3f, 0xf0, 
  0x00, 0x3c, 0x78, 0x00, 0x3e, 0xf8, 0x00, 0x3c, 0x78, 0x00, 0x7c, 0x7c, 0x00, 0x3c, 0x78, 0x00, 
  0xf8, 0x3e, 0x00, 0x3c, 0x3c, 0x01, 0xf0, 0x1f, 0x00, 0x78, 0x3c, 0x03, 0xe0, 0x0f, 0x80, 0x78, 
  0x3e, 0x03, 0xc0, 0x0f, 0x80, 0xf0, 0x1e, 0x03, 0x80, 0x07, 0x80, 0xf0, 0x1f, 0x00, 0x00, 0x00, 
  0x01, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x07, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x07, 0xc0, 
  0x00, 0x00, 0x0f, 0xc0, 0x03, 0xf0, 0x00, 0x00, 0x1f, 0x80, 0x01, 0xf8, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x80, 0x03, 0xfc, 0x00, 0x00, 0x1f, 0xf8, 0x3f, 
  0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
  0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'loading', 47x47px
const unsigned char icon_loading [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 
  0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfe, 
  0x00, 0x00, 0x00, 0x3f, 0xf9, 0xfc, 0x00, 0x00, 0x00, 0x7f, 0x83, 0xf8, 0x38, 0x00, 0x00, 0xff, 
  0x07, 0xf0, 0x7c, 0x00, 0x00, 0xfc, 0x07, 0xe0, 0x7e, 0x00, 0x01, 0xf8, 0x0f, 0xc0, 0x3f, 0x00, 
  0x03, 0xf0, 0x07, 0x80, 0x1f, 0x00, 0x03, 0xe0, 0x03, 0x00, 0x1f, 0x80, 0x07, 0xe0, 0x00, 0x00, 
  0x0f, 0x80, 0x07, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0xc0, 0x00, 0x00, 0x07, 0xc0, 0x0f, 0x80, 
  0x00, 0x00, 0x07, 0xc0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xc0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 
  0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0x80, 0x00, 0x00, 
  0x03, 0xe0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0x80, 
  0x00, 0x00, 0x03, 0xc0, 0x0f, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x07, 0xc0, 0x00, 0x00, 0x07, 0xc0, 
  0x07, 0xc0, 0x00, 0x00, 0x07, 0xc0, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0x80, 0x03, 0xe0, 0x00, 0x00, 
  0x1f, 0x80, 0x03, 0xf0, 0x00, 0x00, 0x1f, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x3f, 0x00, 0x01, 0xfc, 
  0x00, 0x00, 0x7e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x7f, 0x80, 0x07, 0xf8, 0x00, 
  0x00, 0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 
  0xc0, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'success', 47x47px
const unsigned char icon_success [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
  0x80, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0xfc, 0x3f, 0xf0, 0x00, 0x00, 0x7f, 
  0x80, 0x03, 0xfc, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x3f, 0x00, 
  0x03, 0xf0, 0x00, 0x00, 0x1f, 0x80, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xc0, 0x07, 0xc0, 0x00, 0x00, 
  0x03, 0xc0, 0x0f, 0x80, 0x00, 0x00, 0x01, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x1e, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x1e, 0x00, 0x00, 0x00, 0xc0, 0xf8, 0x3c, 0x00, 0x00, 0x01, 0xe0, 0x78, 
  0x3c, 0x00, 0x00, 0x03, 0xe0, 0x78, 0x38, 0x00, 0x00, 0x03, 0xc0, 0x3c, 0x78, 0x00, 0x00, 0x07, 
  0xc0, 0x3c, 0x78, 0x00, 0x00, 0x0f, 0x80, 0x3c, 0x78, 0x00, 0x00, 0x1f, 0x00, 0x1c, 0x78, 0x00, 
  0x00, 0x1e, 0x00, 0x1c, 0x78, 0x0f, 0x00, 0x3e, 0x00, 0x1e, 0x78, 0x0f, 0x80, 0x7c, 0x00, 0x1e, 
  0x78, 0x0f, 0xc0, 0xf8, 0x00, 0x1e, 0x78, 0x07, 0xe0, 0xf0, 0x00, 0x1c, 0x78, 0x03, 0xf1, 0xf0, 
  0x00, 0x1c, 0x78, 0x01, 0xfb, 0xe0, 0x00, 0x3c, 0x78, 0x00, 0xff, 0xc0, 0x00, 0x3c, 0x38, 0x00, 
  0x7f, 0x80, 0x00, 0x3c, 0x3c, 0x00, 0x3f, 0x80, 0x00, 0x78, 0x3c, 0x00, 0x1f, 0x00, 0x00, 0x78, 
  0x1e, 0x00, 0x0e, 0x00, 0x00, 0xf8, 0x1e, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 
  0x01, 0xf0, 0x0f, 0x80, 0x00, 0x00, 0x01, 0xe0, 0x07, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x03, 0xe0, 
  0x00, 0x00, 0x07, 0xc0, 0x03, 0xf0, 0x00, 0x00, 0x1f, 0x80, 0x01, 0xf8, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x80, 0x03, 0xfc, 0x00, 0x00, 0x1f, 0xfc, 0x3f, 
  0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
  0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'transfer', 47x47px
const unsigned char icon_transfer [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x01, 
  0xfe, 0x00, 0x00, 0x3f, 0xe0, 0x07, 0xff, 0x80, 0x00, 0xff, 0xc0, 0x0f, 0xff, 0xc0, 0x01, 0xfb, 
  0xc0, 0x1f, 0x03, 0xe0, 0x03, 0xe3, 0x80, 0x3c, 0x00, 0xf0, 0x07, 0xc3, 0x80, 0x78, 0x00, 0x78, 
  0x0f, 0x80, 0x00, 0x70, 0x00, 0x78, 0x0f, 0x00, 0x00, 0x70, 0x00, 0x38, 0x1e, 0x00, 0x00, 0xf0, 
  0x00, 0x3c, 0x1c, 0x00, 0x00, 0xe0, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0xe0, 0x00, 0x1c, 0x38, 0x00, 
  0x00, 0xe0, 0x00, 0x3c, 0x38, 0x00, 0x00, 0xf0, 0x00, 0x3c, 0x38, 0x00, 0x00, 0x70, 0x00, 0x38, 
  0x38, 0x00, 0x00, 0x78, 0x00, 0x78, 0x38, 0x00, 0x00, 0x78, 0x00, 0x78, 0x30, 0x00, 0x00, 0x3c, 
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x1f, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfc, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x1e, 0x00, 0x00, 0x18, 0x38, 0x00, 
  0x0e, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x3c, 
  0x38, 0x00, 0x0e, 0x00, 0x00, 0x3c, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x38, 0x38, 0x00, 0x0e, 0x00, 
  0x00, 0x38, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x78, 0x38, 0x00, 0x0e, 0x00, 0x00, 0x70, 0x38, 0x00, 
  0x0e, 0x00, 0x00, 0xf0, 0x38, 0x00, 0x0e, 0x00, 0x01, 0xe0, 0x38, 0x00, 0x0e, 0x01, 0x83, 0xe0, 
  0x38, 0x00, 0x0e, 0x03, 0x87, 0xc0, 0x3c, 0x00, 0x1e, 0x03, 0x9f, 0x80, 0x1f, 0x80, 0xfc, 0x07, 
  0xff, 0x00, 0x1f, 0xff, 0xf8, 0x07, 0xfc, 0x00, 0x07, 0xff, 0xf0, 0x0f, 0xf0, 0x00, 0x01, 0xff, 
  0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
