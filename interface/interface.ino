#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"
#include "interfaceData.h"


#define DEBUG

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)

//seq seq1 = seg1.init(); //init values: note = A0, Gate = off, gateLength = 50, activePages = 2;



void setup() {

#ifdef DEBUG
Serial.begin(115200);
Serial.println("HELLO");

#endif

Serial.println("HELLO");


lcd.displayBrightness(125); //Set display brightness

}

void loop() {

lcd.refresh();

}
