#include "interfaceIn.hpp"

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

void inputControls::encode(byte message) {

    byte id = (byte)(B00001111 & message); // extract Message from ID

    if (message & B10000000) { // is a switch?
        if (message & B00010000) {
            push(id, message & B00010000);
            PRINT("Push : ");
        }
    }
    else { // seems to be an encoder
        PRINT("Encoder : ");
        rotate(id, message & B00010000);
    }
}

// rotate message
void inputControls::rotate(byte id, byte dir) {
    PRINTLN(id);
    PRINT("Direction: ");
    PRINTLN(dir);

    FrankData::frankData mappedID = mapping(id);

    id = id + DATAOBJ.get(FrankData::activePage, CHANNEL) * 8;

    switch (mappedID) {

        case STEP:

            if (dir) {
                DATAOBJ.increaseStepCounters(CHANNEL);
            }
            else {
                DATAOBJ.decreaseStepCounters(CHANNEL);
            }
            break;

        // TYPE,Channel,Index;
        case NOTE:
        case CV:

            if (dir) {
                DATAOBJ.increase(mappedID, DATAOBJ.get(FrankData::outputSource, CHANNEL) - 1, id);
            }
            else {
                DATAOBJ.decrease(mappedID, DATAOBJ.get(FrankData::outputSource, CHANNEL) - 1, id);
            }
            break;

            // TYPE,Channel,Index;  DoubleStep

        case GATELENGTH:

            if (dir) {
                DATAOBJ.change(mappedID, 2, DATAOBJ.get(FrankData::outputSource, CHANNEL) - 1, id);
            }
            else {
                DATAOBJ.change(mappedID, -2, DATAOBJ.get(FrankData::outputSource, CHANNEL) - 1, id);
            }
            break;

        case FrankData::screenCalCv:
        case FrankData::screenCalNote:
        case FrankData::screenMainMenu:
        case FrankData::screenOutputChannel:
        case FrankData::screenConfig:
        case FrankData::saveCal: break;

        // Type, Channel;
        case FrankData::outputArpOctave:
        case FrankData::outputClock:
        case FrankData::outputRatchet:
        case FrankData::outputArpMode:
        case FrankData::outputArp:

        // case FrankData::midiSource :
        case FrankData::outputLiveMode:

        case FrankData::stepSpeed:
        case FrankData::outputCc:
        case FrankData::outputChannel:
        case FrankData::outputSource:

        case FrankData::nbPages:

        case FrankData::cvCalOffset:
        case FrankData::cvCalLower:
        case FrankData::cvCalUpper:
        case FrankData::cvPitchbendCalLower:
        case FrankData::cvPitchbendCalUpper:

        case FrankData::liveCalNote:
        case FrankData::noteScaleOffset:

            if (dir) {
                DATAOBJ.increase(mappedID, CHANNEL);
            }
            else {
                DATAOBJ.decrease(mappedID, CHANNEL);
            }

            break;

        // Calibration:
        case FrankData::noteCalOffset:
            if (dir) {
                DATAOBJ.increase(mappedID, CHANNEL, DATAOBJ.get(FrankData::liveCalNote));
            }
            else {
                DATAOBJ.decrease(mappedID, CHANNEL, DATAOBJ.get(FrankData::liveCalNote));
            }

            break;
            // Calibration:
        case FrankData::outputCcEvaluated:
            if (dir) {
                DATAOBJ.increase(FrankData::outputCc, CHANNEL);
            }
            else {
                DATAOBJ.decrease(FrankData::outputCc, CHANNEL);
            }

            break;
        case FrankData::seqTuning:

            if (dir) {
                DATAOBJ.increase(mappedID, SEQCHANNEL);
            }
            else {
                DATAOBJ.decrease(mappedID, SEQCHANNEL);
            }

            break;

        // Type, Channel; DoubleStep
        case FrankData::seqGateLengthOffset:

            if (dir) {
                DATAOBJ.change(mappedID, 2, SEQCHANNEL);
            }
            else {
                DATAOBJ.change(mappedID, -2, SEQCHANNEL);
            }
            break;

        case SUBSCREEN:
            if (dir) {

                if (DATAOBJ.get(SUBSCREEN) == 1 && DATAOBJ.get(FrankData::screenConfig) == 1) {
                    DATAOBJ.toggle(FrankData::screenConfig);
                    DATAOBJ.set(FrankData::screenSubScreen, 0);
                }
                else {
                    DATAOBJ.increase(SUBSCREEN);
                }
            }
            else {

                if (DATAOBJ.get(FrankData::screenSubScreen) == 0 && DATAOBJ.get(FrankData::screenConfig) == 0) {
                    DATAOBJ.toggle(FrankData::screenConfig);
                    DATAOBJ.set(FrankData::screenSubScreen, 1);
                }
                else {
                    DATAOBJ.decrease(FrankData::screenSubScreen);
                }
            }
            break;

        case FrankData::displayBrightness:
            if (dir) {
                DATAOBJ.change(mappedID, 5);
            }
            else {
                DATAOBJ.change(mappedID, -5);
            }
            break;

        // Type;
        default:
            if (dir) {
                DATAOBJ.increase(mappedID);
            }
            else {
                DATAOBJ.decrease(mappedID);
            }
    }
}
void inputControls::push(byte id, byte push) { // switch message

    FrankData::frankData mappedID = mappingPush(id);
    id = id + DATAOBJ.get(FrankData::activePage, CHANNEL) * 8;

    PRINTLN(id);
    PRINT("Mapping:");
    PRINTLN(mappedID);

    switch (mappedID) {
        case FrankData::noteCalOffset: DATAOBJ.toggle(mappedID, CHANNEL, DATAOBJ.get(FrankData::liveCalNote)); break;

        case GATE: DATAOBJ.toggle(mappedID, (byte)SEQCHANNEL, id); break;

        default: DATAOBJ.toggle(mappedID);
    }
}

void inputControls::readSwitches() {

    PRINT("INPUT: SYNC set: ");
    PRINTLN(!digitalRead(SWSYNC));
    PRINT("INPUT: SEQ set: ");
    PRINTLN(digitalRead(SWSEQ));
    PRINT("INPUT: REC set: ");
    PRINTLN(!digitalRead(SWREC));

    DATAOBJ.set(FrankData::bpmSync, !digitalRead(SWSYNC));
    DATAOBJ.set(FrankData::screenOutputChannel, digitalRead(SWSEQ));
    DATAOBJ.set(FrankData::rec, !digitalRead(SWREC));
}

void inputControls::readSync() {
    DATAOBJ.set(FrankData::bpmSync, digitalRead(SWSYNC));
}
void inputControls::readRec() {
    DATAOBJ.set(FrankData::rec, digitalRead(SWREC));
}

void inputControls::readSeq() {
    DATAOBJ.set(FrankData::screenOutputChannel, digitalRead(SWSEQ));
}

void inputControls::readBPMSpeed() {
    static elapsedMillis timer;
    static elapsedMillis timerFast;
    static uint16_t tempRead;

    if (DATAOBJ.get(FrankData::bpmSync))
        return;

    if (timerFast > 9) {
        tempRead = analogRead(BPMPOT);
        timerFast = 0;
    }
    if (timer > 15) {
        DATAOBJ.set(FrankData::bpmPoti, 1023 - (analogRead(BPMPOT) + tempRead) / 2);
        timer = 0;
        timerFast = 0;
    }
}

void inputControls::init() {
    pinMode(SWSYNC, INPUT_PULLUP);
    pinMode(SWSEQ, INPUT_PULLUP);
    pinMode(SWREC, INPUT_PULLUP);
    pinMode(BPMPOT, INPUT);
    readBPMSpeed();
    delay(10);
    readSwitches();
}