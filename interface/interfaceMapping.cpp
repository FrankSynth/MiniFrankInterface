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
FrankData::frankData mappingSeqConfig[] = {FrankData::direction,
                                           FrankData::nbPages,
                                           FrankData::save,
                                           FrankData::load,
                                           FrankData::outputRatchet,
                                           FrankData::stepSpeed,
                                           FrankData::seqTuning,
                                           FrankData::seqGateLengthOffset,
                                           SUBSCREEN,
                                           STEP}; // CV Config Mapping
FrankData::frankData mappingSeqConfigPush[] = {FrankData::direction,     FrankData::nbPages,
                                               FrankData::save,          FrankData::load,
                                               FrankData::outputRatchet, FrankData::stepSpeed,
                                               FrankData::seqTuning,     FrankData::seqGateLengthOffset,
                                               FrankData::screenRouting, PLAY}; // GATE Mapping Push

FrankData::frankData mappingSeqConfig2[] = {FrankData::resetStepCounters,
                                            FrankData::seqResetCC,
                                            FrankData::seqResetNotes,
                                            FrankData::seqResetGates,
                                            FrankData::copySeq,
                                            NONE,
                                            NONE,
                                            FrankData::seqResetGateLengths,
                                            SUBSCREEN,
                                            STEP}; // CV Config Mapping
FrankData::frankData mappingSeqConfig2Push[] = {FrankData::resetStepCounters,
                                                FrankData::seqResetCC,
                                                FrankData::seqResetNotes,
                                                FrankData::seqResetGates,
                                                FrankData::copySeq,
                                                NONE,
                                                NONE,
                                                FrankData::seqResetGateLengths,
                                                FrankData::screenRouting,
                                                PLAY}; // GATE Mapping Push

FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, SUBSCREEN, STEP};                    // NOTE Mapping  Rotary
FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, FrankData::screenRouting, PLAY}; // NOTE Mapping Push

FrankData::frankData mappingSeqGate[] = {
    GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, SUBSCREEN /*Global Gate, funktion fehlt noch */,
    STEP};                                                                                                                    // GATE Mapping Rotary
FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, FrankData::screenRouting, PLAY}; // GATE Mapping Push

FrankData::frankData mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV, SUBSCREEN /*Global CV, funktion fehlt noch */, STEP}; // CV Mapping
FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, FrankData::screenRouting, PLAY};  // CV Mapping Push

FrankData::frankData mappingLive[] = {
    FrankData::outputArp, FrankData::outputLiveMode, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE}; // Live Mapping

FrankData::frankData mappingLivePush[] = {FrankData::outputArp,     NONE, NONE, NONE, NONE, NONE, NONE, NONE,
                                          FrankData::screenRouting, PLAY}; // Live Mapping

FrankData::frankData mappingLiveArp[] = {FrankData::outputArp, FrankData::outputArpOctave, NONE, NONE, FrankData::outputArpMode,
                                         FrankData::stepSpeed, FrankData::outputRatchet,   NONE, NONE, NONE}; // Live Arp Mapping
FrankData::frankData mappingLiveArpPush[] = {FrankData::outputArp,     NONE, NONE, NONE, NONE, NONE, NONE, NONE,
                                             FrankData::screenRouting, PLAY}; // Live Arp Mapping
                                                                              // Push

////MENU////
// FrankData::load FrankData::save

FrankData::frankData mappingMenu[] = {FrankData::midiSource,
                                      NONE,
                                      FrankData::screenCalCv /*CalibrationMenu*/,
                                      FrankData::screenCalNote /*CalibrationMenu*/,
                                      NONE,
                                      NONE,
                                      FrankData::pulseLength /*CLKLength*/,
                                      FrankData::displayBrightness,
                                      NONE,
                                      NONE}; // Menu Mapping
FrankData::frankData mappingMenuPush[] = {NONE,
                                          NONE,
                                          FrankData::screenCalCv /*CalibrationMenu*/,
                                          FrankData::screenCalNote /*CalibrationMenu*/,
                                          NONE,
                                          NONE,
                                          NONE,
                                          NONE,
                                          FrankData::screenRouting,
                                          PLAY}; // Menu Mapping

FrankData::frankData mappingMenuRouting[] = {FrankData::outputSource,
                                             FrankData::outputChannel,
                                             FrankData::outputCc,
                                             NONE,
                                             FrankData::outputClock,
                                             FrankData::stepSpeed,
                                             NONE,
                                             MENU,
                                             NONE,
                                             NONE}; // Routing Menu
                                                    // Mapping
FrankData::frankData mappingMenuRoutingPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, MENU, FrankData::screenRouting,
                                                 PLAY}; // Routing Menu Mapping

FrankData::frankData mappingMenuCalNote[] = {FrankData::liveCalNote, NONE, NONE, NONE, FrankData::noteCalOffset, FrankData::noteScaleOffset, NONE,
                                             FrankData::saveCal,     NONE, NONE}; // Routing Menu Mapping
FrankData::frankData mappingMenuCalNotePush[] = {
    NONE, NONE, NONE, NONE, FrankData::noteCalOffset, FrankData::noteScaleOffset, NONE, FrankData::saveCal, FrankData::screenRouting,
    PLAY}; // Routing Menu Mapping

FrankData::frankData mappingMenuCalCV[] = {FrankData::cvCalLower,
                                           FrankData::cvCalUpper,
                                           FrankData::liveCalCv,
                                           FrankData::outputCc,
                                           FrankData::cvPitchbendCalLower,
                                           FrankData::cvPitchbendCalUpper,
                                           FrankData::cvCalOffset,
                                           FrankData::saveCal,
                                           NONE,
                                           NONE}; // Routing Menu Mapping
FrankData::frankData mappingMenuCalCVPush[] = {
    FrankData::cvCalLower,  FrankData::cvCalUpper, FrankData::liveCalCv,     NONE, FrankData::cvPitchbendCalLower, FrankData::cvPitchbendCalUpper,
    FrankData::cvCalOffset, FrankData::saveCal,    FrankData::screenRouting, PLAY}; // Routing Menu Mapping

FrankData::frankData mapping(byte input) {

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenu[input];

        // fehlen noch die menus, cal und Routing

        // Live Mode
    }
    else if (DATAOBJ.get(FrankData::screenRouting) == 1) {
        return mappingMenuRouting[input];
    }
    else if (DATAOBJ.get(FrankData::screenCalNote) == 1) {
        return mappingMenuCalNote[input];
    }
    else if (DATAOBJ.get(FrankData::screenCalCv) == 1) {
        return mappingMenuCalCV[input];
    }
    else {
        if (DATAOBJ.get(FrankData::outputSource, CHANNEL) == 0) {
            if (DATAOBJ.get(FrankData::outputArp, CHANNEL) == 0) {
                return mappingLive[input];
            }
            else {
                return mappingLiveArp[input];
            }
        }

        // Seq Mode
        else {
            if (!DATAOBJ.get(FrankData::screenConfig)) {
                switch (DATAOBJ.get(FrankData::screenSubScreen)) {
                    case 0: return mappingSeqNote[input];
                    case 1: return mappingSeqGate[input];
                    case 2: return mappingSeqCV[input];
                    default: return FrankData::none;
                }
            }
            else {
                switch (DATAOBJ.get(FrankData::screenSubScreen)) {
                    case 0: return mappingSeqConfig2[input];
                    case 1: return mappingSeqConfig[input];
                    default: return FrankData::none;
                }
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
        return mappingMenuRoutingPush[input];
    }
    else if (DATAOBJ.get(FrankData::screenCalNote) == 1) {
        return mappingMenuCalNotePush[input];
    }
    else if (DATAOBJ.get(FrankData::screenCalCv) == 1) {
        return mappingMenuCalCVPush[input];
    }
    else {
        if (DATAOBJ.get(FrankData::outputSource, CHANNEL) == 0) {
            if (DATAOBJ.get(FrankData::outputArp, CHANNEL) == 0) {
                return mappingLivePush[input];
            }
            else {

                return mappingLiveArpPush[input];
            }
        }

        // Seq Mode
        else {
            if (!DATAOBJ.get(FrankData::screenConfig)) {

                switch (DATAOBJ.get(FrankData::screenSubScreen)) {
                    case 0: return mappingSeqNotePush[input];
                    case 1: return mappingSeqGatePush[input];
                    case 2: return mappingSeqCVPush[input];
                    default: return FrankData::none;
                }
            }
            else {

                switch (DATAOBJ.get(FrankData::screenSubScreen)) {
                    case 0: return mappingSeqConfig2Push[input];
                    case 1: return mappingSeqConfigPush[input];
                }
            }
        }
    }
    return FrankData::none;
}
