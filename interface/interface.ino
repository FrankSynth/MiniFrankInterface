#include "interfaceData.hpp"
#include "interfaceDisplay.hpp"
#include "interfaceIn.hpp"
#include "interfaceMapping.hpp"
#include "interfaceMiddleman.hpp"
#include "interfaceMidi.hpp"
#include <Arduino.h>
#include <MIDI.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Debug logging
#define DEBUG 1

#if DEBUG == 1
#define PRINTLN(x) Serial.println(x)
#define PRINTLN2(x,y) Serial.println(x,y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x,y) Serial.print(x,y)
#define DEBUGPRINTBEGIN Serial.begin(115200)
#else
#define PRINTLN(x) 
#define PRINTLN2(x,y) 
#define PRINT(x)
#define PRINT2(x,y)
#define DEBUGPRINTBEGIN
#endif

// status settings;   //init status object;
// mfMidi midi0; //create midi object

controls cntrl;

Display lcd = Display(160, 128, 3); // create display object, (width, heigh, rotation)

IntervalTimer myTimer;

// PressedNotesList noteList0;
// PressedNotesList noteList1;

void ISRSwitch(); // Switch Interrupt

// readSerial3
void readSerial3() {
    byte rcv;
    rcv = Serial3.read();

    PRINT("Message: ");
    PRINTLN2(rcv, BIN);

    cntrl.encode(rcv);
}

void updateDisplay() { // update interrupt
 
    lcd.refresh();

}

void setup() {
    // GETDATAOBJ
    // pin setup
    SPI.begin() ;

    DEBUGPRINTBEGIN;
    PRINTLN("Debug Mode");
    PRINTLN("HELLO FRANK Mini");

    PRINT("Brightness: ");
    PRINTLN(DATAOBJ.get(FrankData::displayBrightness));
    PRINTLN(DATAOBJ.get(FrankData::play));
    PRINT("BPM: ");
    PRINTLN(DATAOBJ.get(FrankData::bpm));
    PRINT("Clock: ");
    PRINTLN(DATAOBJ.get(FrankData::outputClock,0));

    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);

     lcd.displayBrightness(200);

    initMidi();
    initMiddleman();


    ////////////////////////
    // Start Devices
    digitalWrite(5, HIGH);
    delay(10u);

    // Start Connection to the uC
    Serial3.begin(115200);

    // SayHello to the uC
    byte send = B01010101;
    long timer = millis();
    byte timeout = 0;
    while (!Serial3.available() && !timeout) { // send hello until uC response (max 2seconds)
        Serial3.write(send);

        if (millis() - timer > 2000) {
            timeout = 1;         // we timed out
            DATAOBJ.set(FrankData::error, 1); // set error status
            PRINTLN("uC : connection failed (timeout)");
        }
    }

    if ((B01010101 == Serial3.read()) && !timeout) {

        PRINTLN("Connected");

    } else {
        DATAOBJ.set(FrankData::error, 1); // set error status
    }

    attachInterrupt(digitalPinToInterrupt(SWSYNC), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWSEQ), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWREC), ISRSwitch, CHANGE);

    // Set timer interrupt (display refresh)
    myTimer.begin(updateDisplay, 40000); // display refresh
}

void loop() {

    // GETDATAOBJ
    // NEW Midi Signal
    updateMidi();
    //   Read uC UART Data
    while (Serial3.available()) {
        readSerial3();
    }

    // Temp Clock
    static long timer = 0;
    if (millis() - timer > 500) {
        DATAOBJ.increase(FrankData::stepSeq, (byte)0);
        DATAOBJ.increase(FrankData::stepSeq, (byte)1);
        DATAOBJ.increase(FrankData::stepArp, (byte)0);

     //   PRINT("Step: ");
      //  PRINTLN(DATAOBJ.get(FrankData::stepSeq));
        timer = millis();
    }

    // activate middleman
    //updateAllOutputs();
    cntrl.readBPMSpeed();
}

void ISRSwitch() {
    cntrl.readSwitches();
    //   PRINTLN("INPUT: SWChange");
}
