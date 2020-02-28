#include "interfaceIn.hpp"

// GETDATAOBJ

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
    byte activePage = DATAOBJ.getActivePage();
    byte offset = activePage * STEPPERPAGE;
    Seq *activeSeq = getActiveSeqPointer();

    if (DATAOBJ.getActiveMenu() == 0) { // Note menu
        if (id < 8) {                     // noteGate encoder
            if (dir) {
                activeSeq->increaseNote(offset + id);
            } else {
                activeSeq->decreaseNote(offset + id);
            }
        } else { // menu encoder
        }
    } else if (DATAOBJ.getActiveMenu() == 1) { // Gate length menu
        if (id < 8) {                            // noteGate encoder
            if (dir) {
                activeSeq->changeGateLength(offset + id, 5);
            } else {
                activeSeq->changeGateLength(offset + id, -5);
            }
        } else { // menu encoder
        }
    }
}

void controls::push(byte id, byte push) { // switch message
    byte activePage = DATAOBJ.getActivePage();
    byte offset = activePage * STEPPERPAGE;
    Seq *activeSeq = getActiveSeqPointer();

    if (DATAOBJ.getActiveMenu() == 0) { // Note menu
        if (id < 8) {                     // noteGate encoder
            if (push) {
                activeSeq->toggleGate(offset + id);
            }
        } else { // menu encoder
        }
    } else if (DATAOBJ.getActiveMenu() == 1) { // Gate length menu
        if (id < 8) {                            // noteGate encoder
            if (push) {
                activeSeq->toggleGate(offset + id);
            }
        } else { // menu encoder
        }
    }
}

void controls::readSwitches() {

#ifdef DEBUG
    Serial.print("INPUT: SYNC set: ");
    Serial.println(digitalRead(SWSYNC));
    Serial.print("INPUT: SEQ set: ");
    Serial.println(digitalRead(SWSEQ));
    Serial.print("INPUT: REC set: ");
    Serial.println(digitalRead(SWREC));
#endif

    DATAOBJ.setSync(digitalRead(SWSYNC));
    DATAOBJ.setActiveSeq(digitalRead(SWSEQ));
    DATAOBJ.setRec(digitalRead(SWREC));
}

void controls::readSync() { DATAOBJ.setSync(digitalRead(SWSYNC)); }
void controls::readSeq() { DATAOBJ.setActiveSeq(digitalRead(SWSEQ)); }
void controls::readRec() { DATAOBJ.setRec(digitalRead(SWREC)); }

void controls::init() { // set pointer
    // seq1 = &getSeq()[0];
    // seq2 = &getSeq()[1];
    // settings = getStatus();;
}

// utility
Seq *controls::getActiveSeqPointer() {
    // if (DATAOBJ.getActiveSeq() == 0) {
    //     return seq1;
    // } else if (DATAOBJ.getActiveSeq() == 1) {
    //     return seq2;
    // }
    return &DATAOBJ.seq[DATAOBJ.getActiveSeq()];
    return NULL; // notValid
}

