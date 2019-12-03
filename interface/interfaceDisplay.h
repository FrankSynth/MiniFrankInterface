#ifndef MF_DISPLAY_H_
#define MF_DISPLAY_H_

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define LCD_CS        10
#define LCD_RST       9
#define LCD_DC        8
#define LCD_BL        15

#define BLACK      ST77XX_BLACK
#define WHITE      ST77XX_WHITE
#define RED        ST77XX_RED
#define GREEN      ST77XX_GREEN
#define BLUE       ST77XX_BLUE
#define CYAN       ST77XX_CYAN
#define MAGENTA    ST77XX_MAGENTA
#define YELLOW     ST77XX_YELLOW
#define ORANGE     ST77XX_ORANGE

///  A 16-bit double buffer from the adafruit canvas
class dispBuffer16 : public Adafruit_GFX{
 public:
  dispBuffer16(uint16_t w, uint16_t h);
  ~dispBuffer16(void);
  void      drawPixel(int16_t x, int16_t y, uint16_t color),
            fillScreen(uint16_t color),
            byteSwap(void);

  uint16_t *getBuffer(void) const { return buffer; }    //return buffer 1 pointer
  uint16_t *getBuffer2(void) const { return buffer2; }  //return buffer 2 pointer

  void copyBuffer(uint16_t index);      //copy value from buffer 1 to 2
  int compareBuffer(uint16_t index);    //compare value from both buffers

  uint16_t *buffer2;
  uint16_t *buffer;
};

class display
{
  public:
    display(byte w, byte h, byte rotation)  //constructor
    :w(w), h(h), rotation(rotation) {
      initLCD(w,h,rotation);
      initBuffer();
    }

    void refresh();                              //refresh display
    void displayBrightness(byte brigthness);     //set display display brightness

private:
    Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST);  //initialize LCD

    //Build display buffers
    dispBuffer16 *bufferHead;
    dispBuffer16 *bufferFoot;
    dispBuffer16 *bufferBody;

    uint8_t w;
    uint8_t h;
    uint8_t rotation;

    uint8_t updateHead = 0;
    uint8_t updateFoot = 0;
    uint8_t updateBody = 0;

    void initLCD(byte w, byte h, byte rotation);
    void initBuffer();
    void writeDisplay();
    void updateBuffer();
    void writeRGBMap(int16_t x, int16_t y, dispBuffer16 *bufferObj, int16_t w, int16_t h);
    void drawBuffer();
    void drawHead();
    void drawFoot();
    void drawBody();
    void updateDisplay();
};
#endif
