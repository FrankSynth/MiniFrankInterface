//MIDI input library
//collecting data from USB and din

#pragma once


#include <Arduino.h>
#include <MIDI.h>

#include "interfaceData.h"

void initMidi();
void updateMidi();