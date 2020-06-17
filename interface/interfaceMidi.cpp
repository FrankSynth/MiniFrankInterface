#include "interfaceMidi.hpp"

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

MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI);

// callback handles
void midiNoteOn(const byte &channel, const byte &note, const byte &velocity);
void midiNoteOff(const byte &channel, const byte &note, const byte &velocity);
void midiCC(const byte &channel, const byte &control, const byte &value);
void midiPitchBend(const byte &channel, int value);
void midiAfterTouch(const byte &channel, const byte &pressure);
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

    usbMIDI.begin();
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

// Midi Loop
void updateMidi() {

    // usbMIDI.read();
    // MIDI.read();
    while (usbMIDI.read()) {
    }
    while (MIDI.read()) {
    }
}

// Midi actions

void midiNoteOn(const byte &channel, const byte &note, const byte &velocity) {
    PRINT("Midi Key pressed");
    PRINTLN(note - 21);
    if (note > 20)
        DATAOBJ.receivedKeyPressed(channel, note - 21, velocity);
}

void midiNoteOff(const byte &channel, const byte &note, const byte &velocity) {
    PRINTLN("Midi Key released");
    if (note > 20)
        DATAOBJ.receivedKeyReleased(channel, note - 21);
}

void midiCC(const byte &channel, const byte &cc, const byte &midiData) {
    PRINT(millis());
    PRINT(": Midi CC ");
    PRINT(cc);
    PRINT(", Data: ");
    PRINTLN(midiData);

    switch (cc) {

            // CCs for Live performance
        case 1:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::liveMod, midiData, x);
                }
            }
            break;

        case 64:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::liveSustain, midiData, x);

                    if (DATAOBJ.get(FrankData::outputArp, x) && !DATAOBJ.get(FrankData::outputSource, x)) {
                        DATAOBJ.updateArp(x);
                    }
                }
            }
            break;

        // CCs to access settings
        case 12:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputSource, midiData / 43, x);
                }
            }
            break;

        case 13:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputArp, midiData / 43, x);
                }
            }
            break;

        case 14:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputArpMode, midiData / 12, x);
                }
            }
            break;

        case 15:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputArpOctave, midiData / 18, x);
                }
            }
            break;

        case 17:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputCc, midiData / 25, x);
                }
            }
            break;

        case 18:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputRatchet, midiData / 32, x);
                }
            }
            break;

        case 19:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::stepSpeed, midiData / 5, x);
                }
            }
            break;

        case 21:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputLiveMode, midiData / 43, x);
                }
            }
            break;

        case 23:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::direction, midiData / 64);
                }
            }
            break;

        case 24:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::play, midiData / 64);
                }
            }
            break;

        case 25:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (DATAOBJ.get(FrankData::outputSource, x))
                        DATAOBJ.set(FrankData::seqGateLengthOffset, map(midiData, 0, 127, -100, 100), DATAOBJ.get(FrankData::outputSource, x) - 1);
                }
            }
            break;

        case 26:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::nbPages, midiData / 8 + 1, x);
                }
            }
            break;

        case 27:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (DATAOBJ.get(FrankData::outputSource, x))
                        DATAOBJ.set(FrankData::seqPageEndOffset, -midiData / 16, DATAOBJ.get(FrankData::outputSource, x) - 1);
                }
            }
            break;

        case 28:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::seqNoteOffset, midiData / 5 - 12, x);
                }
            }
            break;

        case 29:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::seqOctaveOffset, midiData / 11 - 5, x);
                }
            }
            break;

        case 30:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (midiData > 63)
                        DATAOBJ.seqResetAllGates(x);
                }
            }
            break;

        case 31:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (midiData > 63)
                        DATAOBJ.seqResetAllGateLengths(x);
                }
            }
            break;

        case 32:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (midiData > 63)
                        DATAOBJ.seqResetAllNotes(x);
                }
            }
            break;

        case 33:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (midiData > 63)
                        DATAOBJ.seqResetAllCC(x);
                }
            }
            break;

        case 34:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    if (midiData > 63)
                        DATAOBJ.resetAllStepCounter();
                }
            }
            break;

        case 36:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputClock, midiData / 5, x);
                }
            }
            break;

        case 37:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputPolyRhythm, midiData / 32, x);
                }
            }
            break;

        case 38:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::outputMidiNotes, midiData / 32, x);
                }
            }
            break;

            // general
        case 40:
            for (byte x = 0; x < OUTPUTS; x++) {
                if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
                    DATAOBJ.set(FrankData::pulseLength, midiData);
                }
            }
            break;

        // reset all
        case 123: DATAOBJ.receivedReset(); break;

        default:;
    }
}

void midiAfterTouch(const byte &channel, const byte &midiData) {
    PRINTLN("Midi AT");

    for (byte x = 0; x < OUTPUTS; x++) {
        if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
            DATAOBJ.set(FrankData::liveAftertouch, midiData, x);
        }
    }
}

// check byte to int convertion
void midiPitchBend(const byte &channel, int value) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (DATAOBJ.get(FrankData::outputChannel, x) == 0 || DATAOBJ.get(FrankData::outputChannel, x) == channel) {
            DATAOBJ.set(FrankData::livePitchbend, (int16_t)value, x);
        }
    }
}

void midiClock() {
    static elapsedMicros midiTimingClock = 0;
    uint32_t microTime = micros();
    PRINT(microTime);
    PRINT(": received new Midi Clock, next clock expected at ");
    PRINT(microTime + midiTimingClock);
    PRINT(", passed Time is ");
    PRINTLN(midiTimingClock);
    midiTimingClock = 0;

    DATAOBJ.receivedMidiClock();
}

void midiSongPosition(unsigned int spp) {
    DATAOBJ.receivedMidiSongPosition(spp);
}

void midiStart() {
    DATAOBJ.receivedStart();
}

void midiContinue() {
    DATAOBJ.receivedContinue();
}

void midiStop() {
    DATAOBJ.receivedStop();
}

void midiSystemReset() {
    DATAOBJ.receivedReset();
}

void sendMidiNoteOn(byte &note, byte &velocity, byte &outChannel) {
    byte midiChannel = testByte(DATAOBJ.get(FrankData::outputChannel, outChannel), 1, 16);

    if (DATAOBJ.get(FrankData::outputMidiNotes, outChannel) > 1) { // usb
        usbMIDI.sendNoteOn(note + 21, velocity, midiChannel);
        // usbMIDI.send_now();
    }
    if (DATAOBJ.get(FrankData::outputMidiNotes, outChannel) == 1 || DATAOBJ.get(FrankData::outputMidiNotes, outChannel) == 3) { // din
        MIDI.sendNoteOn(note + 21, velocity, midiChannel);
    }
}
void sendMidiNoteOff(byte &note, byte &outChannel) {
    byte midiChannel = testByte(DATAOBJ.get(FrankData::outputChannel, outChannel), 1, 16);

    if (DATAOBJ.get(FrankData::outputMidiNotes, outChannel) > 1) { // usb
        usbMIDI.sendNoteOff(note + 21, 0, midiChannel);
        // usbMIDI.send_now();
    }
    if (DATAOBJ.get(FrankData::outputMidiNotes, outChannel) == 1 || DATAOBJ.get(FrankData::outputMidiNotes, outChannel) == 3) { // din
        MIDI.sendNoteOff(note + 21, 0, midiChannel);
    }
}

void sendMidiAllNotesOff() {
    for (byte i = 1; i < 17; i++) {
        // usbMIDI.sendControlChange(64, 0, i);
        // usbMIDI.sendControlChange(121, 0, i);
        usbMIDI.sendControlChange(123, 0, i);
    }
    PRINTLN("midi reset sent");
    // usbMIDI.send_now();
}