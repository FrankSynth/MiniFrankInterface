#pragma once

#include "interfaceData.hpp"

#include "interfaceMapping.hpp"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>


#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define LCD_CS        10
#define LCD_RST       20
#define LCD_DC        18
#define LCD_BL        19

#define BLACK      ST77XX_BLACK
#define WHITE      ST77XX_WHITE
#define RED        ST77XX_RED
#define GREEN      ST77XX_GREEN
#define BLUE       ST77XX_BLUE
#define CYAN       ST77XX_CYAN
#define MAGENTA    ST77XX_MAGENTA
#define YELLOW     ST77XX_YELLOW
#define ORANGE     ST77XX_ORANGE


#define COLOR  0x0410  //Blue??
#define COLOR2 0x94B2
#define COLOR3 0x39E7
#define COLOR4 0x2965  //dark grey
#define COLOR5 0x4208  //grey

// FrankData &data = getDataObject();

///  A 16-bit double buffer from the adafruit canvas
class DispBuffer16 : public Adafruit_GFX {
  public:
    DispBuffer16(uint16_t w, uint16_t h);
    ~DispBuffer16(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color), byteSwap(void);

    uint16_t *getBuffer(void) const {
        return buffer;
    } // return buffer 1 pointer
    uint16_t *getBuffer2(void) const {
        return buffer2;
    } // return buffer 2 pointer

    void copyBuffer(uint16_t index);   // copy value from buffer 1 to 2
    int compareBuffer(uint16_t index); // compare value from both buffers

    uint16_t *buffer2;
    uint16_t *buffer;
};

class Display {
  public:
    Display(byte w, byte h, byte rotation) // constructor
        : w(w), h(h), rotation(rotation) {
        initLCD(w, h, rotation);
        initBuffer();
        // this -> data = getDataObject();

    }

    void refresh();                          // refresh display
    void displayBrightness(byte brigthness); // set display display brightness

    // void init() { // set pointer
    //     data
    //     // seq1 = &getSeqObject()[0];
    //     // seq2 = &getSeqObject()[1];
    // }

  private:
    Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST); // initialize LCD

    // Build display buffers
    DispBuffer16 *bufferHead;
    DispBuffer16 *bufferFoot;
    DispBuffer16 *bufferBody;

    uint8_t w;
    uint8_t h;
    uint8_t rotation;

    // Seq* seq
    // Seq *seq2;
    // status *stat;
    // FrankData & data;

    char valueToNote(byte noteIn);
    char valueToOctave(byte noteIn);
    char valueToSharp(byte noteIn);

    Seq *getActiveSeqPointer();

    const char *tuningToChar(byte tuning);

    void initLCD(byte w, byte h, byte rotation);
    void initBuffer();
    void writeDisplay();
    void updateBuffer();
    void writeRGBMap(int16_t x, int16_t y, DispBuffer16 *bufferObj, int16_t w, int16_t h);
    void drawBuffer();
    void drawHead();
    void drawFoot();
    void drawBody();


    // Templates
    void BodyTemplateSeq();
    void BodyTemplateMenu();
    void BodyTemplateArp();
    void BodyTemplateLive();



    void updateDisplay();
};
