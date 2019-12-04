#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"
#include "interfaceData.h"

#include <avr/io.h>
#include <avr/interrupt.h>



#define DEBUG


seq seq1;
seq seq2;

status stat;   //init staus object;
setting config;   //init config object;

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)


IntervalTimer myTimer;

void updateDisplay(){ //update interrupt
    lcd.refresh();
}


void setup() {
  //transfer the pointer to our data objects to the display library
  lcd.setPointer(&seq1, &seq2, &config, &stat);

  //intSeq
  seq2.init();
  seq1.init();

  //Set timer interrupt
  myTimer.begin(updateDisplay, 40000);


  //init Serial connection
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("HELLO");
  Serial.println(config.getMidiType());
  #endif


  lcd.displayBrightness(255); //Set display brightness

}

void loop() {


}
