#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"
#include "interfaceData.h"
#include "interfaceIn.h"


#include <avr/io.h>
#include <avr/interrupt.h>

#define DEBUG

seq seq1;
seq seq2;
status stat;   //init status object;
controls cntrl;

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)

IntervalTimer myTimer;





//Interrupt on UART message
void readSerial(){
  byte rcv;
  rcv = Serial4.read();

  #ifdef DEBUG
  //Serial.print("Message: ");
  Serial.println(rcv,BIN);
  #endif

 cntrl.encode(rcv);
}


void updateDisplay(){ //update interrupt
  lcd.refresh();
}


void setup() {
  //transfer the pointer to our data objects to the display library
  lcd.displayBrightness(175); //Set display brightness

  lcd.setPointer(&seq1, &seq2, &stat);
  //and to the control object
  cntrl.setPointer(&seq1, &seq2, &stat);


  //intSeq
  seq2.init();
  seq1.init();


  //Set timer interrupt

  myTimer.begin(updateDisplay, 40000);  //display refresh

  //init Serial connection
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("HELLO");
  #endif

  ////////////////////////
  //Start Devices
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(10);
  digitalWrite(5, HIGH);
  delay(10);

  //Connect to Control uC
  Serial4.begin(1000000);
  //SayHello
  byte send = B01010101;

  while(!Serial4.available()){  //wait for response
    Serial4.write(send);
  }

  if(B01010101 == Serial4.read()){
    #ifdef DEBUG
    Serial.println("Connected");
    #endif
  }
  ////////////////////////

}

void loop() {
  while(Serial4.available()){
    readSerial();
  }
  /////////Temp Clock
  static long timer = 0;

  if(millis()- timer > 500){
  stat.increaseStep();
  timer = millis();
  }

  ///////
}
