#include "interfaceDisplay.h"

void display::initLCD(byte w, byte h, byte rotation){
  pinMode(LCD_BL ,OUTPUT);
  displayBrightness(255);

  lcd.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab   ST7735_MADCTL_BGR changed to 0x00 !!!!!
  lcd.setRotation(rotation);      // set display rotation
  lcd.fillScreen(BLACK);          // init black
}

void display::displayBrightness(byte brightness){
  analogWrite(LCD_BL, brightness);
}

void display::initBuffer(){
  bufferHead = new dispBuffer16(w, 15);
  bufferFoot = new dispBuffer16(w, 98);
  bufferBody = new dispBuffer16(w, 15);
}

void display::drawBuffer(){
  drawHead();
}

void display::refresh(){
  drawBuffer();
  updateDisplay();
}

void display::updateDisplay(){
  if(updateHead) {
    writeRGBMap(0, 0, bufferHead,w,15);
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

void display::drawHead(){
  static byte test = 0;

  //setup
  bufferHead->fillScreen(BLACK);
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setTextSize(1);

  //line
  bufferHead->drawRect(0,14,w,1,WHITE);     //box
  bufferHead->drawRect(57,0,1,15,WHITE);   //spacer
  bufferHead->drawRect(107,0,1,15,WHITE);  //spacer

  bufferHead->setCursor(2, 3);
  bufferHead->print("MiniFrank");
  bufferHead->setCursor(75, 3);
  bufferHead->print(test);

  //AreaTwo
  bufferHead->setCursor(110, 3);
  bufferHead->print("MIDI:");
  if(0){
    bufferHead->setTextColor(GREEN, BLACK);
    bufferHead->print("USB");
    bufferHead->setTextColor(WHITE, BLACK);
  }
  else{
    bufferHead->setTextColor(GREEN, BLACK);
    bufferHead->print("DIN");
    bufferHead->setTextColor(WHITE, BLACK);
  }
  updateHead = 1;
  test++;

}
void display::drawFoot(){
}
void display::drawBody(){
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

/////////////Stuff from Adafruit_GFX////////////
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
