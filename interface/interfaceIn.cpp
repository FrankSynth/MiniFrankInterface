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

void controls::encode(byte message) {

    byte id = (byte)(B00001111 & message); // extract Message from ID

    if (message & B10000000) { // is a switch?
        push(id, message & B00010000);

    } else { // seems to be an encoder
        rotate(id, message & B00010000);
    }
}

// rotate message
void controls::rotate(byte id, byte dir) {
    if (id == 8) { // Control Encoder
        DATAOBJ.change(GATE, id, dir); //old!!!!!
    } else if (id == 9) { // Step Encoder
        DATAOBJ.change(STEP, id, dir); // old!!!!!
    } else { // Note Encoder
        DATAOBJ.change(mappingPush(id), id, dir); // old!!!!!
    }
}

void controls::push(byte id, byte push) { // switch message
    if (id == 8) {                        // Control Encoder
        DATAOBJ.toggle(FrankData::screenConfig);
    } else if (id == 9) { // Step Encoder
        DATAOBJ.toggle(FrankData::play);
    } else { // Note Encoder
        // DATAOBJ.toggle(mappingPush(id), id); // old!!!!! what does it do?
        DATAOBJ.toggle(mappingPush(id)); // old!!!!! what does it do?
    }
}

void controls::readSwitches() {

    PRINT("INPUT: SYNC set: ");
    PRINTLN(digitalRead(SWSYNC));
    PRINT("INPUT: SEQ set: ");
    PRINTLN(digitalRead(SWSEQ));
    PRINT("INPUT: REC set: ");
    PRINTLN(digitalRead(SWREC));

    DATAOBJ.set(FrankData::bpmSync, digitalRead(SWREC));
    DATAOBJ.set(FrankData::screenOutputChannel, digitalRead(SWSEQ));
    DATAOBJ.set(FrankData::rec,digitalRead(SWREC));
}

void controls::readSync() { DATAOBJ.set(FrankData::bpmSync, digitalRead(SWSYNC)); }
void controls::readRec() { DATAOBJ.set(FrankData::bpmSync, digitalRead(SWREC)); }

void controls::readSeq() { DATAOBJ.set(FrankData::screenOutputChannel, digitalRead(SWSEQ)); }
