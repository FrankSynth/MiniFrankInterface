#include "interfaceMapping.hpp"

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

////Sequencer////
FrankData::frankData mappingSeqConfig[] = {FrankData::stepSpeed,
                                           FrankData::nbPages,
                                           FrankData::seqResetNotes,
                                           FrankData::seqResetGates,
                                           FrankData::outputRatchet,
                                           FrankData::direction,
                                           FrankData::seqTuning,
                                           FrankData::seqGateLengthOffset,
                                           SUBSCREEN,
                                           STEP}; // CV Config Mapping
FrankData::frankData mappingSeqConfigPush[] = {
    NONE, NONE, FrankData::seqResetNotes, FrankData::seqResetGates, NONE, NONE, NONE, NONE,
    MENU, PLAY}; // GATE Mapping Push

FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE,      NOTE,
                                         NOTE, NOTE, NOTE, SUBSCREEN, STEP}; // NOTE Mapping  Rotary
FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE,
                                             GATE, GATE, GATE, MENU, PLAY}; // NOTE Mapping Push

FrankData::frankData mappingSeqGate[] = {GATELENGTH, GATELENGTH, GATELENGTH,
                                         GATELENGTH, GATELENGTH, GATELENGTH,
                                         GATELENGTH, GATELENGTH, SUBSCREEN /*Global Gate, funktion fehlt noch */,
                                         STEP}; // GATE Mapping Rotary
FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE,
                                             GATE, GATE, GATE, MENU, PLAY}; // GATE Mapping Push

FrankData::frankData mappingSeqCV[] = {CV,  CV, CV, CV, CV, CV, CV, CV, SUBSCREEN /*Global CV, funktion fehlt noch */,
                                       STEP}; // CV Mapping
FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE,
                                           GATE, GATE, GATE, MENU, PLAY}; // CV Mapping Push

FrankData::frankData mappingLive[] = {
    FrankData::outputArp, FrankData::outputLiveMode, NONE, NONE, NONE, NONE, NONE, FrankData::screenRouting, NONE, NONE}; // Live Mapping
FrankData::frankData mappingLivePush[] = {
    FrankData::outputArp, NONE, NONE, NONE, NONE, NONE, NONE, FrankData::screenRouting, MENU, PLAY}; // Live Mapping

FrankData::frankData mappingLiveArp[] = {
    FrankData::outputArp,        FrankData::outputArpOctave, NONE, NONE, FrankData::outputArpMode, FrankData::stepSpeed,
    FrankData::outputRatchet, FrankData::screenRouting,   NONE, NONE}; // Live Arp Mapping
FrankData::frankData mappingLiveArpPush[] = {FrankData::outputArp,     NONE, NONE, NONE, NONE, NONE, NONE,
                                             FrankData::screenRouting, MENU, PLAY}; // Live Arp Mapping
                                                                                    // Push

////MENU////

FrankData::frankData mappingMenu[] = {FrankData::load,
                                      NONE,
                                      FrankData::outputSource,
                                      FrankData::screenCal /*CalibrationMenu*/,
                                      FrankData::save,
                                      FrankData::pulseLength /*CLKLength*/,
                                      FrankData::displayBrightness,
                                      FrankData::screenRouting,
                                      NONE,
                                      NONE}; // Menu Mapping
FrankData::frankData mappingMenuPush[] = {FrankData::load,
                                          NONE,
                                          NONE,
                                          FrankData::screenCal /*CalibrationMenu*/,
                                          FrankData::save,
                                          NONE,
                                          NONE,
                                          FrankData::screenRouting,
                                          MENU,
                                          PLAY}; // Menu Mapping

FrankData::frankData mappingMenuCal[] = {FrankData::midiSource, NONE, NONE, NONE, NONE, NONE, NONE, NONE}; // Routing Menu Mapping
FrankData::frankData mappingMenuCalPush[] = {NONE, NONE, NONE, NONE, NONE,
                                             NONE, NONE, NONE, MENU, PLAY}; // Routing Menu Mapping

FrankData::frankData mappingMenuMidi[] = {FrankData::midiSource,
                                          FrankData::outputChannel,
                                          FrankData::outputCc,
                                          NONE,
                                          FrankData::outputClock,
                                          NONE,
                                          NONE,
                                          FrankData::screenRouting,
                                          NONE,
                                          NONE};                                                                               // Routing Menu Mapping
FrankData::frankData mappingMenuMidiPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, FrankData::screenRouting, MENU, PLAY}; // Routing Menu Mapping

FrankData::frankData mapping(byte input) {

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenu[input];

        // fehlen noch die menus, cal und Routing

        // Live Mode
    }
    else if (DATAOBJ.get(FrankData::screenRouting) == 1) {
        return mappingMenuMidi[input];
    }
    else if (DATAOBJ.get(FrankData::screenCal) == 1) {
        return mappingMenuCal[input];
    }
    else {
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if (DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
                return mappingLive[input];
            }
            else {
                return mappingLiveArp[input];
            }
        }

        // Seq Mode
        else {
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfig[input]; // return mapping from Note Screen
                return mappingSeqNote[input];
            case 1:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfig[input]; // return mapping from Gate Screen
                return mappingSeqGate[input];
            case 2:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfig[input]; // return mapping from CV Screen
                return mappingSeqCV[input];
            default: return FrankData::none;
            }
        }
    }
    return FrankData::none;
}

FrankData::frankData mappingPush(byte input) {

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenuPush[input];

        // fehlen noch die menus, cal und Routing

        // Live Mode
    }
    else if (DATAOBJ.get(FrankData::screenRouting) == 1) {
        return mappingMenuMidiPush[input];
    }
    else if (DATAOBJ.get(FrankData::screenCal) == 1) {
        return mappingMenuCalPush[input];
    }
    else {
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if (DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
                return mappingLivePush[input];
            }
            else {

                return mappingLiveArpPush[input];
            }
        }

        // Seq Mode
        else {
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfigPush[input]; // return mapping from Note Screen
                return mappingSeqNotePush[input];
            case 1:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfigPush[input]; // return mapping from Gate Screen
                return mappingSeqGatePush[input];
            case 2:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqConfigPush[input]; // return mapping from CV Screen
                return mappingSeqCVPush[input];
            default: return FrankData::none;
            }
        }
    }
    return FrankData::none;
}
