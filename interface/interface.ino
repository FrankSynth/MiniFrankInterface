#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"

#define DEBUG

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)


void setup() {

#ifdef DEBUG
Serial.begin(115200);
#endif
Serial.println("HELLO");


lcd.displayBrightness(125); //Set display brightness

}

void loop() {

lcd.refresh();

}
