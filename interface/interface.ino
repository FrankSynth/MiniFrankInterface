#include <Arduino.h>
#include <MIDI.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "interfaceData.hpp"
#include "interfaceDisplay.hpp"
#include "interfaceIn.hpp"
#include "interfaceMiddleman.hpp"
#include "interfaceMidi.hpp"


// #define DEBUG

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

#ifdef DEBUG
    Serial.print("Message: ");
    Serial.println(rcv, BIN);
#endif

    cntrl.encode(rcv);
}

void updateDisplay() { // update interrupt
    lcd.refresh();
}

void setup() {
    // GETDATAOBJ
    // pin setup
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);

    // transfer the pointer to our data objects to the display library
    // lcd.init();
    // and to the control object
    cntrl.init();

    // intSeq

    initMidi();
    initMiddleman();

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("HELLO FRANK Mini");
#endif

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
            timeout = 1;           // we timed out
            DATAOBJ.setError(1); // set error status
#ifdef DEBUG
            Serial.println("uC : connection failed (timeout)");
#endif
        }
    }

    if ((B01010101 == Serial3.read()) && !timeout) {
#ifdef DEBUG

        Serial.println("Connected");
#endif
    } else {
        DATAOBJ.setError(1); // set error status
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
    if (millis() - timer > 250) {
        DATAOBJ.increaseStep();
        timer = millis();
    }

    // activate middleman
    updateAllOutputs();
}

void ISRSwitch() {
    cntrl.readSwitches();
#ifdef DEBUG
    Serial.println("INPUT: SWChange");
#endif
}
