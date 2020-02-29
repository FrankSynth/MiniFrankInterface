#include "interfaceDisplay.hpp"
#include <string.h>

// GETDATAOBJ

void Display::initLCD(byte w, byte h, byte rotation) {
  pinMode(LCD_BL, OUTPUT);

  lcd.initR(INITR_GREENTAB); // Init ST7735S chip, green tab   ST7735_MADCTL_BGR changed to 0x00 !!!!!
  lcd.setRotation(rotation); // set display rotation
  lcd.fillScreen(BLACK);     // init black
}

void Display::displayBrightness(byte brightness) {
  analogWrite(LCD_BL, brightness);
}

void Display::initBuffer() {
  bufferHead = new DispBuffer16(w, 15);
  bufferHead = new DispBuffer16(w, 15);
  bufferBody = new DispBuffer16(w, 98);
  bufferFoot = new DispBuffer16(w, 15);
}

void Display::drawBuffer() {
  drawHead();
  drawFoot();
  drawBody();


  // select active screen
  //switch (0) {        ///hier muss noch der der aktuelle subscreen gefragt werden
  //case 0: drawBodyNote(); break;
  //case 1: drawBodyGate(); break;
  //}
}

void Display::refresh() {
  displayBrightness(DATAOBJ.getDisplayBrightness()); // set display Brightness
  drawBuffer();
  updateDisplay();
}

void Display::updateDisplay() {
  writeRGBMap(0, 0, bufferHead, w, 15);
  writeRGBMap(0, 15, bufferBody, w, 98);
  writeRGBMap(0, 113, bufferFoot, w, 15);
}


void Display::drawBody() {
  bufferBody->fillScreen(0x39E7); // resetBuffer

  if(DATAOBJ.getMainMenuEnabled() == 1 || DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())){ //Load Config Template
    //BodyTemplateSeq();  //Load Seq Template

    BodyTemplateMenu();
  }
  else if(DATAOBJ.getOutputMode(DATAOBJ.getScreenChannel())){
    if(DATAOBJ.getArpModeEnable(DATAOBJ.getScreenChannel())){
      BodyTemplateSeq();  //Load Seq Template

      //BodyTemplateArp(); //Load Arp Template
    }
    else{
      BodyTemplateSeq();  //Load Seq Template

      //  BodyTemplateLive(); //Load Live Template
    }
  }
  else{
    BodyTemplateSeq();  //Load Seq Template
  }
}

void Display::BodyTemplateLive() {  //has 1 dataFields + GateSignal
        /////Draw the squares/////
        bufferBody->drawRect(1, 1, 158, 96, BLUE_DARK);

        /////Print Text to Field/////
        bufferBody->setTextColor(WHITE, BLUE_DARK); //font Color
        bufferBody->setFont();    //reset to default font

        ////Noch schrift anpassen, rote makierung das der ton gerade gedrückt wird.
              // Note Value
                bufferBody->setCursor(50, 50);
                bufferBody->setFont(&FreeSansBold12pt7b);
//                bufferBody->print(valueToNote(DATAOBJ.getData(NOTE)));   //zugriff auf den aktuell gespielten ton??

                bufferBody->setCursor(70, 40);
                bufferBody->setFont(&FreeSansBold9pt7b);
//                bufferBody->print(valueToSharp(DATAOBJ.getData(NOTE)));

                bufferBody->setFont();
                bufferBody->setCursor(70, 60);
//                bufferBody->print(valueToOctave(DATAOBJ.getData(NOTE)));
  }


void Display::BodyTemplateMenu() {  //has 2x4 dataFields + PageBar
  //DataFields

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 2; y++) {
      byte dataField= x + y * 4;         //current DataField
      if(mapping(dataField) != NONE){    //not an empty field?
        byte fieldWidth = 40;
        byte fieldHeight = 48 ;
        byte posX = x * fieldWidth ;
        byte posY = y * fieldHeight;
        /////Draw the squares/////
        bufferBody->fillRect(posX, posY, fieldWidth, 20, 0x230E);
        bufferBody->drawRect(posX, posY, fieldWidth, fieldHeight, BLUE_DARK);

        /////Print Text to Field/////
        bufferBody->setTextColor(WHITE, 0x230E); //font Color
        bufferBody->setFont();    //reset to default font

        /////Name/////
        char* string = DATAOBJ.getDataName(mapping(dataField)); //get name
        byte offset = strlen(string)* 3 ; //get name length

        bufferBody->setCursor(posX + 20 - offset, posY + 7); //set Cursor
        bufferBody->print(string);    //print value to display

        /////Data/////

        bufferBody->setTextColor(WHITE, BLUE); //font Color

        //vll.. kann man alle werte auf string mappen damit wir int, sowie strings hier printen können, müsste aber in der datenklasse passieren, vll getDataString?
        char* data =DATAOBJ.getDataString(mapping(dataField), index); //string buffer

        byte length = strlen(data);   //string length

        if (length == 3) {                                          //3 Digit
          bufferBody->setFont(&FreeSansBold9pt7b);
          bufferBody->setCursor(posX + 20 - 16, posY + 38);
        }
        else if(length == 2){                                       //2 Digit
          bufferBody->setFont(&FreeSansBold12pt7b);
          bufferBody->setCursor(posX + 20 -13, posY + 41);
        }
        else{                                                       //1 Digit
          bufferBody->setFont(&FreeSansBold12pt7b);
          bufferBody->setCursor(posX+20-6, posY + 41);
        }
        bufferBody->print(data);    //print value
      }
    }
  }
}





void Display::BodyTemplateSeq() {  //has 2x4 dataFields + PageBar
  Seq *activeSeq = &DATAOBJ.seq[DATAOBJ.getScreenChannel()];

  //DataFields
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 2; y++) {
      byte dataField= x + y * 4;                              //current DataField
      byte index = x + y * 4 + DATAOBJ.getActivePage() * 8;   //current index

      /////Draw the squares/////
      bufferBody->drawRect(x * 40, y * 36, 40, 35, BLUE_DARK); //

      /////Draw red bar for the ActiveDataField (STEP) /////
      if (DATAOBJ.getStepOnPage() == (x + y * 4)) {
        bufferBody->fillRect(x * 40 + 2, y * 35 + 30, 36, 4, RED); // red bar for active Step
      }


      /////Print Text to Field/////

      //Font Color depends on Gate status
      if (activeSeq->getGate(index)) {
        bufferBody->drawRect(x * 40 + 1, y * 35 + 2, 38, 33, BLACKBLUE); // Blue Gate on Rectangle
        bufferBody->setTextColor(WHITE, BLUE);                     // Note  Color GateOn
      } else {
        bufferBody->setTextColor(BLUE_LIGHT, BLUE); // Note Color GateOff
      }

      //Data is NOTE type
      if(mapping(dataField) == NOTE){
        // Note Value
        bufferBody->setCursor(x * 40 + 5, y * 35 + 20);
        bufferBody->setFont(&FreeSansBold12pt7b);
        bufferBody->print(valueToNote(DATAOBJ.getData(NOTE)));

        bufferBody->setCursor(x * 40 + 26, y * 35 + 27);
        bufferBody->setFont(&FreeSansBold9pt7b);
        bufferBody->print(valueToSharp(DATAOBJ.getData(NOTE)));

        bufferBody->setFont();
        bufferBody->setCursor(x * 40 + 30, y * 35 + 5);
        bufferBody->print(valueToOctave(DATAOBJ.getData(NOTE)));
      }

      //Data is default type (123456789, max 3 digits)
      else{
        if (DATAOBJ.getData(mapping(dataField), index) < 100) {  //value less 100
          bufferBody->setFont(&FreeSansBold12pt7b);
          bufferBody->setCursor(x * 40 + 7, y * 35 + 25);

        } else {                                                //value greater 99
          bufferBody->setFont(&FreeSansBold9pt7b);
          bufferBody->setCursor(x * 40 + 4, y * 35 + 24);
        }
        bufferBody->print(DATAOBJ.getData(mapping(dataField), index));    //print value
      }
    }
  }

  ///// PageBlocks /////
  byte width = 160 / DATAOBJ.getCurrentNumberPages();           // block width
  byte offset = (160 - DATAOBJ.getCurrentNumberPages() * width) / 2; // center blocks

  for (int x = 0; x < DATAOBJ.getCurrentNumberPages(); x++) {
    bufferBody->drawRect(x * width + offset, 72, width, 25, BLUE_DARK);             // dark Rectangle
    bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, GREEN); // grey box

    if (x == DATAOBJ.getActivePage()) {
      bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, RED); // Red Block (active)
    }
  }
}

void Display::drawHead() {
  // setup
  bufferHead->fillScreen(BLACK); // all Black
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setTextSize(1);

  // line
  bufferHead->drawRect(0, 14, w, 1, WHITE);  // box
  bufferHead->fillRect(0, 0, 56, 15, WHITE); // spacer
  // bufferHead->drawRect(105  ,0,1,15,WHITE);  //spacer

  bufferHead->setCursor(2, 3);
  bufferHead->setTextColor(BLACK, WHITE);
  bufferHead->print("MiniFrank");

  // BPM
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->setCursor(61, 3);
  bufferHead->print("BPM:");
  bufferHead->setTextColor(BLACKBLUE, BLACK);
  bufferHead->print(DATAOBJ.getBPM());

  // MIDI Settings
  bufferHead->setCursor(109, 3);
  bufferHead->setTextColor(WHITE, BLACK);
  bufferHead->print("MIDI:");
  bufferHead->setTextColor(BLACKBLUE, BLACK);

  if (DATAOBJ.getMidiSource()) {
    bufferHead->print("USB");

  } else {
    bufferHead->print("DIN");
  }
}

void Display::drawFoot() {
  // setup
  bufferFoot->fillScreen(BLACK); // all Black
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->setTextSize(1);

  // line
  bufferFoot->drawRect(0, 0, w, 1, WHITE);   // box
  bufferFoot->fillRect(0, 1, 34, 14, BLACKBLUE); // spacer

  // Sequence
  bufferFoot->setCursor(2, 5);
  bufferFoot->setTextColor(WHITE, BLACKBLUE);
  bufferFoot->print("CH:");

  bufferFoot->print(DATAOBJ.getScreenChannel());

  // STOP PLAY
  bufferFoot->fillRect(36, 1, 40, 14, BLACKBLUE); // spacer

  bufferFoot->setCursor(40, 5);
  bufferFoot->setTextColor(WHITE, BLACKBLUE);

  if (DATAOBJ.getPlayStop()) {
    bufferFoot->print("PLAY");
  } else {
    bufferFoot->print("STOP");
  }

  // PlayDirection
  bufferFoot->setCursor(67, 5);
  bufferFoot->setTextColor(WHITE, BLACKBLUE);

  if (DATAOBJ.getDirection()) {
    bufferFoot->print((char)175);
  } else {
    bufferFoot->print((char)174);
  }

  // Tuning
  bufferFoot->fillRect(78, 1, 48, 14, BLACKBLUE); // spacer
  bufferFoot->setCursor(82, 5);
  bufferFoot->setTextColor(WHITE, BLACKBLUE);

  bufferFoot->print("TUNE:");

  bufferFoot->print(tuningToChar(DATAOBJ.seq[DATAOBJ.getScreenChannel()].getTuning()));

  // if (DATAOBJ.getActiveSeq() == 0) { // SEQ 1 aktiv
  //     bufferFoot->print(tuningToChar(seq1->getTuning()));
  // } else if (DATAOBJ.getActiveSeq() == 1) { // SEQ 2 aktiv
  //     bufferFoot->print(tuningToChar(seq2->getTuning()));
  // }

  // Version
  bufferFoot->fillRect(128, 1, 36, 14, BLACKBLUE); // spacer
  bufferFoot->setCursor(132, 5);
  bufferFoot->setTextColor(WHITE, BLACKBLUE);

  bufferFoot->print("v0.1");
}


void Display::writeRGBMap(int16_t x, int16_t y, DispBuffer16 *bufferObj, int16_t w, int16_t h) {
  const uint16_t *buffer = bufferObj->getBuffer();
  lcd.startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      int16_t index = j * w + i;
      if (bufferObj->compareBuffer(index)) {
        lcd.writePixel(x + i, y, pgm_read_word(&buffer[index]));
        bufferObj->copyBuffer(index);
      }
    }
  }
  lcd.endWrite();
}

char Display::valueToNote(byte noteIn) {

  byte note;
  note = (noteIn + 9) % 12;
  switch (note) {
    case 0:
    case 1: return 'C';
    case 2:
    case 3: return 'D';
    case 4: return 'E';
    case 5:
    case 6: return 'F';
    case 7:
    case 8: return 'G';
    case 9:
    case 10: return 'A';
    case 11: return 'B';
  }
  return '\0';
}

char Display::valueToOctave(byte noteIn) {

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

char Display::valueToSharp(byte noteIn) {
  byte note;
  note = (noteIn + 9) % 12;

  if (note == 1 or note == 3 or note == 6 or note == 8 or note == 10) {
    return '#';
  }

  return '\0';
}

const char *Display::tuningToChar(byte tuning) {

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

// Display Buffer based on the adafruit canvas, with 2 sepereate buffers for comparison
DispBuffer16::DispBuffer16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
  uint32_t bytes = w * h * 2;
  if ((buffer = (uint16_t *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
  if ((buffer2 = (uint16_t *)malloc(bytes))) {
    memset(buffer2, 0, bytes);
  }
}

DispBuffer16::~DispBuffer16(void) {
  if (buffer)
  free(buffer);
}

void DispBuffer16::copyBuffer(uint16_t index) {
  buffer2[index] = buffer[index];
}

int DispBuffer16::compareBuffer(uint16_t index) {
  return (buffer2[index] != buffer[index]);
}

/////////////Stuff from Adafruit_GFX////////////
void DispBuffer16::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
    return;

    int16_t t;
    switch (rotation) {
      case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
      case 2:
      x = WIDTH - 1 - x;
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

void DispBuffer16::fillScreen(uint16_t color) {
  if (buffer) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++)
      buffer[i] = color;
    }
  }
}

void DispBuffer16::byteSwap(void) {
  if (buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++)
    buffer[i] = __builtin_bswap16(buffer[i]);
  }
}
