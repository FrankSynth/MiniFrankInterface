#pragma once

// #include "Adafruit_ST7735_and_ST7789_Library/Adafruit_ST7735.h" // Hardware-specific library for ST7735 - CHANGE SPI SPEED
#include "interfaceData.hpp"
#include "interfaceMapping.hpp"
#include "src/Adafruit_ST7735_and_ST7789_Library/Adafruit_ST7735.h" // Hardware-specific library for ST7735 - CHANGE SPI SPEED
#include <Adafruit_GFX.h>                                           // Core graphics library
#include <SPI.h>

// Include extra Fonts
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define LCD_CS 10
#define LCD_RST 20
#define LCD_DC 18
#define LCD_BL 19

#define BLACK ST77XX_BLACK
#define WHITE ST77XX_WHITE
#define RED ST77XX_RED
// #define GREEN      ST77XX_GREEN

// #define BLUE       ST77XX_BLUE

#define CYAN ST77XX_CYAN
#define MAGENTA ST77XX_MAGENTA
#define YELLOW ST77XX_YELLOW
#define ORANGE ST77XX_ORANGE

#define GREYWHITE 0xC638 // Hell Blau
#define GREY 0x39E7
#define GREEN 0x04E0 // Grün
#define MEDIUMGREY 0x4208
#define LIGHTGREY 0x7BEF
#define DARKGREY 0x2945 // linien
#define BACKGROUND 0x3186

#define COLORTHEME 0x1AAE

///  A 16-bit double buffer from the adafruit canvas
class DispBuffer16 : public Adafruit_GFX {
  public:
    DispBuffer16(uint16_t w, uint16_t h);
    ~DispBuffer16(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color), byteSwap(void);

    uint16_t *getBuffer(void) const { return buffer; }   // return buffer 1 pointer
    uint16_t *getBuffer2(void) const { return buffer2; } // return buffer 2 pointer

    inline void copyBuffer(uint16_t index);    // copy value from buffer 1 to 2
    inline bool compareBuffer(uint16_t index); // compare value from both buffers

    uint16_t *buffer2;
    uint16_t *buffer;
};

class TLC5916 {
  public:
    void init(byte pin);
    void sendByte(byte send);
    void toggleCLK();
    void updateTLC();

  private:
    byte pinTLC;
};

class Display {
  public:
    Display(byte w, byte h, byte rotation) // constructor
        : w(w), h(h), rotation(rotation) {
        initLCD(w, h, rotation);
        initBuffer();
    }

    void refresh();                                                 // refresh display
    void displayBrightness(byte brigthness);                        // set display display brightness
    Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST); // initialize LCD
    TLC5916 tlc;

  private:
    // Build display buffers
    DispBuffer16 *bufferHead;
    DispBuffer16 *bufferFoot;
    DispBuffer16 *bufferBody;

    uint8_t w;
    uint8_t h;
    uint8_t rotation;

    void initLCD(byte w, byte h, byte rotation);
    void initBuffer();

    void writeDisplay();
    void updateBuffer();

    void writeRGBMap(int16_t x, int16_t y, DispBuffer16 *bufferObj, int16_t w, int16_t h);

    void drawBuffer();
    void drawHead();
    void drawFoot();
    void drawBody();

    void FootLive();
    void FootSeq();

    // Templates
    void BodyTemplateSeq();
    void BodyTemplateMenu();
    void BodyTemplateLive();
    void BodyTemplateArp();
    void BodyTemplateCal();

    void updateDisplay();
};
