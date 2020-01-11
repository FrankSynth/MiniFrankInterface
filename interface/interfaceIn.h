#ifndef MF_IN_H_
#define MF_IN_H_

#include <Arduino.h>
#include "interfaceData.h"

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

      stat->setSync(digitalRead(SWSYNC));
      stat->setActiveSeq(digitalRead(SWSEQ));
      stat->setRec(digitalRead(SWREC));
    }

    void readSync(){
      stat->setSync(digitalRead(SWSYNC));
    }
    void readSeq(){
      stat->setActiveSeq(digitalRead(SWSEQ));
    }
    void readRec(){
      stat->setRec(digitalRead(SWREC));
    }

    void setPointer(seq *seq1Pointer, seq *seq2Pointer, status *statPointer){ //set pointer
      seq1 = seq1Pointer;
      seq2 = seq2Pointer;
      stat = statPointer;
    }

private:
    seq* getActiveSeqPointer();

    seq *seq1;
    seq *seq2;
    status *stat;

};

#endif
