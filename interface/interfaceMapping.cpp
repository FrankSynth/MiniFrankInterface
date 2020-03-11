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

/*

(0)     (1)      (2)      (3)    |  (8)

(4)     (5)      (6)      (7)   |  (9)

0-7 Drehregler werden gemapped auf den jeweiligen Modus

8 + 9 sind standardmäßig eingestellt:

LIVE:
8-> Push => Toggle Play Stop
    Rotate => ??
9-> Push => Toggle Menu
    Rotate => ??

SEQ:
8-> Push => Toggle Play Stop
    Rotate => Step Forward / Backward
9-> Push => Toggle Menu
    Rotate => Global Gate Increase/Decrease
*/

FrankData::frankData mapping(byte input) {

    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenu[input];

        // Live Mode
    } else {
        if (DATAOBJ.get(FrankData::outputLiveMode, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingLiveConfig[input];
                return mappingLive[input];
            }
        }

        // Seq Mode
        else if (DATAOBJ.get(FrankData::outputLiveMode, DATAOBJ.get(FrankData::screenOutputChannel)) == 1) {
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqNoteConfig[input]; // return mapping from Note Screen
                return mappingSeqNote[input];
            case 1:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqGateConfig[input]; // return mapping from Gate Screen
                return mappingSeqGate[input];
            case 2:
                if (DATAOBJ.get(FrankData::screenConfig))
                    return mappingSeqCVConfig[input]; // return mapping from CV Screen
                return mappingSeqNote[input];
            }
        }
    }
    return FrankData::none;
}

FrankData::frankData mappingPush(byte input) {
    // Main Menu is enabled
    if (DATAOBJ.get(FrankData::screenMainMenu) == 1) {
        return mappingMenu[input];

        // Main Menu is disabled
    } else {
        // Live Modus
        if (DATAOBJ.get(FrankData::outputLiveMode, DATAOBJ.get(FrankData::screenOutputChannel)) == 0) {
        }

        // Seq Modus
        else if (DATAOBJ.get(FrankData::outputLiveMode, DATAOBJ.get(FrankData::screenOutputChannel)) == 1) {
            switch (DATAOBJ.get(FrankData::screenSubScreen)) {
            case 0: return mappingSeqNotePush[input];
            case 1: return mappingSeqGatePush[input];
            case 2: return mappingSeqCVPush[input];
            }
        }
    }
    return FrankData::none;
}




    if (id == 8) {                        // Control Encoder
        DATAOBJ.toggle(FrankData::screenConfig);
    } else if (id == 9) { /                 / Step Encoder
        DATAOBJ.toggle(FrankData::play);
    } else { // Note Encoder