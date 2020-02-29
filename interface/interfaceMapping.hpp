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



static byte mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE} ;               //NOTE Mapping  Rotary
static byte mappingSeqNotePush[] = {TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE} ;   //NOTE Mapping Push
static byte mappingSeqNoteConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;         //NOTE Config Mapping

static byte mappingSeqGate[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE} ;             //GATE Mapping Rotary
static byte mappingSeqGatePush[] = {TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE} ;   //NOTE Mapping Push
static byte mappingSeqGateConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;         //GATE Config Mapping

static byte mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV} ;                                 //CV Mapping
static byte mappingSeqCVPush[] = {TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE, TGATE} ;     //NOTE Mapping
static byte mappingSeqCVConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;           //CV Config Mapping

static byte mappingLive[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE} ;                  //Live Mapping
static byte mappingLiveConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;            //Live Mapping

static byte mappingMenu[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;                  //Routing Menu Mapping

/*/////ROUTING//////



*/


byte mapping(byte input); //return target ID
byte mappingPush(byte input); //return target ID
