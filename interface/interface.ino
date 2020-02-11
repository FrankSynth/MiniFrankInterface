#include <Arduino.h>
#include <MIDI.h>
#include <SPI.h>
#include <Wire.h>

#include "interfaceData.hpp"
#include "interfaceDisplay.hpp"
#include "interfaceIn.hpp"
#include "interfaceMidi.hpp"
#include "interfaceMiddleman.hpp"
#include "interfaceOut.hpp"


#include <avr/interrupt.h>
#include <avr/io.h>

// #define DEBUG

// status settings;   //init status object;
// mfMidi midi0; //create midi object

controls cntrl;

display lcd = display(160,128,3); //create display object, (width, heigh, rotation)

IntervalTimer myTimer;

// PressedNotesList noteList0;
// PressedNotesList noteList1;

void ISRSwitch(); // Switch Interrupt

// readSerial4
void readSerial4() {
    byte rcv;
    rcv = Serial4.read();

//readSerial3
void readSerial3(){
  byte rcv;
  rcv = Serial3.read();

  #ifdef DEBUG
  Serial.print("Message: ");
  Serial.println(rcv,BIN);
  #endif

  cntrl.encode(rcv);
}

void updateDisplay() { // update interrupt
    lcd.refresh();
}

void setup() {
    // pin setup
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);

    // transfer the pointer to our data objects to the display library
    lcd.init();
    // and to the control object
    cntrl.init();

    // intSeq
    initData();

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
    Serial4.begin(1000000);

    // SayHello to the uC
    byte send = B01010101;
    long timer = millis();
    byte timeout = 0;
    while (!Serial4.available() && !timeout) { // send hello until uC response (max 2seconds)
        Serial4.write(send);

        if (millis() - timer > 2000) {
            timeout = 1;           // we timed out
            settings::setError(1); // set error status
#ifdef DEBUG
            Serial.println("uC : connection failed (timeout)");
#endif
        }
    }

#ifdef DEBUG
    if ((B01010101 == Serial4.read()) && !timeout) {
        Serial.println("Connected");
    }
#endif

    // Interrupt for the SWITCHES .... macht noch probleme mit dem DisplayUpdateInterrupt
    attachInterrupt(digitalPinToInterrupt(SWSYNC), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWSEQ), ISRSwitch, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWREC), ISRSwitch, CHANGE);

    // Set timer interrupt (display refresh)
    myTimer.begin(updateDisplay, 40000); // display refresh
}

void loop() {
    // receive all new MIDI signals
    updateMidi();
=======
  
void loop() {
  //NEW Midi Signal
 updateMidi();
  //Read uC UART Data
  //while(Serial4.available()){
    // encode(Serial4.read());
  //}
  while(Serial3.available()){

  readSerial3();
}



    //Temp Clock
    static long timer = 0;
    if (millis() - timer > 500) {
        settings::increaseStep();
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
