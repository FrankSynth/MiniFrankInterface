#pragma once

#include "interfaceData.hpp"
#include "interfaceMidi.hpp"
#include "interfaceOut.hpp"
#include <Arduino.h>

// save state for each output lane
class PreviousOutputs {
  public:
    byte note = 0;
    byte velocity = 0;
    int cv = 0;
    byte ratchet = 0;
    byte gateActivated = 0;
    uint32_t gateCloseTime = 0;
    uint32_t ratchetOffsetTime = 0;

    elapsedMicros gateTimer = 0;

    byte stepSeq = 0;

    byte clockPulseActivated = 0;
    uint16_t clockPulseStep = 100;

    byte triggerActivated = 0;
    elapsedMillis triggerTimer = 0;

    PreviousOutputs() {}
};

// save general output state parameters
class PreviousState {
  public:
    byte clockLED = 0;
    uint16_t oldBpmClockCount = 0;

    PreviousState() {}
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

int16_t changeNote(const int16_t &value, const int16_t &change, const int16_t &minimum, const int16_t &maximum);
