#include "interfaceMidi.h"

// #define DEBUG

// callback handles
void midiNoteOn(byte channel, byte note, byte velocity);
void midiNoteOff(byte channel, byte note, byte velocity);
void midiCC(byte channel, byte control, byte value);
void midiAfterTouch(byte channel, byte pressure);
void midiClock();
void midiSongPosition(uint16_t spp);
void midiStart();
void midiContinue();
void midiStop();
void midiSystemReset();

// USB
void usbMidiNoteOn(byte channel, byte note, byte velocity) {    if (settings::getMidiSource() == 1) { midiNoteOn(channel, note, velocity);    } }
void usbMidiNoteOff(byte channel, byte note, byte velocity) {   if (settings::getMidiSource() == 1) { midiNoteOff(channel, note, velocity);   } }
void usbMidiCC(byte channel, byte control, byte value) {        if (settings::getMidiSource() == 1) { midiCC(channel, control, value);        } }
void usbMidiAfterTouch(byte channel, byte pressure) {           if (settings::getMidiSource() == 1) { midiAfterTouch(channel, pressure);      } }
void usbMidiClock() {                                           if (settings::getMidiSource() == 1) { midiClock();                            } }
void usbMidiSongPosition(uint16_t spp) {                        if (settings::getMidiSource() == 1) { midiSongPosition(spp);                  } }
void usbMidiStart() {                                           if (settings::getMidiSource() == 1) { midiStart();                            } }
void usbMidiContinue() {                                        if (settings::getMidiSource() == 1) { midiContinue();                         } }
void usbMidiStop() {                                            if (settings::getMidiSource() == 1) { midiStop();                             } }
void usbMidiSystemReset() {                                     if (settings::getMidiSource() == 1) { midiSystemReset();                      } }

// DIN
// void dinMidiNoteOn(byte channel, byte note, byte velocity) {    if (settings::getMidiSource() == 0) { midiNoteOn(channel, note, velocity);    } }
// void dinMidiNoteOff(byte channel, byte note, byte velocity) {   if (settings::getMidiSource() == 0) { midiNoteOff(channel, note, velocity);   } }
// void dinMidiCC(byte channel, byte control, byte value) {        if (settings::getMidiSource() == 0) { midiCC(channel, control, value);        } }
// void dinMidiAfterTouch(byte channel, byte pressure) {           if (settings::getMidiSource() == 0) { midiAfterTouch(channel, pressure);      } }
// void dinMidiClock() {                                           if (settings::getMidiSource() == 0) { midiClock();                            } }
// void dinMidiSongPosition(uint16_t spp) {                        if (settings::getMidiSource() == 0) { midiSongPosition(spp);                  } }
// void dinMidiStart() {                                           if (settings::getMidiSource() == 0) { midiStart();                            } }
// void dinMidiContinue() {                                        if (settings::getMidiSource() == 0) { midiContinue();                         } }
// void dinMidiStop() {                                            if (settings::getMidiSource() == 0) { midiStop();                             } }
// void dinMidiSystemReset() {                                     if (settings::getMidiSource() == 0) { midiSystemReset();                      } }


// run once at startup
void initMidi() {

    //set USB handles
    usbMIDI.setHandleNoteOn(usbMidiNoteOn);
    usbMIDI.setHandleNoteOff(usbMidiNoteOff);
    usbMIDI.setHandleControlChange(usbMidiCC);
    usbMIDI.setHandleAfterTouch(usbMidiAfterTouch);
    usbMIDI.setHandleSongPosition(usbMidiSongPosition);
    usbMIDI.setHandleClock(usbMidiClock);
    usbMIDI.setHandleStart(usbMidiStart);
    usbMIDI.setHandleContinue(usbMidiContinue);
    usbMIDI.setHandleStop(usbMidiStop);
    usbMIDI.setHandleSystemReset(usbMidiSystemReset);

    // set DIN handles
    // dinMIDI.setHandleNoteOn(dinMidiNoteOn);
    // dinMIDI.setHandleNoteOff(dinMidiNoteOff);
    // dinMIDI.setHandleControlChange(dinMidiCC);
    // dinMIDI.setHandleAfterTouch(dinMidiAfterTouch);
    // dinMIDI.setHandleSongPosition(dinMidiSongPosition);
    // dinMIDI.setHandleClock(dinMidiClock);
    // dinMIDI.setHandleStart(dinMidiStart);
    // dinMIDI.setHandleContinue(dinMidiContinue);
    // dinMIDI.setHandleStop(dinMidiStop);
    // dinMIDI.setHandleSystemReset(dinMidiSystemReset);

    #ifdef DEBUG
    Serial.println("Init Midi");
    #endif
}


// Midi Loop
void updateMidi() {

    while(usbMIDI.read()) {}
    // while(dinMIDI.read()) {}

}


// Midi actions
void midiNoteOn(byte channel, byte note, byte velocity) {
    #ifdef DEBUG
    Serial.println("Key pressed");
    #endif
    receivedKeyPressed(channel, note, velocity);

};

void midiNoteOff(byte channel, byte note, byte velocity) {
    #ifdef DEBUG
    Serial.println("Key released");
    #endif
    receivedKeyReleased(channel, note);

};

void midiCC(byte channel, byte cc, byte data) {
    if (cc == 1) {
        receivedMod(channel, data);
    }
    if (cc == 64) {
        receivedSustain(channel, data);
    }
};

void midiAfterTouch(byte channel, byte data) {
    receivedAftertouch(channel, data);
};

void midiClock() {
    receivedMidiClock();
};

void midiSongPosition(uint16_t spp) {
    receivedMidiSongPosition(spp);
};

void midiStart() {
    receivedStart();
};

void midiContinue() {
    receivedContinue();
};

void midiStop() {
    receivedStop();
};

void midiSystemReset() {

};
