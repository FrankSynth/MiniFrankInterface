#pragma once

#include "interfaceData.hpp"
#include <Arduino.h>

/*

(0)     (1)      (2)      (3)    |  (8)

(4)     (5)      (6)      (7)   |  (9)

0-7 Drehregler werden gemapped auf den jeweiligen Modus

8 + 9 sind standardmäßig eingestellt:

LIVE:
8-> Push => Toggle Play Stop
    Rotate => Step Forward / Backward
9-> Push => Toggle Menu
    Rotate =>

SEQ:
8-> Push => Toggle Play Stop
    Rotate => Step Forward / Backward
9-> Push => Toggle Menu
    Rotate => Global Gate Increase/Decrease
*/

// zum Testen vom mapping
#define NONE 0
#define BPM FrankData::bpm
#define NOTE FrankData::seqNote
#define GATE FrankData::seqGate
// #define GATE 1
// #define CGATE 1
#define STEP FrankData::stepSeq
#define CV FrankData::seqCc
#define CONF FrankData::none

static FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE};             // NOTE Mapping  Rotary
static FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE}; // NOTE Mapping Push
static FrankData::frankData mappingSeqNoteConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF};       // NOTE Config Mapping

static FrankData::frankData mappingSeqGate[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE};             // GATE Mapping Rotary
static FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE}; // NOTE Mapping Push
static FrankData::frankData mappingSeqGateConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF};       // GATE Config Mapping

static FrankData::frankData mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV};                             // CV Mapping
static FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE}; // NOTE Mapping
static FrankData::frankData mappingSeqCVConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF};       // CV Config Mapping

static FrankData::frankData mappingLive[] = {FrankData::liveMod, FrankData::outputCcEvaluated, FrankData::outputClock, FrankData::outputArp, FrankData::liveKeyNoteEvaluated, FrankData::liveLowestKey, FrankData::liveHighestKey, FrankData::outputChannel};       // Live Mapping
static FrankData::frankData mappingLiveConfig[] = {FrankData::outputCc, CONF, CONF, CONF, CONF, CONF, CONF, CONF}; // Live Mapping

static FrankData::frankData mappingMenu[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF}; // Routing Menu Mapping

/*/////ROUTING//////



*/

FrankData::frankData mapping(byte input); // return target ID
FrankData::frankData mappingPush(byte input); // return target ID
