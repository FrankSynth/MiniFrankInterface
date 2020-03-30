#pragma once

#include "interfaceData.hpp"
#include "interfaceOut.hpp"
#include <Arduino.h>

#define TRIGGERLENGTH 20 // default trigger and clock pulse length in ms

// save state for each output lane
class PreviousOutputs {
  public:
    byte note = 0;
    int cv = 0;
    byte ratchet = 0;
    byte gateActivated = 0;
    unsigned long gateCloseTime = 0;
    unsigned long reactivateTime = 0;
    unsigned long ratchetOffsetTime = 0;

    byte stepSeq = 0;

    byte clockPulseActivated = 0;

    byte triggerActivated = 0;
    unsigned long triggerTimer = 0;

  public:
    PreviousOutputs() {}
};

// save general output state parameters
class PreviousState {
  public:
    byte clockLED = 0;
    byte old16thClockCount = 0;

    PreviousState() { this->old16thClockCount = 0; }
};

// init Middleman
void initMiddleman();

// evaluate all outputs and update them
void updateAllOutputs();

// Middleman helper functions
void updateNoteOut();
void updateCVOut();
void closeGates();
void updateClockOut();
void closeTriggers();
void reactivateRatchet();
