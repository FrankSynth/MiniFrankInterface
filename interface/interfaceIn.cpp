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

    id = id + DATAOBJ.get(FrankData::activePage ,DATAOBJ.get(FrankData::screenOutputChannel)) *
                  8;

    switch (mappedID) {

case STEP:

    if (dir) {
        DATAOBJ.increaseStepCounters(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1);
    }
    else {
        DATAOBJ.decreaseStepCounters(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1);
    }
    break;

    // TYPE,Channel,Index;
    case NOTE:
    case CV:

        if (dir) {
            DATAOBJ.increase(mappedID, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, id);
        }
        else {
            DATAOBJ.decrease(mappedID, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, id);
        }
        break;

        // TYPE,Channel,Index;  DoubleStep

    case GATELENGTH:

        if (dir) {
            DATAOBJ.change(mappedID, 2, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, id);
        }
        else {
            DATAOBJ.change(mappedID, -2, DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1, id);
        }
        break;

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
    case FrankData::liveCalNote:
     case   FrankData::noteScaleOffset:

            if (dir) {
            DATAOBJ.increase(mappedID, DATAOBJ.get(FrankData::screenOutputChannel));
        }
        else {
            DATAOBJ.decrease(mappedID, DATAOBJ.get(FrankData::screenOutputChannel));
        }

        break;

    // Calibration:
    case FrankData::noteCalOffset :
        if (dir) {
            DATAOBJ.increase(mappedID, DATAOBJ.get(FrankData::screenOutputChannel), DATAOBJ.get(FrankData::liveCalNote));
        }
        else {
            DATAOBJ.decrease(mappedID, DATAOBJ.get(FrankData::screenOutputChannel), DATAOBJ.get(FrankData::liveCalNote));
        }

        break;

    case FrankData::seqTuning :

            if (dir) {
            DATAOBJ.increase(mappedID, (byte)(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1));
        }
        else {
            DATAOBJ.decrease(mappedID, (byte)(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1));
        }

        break;

    // Type, Channel; DoubleStep
    case FrankData::seqGateLengthOffset:

        if (dir) {
            DATAOBJ.change(mappedID, 2,
                           (byte)(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1));
        }
        else {
            DATAOBJ.change(mappedID, -2, (byte)(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1));
        }
        break;

    case SUBSCREEN:
        if (dir) {

            if (DATAOBJ.get(SUBSCREEN) == 0 && DATAOBJ.get(FrankData::screenConfig) == 1) {
                DATAOBJ.toggle(FrankData::screenConfig);
            }
            else {
                DATAOBJ.increase(SUBSCREEN);
            }
        }
        else {

            if (DATAOBJ.get(FrankData::screenSubScreen) == 0 && DATAOBJ.get(FrankData::screenConfig) == 0) {
                DATAOBJ.toggle(FrankData::screenConfig);
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
    id = id + DATAOBJ.get(FrankData::activePage, DATAOBJ.get(FrankData::screenOutputChannel)) * 8;

    PRINTLN(id);
    PRINT("Mapping:");
    PRINTLN(mappedID);

    switch (mappedID) {
    case FrankData::noteCalOffset: 
    DATAOBJ.toggle(mappedID, DATAOBJ.get(FrankData::screenOutputChannel), DATAOBJ.get(FrankData::liveCalNote)); break;

    case GATE: DATAOBJ.toggle(mappedID, (byte)(DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1), id); break;

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
    DATAOBJ.setBPMPoti(1024-analogRead(BPMPOT));
}

void inputControls::init(){
        pinMode(SWSYNC, INPUT_PULLUP);
        pinMode(SWSEQ, INPUT_PULLUP);
        pinMode(SWREC, INPUT_PULLUP);
        pinMode(BPMPOT, INPUT);
        readBPMSpeed();
        delay(10);
        readSwitches();
}