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
FrankData::frankData mappingSeqConfig[] = {FrankData::stepSpeed, FrankData::direction,  NONE,FrankData::seqGateLengthOffset, FrankData::nbPages, NONE, NONE, NONE, SUBSCREEN, STEP};       // CV Config Mapping
FrankData::frankData mappingSeqConfigPush[] = {NONE, FrankData::direction, NONE, NONE, NONE, NONE, NONE, NONE, MENU, PLAY};         // GATE Mapping Push

FrankData::frankData mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, SUBSCREEN, STEP};             // NOTE Mapping  Rotary
FrankData::frankData mappingSeqNotePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, MENU, PLAY};         // NOTE Mapping Push

FrankData::frankData mappingSeqGate[] = {GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, GATELENGTH, SUBSCREEN  /*Global Gate, funktion fehlt noch */, STEP  };             // GATE Mapping Rotary
FrankData::frankData mappingSeqGatePush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, MENU, PLAY};         // GATE Mapping Push

FrankData::frankData mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV, SUBSCREEN  /*Global CV, funktion fehlt noch */ , STEP };                             // CV Mapping
FrankData::frankData mappingSeqCVPush[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE, MENU, PLAY};         // CV Mapping Push


FrankData::frankData mappingLive[] = {FrankData::outputArp, FrankData::outputLiveMode, NONE, NONE, NONE, NONE, NONE, NONE, NONE, STEP};       // Live Mapping
FrankData::frankData mappingLivePush[] = {FrankData::outputArp, NONE, NONE, NONE, NONE, NONE, NONE, NONE, MENU, PLAY};       // Live Mapping


FrankData::frankData mappingLiveArp[] = {FrankData::outputArp, FrankData::outputArpOctave, NONE, NONE, FrankData::outputArpRatchet, FrankData::outputClock, NONE, NONE, NONE, STEP};       // Live Arp Mapping
FrankData::frankData mappingLiveArpPush[] = {FrankData::outputArp, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, MENU, PLAY};       // Live Arp Mapping Push

////MENU////

FrankData::frankData mappingMenu[] = {FrankData::load /*LOAD*/, FrankData::save /*SAVE*/, FrankData::screenRouting/*RoutingMenu*/, FrankData::screenCal /*CalibrationMenu*/,FrankData::outputClock /*CLK1*/, FrankData::outputClock /*CLK2*/, FrankData::pulseLength /*CLKLength*/, FrankData::displayBrightness, NONE, STEP};  // Menu Mapping
FrankData::frankData mappingMenuPush[] = {NONE /*LOAD*/, NONE /*SAVE*/, FrankData::screenRouting,  FrankData::screenCal /*CalibrationMenu*/,NONE /*CLK1*/, NONE /*CLK2*/, NONE /*CLKLength*/, NONE, MENU, PLAY};  // Menu Mapping

FrankData::frankData mappingMenuCal[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, STEP}; // Routing Menu Mapping
FrankData::frankData mappingMenuCalPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, MENU, PLAY}; // Routing Menu Mapping

FrankData::frankData mappingMenuRouting[] = {FrankData::outputSource, FrankData::midiSource, FrankData::outputChannel, FrankData::outputCc,FrankData::outputSource, FrankData::midiSource, FrankData::outputChannel, FrankData::outputCc, SUBSCREEN, STEP}; // Routing Menu Mapping
FrankData::frankData mappingMenuRoutingPush[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, MENU, PLAY}; // Routing Menu Mapping

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
    else if(DATAOBJ.get(FrankData::screenRouting) == 1){
        return mappingMenuRouting[input];

    }
    else if(DATAOBJ.get(FrankData::screenCal) == 1){
        return mappingMenuCal[input];

    }
    else {  
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if(DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0){
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingMenuRouting[input];
                return mappingLive[input];
            }
            else{

                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingMenuRouting[input];
                return mappingLiveArp[input];       
            }
         
        
        }
    

        // Seq Mode
        else{
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from Note Screen
                return mappingSeqNote[input];
            case 1:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from Gate Screen
                return mappingSeqGate[input];
            case 2:
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingSeqConfig[input]; // return mapping from CV Screen
                return mappingSeqCV[input];
            default: return FrankData::none;
            }
        }
        
    }
    return FrankData::none;
}

FrankData::frankData mappingPush(byte input) {
            Serial.println("MappingPush");

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenuPush[input];

        //fehlen noch die menus, cal und Routing

        // Live Mode
    } 
        else if(DATAOBJ.get(FrankData::screenRouting) == 1){
        return mappingMenuRoutingPush[input];

    }
    else if(DATAOBJ.get(FrankData::screenCal) == 1){
        return mappingMenuCalPush[input];

    }
    else {
        if (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            if(DATAOBJ.get(FrankData::outputArp, DATAOBJ.get(FrankData::screenOutputChannel)) == 0){
                if (DATAOBJ.get(FrankData::screenConfig)) return mappingMenuRoutingPush[input];
                return mappingLivePush[input];
            }   
            else{
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfigPush[input];
                return mappingMenuRoutingPush[input];       
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
            default: return FrankData::none;

            }
        }
        
    }
    return FrankData::none;

}
