#include "interfaceMiddleman.hpp"

//OUTPUT Channels and clocks
channel outputChannel1 = channel(DAC1,0,DAC2,0,TRIGGER1,GATE1);
channel outputChannel2 = channel(DAC1,1,DAC2,1,TRIGGER2,GATE2);

clock outputClock1 = clock(CLK1);
clock outputClock2 = clock(CLK2);
//

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];

// GETDATAOBJ

// Connection to data
// LiveMidi* liveMidiData;
// Seq* seqData;

void initMiddleman() {
    initOutput(); //init Outputs
    // data= getDataObject();
    // liveMidiData = getLiveMidiObject();
    // seqData = getSeqObject();
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
    byte currentClock = DATAOBJ.getBpm16thCount();
}
void updateTriggerOut(byte output) {}

void PreviousOutputs::setNote(byte midiData) { note = midiData; }
void PreviousOutputs::setCustomCV(byte midiData) { customCV = midiData; }
void PreviousOutputs::setGate(byte midiData) { gate = midiData; }
void PreviousOutputs::setGateLength(byte midiData) { gatelength = midiData; }
void PreviousOutputs::setGateTimeSet() { gateTimeSet = millis(); }
void PreviousOutputs::setClock(byte midiData) { clock = midiData; }
void PreviousOutputs::setClockTimeSet() { clockTimeSet = millis(); }
void PreviousOutputs::setTrigger(byte midiData) { trigger = midiData; }
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

void PreviousState::setOld16thClockCount(byte midiData) { old16thClockCount = midiData; }
byte PreviousState::getOld16thClockCount() { return old16thClockCount; }
