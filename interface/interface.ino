#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"
#include "interfaceData.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define DEBUG

String inString = "";    // string to hold input


seq seq1;
seq seq2;
status stat;   //init status object;

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)


IntervalTimer myTimer;

void updateDisplay(){ //update interrupt
  lcd.refresh();
}


void setup() {
  //transfer the pointer to our data objects to the display library
  lcd.setPointer(&seq1, &seq2, &stat);

  //intSeq
  seq2.init();
  seq1.init();

  //Set timer interrupt
  myTimer.begin(updateDisplay, 40000);


  //init Serial connection
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("HELLO");
  #endif

 lcd.displayBrightness(175); //Set display brightness
}

void loop() {
  static double tmp= 0;
  if(millis() -tmp  > 499){
    stat.increaseStep();
    tmp = millis();

  }


  if(Serial.available()){
    inString = Serial.read(); //readSerial

    byte tmp = (byte)inString.toInt(); //convert to byte
    stat.setNumberPages(tmp);     //testValue

    seq1.setTuning(tmp);     //testValue
    seq2.setTuning(tmp);     //testValue

    while(Serial.available()){//empty Serial
      tmp = Serial.read();
    }
  }
}
