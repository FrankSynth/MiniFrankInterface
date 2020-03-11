#include "interfaceMapping.hpp"

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


////Sequencer////
FrankData::frankData mappingSeqConfig[] = {FrankData::outputClock, FrankData::direction, NONE, NONE, NONE, NONE, NONE, NONE, STEP, NONE};       // CV Config Mapping
FrankData::frankData mappingSeqConfigPush[] = {NONE, FrankData::direction, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG};         // GATE Mapping Push

FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, STEP, NONE};             // NOTE Mapping  Rotary
FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // NOTE Mapping Push

FrankData::frankData mappingSeqGate[] = {GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, STEP, NONE  /*Global Gate, funktion fehlt noch */  };             // GATE Mapping Rotary
FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // GATE Mapping Push

FrankData::frankData mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV, STEP, NONE  /*Global CV, funktion fehlt noch */  };                             // CV Mapping
FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, PLAY, CONFIG};         // CV Mapping Push


FrankData::frankData mappingLive[] = {FrankData::outputArp, FrankData::outputLiveMode, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};       // Live Mapping
FrankData::frankData mappingLivePush[] = {FrankData::outputArp, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};       // Live Mapping


FrankData::frankData mappingLiveArp[] = {FrankData::outputArp, NONE /*octaves*/, NONE, NONE, NONE /*ratchet*/, FrankData::outputClock, NONE, NONE, STEP, NONE};       // Live Arp Mapping
FrankData::frankData mappingLiveArpPush[] = {FrankData::outputArp, NONE /*octaves*/, NONE, NONE, NONE /*ratchet*/, FrankData::outputClock, NONE, NONE, STEP, NONE};       // Live Arp Mapping Push

////MENU////

FrankData::frankData mappingMenu[] = {NONE /*LOAD*/, NONE /*SAVE*/, NONE /*RoutingMenu*/, NONE /*CalibrationMenu*/,NONE /*CLK1*/, NONE /*CLK2*/, NONE /*CLKLength*/, FrankData::displayBrightness, STEP, NONE};  // Menu Mapping
FrankData::frankData mappingMenuPush[] = {NONE /*LOAD*/, NONE /*SAVE*/, NONE /*RoutingMenu*/, NONE /*CalibrationMenu*/,NONE /*CLK1*/, NONE /*CLK2*/, NONE /*CLKLength*/, FrankData::displayBrightness, STEP, NONE};  // Menu Mapping

FrankData::frankData mappingMenuCal[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, STEP, NONE}; // Routing Menu Mapping
FrankData::frankData mappingMenuCalPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG}; // Routing Menu Mapping

FrankData::frankData mappingMenuRouting[] = {FrankData::liveMod, FrankData::outputSource, FrankData::outputChannel, FrankData::outputCc,FrankData::liveMod, FrankData::outputSource, FrankData::outputChannel, FrankData::outputCc}; // Routing Menu Mapping
FrankData::frankData mappingMenuRoutingPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, PLAY, CONFIG}; // Routing Menu Mapping

////LIVE////
FrankData::frankData *mappingLiveConfig = mappingMenuRouting; // Live Mapping
FrankData::frankData *mappingLiveConfigPush = mappingMenuRoutingPush; // Live Mapping,


FrankData::frankData mapping(byte input) {
    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenu[input];

        //fehlen noch die menus, cal und Routing

        // Live Mode
    } 
    else {
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if(DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0){
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfig[input];
                return mappingLive[input];
            }
            else{
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfig[input];
                return mappingLiveArp[input];       
            }
         
        }
    

        // Seq Mode
        else{
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from Note Screen

                PRINTLN("Mapping: NOTE");
                return mappingSeqNote[input];
            case 1:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from Gate Screen
                return mappingSeqGate[input];
            case 2:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from CV Screen
                return mappingSeqCV[input];
            }
        }
        
    }
    return FrankData::none;
}

FrankData::frankData mappingPush(byte input) {

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenuPush[input];

        //fehlen noch die menus, cal und Routing

        // Live Mode
    } 
    else {
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if(DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0){
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfigPush[input];
                return mappingLivePush[input];
            }
            else{
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfigPush[input];
                return mappingLiveArpPush[input];       
            }
         
        }
    

        // Seq Mode
        else{
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
            }
        }
        
    }
    return FrankData::none;

}
