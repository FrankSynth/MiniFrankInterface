#pragma once

#include "interfaceData.hpp"
#include "interfaceMapping.hpp"
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

    void readSwitches();  //readSwitches

    void readSync();  //read Sync Switch
    void readSeq();   //read Seq Swicch
    void readRec();   //read Rec Switch

    void init();
};
