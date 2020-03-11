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


////Sequencer////
static FrankData::frankData mappingSeqConfig[] = {FrankData::outputClock, FrankData::direction, NONE, NONE, NONE, NONE, NONE, NONE, STEP, NONE};       // CV Config Mapping
static FrankData::frankData mappingSeqConfigPush[] = {NONE, FrankData::direction, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG};         // GATE Mapping Push

static FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, STEP, NONE};             // NOTE Mapping  Rotary
static FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // NOTE Mapping Push

static FrankData::frankData mappingSeqGate[] = {GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, STEP, NONE  /*Global Gate, funktion fehlt noch */  };             // GATE Mapping Rotary
static FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // GATE Mapping Push

static FrankData::frankData mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV, STEP, NONE  /*Global CV, funktion fehlt noch */  };                             // CV Mapping
static FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // CV Mapping Push




////LIVE////
static FrankData::frankData *mappingLiveConfig = mappingMenuRouting; // Live Mapping
static FrankData::frankData *mappingLiveConfigPush = mappingMenuRoutingPush; // Live Mapping,

static FrankData::frankData mappingLive[] = {FrankData::outputArp, FrankData::outputLiveMode, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};       // Live Mapping
static FrankData::frankData mappingLivePush[] = {FrankData::outputArp, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};       // Live Mapping


static FrankData::frankData mappingLiveArp[] = {FrankData::outputArp, NONE /*octaves*/, NONE, NONE, NONE /*ratchet*/, FrankData::outputClock, NONE, NONE, STEP, NONE};       // Live Arp Mapping
static FrankData::frankData mappingLivePush[] = {FrankData::outputArp, NONE /*octaves*/, NONE, NONE, NONE /*ratchet*/, FrankData::outputClock, NONE, NONE, STEP, NONE};       // Live Arp Mapping Push

////MENU////

static FrankData::frankData mappingMenu[] = {NONE /*LOAD*/, NONE /*SAVE*/, NONE /*RoutingMenu*/, NONE /*CalibrationMenu*/,NONE /*CLK1*/, NONE /*CLK2*/, NONE /*CLKLength*/, FrankData::displayBrightness, STEP, NONE};  // Menu Mapping
static FrankData::frankData mappingMenuCal[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, STEP, NONE}; // Routing Menu Mapping
static FrankData::frankData mappingMenuCalPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG}; // Routing Menu Mapping

static FrankData::frankData mappingMenuRouting[] = {FrankData::liveMod, FrankData::outputSource, FrankData::outputChannel, FrankData::outputCc,FrankData::liveMod, FrankData::outputSource, FrankData::outputChannel, FrankData::outputCc}; // Routing Menu Mapping
static FrankData::frankData mappingMenuRoutingPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG}; // Routing Menu Mapping



FrankData::frankData mapping(byte input); // return target ID
FrankData::frankData mappingPush(byte input); // return target ID
