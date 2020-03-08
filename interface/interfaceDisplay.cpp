#include "interfaceDisplay.hpp"
#include <string.h>

// Debug logging
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
#endif

// GETDATAOBJ

void Display::initLCD(byte w, byte h, byte rotation) {
    pinMode(LCD_BL, OUTPUT);

    lcd.initR(INITR_GREENTAB); // Init ST7735S chip, green tab   ST7735_MADCTL_BGR changed to 0x00 !!!!!
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

inline void Display::updateDisplay() {
    writeRGBMap(0, 0, bufferHead, w, 15);
    writeRGBMap(0, 15, bufferBody, w, 98);
    writeRGBMap(0, 113, bufferFoot, w, 15);
}

inline void Display::drawBody() {
    bufferBody->fillScreen(0x39E7); // resetBuffer

    // !!!!! old statement said DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) - ist aber kein array??
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1 || DATAOBJ.get(FrankData::screenConfig)) { // Load Config Template

        // BodyTemplateSeq();  //Load Seq Template

        BodyTemplateMenu();
    } else if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel))) {
        if (DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel))) {
            BodyTemplateSeq(); // Load Seq Template

            // BodyTemplateArp(); //Load Arp Template
        } else {
            BodyTemplateSeq(); // Load Seq Template

            //  BodyTemplateLive(); //Load Live Template
        }
    } else {
        BodyTemplateSeq(); // Load Seq Template
    }
}

inline void Display::BodyTemplateLive() { // has 1 dataFields + GateSignal
    /////Draw the squares/////
    bufferBody->drawRect(1, 1, 158, 96, BLUE_DARK);

    /////Print Text to Field/////
    bufferBody->setTextColor(WHITE, BLUE_DARK); // font Color
    bufferBody->setFont();                      // reset to default font

    ////Noch schrift anpassen, rote makierung das der ton gerade gedrückt wird.
    // Note Value
    bufferBody->setCursor(50, 50);
    bufferBody->setFont(&FreeSansBold12pt7b);
    //                bufferBody->print(valueToNote(DATAOBJ.getData(NOTE)));   //zugriff auf den aktuell gespielten
    //                ton??

    bufferBody->setCursor(70, 40);
    bufferBody->setFont(&FreeSansBold9pt7b);
    //                bufferBody->print(valueToSharp(DATAOBJ.getData(NOTE)));

    bufferBody->setFont();
    bufferBody->setCursor(70, 60);
    //                bufferBody->print(valueToOctave(DATAOBJ.getData(NOTE)));
}

inline void Display::BodyTemplateMenu() { // has 2x4 dataFields + PageBar
    // DataFields

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 2; y++) {
            byte dataField = x + y * 4;       // current DataField
            if (mapping(dataField) != NONE) { // not an empty field?
                byte fieldWidth = 40;
                byte fieldHeight = 48;
                byte posX = x * fieldWidth;
                byte posY = y * fieldHeight;
                /////Draw the squares/////
                bufferBody->fillRect(posX, posY, fieldWidth, 20, 0x230E);
                bufferBody->drawRect(posX, posY, fieldWidth, fieldHeight, BLUE_DARK);

                /////Print Text to Field/////
                bufferBody->setTextColor(WHITE, 0x230E); // font Color
                bufferBody->setFont();                   // reset to default font

                /////Name/////
                char *string = toStr(mapping(dataField)); // get name
                byte offset = strlen(string) * 3;         // get name length

                bufferBody->setCursor(posX + 20 - offset, posY + 7); // set Cursor
                bufferBody->print(string);                           // print value to display

                /////Data/////

                bufferBody->setTextColor(WHITE, BLUE); // font Color

                // vll.. kann man alle werte auf string mappen damit wir int, sowie strings hier printen können, müsste
                // aber in der datenklasse passieren, vll getDataString?

                /*
                                ===== what does the next line mean/should do?
                                 "index" is a external function, thus a bad variable name,
                                 because if not defined differently, it is known as a function pointer.

                          */
                // char *data = toStr(mapping(dataField), index); // string buffer
                char *data = toStr(mapping(dataField)); // temporary removed index

                byte length = strlen(data); // string length

                if (length == 3) { // 3 Digit
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(posX + 20 - 16, posY + 38);
                } else if (length == 2) { // 2 Digit
                    bufferBody->setFont(&FreeSansBold12pt7b);
                    bufferBody->setCursor(posX + 20 - 13, posY + 41);
                } else { // 1 Digit
                    bufferBody->setFont(&FreeSansBold12pt7b);
                    bufferBody->setCursor(posX + 20 - 6, posY + 41);
                }
                bufferBody->print(data); // print value
            }
        }
    }
}

inline void Display::BodyTemplateSeq() { // has 2x4 dataFields + PageBar
    // Seq activeSeq = &DATAOBJ.seq[DATAOBJ.getScreenChannel()];

    // DataFields
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 2; y++) {
            byte dataField = x + y * 4; // current DataField

            byte dataFieldIndex = x + y * 4 + DATAOBJ.get(FrankData::activePage) * 8; // current index

            /////Draw the squares/////
            bufferBody->drawRect(x * 40, y * 36, 40, 35, BLUE_DARK); //

            /////Draw red bar for the ActiveDataField (STEP) /////
            if (DATAOBJ.get(FrankData::stepOnPage) == (x + y * 4)) {
                bufferBody->fillRect(x * 40 + 2, y * 35 + 30, 36, 4, RED); // red bar for active Step
            }

            /////Print Text to Field/////

            // Font Color depends on Gate status

            if (DATAOBJ.get(FrankData::seqGate, DATAOBJ.get(FrankData::screenOutputChannel), dataFieldIndex)) {
                bufferBody->drawRect(x * 40 + 1, y * 35 + 2, 38, 33, BLACKBLUE); // Blue Gate on Rectangle
                bufferBody->setTextColor(WHITE, BLUE);                           // Note  Color GateOn
            } else {
                bufferBody->setTextColor(BLUE_LIGHT, BLUE); // Note Color GateOff
            }

            // Data is NOTE type
            if (mapping(dataField) == NOTE) {
                // Note Value
                byte note =
                    DATAOBJ.get(FrankData::seqNote, DATAOBJ.get(FrankData::screenOutputChannel), dataFieldIndex);
                bufferBody->setCursor(x * 40 + 5, y * 35 + 20);
                bufferBody->setFont(&FreeSansBold12pt7b);
                bufferBody->print(valueToNote(note));

                bufferBody->setCursor(x * 40 + 26, y * 35 + 27);
                bufferBody->setFont(&FreeSansBold9pt7b);
                bufferBody->print(valueToSharp(note));

                bufferBody->setFont();
                bufferBody->setCursor(x * 40 + 30, y * 35 + 5);
                bufferBody->print(valueToOctave(note));
            }

            // Data is default type (123456789, max 3 digits)
            else {
                if (DATAOBJ.getData(mapping(dataField), dataFieldIndex) < 100) { // value less 100
                    bufferBody->setFont(&FreeSansBold12pt7b);
                    bufferBody->setCursor(x * 40 + 7, y * 35 + 25);

                } else { // value greater 99
                    bufferBody->setFont(&FreeSansBold9pt7b);
                    bufferBody->setCursor(x * 40 + 4, y * 35 + 24);
                }
                bufferBody->print(DATAOBJ.getData(mapping(dataField), dataFieldIndex)); // print value
            }
        }
    }

    ///// PageBlocks /////
    byte width = 160 / DATAOBJ.get(FrankData::currentPageNumber);                // block width
    byte offset = (160 - DATAOBJ.get(FrankData::currentPageNumber) * width) / 2; // center blocks

    for (int x = 0; x < DATAOBJ.get(FrankData::currentPageNumber); x++) {
        bufferBody->drawRect(x * width + offset, 72, width, 25, BLUE_DARK);         // dark Rectangle
        bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, GREEN); // grey box

        if (x == DATAOBJ.get(FrankData::activePage)) {
            bufferBody->fillRect(x * width + 1 + offset, 72 + 1, width - 2, 23, RED); // Red Block (active)
        }
    }
}

inline void Display::drawHead() {
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
    bufferHead->print(DATAOBJ.get(FrankData::bpm));

    // MIDI Settings
    bufferHead->setCursor(109, 3);
    bufferHead->setTextColor(WHITE, BLACK);
    bufferHead->print("MIDI:");
    bufferHead->setTextColor(BLACKBLUE, BLACK);

    if (DATAOBJ.get(FrankData::midiSource)) {
        bufferHead->print("USB");
        6
    } else {
        bufferHead->print("DIN");
    }
}

inline void Display::drawFoot() {
    // setup
    bufferFoot->fillScreen(BLACK); // all Black
    bufferFoot->setTextColor(WHITE, BLACK);
    bufferFoot->setTextSize(1);

    // line
    bufferFoot->drawRect(0, 0, w, 1, WHITE);       // box
    bufferFoot->fillRect(0, 1, 34, 14, BLACKBLUE); // spacer

    // Sequence
    bufferFoot->setCursor(2, 5);
    bufferFoot->setTextColor(WHITE, BLACKBLUE);
    bufferFoot->print("CH:");

    bufferFoot->print(DATAOBJ.get(FrankData::screenOutputChannel));

    // STOP PLAY
    bufferFoot->fillRect(36, 1, 40, 14, BLACKBLUE); // spacer

    bufferFoot->setCursor(40, 5);
    bufferFoot->setTextColor(WHITE, BLACKBLUE);

    if (DATAOBJ.get(FrankData::play)) {
        bufferFoot->print("PLAY");
    } else {
        bufferFoot->print("STOP");
    }

    // PlayDirection
    bufferFoot->setCursor(67, 5);
    bufferFoot->setTextColor(WHITE, BLACKBLUE);

    if (DATAOBJ.get(FrankData::direction)) {
        bufferFoot->print((char)175);
    } else {
        bufferFoot->print((char)174);
    }

    // Tuning
    bufferFoot->fillRect(78, 1, 48, 14, BLACKBLUE); // spacer
    bufferFoot->setCursor(82, 5);
    bufferFoot->setTextColor(WHITE, BLACKBLUE);

    bufferFoot->print("TUNE:");

    bufferFoot->print(tuningToChar(DATAOBJ.get(FrankData::seqTuning, DATAOBJ.get(FrankData::screenOutputChannel))));

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

inline void Display::writeRGBMap(int16_t x, int16_t y, DispBuffer16 *bufferObj, int16_t w, int16_t h) {
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
    if (buffer)
        free(buffer);
}

inline void DispBuffer16::copyBuffer(uint16_t bufferIndex) { buffer2[bufferIndex] = buffer[bufferIndex]; }

inline int DispBuffer16::compareBuffer(uint16_t bufferIndex) { return (buffer2[bufferIndex] != buffer[bufferIndex]); }

/////////////Stuff from Adafruit_GFX////////////
inline void DispBuffer16::drawPixel(int16_t x, int16_t y, uint16_t color) {
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

inline void DispBuffer16::fillScreen(uint16_t color) {
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

inline void DispBuffer16::byteSwap(void) {
    if (buffer) {
        uint32_t i, pixels = WIDTH * HEIGHT;
        for (i = 0; i < pixels; i++)
            buffer[i] = __builtin_bswap16(buffer[i]);
    }
}
