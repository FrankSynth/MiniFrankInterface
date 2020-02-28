#include "interfaceMidi.hpp"


// GETDATAOBJ

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // Which serial???
// #define DEBUG

// callback handles
void midiNoteOn(byte channel, byte note, byte velocity);
void midiNoteOff(byte channel, byte note, byte velocity);
void midiCC(byte channel, byte control, byte value);
void midiAfterTouch(byte channel, byte pressure);
void midiClock();
void midiSongPosition(unsigned int spp);
void midiStart();
void midiContinue();
void midiStop();
void midiSystemReset();

// USB
void usbMidiNoteOn(byte channel, byte note, byte velocity) {
    if (DATAOBJ.getMidiSource() == 1) {
        midiNoteOn(channel, note, velocity);
    }
}
void usbMidiNoteOff(byte channel, byte note, byte velocity) {
    if (DATAOBJ.getMidiSource() == 1) {
        midiNoteOff(channel, note, velocity);
    }
}
void usbMidiCC(byte channel, byte control, byte value) {
    if (DATAOBJ.getMidiSource() == 1) {
        midiCC(channel, control, value);
    }
}
void usbMidiAfterTouch(byte channel, byte pressure) {
    if (DATAOBJ.getMidiSource() == 1) {
        midiAfterTouch(channel, pressure);
    }
}
void usbMidiClock() {
    if (DATAOBJ.getMidiSource() == 1) {
        midiClock();
    }
}
void usbMidiSongPosition(uint16_t spp) {
    if (DATAOBJ.getMidiSource() == 1) {
        midiSongPosition(spp);
    }
}
void usbMidiStart() {
    if (DATAOBJ.getMidiSource() == 1) {
        midiStart();
    }
}
void usbMidiContinue() {
    if (DATAOBJ.getMidiSource() == 1) {
        midiContinue();
    }
}
void usbMidiStop() {
    if (DATAOBJ.getMidiSource() == 1) {
        midiStop();
    }
}
void usbMidiSystemReset() {
    if (DATAOBJ.getMidiSource() == 1) {
        midiSystemReset();
    }
}

// DIN
void dinMidiNoteOn(byte channel, byte note, byte velocity) {
    if (DATAOBJ.getMidiSource() == 0) {
        midiNoteOn(channel, note, velocity);
    }
}
void dinMidiNoteOff(byte channel, byte note, byte velocity) {
    if (DATAOBJ.getMidiSource() == 0) {
        midiNoteOff(channel, note, velocity);
    }
}
void dinMidiCC(byte channel, byte control, byte value) {
    if (DATAOBJ.getMidiSource() == 0) {
        midiCC(channel, control, value);
    }
}
void dinMidiAfterTouch(byte channel, byte pressure) {
    if (DATAOBJ.getMidiSource() == 0) {
        midiAfterTouch(channel, pressure);
    }
}
void dinMidiClock() {
    if (DATAOBJ.getMidiSource() == 0) {
        midiClock();
    }
}
void dinMidiSongPosition(unsigned int spp) {
    if (DATAOBJ.getMidiSource() == 0) {
        midiSongPosition(spp);
    }
}
void dinMidiStart() {
    if (DATAOBJ.getMidiSource() == 0) {
        midiStart();
    }
}
void dinMidiContinue() {
    if (DATAOBJ.getMidiSource() == 0) {
        midiContinue();
    }
}
void dinMidiStop() {
    if (DATAOBJ.getMidiSource() == 0) {
        midiStop();
    }
}
void dinMidiSystemReset() {
    if (DATAOBJ.getMidiSource() == 0) {
        midiSystemReset();
    }
}

// run once at startup
void initMidi() {

        // data = getDataObject();

#ifdef DEBUG
    Serial.println("Init Midi");
#endif

    // set USB handles
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
    MIDI.setHandleNoteOn(dinMidiNoteOn);
    MIDI.setHandleNoteOff(dinMidiNoteOff);
    MIDI.setHandleControlChange(dinMidiCC);
    MIDI.setHandleAfterTouchChannel(dinMidiAfterTouch);
    MIDI.setHandleSongPosition(dinMidiSongPosition);
    MIDI.setHandleClock(dinMidiClock);
    MIDI.setHandleStart(dinMidiStart);
    MIDI.setHandleContinue(dinMidiContinue);
    MIDI.setHandleStop(dinMidiStop);
    MIDI.setHandleSystemReset(dinMidiSystemReset);
}

// Midi Loop
void updateMidi() {

    while (usbMIDI.read()) {
    }
    while (MIDI.read()) {
    }
}

// Midi actions

void midiNoteOn(byte channel, byte note, byte velocity) {
#ifdef DEBUG
    Serial.println("Key pressed");
#endif
    DATAOBJ.receivedKeyPressed(channel, note, velocity);
};

void midiNoteOff(byte channel, byte note, byte velocity) {
#ifdef DEBUG
    Serial.println("Key released");
#endif
    DATAOBJ.receivedKeyReleased(channel, note);
};

void midiCC(byte channel, byte cc, byte midiData) {
    if (cc == 1) {
        DATAOBJ.receivedMod(channel, midiData);
    }
    if (cc == 64) {
        DATAOBJ.receivedSustain(channel, midiData);
    }
};

void midiAfterTouch(byte channel, byte midiData) { DATAOBJ.receivedAftertouch(channel, midiData); };

void midiClock() { DATAOBJ.receivedMidiClock(); };

void midiSongPosition(unsigned int spp) { DATAOBJ.receivedMidiSongPosition(spp); };

void midiStart() { DATAOBJ.receivedStart(); };

void midiContinue() { DATAOBJ.receivedContinue(); };

void midiStop() { DATAOBJ.receivedStop(); };

void midiSystemReset() { DATAOBJ.receivedReset(); };
