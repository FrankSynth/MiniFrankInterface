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
channel outputChannel[OUTPUTS] = {channel(DAC1, 0, DAC2, 0, TRIGGER1, GATE1), channel(DAC1, 1, DAC2, 1, TRIGGER2, GATE2)};

clock outputClock[OUTPUTS] = {clock(CLK1), clock(CLK2)};

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];

void initMiddleman() {
    initOutput(); // init Outputs
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
    // Live
    if (DATAOBJ.get(FrankData::outputSource, output) == 0) {

        // Arp on
        if (DATAOBJ.get(FrankData::outputArp, output)) {
            byte tempStep = DATAOBJ.get(FrankData::stepArp);
            if (tempStep != previousOutputs[output].stepArp) {
                previousOutputs[output].stepArp = tempStep;

                byte tempNote = DATAOBJ.get(FrankData::liveKeyArpEvaluated, output);
                if (tempNote != previousOutputs[output].note) {
                    previousOutputs[output].note = tempNote;
                    // send new Note
                }
            }
        }

        // Arp Off
        else {
            byte tempNote = DATAOBJ.get(FrankData::liveKeyNoteEvaluated, output);
            if (tempNote != previousOutputs[output].note) {
                previousOutputs[output].note = tempNote;
                // send new Note
            }
        }
    }

    // Seq
    else {
    }
}
void updateCustomCVOut(byte output) {}
void updateGateOut(byte output) {}
void updateClockOut(byte output) {
    byte currentClock = DATAOBJ.get(FrankData::bpm16thCount);
}
void updateTriggerOut(byte output) {}

void updateArp(byte output) {
    if (DATAOBJ.get(FrankData::outputSource, output) == 0) {
        if (DATAOBJ.get(FrankData::outputArp, output)) {
            if (previousOutputs[output].sustain > DATAOBJ.get(FrankData::liveSustain) && DATAOBJ.get(FrankData::liveSustain) < 64) {
                PRINTLN("Middleman updates Arp");
                DATAOBJ.updateArp(output);
            }
        }
    }
}

void PreviousOutputs::setNewGateTimeSet() {
    gateTimeSet = millis();
}
void PreviousOutputs::setNewClockTimeSet() {
    clockTimeSet = millis();
}
void PreviousOutputs::setNewTriggerTimeSet() {
    triggerTimeSet = millis();
}
