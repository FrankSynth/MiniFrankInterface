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
  bufferBody = new dispBuffer16(w, 98);
  bufferFoot = new dispBuffer16(w, 15);

}

void display::drawBuffer(){
  drawHead();
  drawBody();
  drawFoot();

}

void display::refresh(){
  drawBuffer();
  updateDisplay();
}

void display::updateDisplay(){
  writeRGBMap(0, 0, bufferHead,w,15);
  writeRGBMap(0, 15, bufferBody,w,98);
  writeRGBMap(0, 113, bufferFoot,w,15);

}

void display::drawHead(){

  //setup
  bufferHead->fillScreen(BLACK); //all Black
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setTextSize(1);

  //line
  bufferHead->drawRect(0,14,w,1,WHITE);     //box
  bufferHead->fillRect(0,0,56,15,WHITE);   //spacer
  //bufferHead->drawRect(105  ,0,1,15,WHITE);  //spacer

  bufferHead->setCursor(2, 3);
  bufferHead->setTextColor(BLACK, WHITE);
  bufferHead->print("MiniFrank");

  //BPM
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setCursor(61, 3);
  bufferHead->print("BPM:");
  bufferHead->setTextColor(COLOR, BLACK);
  bufferHead->print(stat->getBPM());

  //MIDI Settings
  bufferHead->setCursor(109, 3);
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->print("MIDI:");
  bufferHead->setTextColor(COLOR, BLACK);

  if(config->getMidiType()){
    bufferHead->print("USB");

  }
  else{
    bufferHead->print("DIN");
  }
}

void display::drawFoot(){
  //setup
  bufferFoot->fillScreen(BLACK); //all Black
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->setTextSize(1);

  //line
  bufferFoot->drawRect(0,0,w,1,WHITE);     //box
  bufferFoot->fillRect(0,0,35,15,WHITE);   //spacer
  //bufferFoot->drawRect(105  ,0,1,15,WHITE);  //spacer

   bufferFoot->setCursor(2, 4);
   bufferFoot->setTextColor(BLACK, WHITE);
   bufferFoot->print("SEQ:");

   bufferFoot->print(stat->getActiveSeq());

  //BPM
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->setCursor(61, 4);
  bufferFoot->print("CLK:");
  bufferFoot->setTextColor(COLOR, BLACK);
  bufferFoot->print(stat->getClock());

  //MIDI Settings
  bufferFoot->setCursor(115, 4);
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->print("PAGE:");
  bufferFoot->setTextColor(COLOR, BLACK);
  bufferFoot->print(config->getNumberPages());

}

void display::drawBody(){
  bufferBody->fillScreen(0x39E7); //all Black

  bufferBody->setTextColor(WHITE, BLACK);
  bufferBody->setFont(&FreeSansBold12pt7b);

  for(int x = 0; x < 4 ; x++){
    for(int y = 0; y < 2 ; y++){
      bufferBody->drawRect(x*40,y*35,40,35,BLACK);     //box
      bufferBody->setCursor(x*40 +5, y*35+25);

      bufferBody->print("A0");

    }
  }


  byte width = 160 / config->getNumberPages();

  for(int x = 0; x < config->getNumberPages() ; x++){
    if(x == stat->getActivePage()){
      bufferBody->fillRect(x*width,70,width,38,COLOR);     //box
      bufferBody->drawRect(x*width,70,width,38,0x39E7);     //box
    }
    else{
      bufferBody->fillRect(x*width,70,width,38,BLACK);     //box
      bufferBody->drawRect(x*width,70,width,38,0x39E7);     //box

    }
  }


  // static byte test = 0;
  //
  //
  // bufferBody->setFont(&FreeSansBold24pt7b);
  // bufferBody->setTextSize(1);
  // bufferBody->setCursor(52  , 60);
  // bufferBody->setTextColor(WHITE, BLACK);
  // bufferBody->print("A");
  // bufferBody->print(test);
  //
  // test = (test + 1)%8;

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
    memset(buffer2, 0, bytes);
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
