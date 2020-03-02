#include "interfaceMiddleman.hpp"

// Debug logging
#define DEBUG 0

#if DEBUG == 1
#define PRINTLN(x) Serial.println(x)
#define PRINTLN2(x, y) Serial.println(x, y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x, y) Serial.print(x, y)
#else
#define PRINTLN(x)
#define PRINTLN2(x, y)
#define PRINT(x)
#define PRINT2(x, y)
#endif

// OUTPUT Channels and clocks
channel outputChannel[OUTPUTS] = {channel(DAC1, 0, DAC2, 0, TRIGGER1, GATE1),
                                  channel(DAC1, 1, DAC2, 1, TRIGGER2, GATE2)};

clock outputClock[OUTPUTS] = {clock(CLK1), clock(CLK2)};

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];

void initMiddleman() {
    initOutput(); // init Outputs
    long testlong = previousOutputs[0].get<long>(PreviousOutputs::dataNote);
}

void updateAllOutputs() {
    for (byte output = 0; output < OUTPUTS; output++) {
        updateNoteOut(output);
        updateCustomCVOut(output);
        updateGateOut(output);
        updateClockOut(output);
        updateTriggerOut(output);
    }
}

void updateNoteOut(byte output) {

}
void updateCustomCVOut(byte output) {}
void updateGateOut(byte output) {}
void updateClockOut(byte output) { byte currentClock = DATAOBJ.getBpm16thCount(); }
void updateTriggerOut(byte output) {}

void PreviousOutputs::setNote(byte midiData) { note = midiData; }
void PreviousOutputs::setCustomCV(byte midiData) { customCV = midiData; }
void PreviousOutputs::setGate(byte midiData) { gate = midiData; }
void PreviousOutputs::setGateLength(byte midiData) { gatelength = midiData; }
void PreviousOutputs::setClock(byte midiData) { clock = midiData; }
void PreviousOutputs::setTrigger(byte midiData) { trigger = midiData; }
void PreviousOutputs::setNewGateTimeSet() { gateTimeSet = millis(); }
void PreviousOutputs::setNewClockTimeSet() { clockTimeSet = millis(); }
void PreviousOutputs::setNewTriggerTimeSet() { triggerTimeSet = millis(); }

template <typename T> void PreviousOutputs::set(T data, previousData destination) {}
template <typename T> T PreviousOutputs::get(previousData destination) {
    switch(destination) {

    case dataNote: return note;
    default:
    break;

    }
}

byte PreviousOutputs::getNote() { return note; }
byte PreviousOutputs::getCustomCV() { return customCV; }
byte PreviousOutputs::getGate() { return gate; }
byte PreviousOutputs::getGateLength() { return gatelength; }
long PreviousOutputs::getGateTimeSet() { return gateTimeSet; }
byte PreviousOutputs::getClock() { return clock; }
long PreviousOutputs::getClockTimeSet() { return clockTimeSet; }
byte PreviousOutputs::getTrigger() { return trigger; }
long PreviousOutputs::getTriggerTimeSet() { return triggerTimeSet; }

void PreviousState::setOld16thClockCount(byte midiData) { old16thClockCount = midiData; }
byte PreviousState::getOld16thClockCount() { return old16thClockCount; }
