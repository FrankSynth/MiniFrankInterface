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
#define PRINTLN2(x, y) Serial.println(x, y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x, y) Serial.print(x, y)
#else
#define PRINTLN(x)
#define PRINTLN2(x, y)
#define PRINT(x)
#define PRINT2(x, y)
#endif

inputControls cntrl;

Display lcd = Display(160, 128, 3); // create display object, (width, heigh, rotation)
TLC5916 tlc;

void ISRSwitch(); // Switch Interrupt

// readSerial3
void readSerial3() {
    byte rcv;
    rcv = Serial3.read();

    // PRINT("Message: ");
    // PRINTLN2(rcv, BIN);

    cntrl.encode(rcv);
}

void updateDisplay() { // update interrupt
    lcd.refresh();
}

void updateTLC() { // update interrupt
    static byte sendOld = 0;
    byte source = DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel));
    if (source) { // seq modus an?
        byte send = 0;
        for (int x = 0; x < 2; x++) {

            for (int i = 0; i < 4; i++) {
                if (DATAOBJ.get(FrankData::seqGate, source - 1, DATAOBJ.get(FrankData::activePage, source - 1) * 8 + i + 4 * x)) { // gate an?,
                    if (x == 0) {
                        send = send | 1 << i;
                    }
                    else {
                        send = send | 1 << (7 - i);
                    }
                }
            }
        }
        if (send != sendOld) {
            tlc.sendByte(send);
            sendOld = send;
        }
    }
    else {
        if (sendOld) {
            tlc.sendByte(0);
            sendOld = 0;
        }
    }
}

void setup() {

    Serial.begin(115200);
    
    DATAOBJ.init();

    PRINTLN("Debug Mode");
    PRINTLN("Hello FRANK Mini");

    PRINT("Brightness: ");
    PRINTLN(DATAOBJ.get(FrankData::displayBrightness));
    PRINT("BPM: ");
    PRINTLN(DATAOBJ.get(FrankData::bpm));

    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);

    lcd.displayBrightness(200);

    initMidi();
    initMiddleman();
    tlc.init(7);
    cntrl.init();

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
            timeout = 1;                      // we timed out
            DATAOBJ.set(FrankData::error, 1); // set error status
            PRINTLN("uC : connection failed (timeout)");
        }
    }

    if ((B01010101 == Serial3.read()) && !timeout) {

        PRINTLN("Connected");
    }
    else {
        DATAOBJ.set(FrankData::error, 1); // set error status
    }

    attachInterrupt(digitalPinToInterrupt(SWSYNC), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWSEQ), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWREC), ISRSwitch, CHANGE);

    // Set timer interrupt (display refresh)
    // myTimerLCD.begin(updateDisplay, 17000); // display refresh
    // myTimerLED.begin(updateTLC, 20000);     // display refresh

}

void loop() {

    static unsigned int screenTimer = millis();

    if (millis() > screenTimer + 16) {
        
        cli();
        updateDisplay();
        updateTLC();
        sei();

        screenTimer = millis();
    }

    // NEW Midi Signal
    updateMidi();

    //   Read uC UART Data
    while (Serial3.available()) {
        readSerial3();
    }

    // count all clocks forward if not synced
    DATAOBJ.updateClockCounter();

    // activate middleman
    updateAllOutputs();

    cntrl.readBPMSpeed();
}

void ISRSwitch() {
    cntrl.readSwitches();
    //   PRINTLN("INPUT: SWChange");
}
