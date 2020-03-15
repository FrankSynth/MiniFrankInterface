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
Channel outputChannel[OUTPUTS] = {Channel(DAC1, 1, DAC2, 0, TRIGGER1, GATE1), Channel(DAC1, 0, DAC2, 1, TRIGGER2, GATE2)};
clock outputClock[OUTPUTS] = {clock(CLK1), clock(CLK2)};

PreviousState previousState;
PreviousOutputs previousOutputs[OUTPUTS];
ClkLed clkLed;

void initMiddleman() {
    initOutput(); // init Outputs
    clkLed.init(CLKLED);
}

void updateAllOutputs() {
    updateClockOut();
    updateNoteOut();
    updateCVOut();
    reactivateRatchet();
    closeGates();
    closeTriggers();
}

void updateNoteOut() {

    for (byte output = 0; output < OUTPUTS; output++) {

        if (DATAOBJ.get(FrankData::liveTriggered, output)) {

            byte newNote;
            float gateDuration = 0.5f;
            if (DATAOBJ.get(FrankData::outputSource, output) == 0) {
                if (DATAOBJ.get(FrankData::outputArp, output)) {
                    newNote = DATAOBJ.get(FrankData::liveKeyArpNoteEvaluated, output);
                }
                else {
                    newNote = DATAOBJ.get(FrankData::liveKeyNoteEvaluated, output);
                }
            }
            else {
                byte seqStep = DATAOBJ.get(FrankData::stepSeq, output);

                if (DATAOBJ.get(FrankData::liveRecModePlayback, output)) {
                    if (DATAOBJ.get(FrankData::play)) DATAOBJ.set(FrankData::liveRecModePlayback, 0, output);

                    seqStep = changeByteReverse(seqStep, -1, 0, DATAOBJ.get(FrankData::nbPages, output) * STEPSPERPAGE - 1);
                }

                newNote = DATAOBJ.get(FrankData::seqNote, DATAOBJ.get(FrankData::outputSource, output) - 1, seqStep);
                gateDuration = (float)testInt((int)DATAOBJ.get(FrankData::seqGateLength, DATAOBJ.get(FrankData::outputSource, output) - 1, seqStep) +
                                                  (int)DATAOBJ.get(FrankData::seqGateLengthOffset, DATAOBJ.get(FrankData::outputSource, output) - 1) -
                                                  GATELENGTHOFFSET,
                                              0, 100) /
                               100.0f;
            }

            // only if not a key was released
            if (!DATAOBJ.get(FrankData::liveReleased, output)) {
                // calc gate closure time if arp or seq mode
                if (DATAOBJ.get(FrankData::outputArp, output) || DATAOBJ.get(FrankData::outputSource, output)) {
                    // ratchet calc
                    previousOutputs[output].ratchet = DATAOBJ.get(FrankData::outputRatchet, output);

                    int bpmMult = 1; // achtung, doppelte value
                    switch (DATAOBJ.get(FrankData::stepSpeed, output)) {
                    case 0: bpmMult = 32; break; // 1/16
                    case 1: bpmMult = 16; break;
                    case 2: bpmMult = 8; break; // 1/4
                    case 3: bpmMult = 4; break;
                    case 4: bpmMult = 2; break;
                    case 5: bpmMult = 1; break; // 2/1
                    default:;
                    }

                    // /2 cause of double val, /4 cause of 4*16th per beat
                    int divider = ((int)DATAOBJ.get(FrankData::bpm) * bpmMult * (int)(DATAOBJ.get(FrankData::outputRatchet, output) + 1) / 8);
                    previousOutputs[output].ratchetOffsetTime = (60000 / divider);
                    previousOutputs[output].gateCloseTime = millis() + previousOutputs[output].ratchetOffsetTime * gateDuration;
                    previousOutputs[output].reactivateTime = millis() + previousOutputs[output].ratchetOffsetTime;
                    // PRINT("current Time is ");
                    // PRINT(millis());
                    // PRINT(", New gate Close Time is ");
                    // PRINT(previousOutputs[output].gateCloseTime);
                    // PRINT(", next ratchet is ");
                    // PRINT(previousOutputs[output].reactivateTime);
                    // PRINT(", next step ist ");
                    // PRINTLN(millis() + 60000 / (DATAOBJ.get(FrankData::bpm) * bpmMult / 8));
                }

                // output Trigger and Gate
                outputChannel[output].setGate(1);
                previousOutputs[output].gateActivated = 1;

                if (DATAOBJ.get(FrankData::outputSource, output) == 0 || DATAOBJ.get(FrankData::play)) {
                    outputChannel[output].setTrigger(1);
                    previousOutputs[output].triggerActivated = 1;
                    previousOutputs[output].triggerTimer = millis();
                }
            }

            // output Note
            if (newNote != previousOutputs[output].note) {
                outputChannel[output].setNote(newNote);
                previousOutputs[output].note = newNote;
                PRINT("set new Note ");
                PRINTLN(newNote);
            }

            DATAOBJ.set(FrankData::liveTriggered, 0, output);
            DATAOBJ.set(FrankData::liveReleased, 0, output);
        }
    }
}

void reactivateRatchet() {
    for (byte output = 0; output < OUTPUTS; output++) {
        if (previousOutputs[output].ratchet && (DATAOBJ.get(FrankData::outputArp, output) || DATAOBJ.get(FrankData::outputSource, output))) {
            if (millis() >= previousOutputs[output].reactivateTime) {

                // PRINT("Ratchet reactivated");
                // PRINT(", current Time is ");
                // PRINT(millis());
                float gateDuration = 0.5f;
                if (DATAOBJ.get(FrankData::outputSource, output)) {

                    gateDuration =
                        (float)testInt((int)DATAOBJ.get(FrankData::seqGateLength, DATAOBJ.get(FrankData::outputSource, output) - 1,
                                                        DATAOBJ.get(FrankData::stepSeq, output)) +
                                           (int)DATAOBJ.get(FrankData::seqGateLengthOffset, DATAOBJ.get(FrankData::outputSource, output) - 1) -
                                           GATELENGTHOFFSET,
                                       0, 100) /
                        100.0f;
                }

                previousOutputs[output].gateCloseTime = millis() + previousOutputs[output].ratchetOffsetTime * gateDuration;
                previousOutputs[output].reactivateTime = millis() + previousOutputs[output].ratchetOffsetTime;
                // PRINT(", New gate Close Time is ");
                // PRINTLN(previousOutputs[output].gateCloseTime);

                // output
                outputChannel[output].setGate(1);
                previousOutputs[output].gateActivated = 1;

                outputChannel[output].setTrigger(1);
                previousOutputs[output].triggerActivated = 1;
                previousOutputs[output].triggerTimer = millis();

                previousOutputs[output].ratchet--;
            }
        }
    }
}

void updateCVOut() {
    byte newCV = 0;
    for (byte output = 0; output < OUTPUTS; output++) {

        if (DATAOBJ.get(FrankData::outputSource, output) == 0) {
            newCV = DATAOBJ.get(FrankData::outputCcEvaluated, output);
        }
        else {
            byte currentStep = DATAOBJ.get(FrankData::stepSeq, output);
            if (currentStep != previousOutputs[output].stepSeq) {
                previousOutputs[output].stepSeq = currentStep;
                newCV = DATAOBJ.get(FrankData::seqCc, DATAOBJ.get(FrankData::outputSource, output) - 1, currentStep);
            }
        }

        if (newCV != previousOutputs[output].cv) {
            outputChannel[output].setCV(newCV * 8); // expects 0-1023
            previousOutputs[output].cv = newCV;
        }
    }
}

void closeGates() {
    for (byte output = 0; output < OUTPUTS; output++) {

        if (previousOutputs[output].gateActivated) {

            if (DATAOBJ.get(FrankData::liveSustain, output) < 64) {

                if (DATAOBJ.get(FrankData::outputSource, output)) {
                    if (DATAOBJ.get(FrankData::play)) {
                        if (millis() >= previousOutputs[output].gateCloseTime) {
                            outputChannel[output].setGate(0);
                            previousOutputs[output].gateActivated = 0;
                            // PRINT("close gate, arp/seq, on output ");
                            // PRINT(output + 1);
                            // PRINT(", time is ");
                            // PRINTLN(millis());
                        }
                    }
                }
                else if (DATAOBJ.get(FrankData::outputArp, output)) {
                    if (millis() >= previousOutputs[output].gateCloseTime) {
                        outputChannel[output].setGate(0);
                        previousOutputs[output].gateActivated = 0;
                        // PRINT("close gate, arp/seq, on output ");
                        // PRINT(output + 1);
                        // PRINT(", time is ");
                        // PRINTLN(millis());
                    }
                }
                else if (!DATAOBJ.get(FrankData::liveKeysPressed, output)) {
                    outputChannel[output].setGate(0);
                    previousOutputs[output].gateActivated = 0;
                    // PRINT("close gate, live, on output ");
                    // PRINT(output + 1);
                    // PRINT(", time is ");
                    // PRINTLN(millis());
                }
            }
        }
    }
}

void updateClockOut() {

    static unsigned long timer[2] = {0};

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

void closeTriggers() {
    for (byte output = 0; output < OUTPUTS; output++) {
        if (previousOutputs[output].triggerActivated) {
            if (millis() - previousOutputs[output].triggerTimer >= DATAOBJ.get(FrankData::pulseLength)) {
                outputChannel[output].setTrigger(0);
                previousOutputs[output].triggerActivated = 0;
                // PRINTLN("Trigger closed");
            }
        }
    }
}
