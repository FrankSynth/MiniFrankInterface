#include "interfaceMiddleman.hpp"

//OUTPUT Channels and clocks
channel outputChannel1 = channel(DAC1,0,DAC2,0,TRIGGER1,GATE1);
channel outputChannel2 = channel(DAC1,1,DAC2,1,TRIGGER2,GATE2);

clock outputClock1 = clock(CLK1);
clock outputClock2 = clock(CLK2);
//

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];

// Connection to data
LiveMidi* liveMidiData;
Seq* seqData;

void initMiddleman() {
    initOutput(); //init Outputs
    liveMidiData = getLiveMidi();
    seqData = getSeq();
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

void updateNoteOut(byte output) {}
void updateCustomCVOut(byte output) {}
void updateGateOut(byte output) {}
void updateClockOut(byte output) {
    byte currentClock = getBpm16thCount();
}
void updateTriggerOut(byte output) {}

void PreviousOutputs::setNote(byte data) { note = data; }
void PreviousOutputs::setCustomCV(byte data) { customCV = data; }
void PreviousOutputs::setGate(byte data) { gate = data; }
void PreviousOutputs::setGateLength(byte data) { gatelength = data; }
void PreviousOutputs::setGateTimeSet() { gateTimeSet = millis(); }
void PreviousOutputs::setClock(byte data) { clock = data; }
void PreviousOutputs::setClockTimeSet() { clockTimeSet = millis(); }
void PreviousOutputs::setTrigger(byte data) { trigger = data; }
void PreviousOutputs::setTriggerTimeSet() { triggerTimeSet = millis(); }

byte PreviousOutputs::getNote() { return note; }
byte PreviousOutputs::getCustomCV() { return customCV; }
byte PreviousOutputs::getGate() { return gate; }
byte PreviousOutputs::getGateLength() { return gatelength; }
long PreviousOutputs::getGateTimeSet() { return gateTimeSet; }
byte PreviousOutputs::getClock() { return clock; }
long PreviousOutputs::getClockTimeSet() { return clockTimeSet; }
byte PreviousOutputs::getTrigger() { return trigger; }
long PreviousOutputs::getTriggerTimeSet() { return triggerTimeSet; }

void PreviousState::setOld16thClockCount(byte data) { old16thClockCount = data; }
byte PreviousState::getOld16thClockCount() { return old16thClockCount; }
