#include "interfaceIn.hpp"

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

void controls::encode(byte message) {

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
void controls::rotate(byte id, byte dir) {
    PRINTLN(id);
    PRINT("Direction: ");
    PRINTLN(dir);
    Serial.print("mapping: ");

    FrankData::frankData mappedID = mapping(id);

    id = id + DATAOBJ.get(FrankData::activePage,
                          DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1) *
                  8;

    Serial.println(mappedID);
    switch (mappedID) {

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
    case FrankData::outputArpRatchet:
    case FrankData::outputArpMode:
    case FrankData::outputArp:

    // case FrankData::midiSource :
    case FrankData::outputLiveMode:
    
    case FrankData::stepSpeed:
    case FrankData::outputCc:
    case FrankData::outputChannel:
    case FrankData::outputSource:

        if (dir) {
            DATAOBJ.increase(mappedID, DATAOBJ.get(FrankData::screenOutputChannel));
        }
        else {
            DATAOBJ.decrease(mappedID, DATAOBJ.get(FrankData::screenOutputChannel));
        }

        break;

    case FrankData::nbPages:
    case FrankData::seqTuning:

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
void controls::push(byte id, byte push) { // switch message

    FrankData::frankData mappedID = mappingPush(id);
    id = id + DATAOBJ.get(FrankData::activePage, DATAOBJ.get(FrankData::screenOutputChannel)) * 8;

    PRINTLN(id);
    PRINT("Mapping:");
    PRINTLN(mappedID);
    switch (mappedID) {

        // TYPE,Channel,index
    case GATE:

    DATAOBJ.toggle(mappedID, DATAOBJ.get(FrankData::screenOutputChannel), id);
    break;

    default: DATAOBJ.toggle(mappedID);
    }
}

void controls::readSwitches() {

    PRINT("INPUT: SYNC set: ");
    PRINTLN(digitalRead(SWSYNC));
    PRINT("INPUT: SEQ set: ");
    PRINTLN(digitalRead(SWSEQ));
    PRINT("INPUT: REC set: ");
    PRINTLN(digitalRead(SWREC));

    DATAOBJ.set(FrankData::bpmSync,! digitalRead(SWREC));
    DATAOBJ.set(FrankData::screenOutputChannel,digitalRead(SWSEQ));
    DATAOBJ.set(FrankData::rec,! digitalRead(SWREC));
}

void controls::readSync() {
    DATAOBJ.set(FrankData::bpmSync, digitalRead(SWSYNC));
}
void controls::readRec() {
    DATAOBJ.set(FrankData::bpmSync, digitalRead(SWREC));
}

void controls::readSeq() {
    DATAOBJ.set(FrankData::screenOutputChannel, digitalRead(SWSEQ));
}

void controls::readBPMSpeed() {

    // PRINTLN("Speed:");
    // PRINTLN(analogRead(BPMPOT));
}