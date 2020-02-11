#pragma once

#include "interfaceData.hpp"
#include "interfaceOut.hpp"
#include <Arduino.h>

#define TRIGGERLENGTH 20 // default trigger and clock pulse length in ms

// save state for each output lane
class PreviousOutputs {
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

    // set previous state values
    void setNote(byte data);
    void setCustomCV(byte data);
    void setGate(byte data);
    void setGateLength(byte data);
    void setGateTimeSet();
    void setClock(byte data);
    void setClockTimeSet();
    void setTrigger(byte data);
    void setTriggerTimeSet();

    // get previous state values
    byte getNote();
    byte getCustomCV();
    byte getGate();
    byte getGateLength();
    long getGateTimeSet();
    byte getClock();
    long getClockTimeSet();
    byte getTrigger();
    long getTriggerTimeSet();
};

// save general output state parameters
class PreviousState {
    byte old16thClockCount;

  public:
    PreviousState() { this->old16thClockCount = 0; }

    void setOld16thClockCount(byte data);
    byte getOld16thClockCount();
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

