
 #include "interfaceMapping.hpp"

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
/*
static byte mappingSeqNote[] = {NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE, NOTE} ;           //NOTE Mapping
static byte mappingSeqNoteConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;     //NOTE Config Mapping

static byte mappingSeqGate[] = {GATE, GATE, GATE, GATE, GATE, GATE, GATE, GATE} ;          //GATE Mapping
static byte mappingSeqGateConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;    //GATE Config Mapping

static byte mappingSeqCV[] = {CV, CV, CV, CV, CV, CV, CV, CV} ;                            //CV Mapping
static byte mappingSeqCVConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;    //CV Config Mapping

static byte mappingLive[] = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE} ;             //Live Mapping
static byte mappingLiveConfig[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;       //Live Mapping

static byte mappingMenu[] = {CONF, CONF, CONF, CONF, CONF, CONF, CONF, CONF} ;             //Routing Menu Mapping

*/
byte mapping(byte input){

  if(DATAOBJ.getMainMenuEnabled() == 1){ //Main Menu is enabled
    return mappingMenu[input];

  }
  else{
    if(DATAOBJ.getOutputMode(DATAOBJ.getScreenChannel()) == 0){  //Sequencer Modus
      switch(DATAOBJ.getSubScreen()){
        case 0:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqNoteConfig[input];    //return mapping from Note Screen
                    return mappingSeqNote[input];
        case 1:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqGateConfig[input];    //return mapping from Gate Screen
                    return mappingSeqGate[input];
        case 2:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqCVConfig[input];       //return mapping from CV Screen
                    return mappingSeqNote[input];
      }
    }

    else if(DATAOBJ.getOutputMode(DATAOBJ.getScreenChannel()) == 1){ //Live Modus
      switch(DATAOBJ.getSubScreen()){
        case 0:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqNoteConfig[input];    //return mapping from Note Screen
                    return mappingSeqNote[input];
        case 1:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqGateConfig[input];    //return mapping from Gate Screen
                    return mappingSeqGate[input];
        case 2:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingSeqCVConfig[input];       //return mapping from CV Screen
                    return mappingSeqNote[input];
      }
    }
  }
  return NONE;

}
