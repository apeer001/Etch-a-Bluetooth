#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "Adafruit_LCD.h"

// --------- Definitions------------------------------------

// Define terms to be used with LCD
#define BLACK 1
#define WHITE 0

#define LOW 0
#define HIGH 1

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

// --------- Definitions End ------------------------------------

// ---------LCD Screen BUFFER------------------------------------

// the memory buffer for the LCD
// Working buffer throughout the program
uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xFC, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
	0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x7F,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x87, 0x8F, 0x9F, 0x9F, 0xFF, 0xFF, 0xFF,
	0xC1, 0xC0, 0xE0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE,
	0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xC0, 0xE0, 0xF1, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x87,
	0xE7, 0xFF, 0xFF, 0xFF, 0x1F, 0x1F, 0x3F, 0xF9, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xFD, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
	0x7E, 0x3F, 0x3F, 0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFC, 0xF0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// Bitmaps for extra content
const uint8_t Pokemon [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xA0, 0xF0, 0xF0,
	0x70, 0xF0, 0xF0, 0x70, 0xF0, 0xF0, 0xF0, 0xF0, 0x70, 0xF0, 0xF0, 0x70, 0xF0, 0x70, 0xC0, 0xC0,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xC0, 0x80, 0xC0, 0x80, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x90, 0x70, 0x30, 0x7A, 0x3E, 0x7E, 0x07,
	0x0D, 0x07, 0x2A, 0x3F, 0x3D, 0xE2, 0x5F, 0xEB, 0xDC, 0xFB, 0xD6, 0xED, 0xDB, 0xF6, 0xDD, 0xE7,
	0xBD, 0xEB, 0xDB, 0xF7, 0xEB, 0xFE, 0xFE, 0xD4, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0xC0, 0x80, 0x70, 0x70, 0x30, 0x70, 0x70, 0x70, 0x70, 0x70, 0x0E, 0x06, 0x0E,
	0x0E, 0x0E, 0x0E, 0x7C, 0x7E, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0x2F, 0x0E, 0x0E, 0x0E, 0x06, 0x0E,
	0xCE, 0x86, 0x4E, 0x7E, 0x7E, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x0E, 0x06,
	0xAE, 0xFE, 0xFE, 0x0E, 0x04, 0x8E, 0xF4, 0xFE, 0xA4, 0x0F, 0x05, 0x05, 0x01, 0x01, 0xFF, 0xFF,
	0xAF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x04, 0x00,
	0x00, 0x80, 0x80, 0x54, 0x7E, 0x7C, 0x29, 0x03, 0x01, 0xC0, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x28, 0x7E, 0x7E, 0xAD, 0x8F, 0x8F, 0x10, 0x00, 0x90, 0xF0,
	0xF0, 0xCA, 0x8E, 0xCC, 0x83, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0D, 0x0F, 0x1F, 0x70, 0x70, 0x50, 0x83, 0x81, 0xC1, 0x80, 0x80, 0x80,
	0xC0, 0x80, 0x83, 0x81, 0xC1, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0x5F, 0x0F, 0x3F, 0xF1, 0xF1, 0x51,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x0F, 0x2D, 0xF0, 0xF0, 0xA0, 0x00, 0x00, 0x01, 0x43, 0x81,
	0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x20, 0x70, 0x30, 0x70, 0x20, 0x70, 0x37, 0x7F, 0x2F,
	0xFE, 0xFE, 0xFE, 0xF1, 0xA3, 0x71, 0xF3, 0xF1, 0x8F, 0x8F, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x70, 0x60, 0xAC, 0x8E, 0x9C, 0x0E, 0x0C, 0x1D, 0x7F, 0x7F,
	0x5F, 0x8F, 0xCF, 0x8F, 0x8F, 0x9F, 0xFF, 0xFF, 0xD7, 0x81, 0x83, 0x81, 0x83, 0x81, 0x7F, 0x7C,
	0x5F, 0x0F, 0x1F, 0xA5, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x80, 0x80, 0x55, 0x7F, 0x7F, 0x74, 0x70,
	0x70, 0x8C, 0xCE, 0x8C, 0x0E, 0x0E, 0x4C, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0E, 0xCC, 0x86,
	0x8C, 0xDE, 0xFC, 0xFC, 0x7F, 0x7F, 0x7F, 0xFD, 0xFB, 0xAF, 0x0F, 0x0F, 0x01, 0x03, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x0E,
	0x1C, 0x0E, 0x1C, 0x0C, 0x1F, 0x0F, 0x1F, 0x03, 0x03, 0x01, 0x03, 0x03, 0x03, 0x01, 0x03, 0x01,
	0x03, 0x03, 0x0C, 0x0E, 0x1C, 0x0E, 0x0C, 0x05, 0x03, 0x03, 0x01, 0x00, 0x00, 0x03, 0x03, 0x0D,
	0x1C, 0x0C, 0x1E, 0x0C, 0x1C, 0x03, 0x01, 0x13, 0x1C, 0x0C, 0x1C, 0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x0C, 0x0C, 0x2E, 0x71, 0x73, 0x63, 0x71, 0x73, 0x0C, 0x0E, 0x0A, 0x03, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// Bitmap for the start screen part 1 of 2
const uint8_t StartScreen1 [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x60, 0x70, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x80,
	0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x81, 0xF1, 0xFE, 0xFF, 0x3F, 0x07, 0x01, 0xC3, 0xFF, 0xFF, 0x7F, 0x1E, 0xC0, 0xF0,
	0xFE, 0x00, 0x00, 0xC0, 0xF0, 0x78, 0x18, 0x8C, 0xF4, 0xFC, 0xFC, 0xFC, 0x00, 0xE0, 0xF9, 0x79,
	0x00, 0xCC, 0xFE, 0xFF, 0x3F, 0x0E, 0x04, 0x00, 0xC0, 0xF0, 0xF8, 0x1C, 0x04, 0x80, 0xFC, 0xFC,
	0xFC, 0x00, 0xE0, 0xFC, 0xFE, 0x03, 0xBC, 0xDF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03, 0x01, 0x80,
	0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x03, 0x13, 0x3B, 0x3F, 0x11, 0x1F, 0x0F, 0x07, 0x00,
	0x00, 0x03, 0x03, 0x00, 0x00, 0x03, 0x83, 0x83, 0x83, 0x02, 0x00, 0x80, 0x03, 0x03, 0x03, 0x03,
	0x01, 0x01, 0x03, 0x03, 0x03, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x20, 0x20, 0x20, 0x10, 0xFE, 0x10, 0x00, 0xE0, 0x10, 0x10,
	0x10, 0x60, 0x00, 0xFF, 0x20, 0x20, 0x10, 0xF0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x90,
	0x90, 0x90, 0xE0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x0F, 0x10, 0x20, 0x40, 0x87, 0x00, 0xFF,
	0xC0, 0x20, 0x10, 0x00, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x10, 0xFE, 0x10, 0x00, 0xE0, 0x10, 0x10,
	0x10, 0x60, 0x00, 0xFF, 0x20, 0x20, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x08, 0x08, 0x08, 0x00, 0x07, 0x08,
	0x00, 0x07, 0x08, 0xC8, 0x48, 0x44, 0x80, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x08, 0x08, 0x04, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x47, 0x88, 0x08,
	0x08, 0x07, 0x00, 0x0F, 0xC1, 0x06, 0x08, 0x00, 0x07, 0x08, 0x08, 0x08, 0x06, 0x00, 0x07, 0x08,
	0x00, 0x07, 0x08, 0x08, 0x08, 0x04, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x08, 0x08, 0x07, 0x00, 0xFC, 0x08, 0x0C, 0x00,
	0x78, 0x94, 0x58, 0x00, 0x58, 0xB4, 0x68, 0x00, 0x58, 0xB4, 0x68, 0x00, 0x00, 0x00, 0x00, 0x61,
	0x86, 0x8C, 0x71, 0x00, 0x78, 0x94, 0x58, 0x00, 0xFF, 0x00, 0x78, 0x94, 0x58, 0x00, 0x78, 0x84,
	0x48, 0x04, 0x7F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// Bitmap for the start screen part 2 of 2
const uint8_t StartScreen2 [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x60, 0x70, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x80,
	0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x81, 0xF1, 0xFE, 0xFF, 0x3F, 0x07, 0x01, 0xC3, 0xFF, 0xFF, 0x7F, 0x1E, 0xC0, 0xF0,
	0xFE, 0x00, 0x00, 0xC0, 0xF0, 0x78, 0x18, 0x8C, 0xF4, 0xFC, 0xFC, 0xFC, 0x00, 0xE0, 0xF9, 0x79,
	0x00, 0xCC, 0xFE, 0xFF, 0x3F, 0x0E, 0x04, 0x00, 0xC0, 0xF0, 0xF8, 0x1C, 0x04, 0x80, 0xFC, 0xFC,
	0xFC, 0x00, 0xE0, 0xFC, 0xFE, 0x03, 0xBC, 0xDF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03, 0x01, 0x80,
	0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x03, 0x13, 0x3B, 0x3F, 0x11, 0x1F, 0x0F, 0x07, 0x00,
	0x00, 0x03, 0x03, 0x00, 0x00, 0x03, 0x83, 0x83, 0x83, 0x02, 0x00, 0x80, 0x03, 0x03, 0x03, 0x03,
	0x01, 0x01, 0x03, 0x03, 0x03, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x20, 0x20, 0x20, 0x10, 0xFE, 0x10, 0x00, 0xE0, 0x10, 0x10,
	0x10, 0x60, 0x00, 0xFF, 0x20, 0x20, 0x10, 0xF0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x90,
	0x90, 0x90, 0xE0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x0F, 0x10, 0x20, 0x40, 0x87, 0x00, 0xFF,
	0xC0, 0x20, 0x10, 0x00, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x10, 0xFE, 0x10, 0x00, 0xE0, 0x10, 0x10,
	0x10, 0x60, 0x00, 0xFF, 0x20, 0x20, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x08, 0x08, 0x08, 0x00, 0x07, 0x08,
	0x00, 0x07, 0x08, 0x08, 0x08, 0x04, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x08, 0x08, 0x04, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x08,
	0x08, 0x07, 0x00, 0x0F, 0x01, 0x06, 0x08, 0x00, 0x07, 0x08, 0x08, 0x08, 0x06, 0x00, 0x07, 0x08,
	0x00, 0x07, 0x08, 0x08, 0x08, 0x04, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t draw_BlinkOn [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x44, 0x1C, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30,
	0x00, 0xFC, 0x10, 0xF0, 0x00, 0x40, 0x40, 0xB0, 0x90, 0xF0, 0x00, 0x40, 0x40, 0x3C, 0x44, 0x9C,
	0x00, 0xFC, 0xC0, 0x30, 0x00, 0xF0, 0x90, 0xF0, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30, 0x00, 0xFC,
	0x10, 0xF0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xFC, 0xF0, 0x00, 0xF0, 0xFC, 0x00, 0xF0, 0x90,
	0xF0, 0x00, 0xF0, 0x10, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07,
	0x04, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00,
	0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x01, 0x06, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x04, 0x07,
	0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
	0x07, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xCF, 0xCF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x01, 0xFD, 0x01, 0xFF, 0x07, 0xF7, 0xE7, 0x3F, 0xB7, 0x07, 0xFF, 0x07, 0x7F, 0x07, 0x7F,
	0x07, 0xFF, 0xFF, 0xFF, 0x01, 0x87, 0x7F, 0x87, 0x01, 0xFF, 0x07, 0xF7, 0x07, 0xFF, 0x0F, 0xEF,
	0x01, 0xFF, 0x03, 0xBB, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xEC, 0x2D, 0xEC, 0x0F, 0xEC, 0x0F, 0xAF, 0x0C, 0x8D, 0x8C, 0xEF,
	0x0F, 0x8C, 0x8E, 0x8C, 0x0F, 0xEF, 0x2F, 0xEF, 0x0C, 0xEF, 0x8C, 0x8F, 0x0C, 0x8F, 0x8C, 0x8D,
	0x0C, 0x8F, 0x0C, 0x8D, 0x0C, 0x8F, 0x0C, 0x0D, 0x0C, 0xEF, 0x8F, 0x0F, 0x8F, 0xEF, 0x0F, 0x8F,
	0x8F, 0x8F, 0x0F, 0x8F, 0x8F, 0xEF, 0x0F, 0x8F, 0x8F, 0x8F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x39, 0x22, 0x3C, 0x00, 0x3F, 0x00, 0x3F,
	0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x24, 0x37, 0x00, 0x39, 0x22, 0x3C, 0x00, 0x3F, 0x00, 0x3F,
	0x00, 0x3F, 0x20, 0x3F, 0x00, 0x0F, 0x30, 0x1F, 0x30, 0x0F, 0x00, 0x00, 0x00, 0x3F, 0x07, 0x38,
	0x07, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x24, 0x37, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t draw_BlinkOff [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x44, 0x1C, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30,
	0x00, 0xFC, 0x10, 0xF0, 0x00, 0x40, 0x40, 0xB0, 0x90, 0xF0, 0x00, 0x40, 0x40, 0x3C, 0x44, 0x9C,
	0x00, 0xFC, 0xC0, 0x30, 0x00, 0xF0, 0x90, 0xF0, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30, 0x00, 0xFC,
	0x10, 0xF0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xFC, 0xF0, 0x00, 0xF0, 0xFC, 0x00, 0xF0, 0x90,
	0xF0, 0x00, 0xF0, 0x10, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07,
	0x04, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00,
	0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x01, 0x06, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x04, 0x07,
	0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
	0x07, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x02, 0xFE, 0x00, 0xF8, 0x08, 0x18, 0xD8, 0x48, 0xF8, 0x00,
	0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0xFE, 0x78, 0x80, 0x78, 0xFE, 0x00, 0xF8, 0x08,
	0xF8, 0x00, 0xF8, 0x08, 0xFE, 0x00, 0xF8, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x20, 0xE0, 0x00, 0xE3, 0x02, 0xA3, 0x00, 0x83, 0x80, 0xE0,
	0x03, 0x82, 0x83, 0x80, 0x00, 0xE3, 0x21, 0xE3, 0x00, 0xE0, 0x80, 0x80, 0x03, 0x80, 0x83, 0x80,
	0x03, 0x80, 0x03, 0x82, 0x03, 0x80, 0x03, 0x02, 0x03, 0xE0, 0x83, 0x02, 0x83, 0xE0, 0x00, 0x80,
	0x80, 0x80, 0x00, 0x80, 0x80, 0xE0, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x39, 0x22, 0x3C, 0x00, 0x3F, 0x00, 0x3F,
	0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x24, 0x37, 0x00, 0x39, 0x22, 0x3C, 0x00, 0x3F, 0x00, 0x3F,
	0x00, 0x3F, 0x20, 0x3F, 0x00, 0x0F, 0x30, 0x1F, 0x30, 0x0F, 0x00, 0x00, 0x00, 0x3F, 0x07, 0x38,
	0x07, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x24, 0x37, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t slideshow_BlinkOn [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x44, 0x1C, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30,
	0x00, 0xFC, 0x10, 0xF0, 0x00, 0x40, 0x40, 0xB0, 0x90, 0xF0, 0x00, 0x40, 0x40, 0x3C, 0x44, 0x9C,
	0x00, 0xFC, 0xC0, 0x30, 0x00, 0xF0, 0x90, 0xF0, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30, 0x00, 0xFC,
	0x10, 0xF0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xFC, 0xF0, 0x00, 0xF0, 0xFC, 0x00, 0xF0, 0x90,
	0xF0, 0x00, 0xF0, 0x10, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07,
	0x04, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00,
	0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x01, 0x06, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x04, 0x07,
	0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
	0x07, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00,
	0x00, 0xFE, 0x02, 0xFE, 0x00, 0xF8, 0x08, 0x18, 0xD8, 0x48, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00,
	0xF8, 0x00, 0x00, 0x00, 0xFE, 0x78, 0x80, 0x78, 0xFE, 0x00, 0xF8, 0x08, 0xF8, 0x00, 0xF8, 0x08,
	0xFE, 0x00, 0xF8, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x13, 0xD2, 0x13, 0xF0, 0x13, 0xF0, 0x50, 0xF3, 0xF2, 0xF3, 0x10,
	0xF0, 0x73, 0x71, 0x73, 0xF0, 0x10, 0xD0, 0x90, 0xF3, 0x10, 0x73, 0x70, 0xF3, 0x70, 0x73, 0x72,
	0xF3, 0x70, 0xF3, 0x72, 0xF3, 0x70, 0xF3, 0xF2, 0xF3, 0x10, 0x70, 0xF0, 0x70, 0x10, 0xF0, 0x70,
	0x70, 0x70, 0xF0, 0x70, 0x70, 0x10, 0xF0, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0x7F, 0x7F, 0x7F, 0x79, 0x79, 0x7F, 0x7F, 0x7F, 0x7F, 0x44, 0x5D, 0x41, 0x7F, 0x40, 0x7F, 0x40,
	0x7F, 0x40, 0x5E, 0x40, 0x7F, 0x40, 0x5B, 0x48, 0x7F, 0x44, 0x5D, 0x41, 0x7F, 0x40, 0x7F, 0x40,
	0x7F, 0x40, 0x5F, 0x40, 0x7F, 0x70, 0x4F, 0x60, 0x4F, 0x70, 0x7F, 0x7F, 0x7F, 0x40, 0x78, 0x47,
	0x78, 0x40, 0x7F, 0x40, 0x5F, 0x40, 0x7F, 0x40, 0x5F, 0x40, 0x7F, 0x40, 0x5B, 0x48, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t slideshow_BlinkOff [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x44, 0x1C, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30,
	0x00, 0xFC, 0x10, 0xF0, 0x00, 0x40, 0x40, 0xB0, 0x90, 0xF0, 0x00, 0x40, 0x40, 0x3C, 0x44, 0x9C,
	0x00, 0xFC, 0xC0, 0x30, 0x00, 0xF0, 0x90, 0xF0, 0x10, 0xFC, 0x10, 0xF0, 0x10, 0x30, 0x00, 0xFC,
	0x10, 0xF0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xFC, 0xF0, 0x00, 0xF0, 0xFC, 0x00, 0xF0, 0x90,
	0xF0, 0x00, 0xF0, 0x10, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07,
	0x04, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00,
	0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x01, 0x06, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x04, 0x07,
	0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
	0x07, 0x00, 0x07, 0x04, 0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07, 0x04, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00,
	0x00, 0xFE, 0x02, 0xFE, 0x00, 0xF8, 0x08, 0x18, 0xD8, 0x48, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00,
	0xF8, 0x00, 0x00, 0x00, 0xFE, 0x78, 0x80, 0x78, 0xFE, 0x00, 0xF8, 0x08, 0xF8, 0x00, 0xF8, 0x08,
	0xFE, 0x00, 0xF8, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x03, 0x00, 0xE3, 0x20, 0xE0, 0x03, 0xE2, 0x03, 0xA0,
	0x00, 0x83, 0x81, 0xE3, 0x00, 0x80, 0x80, 0x80, 0x03, 0xE0, 0x23, 0xE0, 0x03, 0xE0, 0x83, 0x82,
	0x03, 0x80, 0x83, 0x82, 0x03, 0x80, 0x03, 0x82, 0x03, 0x80, 0x00, 0x00, 0x00, 0xE0, 0x80, 0x00,
	0x80, 0xE0, 0x00, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0xE0, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x22, 0x3C,
	0x00, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x24, 0x37, 0x00, 0x39, 0x22, 0x3C,
	0x00, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x0F, 0x30, 0x1F, 0x30, 0x0F, 0x00, 0x00,
	0x00, 0x3F, 0x07, 0x38, 0x07, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F, 0x20, 0x3F, 0x00, 0x3F,
	0x24, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char ImageSaved [] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xEF,
	0xF7, 0xFB, 0xFD, 0xFC, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xFE,
	0xC6, 0xC6, 0xFE, 0xFE, 0xFE, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0xBF, 0xFF, 0xFF,
	0xFF, 0xBF, 0x1F, 0xBF, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0xBF, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0xBF,
	0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0xBF, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0xBF, 0xFF, 0xFF, 0xFF, 0xBF,
	0x1F, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xF3, 0xF7,
	0xE7, 0xCF, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07,
	0xFF, 0x1F, 0xDF, 0x3F, 0xDF, 0x1F, 0xFF, 0x9F, 0xDF, 0x1F, 0xFF, 0x1F, 0xDF, 0x1F, 0xFF, 0x1F,
	0xDF, 0x1F, 0xFF, 0xFF, 0xFF, 0x87, 0x77, 0xC7, 0xFF, 0x9F, 0xDF, 0x1F, 0xFF, 0x1F, 0xFF, 0x1F,
	0xFF, 0x1F, 0xDF, 0x1F, 0xFF, 0x1F, 0xDF, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x8F, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xF0, 0x7F, 0xF0, 0xFF, 0xF0, 0xFF, 0xF0, 0x7F, 0xF0, 0xF6, 0xF0, 0xFF, 0xD0,
	0x57, 0xC0, 0xFF, 0xF0, 0xF6, 0xF2, 0x7F, 0xFF, 0xFF, 0xF1, 0xF7, 0xF0, 0x7F, 0xF0, 0xF6, 0xF0,
	0xFF, 0xF8, 0x77, 0xF8, 0xFF, 0xF0, 0xF6, 0xF2, 0x7F, 0xF0, 0xF7, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF,
	0xDC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDC, 0xDE, 0xDE, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF,
	0xDF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE,
	0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFE, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};


// ---------LCD Screen BUFFER End--------------------------------



// ---------LCD Struct-------------------------------------------

typedef struct _ADAFRUIT_LCD{
	// Member variables for LCD usage
	int8_t			 _din,
					 _sclk,
					 _dc,
					 _rst,
					 _cs;
	volatile uint8_t *mosiport,
					 *clkport,
					 *csport,
					 *dcport;
	uint8_t			 mosipinmask,
					 clkpinmask,
					 cspinmask,
					 dcpinmask;

} LCD;

// ---------LCD Struct End---------------------------------------


// ---------Function Headers-------------------------------------
void drawPixel(int16_t x, int16_t y, uint16_t color);
unsigned char getPixel(int8_t x, int8_t y);

#include <Adafruit_GFX_C.c>

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax);
void fillLCD1(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST, GFX * g, LCD * l);
void fillLCD2(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST, GFX * g, LCD * l);
void display(LCD * l);
void begin(uint8_t contrast, LCD * l);
void digitalWrite(int8_t pin, int8_t state);
inline void fastSPIwrite(uint8_t d, LCD * l);
void command(uint8_t c, LCD * l);
void data(uint8_t c, LCD * l);
void setContrast(uint8_t val , LCD * l);
void clearDisplay(GFX * g);

// ---------Function Headers End---------------------------------


// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
//#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) 
{
	#ifdef enablePartialUpdate
		if (xmin < xUpdateMin) xUpdateMin = xmin;
		if (xmax > xUpdateMax) xUpdateMax = xmax;
		if (ymin < yUpdateMin) yUpdateMin = ymin;
		if (ymax > yUpdateMax) yUpdateMax = ymax;
	#endif
}
// Consructor functions
// 5 inputs 
void fillLCD1(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST, GFX * g, LCD * l) 
{	
	fillGFX(LCDWIDTH, LCDHEIGHT,g); 
	l->_din = DIN;
	l->_sclk = SCLK;
	l->_dc = DC;
	l->_rst = RST;
	l->_cs = CS;	
}
// 4 Inputs -> NO Chip select
void fillLCD2(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST, GFX * g, LCD * l) 
{
  fillGFX(LCDWIDTH, LCDHEIGHT, g); 
  // give values to signals for where they are located as PIN
  l->_din = DIN;
  l->_sclk = SCLK;
  l->_dc = DC;
  l->_rst = RST;
  l->_cs = -1;								// will not be needing to switch from the current LCD
  l->clkport     = &PORTB;					//_sclk;
  l->clkpinmask  = (0x01 << l->_sclk);
  l->mosiport    = &PORTB;					// _din;
  l->mosipinmask = (0x01 << l->_din);
  l->dcport      = &PORTB;					// _dc;
  l->dcpinmask   = (0x01 << l->_dc);
}

// Member functions
// the most basic function, set a single pixel
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (color)
  { 
    pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= (1 << (y%8)); 
  }
  else
  {
    pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~(1 << (y%8)); 
  }

  updateBoundingBox(x,y,x,y);
}

// the most basic function, get a single pixel
unsigned char getPixel(int8_t x, int8_t y) {
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return 0;

  return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;  
}

void display(LCD * l) {
	uint8_t col, maxcol, p;
	
	for(p = 0; p < 6; p++) {
		#ifdef enablePartialUpdate
		// check if this page is part of update
		if ( yUpdateMin >= ((p+1)*8) ) {
			continue;   // nope, skip it!
		}
		if (yUpdateMax < p*8) {
			break;
		}
		#endif

		command(PCD8544_SETYADDR | p, l);


		#ifdef enablePartialUpdate
		col = xUpdateMin;
		maxcol = xUpdateMax;
		#else
		// start at the beginning of the row
		col = 0;
		maxcol = LCDWIDTH-1;
		#endif 

		command(PCD8544_SETXADDR | col, l);

		digitalWrite(l->_dc, HIGH);
		if (l->_cs > 0)
		digitalWrite(l->_cs, LOW);
		for(; col <= maxcol; col++) {
			//uart_putw_dec(col);
			//uart_putchar(' ');
			fastSPIwrite(pcd8544_buffer[(LCDWIDTH*p)+col] , l);
		}
		if (l->_cs > 0)
		digitalWrite(l->_cs, HIGH);

	}

	command(PCD8544_SETYADDR , l);  // no idea why this is necessary but it is to finish the last byte?
	#ifdef enablePartialUpdate
	xUpdateMin = LCDWIDTH - 1;
	xUpdateMax = 0;
	yUpdateMin = LCDHEIGHT-1;
	yUpdateMax = 0;
	#endif

}

void displayBitmap(const uint8_t bitmap[], LCD * l)
{
		uint8_t col, maxcol, p;
		
		for(p = 0; p < 6; p++) {
			#ifdef enablePartialUpdate
			// check if this page is part of update
			if ( yUpdateMin >= ((p+1)*8) ) {
				continue;   // nope, skip it!
			}
			if (yUpdateMax < p*8) {
				break;
			}
			#endif

			command(PCD8544_SETYADDR | p, l);
			
			#ifdef enablePartialUpdate
			col = xUpdateMin;
			maxcol = xUpdateMax;
			#else
			// start at the beginning of the row
			col = 0;
			maxcol = LCDWIDTH-1;
			#endif

			command(PCD8544_SETXADDR | col, l);

			digitalWrite(l->_dc, HIGH);
			if (l->_cs > 0)
			digitalWrite(l->_cs, LOW);
			for(; col <= maxcol; col++) {
				//uart_putw_dec(col);
				//uart_putchar(' ');
				fastSPIwrite(bitmap[(LCDWIDTH*p)+col] , l);
			}
			if (l->_cs > 0)
			digitalWrite(l->_cs, HIGH);

		}

		command(PCD8544_SETYADDR , l);  // no idea why this is necessary but it is to finish the last byte?
		#ifdef enablePartialUpdate
		xUpdateMin = LCDWIDTH - 1;
		xUpdateMax = 0;
		yUpdateMin = LCDHEIGHT-1;
		yUpdateMax = 0;
		#endif

}

void begin(uint8_t contrast, LCD * l) {

	// set pin directions
	PORTB = 0xF8; DDRB = 0xFF; 
	
	// toggle RST low to reset
	digitalWrite(l->_rst, LOW);
	_delay_ms(100);
	digitalWrite(l->_rst, HIGH);
	
	// get into the EXTENDED mode!
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, l );
	
	// LCD bias select (4 is optimal?)
	command(PCD8544_SETBIAS | 0x4 , l);
	
	// Set contrast for LCD screen
	if(contrast >= 0x7F)
	{
		contrast = 0x7F;	
	}
	command( PCD8544_SETVOP | contrast , l); // Experimentally determined
	
	// normal mode
	command(PCD8544_FUNCTIONSET, l);
	
	// Set display to Normal
	command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL, l);
	
	// initial display line
	// set page address
	// set column address
	// write display data
	// set up a bounding box for screen updates
	updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
	// Push out pcd8544_buffer to the Display (will show the AFI logo)
	//display(l);
}

void digitalWrite(int8_t pin, int8_t state)
{
	if(state == 0)
	{
		PORTB &= ~(0x01 << pin);
	}
	else
	{
		PORTB |= (0x01 << pin);
	}
}

inline void fastSPIwrite(uint8_t d, LCD * l) 
{
	/*
	for(uint8_t bit = 0x80; bit; bit >>= 1) {
		
	  *(l->clkport) &= ~l->clkpinmask;
	  if(d & bit){ *(l->mosiport) |=  l->mosipinmask;}
	  else       { *(l->mosiport) &= ~l->mosipinmask;}
	  *(l->clkport) |=  l->clkpinmask;
	}*/
	
	for (int8_t i = 7; i >= 0; i--) {
		*l->clkport &= ~(l->clkpinmask);
		if ((d & (1 << i)) > 0){
			*l->mosiport |= (l->mosipinmask);
		} else {
			*l->mosiport &= ~(l->mosipinmask);
		}
		*l->clkport |= (l->clkpinmask);
	}
	*l->clkport &= ~(l->clkpinmask);
}

//inline void Adafruit_PCD8544::slowSPIwrite(uint8_t c) {
  //shiftOut(_din, _sclk, MSBFIRST, c);
//}

void command(uint8_t c, LCD * l)
{
	digitalWrite(l->_dc, LOW);
	if (l->_cs > 0)
	{
		digitalWrite(l->_cs, LOW);
	}
	fastSPIwrite(c, l);
	if (l->_cs > 0)
	{	
		digitalWrite(l->_cs, HIGH);
	}
}

void data(uint8_t c, LCD * l) 
{
	digitalWrite(l->_dc, HIGH);
	if (l->_cs > 0)
	{
		digitalWrite(l->_cs, LOW);
	}
	fastSPIwrite(c, l);
	if (l->_cs > 0)
	{
		digitalWrite(l->_cs, HIGH);
	}
}

void setContrast(uint8_t val , LCD * l) {
	if (val > 0x7f) {
		val = 0x7f;
	}
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, l);
	command( PCD8544_SETVOP | val, l); 
	command(PCD8544_FUNCTIONSET, l);	
 }
 
void clearBuffer(uint8_t * buf , int8_t val, GFX * g)
{
	unsigned short size = (g->_width * g->_height)/ 8;
	for(uint16_t i = 0; i < size; i++)
	{
		buf[i] = 0x00;
	}
}

// clear everything
void clearDisplay(GFX * g) {
  clearBuffer(pcd8544_buffer, 0, g);
  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
  g->cursor_y = 0;
  g->cursor_x = 0;
}

/*
// this doesnt touch the buffer, just clears the display RAM - might be handy
void Adafruit_PCD8544::clearDisplay(void) {
  
  uint8_t p, c;
  
  for(p = 0; p < 8; p++) {

    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      //uart_putw_dec(c);
      //uart_putchar(' ');
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }     
    }

}

*/

