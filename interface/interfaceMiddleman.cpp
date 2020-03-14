#include "interfaceMiddleman.hpp"

// Debug logging
#define DEBUG 1

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
Channel outputChannel[OUTPUTS] = {Channel(DAC1, 0, DAC2, 0, TRIGGER1, GATE1), Channel(DAC1, 1, DAC2, 1, TRIGGER2, GATE2)};

clock outputClock[OUTPUTS] = {clock(CLK1), clock(CLK2)};

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];
ClkLed clkLed;

void initMiddleman() {
    initOutput(); // init Outputs
    clkLed.init(CLKLED);
}   
void updateAllOutputs() {

    updateNoteOut();
    updateCustomCVOut();
    updateGateOut();
    updateClockOut();
    updateTriggerOut();
}

void updateNoteOut() {
}
void updateCustomCVOut() {}
void updateGateOut() {}
void updateClockOut() {

    static double timer[2] = {0};

    if (!(DATAOBJ.get(FrankData::bpm16thCount) == previousState.old16thClockCount)) {
        previousState.old16thClockCount = DATAOBJ.get(FrankData::bpm16thCount);

        if (DATAOBJ.get(FrankData::bpm16thCount) % 4 < 2) {
            if (!previousState.clockLED) {
                clkLed.setClkLed(1);
                previousState.clockLED = 1;
            }
        }
        else {
            if (previousState.clockLED) {
                clkLed.setClkLed(0);
                previousState.clockLED = 0;
            }
        }

        for (byte output = 0; output < OUTPUTS; output++) {

            if (!previousOutputs[output].clockPulseActivated) {

                if ((int)(DATAOBJ.get(FrankData::bpm16thCount)) % (int)pow(2, (int)DATAOBJ.get(FrankData::outputClock, output)) == 0) {
                    outputClock[output].setClock(1);
                    previousOutputs[output].clockPulseActivated = 1;
                    timer[output] = millis();
                    PRINT("Clock 1 on output ");
                    PRINTLN(output+1);
                }
            }
        }
    }

    for (byte output = 0; output < OUTPUTS; output++) {
        if (previousOutputs[output].clockPulseActivated) {

            if (millis() - timer[output] >= DATAOBJ.get(FrankData::pulseLength)) {
                outputClock[output].setClock(0);
                previousOutputs[output].clockPulseActivated = 0;
            }
        }
    }
}

void updateTriggerOut() {}

void updateArp() {
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
