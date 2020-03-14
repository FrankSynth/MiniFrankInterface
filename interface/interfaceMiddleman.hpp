#pragma once

#include "interfaceData.hpp"
#include "interfaceOut.hpp"
#include <Arduino.h>

#define TRIGGERLENGTH 20 // default trigger and clock pulse length in ms

// save state for each output lane
class PreviousOutputs {
    public:
    enum previousData {
        dataNote, dataGate
    };

    byte note = 0;
    byte customCV = 0;
    byte sustain = 0;
    byte gate = 0;
    byte gatelength = 0;
    byte stepSeq = 0;
    byte stepArp = 0;
    long gateTimeSet = 0;
    byte clockPulseActivated = 0;
    byte clockPulseDeactivated = 0;
    long clockTimeSet = 0;
    byte trigger = 0;
    long triggerTimeSet = 0;

public:
    PreviousOutputs() {    }

    void setNewGateTimeSet();
    void setNewClockTimeSet();
    void setNewTriggerTimeSet();
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
void updateNoteOut(byte output);
void updateCustomCVOut(byte output);
void updateGateOut(byte output);
void updateClockOut(byte output);
void updateTriggerOut(byte output);
void updateArp(byte output);

    // advanced helper functions
    byte getArpeggiatedNote();
