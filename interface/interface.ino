#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>

#include "interfaceDisplay.h"
#include "interfaceData.h"
#include "interfaceIn.h"
#include "interfaceMidi.h"


#include <avr/io.h>
#include <avr/interrupt.h>

#define DEBUG

seq seq1;
seq seq2;
status stat;   //init status object;
mfMidi midi0; //create midi object

controls cntrl;

display lcd = display(160,128,1); //create display object, (width, heigh, rotation)

IntervalTimer myTimer;


void ISRSwitch();  //Switch Interrupt



//readSerial4
void readSerial4(){
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
  //pin setup
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  //transfer the pointer to our data objects to the display library
  lcd.setPointer(&seq1, &seq2, &stat);
  //and to the control object
  cntrl.setPointer(&seq1, &seq2, &stat);

  //intSeq
  seq2.init();
  seq1.init();
  midi0.init();

  //Interrupt for the SWITCHES .... macht noch probleme mit dem DisplayUpdateInterrupt
  attachInterrupt (digitalPinToInterrupt(SWSYNC), ISRSwitch, CHANGE);
  attachInterrupt (digitalPinToInterrupt(SWSEQ), ISRSwitch, CHANGE);
  attachInterrupt (digitalPinToInterrupt(SWREC), ISRSwitch, CHANGE);

  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("HELLO");
  #endif

  ////////////////////////
  //Start Devices
  digitalWrite(5, HIGH);
  delay(10);

  //Start Connection to the uC
  Serial4.begin(1000000);

  //SayHello to the uC
  byte send = B01010101;
  long timer = millis();
  byte timeout = 0;
  while(!Serial4.available() && !timeout ){  //send hello until uC response (max 2seconds)
    Serial4.write(send);

    if(millis() - timer > 2000){
      timeout = 1; //we timed out
      stat.setError(1); //set error status
      #ifdef DEBUG
      Serial.println("uC : connection failed (timeout)");
      #endif
    }
  }

  #ifdef DEBUG
  if((B01010101 == Serial4.read()) && !timeout){
    Serial.println("Connected");
  }
  #endif
  ////////////////////////

  //Set timer interrupt (display refresh)
  myTimer.begin(updateDisplay, 40000);  //display refresh
}

void loop() {
  //NEW Midi Signal
  while(midi0.available()){
    midi0.updateMidi();
  }

  //Read uC UART Data
  while(Serial4.available()){
    encode(Serial4.read());
  }

  /////////Temp Clock
  static long timer = 0;
  if(millis()- timer > 500){
    stat.increaseStep();
    timer = millis();
  }
  ///////
}


void ISRSwitch(){
  cntrl.readSwitches();
  #ifdef DEBUG
  Serial.println("INPUT: SWChange");
  #endif

}
