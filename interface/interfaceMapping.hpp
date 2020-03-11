#pragma once

#include "interfaceData.hpp"
#include <Arduino.h>

#define NONE FrankData::none
#define BPM FrankData::bpm
#define NOTE FrankData::seqNote
#define GATE FrankData::seqGate
#define GATELENGTH FrankData::seqGateLength

#define STEP FrankData::stepSeq
#define CV FrankData::seqCc
#define PLAY FrankData::play
#define CONFIG FrankData::screenConfig

// Encoder{0,1,2,3,4,5,6,7,8,9}


FrankData::frankData mapping(byte input); // return target ID
FrankData::frankData mappingPush(byte input); // return target ID
