#include "interfaceIn.hpp"



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
}

void controls::push(byte id, byte push) { // switch message
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
  DATAOBJ.setScreenChannel(digitalRead(SWSEQ));
  DATAOBJ.setRec(digitalRead(SWREC));
}

void controls::readSync() { DATAOBJ.setSync(digitalRead(SWSYNC)); }
void controls::readRec() { DATAOBJ.setRec(digitalRead(SWREC)); }

void controls::readSeq() { DATAOBJ.setScreenChannel(digitalRead(SWSEQ)); }
