#pragma once

#include <Arduino.h>
#include "interfaceData.hpp"

#define SWSYNC 6
#define SWSEQ 7
#define SWREC 8

//#define DEBUG


//Controls object for the incoming UART messages
class controls
{
  public:
    controls(){   //SWITCHES init
      pinMode(SWSYNC,INPUT_PULLUP);
      pinMode(SWSEQ,INPUT_PULLUP);
      pinMode(SWREC,INPUT_PULLUP);
      readSwitches();
    }

    void encode(byte message);  //encode message
    void rotate(byte id ,byte dir); // is a rotate message
    void push( byte id,byte push);  //is a switch message

    void readSwitches(){

      #ifdef DEBUG
      Serial.print("INPUT: SYNC set: ");
      Serial.println(digitalRead(SWSYNC));
      Serial.print("INPUT: SEQ set: ");
      Serial.println(digitalRead(SWSEQ));
      Serial.print("INPUT: REC set: ");
      Serial.println(digitalRead(SWREC));
      #endif

      settings::setSync(digitalRead(SWSYNC));
      settings::setActiveSeq(digitalRead(SWSEQ));
      settings::setRec(digitalRead(SWREC));
    }

    void readSync(){
      settings::setSync(digitalRead(SWSYNC));
    }
    void readSeq(){
      settings::setActiveSeq(digitalRead(SWSEQ));
    }
    void readRec(){
      settings::setRec(digitalRead(SWREC));
    }

    void setPointer(Seq *seq1Pointer, Seq *seq2Pointer){ //set pointer
      seq1 = seq1Pointer;
      seq2 = seq2Pointer;
      // settings = getStatus();;
    }

private:
    Seq* getActiveSeqPointer();

    Seq *seq1;
    Seq *seq2;

};
