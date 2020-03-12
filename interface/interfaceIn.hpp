#pragma once

#include "interfaceData.hpp"
#include "interfaceMapping.hpp"
#include <Arduino.h>

#define SWSYNC 9
#define SWSEQ 31
#define SWREC 23
#define BPMPOT 22 

//#define DEBUG

// Controls object for the incoming UART messages
class controls {
  public:
    controls() { // SWITCHES init
        pinMode(SWSYNC, INPUT_PULLUP);
        pinMode(SWSEQ, INPUT_PULLUP);
        pinMode(SWREC, INPUT_PULLUP);
        pinMode(BPMPOT, INPUT);
        readBPMSpeed();
        readSwitches();
    }

    void encode(byte message);      // encode message
    void rotate(byte id, byte dir); // is a rotate message
    void push(byte id, byte push);  // is a switch message

    void readSwitches(); // readSwitches

    void readSync(); // read Sync Switch
    void readSeq();  // read Seq Swicch
    void readRec();  // read Rec Switch

    void readBPMSpeed(); //read sync potentiometer

    void init();
};
