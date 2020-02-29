
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
        case 0:     if(DATAOBJ.getScreenConfig(DATAOBJ.getScreenChannel())) return mappingLiveConfig[input];
                    return mappingLive[input];
      }
    }
  }
  return NONE;

}


byte mappingPush(byte input){
  if(DATAOBJ.getMainMenuEnabled() == 1){ //Main Menu is enabled
    return mappingMenu[input];

  }
  else{
    if(DATAOBJ.getOutputMode(DATAOBJ.getScreenChannel()) == 0){  //Sequencer Modus
      switch(DATAOBJ.getSubScreen()){
          case 0:  return mappingSeqNotePush[input];
          case 1:  return mappingSeqGatePush[input];
          case 2:  return mappingSeqCVPush[input];
      }
    }

    else if(DATAOBJ.getOutputMode(DATAOBJ.getScreenChannel()) == 1){ //Live Modus
    }
  }
  return NONE;

}
