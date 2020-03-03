#include "interfaceMidi.hpp"

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

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // Which serial???

// callback handles
void midiNoteOn(byte channel, byte note, byte velocity);
void midiNoteOff(byte channel, byte note, byte velocity);
void midiCC(byte channel, byte control, byte value);
void midiPitchBend(byte channel, int value);
void midiAfterTouch(byte channel, byte pressure);
void midiClock();
void midiSongPosition(unsigned int spp);
void midiStart();
void midiContinue();
void midiStop();
void midiSystemReset();

// USB
void usbMidiNoteOn(byte channel, byte note, byte velocity) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiNoteOn(channel, note, velocity);
    }
}
void usbMidiNoteOff(byte channel, byte note, byte velocity) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiNoteOff(channel, note, velocity);
    }
}
void usbMidiCC(byte channel, byte control, byte value) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiCC(channel, control, value);
    }
}
void usbMidiPitchBend(byte channel, int value) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiPitchBend(channel, value);
    }
}
void usbMidiAfterTouch(byte channel, byte pressure) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiAfterTouch(channel, pressure);
    }
}
void usbMidiClock() {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiClock();
    }
}
void usbMidiSongPosition(uint16_t spp) {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiSongPosition(spp);
    }
}
void usbMidiStart() {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiStart();
    }
}
void usbMidiContinue() {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiContinue();
    }
}
void usbMidiStop() {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiStop();
    }
}
void usbMidiSystemReset() {
    if (DATAOBJ.get(FrankData::midiSource) == 1) {
        midiSystemReset();
    }
}

// DIN
void dinMidiNoteOn(byte channel, byte note, byte velocity) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiNoteOn(channel, note, velocity);
    }
}
void dinMidiNoteOff(byte channel, byte note, byte velocity) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiNoteOff(channel, note, velocity);
    }
}
void dinMidiCC(byte channel, byte control, byte value) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiCC(channel, control, value);
    }
}
void dinMidiAfterTouch(byte channel, byte pressure) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiAfterTouch(channel, pressure);
    }
}
void dinMidiPitchBend(byte channel, int value) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiPitchBend(channel, value);
    }
}
void dinMidiClock() {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiClock();
    }
}
void dinMidiSongPosition(unsigned int spp) {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiSongPosition(spp);
    }
}
void dinMidiStart() {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiStart();
    }
}
void dinMidiContinue() {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiContinue();
    }
}
void dinMidiStop() {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiStop();
    }
}
void dinMidiSystemReset() {
    if (DATAOBJ.get(FrankData::midiSource) == 0) {
        midiSystemReset();
    }
}

// run once at startup
void initMidi() {

    // data = getDataObject();

    PRINTLN("Init Midi");

    // set USB handles
    usbMIDI.setHandleNoteOn(usbMidiNoteOn);
    usbMIDI.setHandleNoteOff(usbMidiNoteOff);
    usbMIDI.setHandleControlChange(usbMidiCC);
    usbMIDI.setHandlePitchChange(usbMidiPitchBend);
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
    MIDI.setHandlePitchBend(dinMidiPitchBend);
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
    PRINTLN("Key pressed");
    DATAOBJ.receivedKeyPressed(channel, note, velocity);
}

void midiNoteOff(byte channel, byte note, byte velocity) {
    PRINTLN("Key released");
    DATAOBJ.receivedKeyReleased(channel, note);
}

void midiCC(byte channel, byte cc, byte midiData) {
    if (cc == 1) {
        for (byte x = 0; x < OUTPUTS; x++) {
            if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                DATAOBJ.set(FrankData::liveMod, midiData, x);
            }
        }
    }
    if (cc == 64) {
        for (byte x = 0; x < OUTPUTS; x++) {
            if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                DATAOBJ.set(FrankData::liveSustain, midiData, x);
            }
        }
    }
}

void midiAfterTouch(byte channel, byte midiData) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
            DATAOBJ.set(FrankData::liveAftertouch, midiData, x);
        }
    }
 }

// check byte to int convertion
 void midiPitchBend(byte channel, int value) {
     for (byte x = 0; x < OUTPUTS; x++) {
         if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
             DATAOBJ.set(FrankData::livePitchbend, (byte)value, x);
         }
     }
 }

void midiClock() { DATAOBJ.receivedMidiClock(); }

void midiSongPosition(unsigned int spp) { DATAOBJ.receivedMidiSongPosition(spp); }

void midiStart() { DATAOBJ.receivedStart(); }

void midiContinue() { DATAOBJ.receivedContinue(); }

void midiStop() { DATAOBJ.receivedStop(); }

void midiSystemReset() { DATAOBJ.receivedReset(); }
