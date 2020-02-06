#pragma once

#include "interfaceData.hpp"
#include <Arduino.h>


#define SWSYNC 6
#define SWSEQ 7
#define SWREC 8

//#define DEBUG

// Controls object for the incoming UART messages
class controls {
  public:
    controls() { // SWITCHES init
        pinMode(SWSYNC, INPUT_PULLUP);
        pinMode(SWSEQ, INPUT_PULLUP);
        pinMode(SWREC, INPUT_PULLUP);
        readSwitches();
    }

    void encode(byte message);      // encode message
    void rotate(byte id, byte dir); // is a rotate message
    void push(byte id, byte push);  // is a switch message

    void readSwitches() {

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

    void readSync() {
        settings::setSync(digitalRead(SWSYNC));
    }
    void readSeq() {
        settings::setActiveSeq(digitalRead(SWSEQ));
    }
    void readRec() {
        settings::setRec(digitalRead(SWREC));
    }

    void init() { // set pointer
        seq1 = &getSeq()[0];
        seq2 = &getSeq()[1];
        // settings = getStatus();;
    }

  private:
    Seq *getActiveSeqPointer();

    Seq *seq1;
    Seq *seq2;
};
