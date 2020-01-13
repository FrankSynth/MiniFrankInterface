#include "interfaceDisplay.hpp"

void display::initLCD(byte w, byte h, byte rotation) {
  pinMode(LCD_BL , OUTPUT);

  lcd.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab   ST7735_MADCTL_BGR changed to 0x00 !!!!!
  lcd.setRotation(rotation);      // set display rotation
  lcd.fillScreen(BLACK);          // init black
}

void display::displayBrightness(byte brightness) {
  analogWrite(LCD_BL, brightness);
}

void display::initBuffer() {
  bufferHead = new dispBuffer16(w, 15);
  bufferBody = new dispBuffer16(w, 98);
  bufferFoot = new dispBuffer16(w, 15);

}

void display::drawBuffer() {
  drawHead();
  drawFoot();

  //select active pane
  switch (settings::getActivePane()) {
    case 0:  drawBodyNote(); break;
    case 1:  drawBodyGate(); break;
  }

}

void display::refresh() {
  displayBrightness(settings::getDisplayBrightness()); //set display Brightness
  drawBuffer();
  updateDisplay();
}

void display::updateDisplay() {
  writeRGBMap(0, 0, bufferHead, w, 15);
  writeRGBMap(0, 15, bufferBody, w, 98);
  writeRGBMap(0, 113, bufferFoot, w, 15);

}

void display::drawHead() {
  //setup
  bufferHead->fillScreen(BLACK); //all Black
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setTextSize(1);

  //line
  bufferHead->drawRect(0, 14, w, 1, WHITE); //box
  bufferHead->fillRect(0, 0, 56, 15, WHITE); //spacer
  //bufferHead->drawRect(105  ,0,1,15,WHITE);  //spacer

  bufferHead->setCursor(2, 3);
  bufferHead->setTextColor(BLACK, WHITE);
  bufferHead->print("MiniFrank");

  //BPM
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setCursor(61, 3);
  bufferHead->print("BPM:");
  bufferHead->setTextColor(COLOR, BLACK);
  bufferHead->print(settings::getBPM());

  //MIDI Settings
  bufferHead->setCursor(109, 3);
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->print("MIDI:");
  bufferHead->setTextColor(COLOR, BLACK);

  if (settings::getMidiSource()) {
    bufferHead->print("USB");

  }
  else {
    bufferHead->print("DIN");
  }
}

void display::drawFoot() {
  //setup
  bufferFoot->fillScreen(BLACK); //all Black
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->setTextSize(1);

  //line
  bufferFoot->drawRect(0, 0, w, 1, WHITE); //box
  bufferFoot->fillRect(0, 1, 34, 14  , COLOR); //spacer

  //Sequence
  bufferFoot->setCursor(2, 5);
  bufferFoot->setTextColor(WHITE, COLOR);
  bufferFoot->print("SEQ:");

  bufferFoot->print(settings::getActiveSeq());

  //STOP PLAY
  bufferFoot->fillRect(36, 1, 40, 14, COLOR); //spacer

  bufferFoot->setCursor(40, 5);
  bufferFoot->setTextColor(WHITE, COLOR);

  if (settings::getPlayStop()) {
    bufferFoot->print("PLAY");
  }
  else {
    bufferFoot->print("STOP");

  }

  //PlayDirection
  bufferFoot->setCursor(67, 5);
  bufferFoot->setTextColor(WHITE, COLOR);

  if (settings::getDirection()) {
    bufferFoot->print((char)175);
  }
  else {
    bufferFoot->print((char)174);
  }


  //Tuning
  bufferFoot->fillRect(78, 1, 48, 14, COLOR); //spacer
  bufferFoot->setCursor(82, 5);
  bufferFoot->setTextColor(WHITE, COLOR);

  bufferFoot->print("TUNE:");
  if (settings::getActiveSeq() == 0) { //SEQ 1 aktiv
    bufferFoot->print(tuningToChar(seq1->getTuning()));
  }
  else if (settings::getActiveSeq() == 1) { //SEQ 2 aktiv
    bufferFoot->print(tuningToChar(seq2->getTuning()));
  }


  //Version
  bufferFoot->fillRect(128, 1, 36, 14, COLOR); //spacer
  bufferFoot->setCursor(132, 5);
  bufferFoot->setTextColor(WHITE, COLOR);

  bufferFoot->print("v0.1");
}

void display::drawBodyGate() {
  Seq *activeSeq;
  activeSeq = getActiveSeqPointer();

  bufferBody->fillScreen(0x39E7); //resetBuffer

  //NoteBlocks
  for (int x = 0; x < 4 ; x++) {
    for (int y = 0; y < 2 ; y++) {
      byte myStep = x + y * 4 + settings::getActivePage() * 8;

      bufferBody->setFont(); //reset Font
      bufferBody->drawRect(x * 40, y * 35 + 1, 40, 35, 0x2965); //

      //GateLength Value

      //Gate
      if (activeSeq->getGate(myStep)) {
        bufferBody->drawRect(x * 40 + 1, y * 35 + 2, 38, 33, COLOR); //Blue Gate on Rectangle
        bufferBody->setTextColor(WHITE, 0x39E7);  //Note  Color GateOn
      }
      else {
        bufferBody->setTextColor(0x94B2, 0x39E7); //Note Color GateOff
      }

      //Gate Value
      if (activeSeq->getGateLength(myStep) < 100) {
        bufferBody->setFont(&FreeSansBold12pt7b);
        bufferBody->setCursor(x * 40 + 7, y * 35 + 25);

      }
      else {
        bufferBody->setFont(&FreeSansBold9pt7b);
        bufferBody->setCursor(x * 40 + 4, y * 35 + 24);

      }
      bufferBody->print(activeSeq->getGateLength(myStep));

      //bufferBody->print("%");


      //CurrentStep
      if (settings::getStepOnPage() == (x + y * 4)) {
        bufferBody->fillRect(x * 40 + 2, y * 35 + 30, 36, 4, RED); //red bar for active Step
      }
    }
  }

  //PageBlocks
  byte width = 160 / settings::getCurrentNumberPages();  //block width
  byte offset = (160 - settings::getCurrentNumberPages() * width) / 2; //center blocks

  for (int x = 0; x < settings::getCurrentNumberPages() ; x++) {
    bufferBody->drawRect(x * width + offset, 72, width, 25, 0x2965); //dark Rectangle
    bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, 0x4208); //grey box

    if (x == settings::getActivePage()) {
      bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, RED); //RedBlock of active
    }
  }
}

void display::drawBodyNote() {
  Seq* activeSeq;

  activeSeq = getActiveSeqPointer();  //get active Sequence
  bufferBody->fillScreen(0x39E7);     //resetBuffer

  //NoteBlocks
  for (int x = 0; x < 4 ; x++) {
    for (int y = 0; y < 2 ; y++) {

      byte myStep = x + y * 4 + settings::getActivePage() * 8;


      bufferBody->setFont(); //reset Font
      bufferBody->setCursor(x * 40 + 5, y * 35 + 20);
      bufferBody->drawRect(x * 40, y * 35 + 1, 40, 35, 0x2965); //

      //Note Value

      byte noteValue = 0;
      noteValue = activeSeq->getNote(myStep);


      //Gate
      if (activeSeq->getGate(myStep)) {
        bufferBody->drawRect(x * 40 + 1, y * 35 + 2, 38, 33, COLOR); //Blue Gate on Rectangle
        bufferBody->setTextColor(WHITE, 0x39E7);  //Note  Color GateOn
      }
      else {
        bufferBody->setTextColor(0x94B2, 0x39E7); //Note Color GateOff
      }

      //Note Value
      bufferBody->setFont(&FreeSansBold12pt7b);
      bufferBody->print(valueToNote(noteValue));

      bufferBody->setCursor(x * 40 + 26, y * 35 + 27);
      bufferBody->setFont(&FreeSansBold9pt7b);
      bufferBody->print(valueToSharp(noteValue));

      bufferBody->setFont();
      bufferBody->setCursor(x * 40 + 30, y * 35 + 5);
      bufferBody->print(valueToOctave(noteValue));

      //CurrentStep
      if (settings::getStepOnPage() == (x + y * 4)) {
        bufferBody->fillRect(x * 40 + 2, y * 35 + 30, 36, 4, RED); //red bar for active Step
      }
    }
  }

  //PageBlocks
  byte width = 160 / settings::getCurrentNumberPages();  //block width
  byte offset = (160 - settings::getCurrentNumberPages() * width) / 2; //center blocks

  for (int x = 0; x < settings::getCurrentNumberPages() ; x++) {
    bufferBody->drawRect(x * width + offset, 72, width, 25, 0x2965); //dark Rectangle
    bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, 0x4208); //grey box

    if (x == settings::getActivePage()) {
      bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, RED); //RedBlock of active
    }
  }
}


void display::writeRGBMap(int16_t x, int16_t y, dispBuffer16 *bufferObj, int16_t w, int16_t h) {
  const uint16_t *buffer = bufferObj->getBuffer();
  lcd.startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++ ) {
      int16_t index = j * w + i;
      if (bufferObj->compareBuffer(index)) {
        lcd.writePixel(x + i, y, pgm_read_word(&buffer[index]));
        bufferObj->copyBuffer(index);
      }
    }
  }
  lcd.endWrite();
}

char display::valueToNote(byte noteIn) {

  byte note;
  note = (noteIn + 9) % 12;
  switch (note) {
    case 0:
    case 1:    return 'C';
    case 2:
    case 3:    return 'D';
    case 4:    return 'E';
    case 5:
    case 6:    return 'F';
    case 7:
    case 8:    return 'G';
    case 9:
    case 10:   return 'A';
    case 11:   return 'B';
  }
  return '\0';
}

char display::valueToOctave(byte noteIn) {

  byte octave;
  octave = (noteIn + 9) / 12;


  switch (octave) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
  }
  return '\0';

}

char display::valueToSharp(byte noteIn) {
  byte note;
  note = (noteIn + 9) % 12;


  if (note == 1 or note == 3 or note == 6 or note == 8 or note == 10)
  {
    return '#';
  }

  return '\0';
}

Seq* display::getActiveSeqPointer() {
  if ( settings::getActiveSeq() == 0) {
    return seq1;
  }
  else if (settings::getActiveSeq() == 1) {
    return seq2;
  }
  return NULL; //notValid

}



const char* display::tuningToChar(byte tuning) {

  switch (tuning) {
    case 0: return "C";
    case 1: return "C#";
    case 2: return "D";
    case 3: return "D#";
    case 4: return "E";
    case 5: return "F";
    case 6: return "F#";
    case 7: return "G";
    case 8: return "G#";
    case 9: return "A";
    case 10: return "A#";
    case 11: return "B";
    case 12: return "F";
    default: return "-";
  }
}


//Display Buffer based on the adafruit canvas, with 2 sepereate buffers for comparison
dispBuffer16::dispBuffer16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
  uint32_t bytes = w * h * 2;
  if ((buffer = (uint16_t *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
  if ((buffer2 = (uint16_t *)malloc(bytes))) {
    memset(buffer2, 0, bytes);
  }
}

dispBuffer16::~dispBuffer16(void) {
  if (buffer) free(buffer);
}

void dispBuffer16::copyBuffer(uint16_t index) {
  buffer2[index] = buffer[index];
}

int dispBuffer16::compareBuffer(uint16_t index) {
  return (buffer2[index] != buffer[index]);
}

/////////////Stuff from Adafruit_GFX////////////
void dispBuffer16::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

    int16_t t;
    switch (rotation) {
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
  if (buffer) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++) buffer[i] = color;
    }
  }
}

void dispBuffer16::byteSwap(void) {
  if (buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++) buffer[i] = __builtin_bswap16(buffer[i]);
  }
}
