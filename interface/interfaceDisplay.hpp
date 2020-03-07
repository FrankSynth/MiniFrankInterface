#pragma once

#include "interfaceData.hpp"
#include "interfaceMapping.hpp"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
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
//#define GREEN      ST77XX_GREEN
//#define BLUE       ST77XX_BLUE
#define CYAN ST77XX_CYAN
#define MAGENTA ST77XX_MAGENTA
#define YELLOW ST77XX_YELLOW
#define ORANGE ST77XX_ORANGE

#define BLACKBLUE 0x0410  // Schwarz Blau
#define BLUE_LIGHT 0x94B2 // Hell Blau
#define BLUE 0x39E7       // Blau
#define BLUE_DARK 0x2965  // Dunkel Blau
#define GREEN 0x4208      // Grün

///  A 16-bit double buffer from the adafruit canvas
class DispBuffer16 : public Adafruit_GFX {
  public:
    DispBuffer16(uint16_t w, uint16_t h);
    ~DispBuffer16(void);
    inline void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color), byteSwap(void);

    inline uint16_t *getBuffer(void) const { return buffer; }   // return buffer 1 pointer
    inline uint16_t *getBuffer2(void) const { return buffer2; } // return buffer 2 pointer

    inline void copyBuffer(uint16_t index);   // copy value from buffer 1 to 2
    inline int compareBuffer(uint16_t index); // compare value from both buffers

    uint16_t *buffer2;
    uint16_t *buffer;
};

class Display {
  public:
    Display(byte w, byte h, byte rotation) // constructor
        : w(w), h(h), rotation(rotation) {
        initLCD(w, h, rotation);
        initBuffer();
    }

    void refresh();                   // refresh display
    void displayBrightness(byte brigthness); // set display display brightness

  private:
    Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST); // initialize LCD

    // Build display buffers
    DispBuffer16 *bufferHead;
    DispBuffer16 *bufferFoot;
    DispBuffer16 *bufferBody;

    uint8_t w;
    uint8_t h;
    uint8_t rotation;

    inline char valueToNote(byte noteIn);
    inline char valueToOctave(byte noteIn);
    inline char valueToSharp(byte noteIn);

    inline const char *tuningToChar(byte tuning);

    inline void initLCD(byte w, byte h, byte rotation);
    inline void initBuffer();

    inline void writeDisplay();
    inline void updateBuffer();

    inline void writeRGBMap(int16_t x, int16_t y, DispBuffer16 *bufferObj, int16_t w, int16_t h);

    inline void drawBuffer();
    inline void drawHead();
    inline void drawFoot();
    inline void drawBody();

    // Templates
    inline void BodyTemplateSeq();
    inline void BodyTemplateMenu();
    inline void BodyTemplateLive();
    inline void BodyTemplateArp();

    inline void updateDisplay();
};
