#pragma once

#include "interfaceData.hpp"
#include "interfaceMapping.hpp"
#include <Arduino.h>

#define SWSYNC 9
#define SWSEQ 23
#define SWREC 31
#define BPMPOT 22

//#define DEBUG

// Controls object for the incoming UART messages
class inputControls {
  public:
    void encode(byte message);      // encode message
    void rotate(byte id, byte dir); // is a rotate message
    void push(byte id, byte push);  // is a switch message

    void readSwitches(); // readSwitches

    void readSync(); // read Sync Switch
    void readSeq();  // read Seq Swicch
    void readRec();  // read Rec Switch

    void readBPMSpeed(); // read sync potentiometer

    void init();
};
