#include "interfaceDisplay.h"

void display::initLCD(byte w, byte h, byte rotation){

  pinMode(LCD_BL ,OUTPUT);
  displayBrightness(255);

  lcd.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab   ST7735_MADCTL_BGR changed to 0x00 !!!!!
  lcd.setRotation(rotation);      // set display rotation
  lcd.fillScreen(BLACK);   // init black

}

void display::displayBrightness(byte brightness){

  analogWrite(LCD_BL, brightness);
}

void display::initBuffer(){
  bufferTop = new dispBuffer16(w, 15);

  //pBuffer_BOTTOM = new GFXcanvas1(w, 15);
  //pBuffer_MAIN = new GFXcanvas1(w, 96);
}

void display::drawBuffer(){
 drawTop();
 //drawBottom();
 //drawMain();
}


void display::refresh(){
  drawBuffer();
  updateDisplay();
}

void display::updateDisplay(){
  if(updateTop) {
      writeRGBMap(0, 0, bufferTop,w,15);
}

  //   lcd.drawRGBBitmap(0, 0, pBuffer_TOP->getBuffer(), w, 15, WHITE, BLACK);
  // }
  //
  // if(updateBottom){
  //
  // }
  // if(updateMain){s
  // }
}

void display::drawTop(){

  static byte test = 0;

  //setup
  bufferTop->fillScreen(BLACK);
  bufferTop->setTextColor(WHITE, BLACK);
  bufferTop->setTextSize(1);

  //line
  bufferTop->drawRect(0,14,w,1,WHITE);     //box
  bufferTop->drawRect(57,0,1,15,WHITE);   //spacer
  bufferTop->drawRect(107,0,1,15,WHITE);  //spacer

  bufferTop->setCursor(2, 3);
  bufferTop->print("MiniFrank");
  bufferTop->setCursor(75, 3);
  bufferTop->print(test);

  //AreaTwo
  bufferTop->setCursor(110, 3);
  bufferTop->print("MIDI:");
  if(0){
    bufferTop->setTextColor(GREEN, BLACK);
    bufferTop->print("USB");
    bufferTop->setTextColor(WHITE, BLACK);
  }
  else{
    bufferTop->setTextColor(GREEN, BLACK);
    bufferTop->print("DIN");
    bufferTop->setTextColor(WHITE, BLACK);
  }


  //spacer
   updateTop = 1;

   test++;

}
void display::drawBottom(){
}
void display::drawMain(){
}

void display::writeRGBMap(int16_t x, int16_t y, dispBuffer16 *bufferObj, int16_t w, int16_t h) {
  const uint16_t *buffer = bufferObj->getBuffer();
  lcd.startWrite();
    for(int16_t j=0; j<h; j++, y++) {
      for(int16_t i=0; i<w; i++ ) {
        int16_t index = j * w + i;
        if(bufferObj->compareBuffer(index)){
          lcd.writePixel(x+i, y, pgm_read_word(&buffer[index]));
          bufferObj->copyBuffer(index);
        }
      }
    }
  lcd.endWrite();
}


//Display Buffer based on the adafruit canvas, with 2 sepereate buffers for comparison
 dispBuffer16::dispBuffer16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    uint32_t bytes = w * h * 2;
    if((buffer = (uint16_t *)malloc(bytes))) {
        memset(buffer, 0, bytes);
    }
    if((buffer2 = (uint16_t *)malloc(bytes))) {
        memset(buffer, 0, bytes);
    }
}

 dispBuffer16::~dispBuffer16(void) {
    if(buffer) free(buffer);
}


void dispBuffer16::copyBuffer(uint16_t index){
  buffer2[index] = buffer[index];
 }

int dispBuffer16::compareBuffer(uint16_t index){
return (buffer2[index] != buffer[index]);
 }



//Stuff from Adafruit_GFX

void dispBuffer16::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if(buffer) {
        if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

        int16_t t;
        switch(rotation) {
            case 1:
                t = x;
                x = WIDTH  - 1 - y;
                y = t;
                break;
            case 2:
                x = WIDTH  - 1 - x;
                y = HEIGHT - 1 - y;
                break;
            case 3:
                t = x;
                x = y;
                y = HEIGHT - 1 - t;
                break;
        }

        buffer[x + y * WIDTH] = color;
    }
}

void dispBuffer16::fillScreen(uint16_t color) {
    if(buffer) {
        uint8_t hi = color >> 8, lo = color & 0xFF;
        if(hi == lo) {
            memset(buffer, lo, WIDTH * HEIGHT * 2);
        } else {
            uint32_t i, pixels = WIDTH * HEIGHT;
            for(i=0; i<pixels; i++) buffer[i] = color;
        }
    }
}

void dispBuffer16::byteSwap(void) {
    if(buffer) {
        uint32_t i, pixels = WIDTH * HEIGHT;
        for(i=0; i<pixels; i++) buffer[i] = __builtin_bswap16(buffer[i]);
    }
}
