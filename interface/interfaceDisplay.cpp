#include "interfaceDisplay.hpp"
#include <string.h>


#define DEBUG 0

#if DEBUG == 1
#define PRINTLN(x) Serial.println(x)
#define PRINTLN2(x, y) Serial.println(x, y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x, y) Serial.print(x, y)
#else
#define PRINTLN(x)
#define PRINTLN2(x, y)
#define PRINT(x)
#define PRINT2(x, y)
#define DEBUGPRINTBEGIN
#endif


// GETDATAOBJ

void Display::initLCD(byte w, byte h, byte rotation) {
    pinMode(LCD_BL, OUTPUT);

    ////////////WICHTIG!!///////
        
    ////ST7735_MADCTL_BGR changed to 0x00 !!!!!

    //////////////

    lcd.initR(INITR_GREENTAB); // Init ST7735S chip, green tab   
    lcd.setRotation(rotation); // set display rotation
    lcd.fillScreen(BLACK);     // init black
}

void Display::displayBrightness(byte brightness) { analogWrite(LCD_BL, brightness); }

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
    // switch (0) {        ///hier muss noch der der aktuelle subscreen gefragt werden
    // case 0: drawBodyNote(); break;
    // case 1: drawBodyGate(); break;
    //}
}

void Display::refresh() {
    displayBrightness(DATAOBJ.get(FrankData::displayBrightness)); // set display Brightness
    drawBuffer();
    updateDisplay();
}

void Display::updateDisplay() {
    writeRGBMap(0, 0, bufferHead, w, 15);
    writeRGBMap(0, 15, bufferBody, w, 98);
    writeRGBMap(0, 113, bufferFoot, w, 15);
}

void Display::drawBody() {
    bufferBody->fillScreen(BACKGROUND); // resetBuffer

    if (DATAOBJ.get(FrankData::screenMainMenu) == 1 || DATAOBJ.get(FrankData::screenConfig) == 1 || DATAOBJ.get(FrankData::screenRouting) == 1 || DATAOBJ.get(FrankData::screenCal) == 1) { // Load Config Template
       BodyTemplateMenu();
    } 
    
    else if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) { //Live Mode
            BodyTemplateLive();
    } 
    else if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) > 0) { //Seq Mode
            BodyTemplateSeq();
    }
}


void Display::drawFoot() {
    if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) { //Live Mode
        FootLive();
    } 
    else if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) > 0) { //Seq Mode
        FootSeq();
    }
}

void Display::BodyTemplateLive() { // has 1 dataFields + GateSignal
   for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            byte dataField = x + y * 4;       // current DataField
            if (mapping(dataField) != NONE) { // not an empty field?
                byte fieldWidth = 40;
                byte fieldHeight = 48;
                byte posX = x * fieldWidth;
                byte posY = y * fieldHeight;
                /////Draw the squares/////
                bufferBody->fillRect(posX - 1, posY + y, fieldWidth + 1, 20, COLORTHEME);
                bufferBody->drawRect(posX - 1, posY + y, fieldWidth + 1, fieldHeight + 2, DARKGREY);

                /////Print Text to Field/////
                bufferBody->setTextColor(WHITE, COLORTHEME); // font Color
                bufferBody->setFont();                   // reset to default font

                /////Name/////
                const char *string = DATAOBJ.getNameAsStr(mapping(dataField)); // get name
                byte offset = strlen(string) * 3;         // get name length

                bufferBody->setCursor(posX + 20 - offset, posY + 9 + y * 1); // set Cursor
                bufferBody->print(string);                                   // print value to display

                /////Data/////

                bufferBody->setTextColor(WHITE, GREY); // font Color

                const char *data = DATAOBJ.getValueAsStr(mapping(dataField)); // temporary removed index

                byte length = strlen(data); // string length

                if (length == 4) { // 3 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 19, posY + 40 + y);
                }
                if (length == 3) { // 3 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 17, posY + 40 + y);
                }
                else if (length == 2) { // 2 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 12, posY + 40 + y);
                }
                else if (length == 1) { // 1 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 6, posY + 40 + y);
                }
                bufferBody->print(data); // print value
            }
        }
    }
                bufferBody->setTextColor(WHITE,0x230E); // font Color
                //byte note = 1;
                byte note;
                
                if (DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel))) note = DATAOBJ.get(FrankData::liveKeyArpNoteEvaluated, DATAOBJ.get(FrankData::screenOutputChannel));
                else note = DATAOBJ.get(FrankData::liveKeyNoteEvaluated, DATAOBJ.get(FrankData::screenOutputChannel));

                bufferBody->setFont(&FreeSansBold24pt7b);
                bufferBody->setCursor(95, 70);
                bufferBody->print(valueToNote(note));

                bufferBody->setFont(&FreeSansBold12pt7b);
                bufferBody->setCursor(135, 70);
                bufferBody->print(valueToSharp(note));

                bufferBody->setFont(&FreeSansBold9pt7b);
                bufferBody->setCursor(136, 45);
                bufferBody->print(valueToOctave(note));
    



}

void Display::BodyTemplateSeq() { // has 2x4 dataField

    // Seq activeSeq = &DATAOBJ.seq[DATAOBJ.getScreenChannel()];

    // DataFields
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 2; y++) {
            byte dataField = x + y * 4; // current DataField

            byte dataFieldIndex = x + y * 4 + DATAOBJ.get(FrankData::activePage, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1) * 8; // current index

            /////Draw the squares/////
            bufferBody->drawRect(x * 40, y * 36 - 1 + y, 40, 38, DARKGREY); //

            /////Draw red bar for the ActiveDataField (STEP) /////
            if (DATAOBJ.get(FrankData::stepOnPage, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1) == (x + y * 4)) {
                bufferBody->fillRect(x * 40 + 1, y * 35 + 32 +y, 38, 4, RED); // red bar for active Step
            }


            // Font Color depends on Gate status

            if (DATAOBJ.get(FrankData::seqGate, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, dataFieldIndex)) {
                bufferBody->drawRect(x * 40+1 , y * 36 +y, 38, 36, GREYWHITE); // Blue Gate on Rectangle
            }

            bufferBody->setTextColor(WHITE, BACKGROUND); // Note  Color GateOn

            // PRINT("DataFieldType:");
            // PRINTLN(mapping(dataField));

            // Data is NOTE type
            if (mapping(dataField) == NOTE) {
                // Note Value
                byte note =
                    DATAOBJ.get(FrankData::seqNote, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, dataFieldIndex);
                bufferBody->setFont(&FreeSansBold12pt7b);
                bufferBody->setCursor(x * 40 + 7, y * 35 + 27 + y);
                bufferBody->print(valueToNote(note));

                bufferBody->setFont(&FreeSansBold9pt7b);
                bufferBody->setCursor(x * 40 + 26, y * 35 + 27);
                bufferBody->print(valueToSharp(note));

                bufferBody->setFont();
                bufferBody->setCursor(x * 40 + 30, y * 35 + 5);
                bufferBody->print(valueToOctave(note));
            }

            // Data is default type (123456789, max 3 digits)
            else { 
                if (DATAOBJ.get(mapping(dataField), DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, dataFieldIndex) <
                    10) { //1 digit
                    bufferBody->setFont(&FreeSansBold12pt7b);
                    bufferBody->setCursor(x * 40+ 12, y * 35 + 25);
                    }
                else if (DATAOBJ.get(mapping(dataField), DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, dataFieldIndex) <
                    100) { // 2digit
                    bufferBody->setFont(&FreeSansBold12pt7b);
                    bufferBody->setCursor(x * 40 + 6, y * 35 + 25);

                } else { // 3 digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(x * 40 + 4, y * 35 + 24);
                }
                bufferBody->print(DATAOBJ.get(mapping(dataField), DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1,
                                                  dataFieldIndex)); // print value
            }
        }
    }

    ///// PageBlocks /////
    byte width = 160 / DATAOBJ.get(FrankData::currentPageNumber, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1);                // block width
    byte offset = (160 - DATAOBJ.get(FrankData::currentPageNumber, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1) * width) / 2; // center blocks

    for (int x = 0; x < DATAOBJ.get(FrankData::currentPageNumber, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1); x++) {
        bufferBody->drawRect(x * width + offset, 73, width, 25, DARKGREY);          // dark Rectangle
        bufferBody->fillRect(x * width + 1 + offset, 73 + 1, width - 2, 23, GREEN); // grey box

        if (x == DATAOBJ.get(FrankData::activePage, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1)) {
            bufferBody->fillRect(x * width + 1 + offset, 73 + 1, width - 2, 23, RED); // Red Block (active)
        }
    }
}

void Display::BodyTemplateMenu() { // has 2x4 dataFields + PageBar
      for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 2; y++) {
            byte dataField = x + y * 4;       // current DataField
            if (mapping(dataField) != NONE) { // not an empty field?
                byte fieldWidth = 40;
                byte fieldHeight = 48;
                byte posX = x * fieldWidth;
                byte posY = y * fieldHeight;
                /////Draw the squares/////
                bufferBody->fillRect(posX - 1, posY + y, fieldWidth + 1, 20, COLORTHEME);
                bufferBody->drawRect(posX-1, posY + y, fieldWidth + 1, fieldHeight +2, DARKGREY);

                /////Print Text to Field/////
                bufferBody->setTextColor(WHITE, COLORTHEME); // font Color
                bufferBody->setFont();                   // reset to default font

                /////Name/////
                const char *string = DATAOBJ.getValueAsStrChannel(mapping(dataField),DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel))); // get name
                byte offset = strlen(string) * 3;         // get name length

                bufferBody->setCursor(posX + 20 - offset, posY + 7 + y * 1); // set Cursor
                bufferBody->print(string);                           // print value to display

                /////Data/////

                bufferBody->setTextColor(WHITE, GREY); // font Color

                // vll.. kann man alle werte auf string mappen damit wir int, sowie strings hier printen können, müsste
                // aber in der datenklasse passieren, vll getDataString?

                /*
                                ===== what does the next line mean/should do?
                                 "index" is a external function, thus a bad variable name,
                                 because if not defined differently, it is known as a function pointer.

                          */
                // char *data = toStr(mapping(dataField), index); // string buffer
                const char *data = DATAOBJ.getValueAsStrChannel(mapping(dataField),DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel))); // temporary removed index

                byte length = strlen(data); // string length
                if((byte)data[0] == 64){
                
                bufferBody->fillCircle(posX + 20,  posY + 35, 7, WHITE);
                
                }
                else{
                
                if (length == 4) { // 3 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 19, posY + 40+ y);
                }
                if (length == 3) { // 3 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 17, posY + 40 + y);
                } else if (length == 2) { // 2 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 12, posY + 40 + y);
                } else if(length == 1) { // 1 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 6, posY + 40 + y);
                }
                bufferBody->print(data); // print value
                }
                }
            }
        }
    }


void Display::drawHead() {
    // setup
    bufferHead->fillScreen(COLORTHEME); // all Black
    bufferHead->setTextColor(WHITE, COLORTHEME);
    bufferHead->setTextSize(1);

    // BPM
    bufferHead->setCursor(4, 4);
    bufferHead->print("BPM:");
    bufferHead->print(DATAOBJ.get(FrankData::bpm));

    // CLK
    bufferHead->setCursor(58, 4);
    bufferHead->print("CLK: ");

    bufferHead->print(DATAOBJ.getValueAsStrChannel(FrankData::outputClock, 0));
    bufferHead->setCursor(130, 4);

    bufferHead->print(DATAOBJ.getValueAsStrChannel(FrankData::outputClock, 1));

}




void Display::FootLive() {
  // setup
  bufferFoot->fillScreen(COLORTHEME); // all Black
  bufferFoot->setTextColor(WHITE, BLACK);
  bufferFoot->setTextSize(1);

  bufferFoot->setCursor(4, 4);
  bufferFoot->setTextColor(WHITE, COLORTHEME);
  bufferFoot->print("OUT:");

  bufferFoot->print(DATAOBJ.get(FrankData::screenOutputChannel)+1);


  bufferFoot->setCursor(48, 4);
  bufferFoot->print(DATAOBJ.getValueAsStr(FrankData::midiSource));


  bufferFoot->setCursor(76, 4);
  bufferFoot->print("CH:");
  bufferFoot->print(DATAOBJ.getValueAsStr(FrankData::outputChannel));


  bufferFoot->setCursor(117, 4);
  bufferFoot->print("CC:");
  bufferFoot->print(DATAOBJ.getNameAsStr(FrankData::outputCcEvaluated));

}

void Display::FootSeq() {
    // setup
    bufferFoot->fillScreen(COLORTHEME); // all Black
    bufferFoot->setTextColor(WHITE, COLORTHEME);
    bufferFoot->setTextSize(1);

    // line
    
    // OUT CHannel
    bufferFoot->setCursor(4, 4);
    bufferFoot->print("OUT:");
    bufferFoot->print(DATAOBJ.get(FrankData::screenOutputChannel)+1);

    // STOP PLAY
    bufferFoot->setCursor(45, 4);
    bufferFoot->print(DATAOBJ.getValueAsStr(FrankData::play));

    // PlayDirection
    bufferFoot->setCursor(72, 4);

    if (DATAOBJ.get(FrankData::direction)) {
        bufferFoot->print((char)175);
    } else {
        bufferFoot->print((char)174);
    }

    // Tuning
    bufferFoot->setCursor(87, 4);
    bufferFoot->print("TUNE:");
    bufferFoot->print(tuningToChar(DATAOBJ.get(FrankData::seqTuning, DATAOBJ.get(FrankData::screenOutputChannel))));

    // Offset Gate

    const char *data = DATAOBJ.getValueAsStr(FrankData::seqGateLengthOffset); // temporary removed index

    byte length = strlen(data); // string length
 
        if (length == 4) { // 3 Digit
            bufferFoot->setCursor(134, 4);
        }
        if (length == 3) { // 3 Digit
            bufferFoot->setCursor(136, 4);
        }
        else if (length == 2) { // 2 Digit
            bufferFoot->setCursor(138, 4);
        }
        else if (length == 1) { // 1 Digit
            bufferFoot->setCursor(140, 4);
        }
        bufferFoot->print(data); // print value
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
    if (buffer) free(buffer);
    if (buffer2) free(buffer2);
}

void DispBuffer16::copyBuffer(uint16_t bufferIndex) { buffer2[bufferIndex] = buffer[bufferIndex]; }

int DispBuffer16::compareBuffer(uint16_t bufferIndex) { return (buffer2[bufferIndex] != buffer[bufferIndex]); }

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
