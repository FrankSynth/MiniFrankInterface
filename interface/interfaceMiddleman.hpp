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

    byte note;
    byte customCV;
    byte gate;
    byte gatelength;
    long gateTimeSet;
    byte clock;
    long clockTimeSet;
    byte trigger;
    long triggerTimeSet;

public:
    PreviousOutputs() {
        this->note = 0;
        this->customCV = 0;
        this->gate = 0;
        this->gatelength = 0;
        this->gateTimeSet = 0;
        this->clock = 0;
        this->clockTimeSet = 0;
        this->trigger = 0;
        this->triggerTimeSet = 0;
    }

    void setNewGateTimeSet();
    void setNewClockTimeSet();
    void setNewTriggerTimeSet();
};

// save general output state parameters
class PreviousState {
  public:
    byte old16thClockCount;

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

// advanced helper functions
byte getArpeggiatedNote();
