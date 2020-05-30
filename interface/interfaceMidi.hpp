// MIDI input library
// collecting data from USB and din

#pragma once

#include "interfaceData.hpp"
#include <Arduino.h>
#include <MIDI.h>

void initMidi();
void updateMidi();
void sendMidiNoteOn(byte &note, byte &velocity, byte &outChannel);
void sendMidiNoteOff(byte &note, byte &outChannel);
void sendMidiAllNotesOff();
