#include "interfaceIn.h"


void controls::encode(byte message){

    byte id = (byte)(B00001111 & message); //extract Message from ID

    if(message & B10000000){    //is a switch?
      push(id, message & B00010000);

    }
    else{ //seems to be an encoder
      rotate(id, message & B00010000);
    }

}

void controls::rotate(byte id ,byte dir){ //rotate message
  byte activePage = stat->getActivePage();
  byte offset = activePage * STEPPERPAGE;
  seq *activeSeq;

  if(stat->getActiveMenu() == 0){ //Note menu
    if(id < 8){//noteGate encoder
      if(dir){
        activeSeq->increaseNote(offset + id) ;
      }
      else{
        activeSeq->decreaseNote(offset + id) ;
      }
    }
    else{ //menu encoder
    }
  }
  else if (stat->getActiveMenu() == 1){  //Gate length menu
    if(id < 8){ //noteGate encoder
      if(dir){
        activeSeq->changeGateLength(offset + id, 5) ;
      }
      else{
        activeSeq->changeGateLength(offset + id, -5) ;
      }
    }
    else{ //menu encoder
    }
  }
}

void controls::push( byte id,byte push){ //switch message
  byte activePage = stat->getActivePage();
  byte offset = activePage * STEPPERPAGE;
  seq *activeSeq;

  activeSeq = getActiveSeqPointer();

  if(stat->getActiveMenu() == 0){ //Note menu
    if(id < 8){//noteGate encoder
      if(push){
        activeSeq->toggleGate(offset + id) ;
      }
    }
    else{ //menu encoder
    }
  }
  else if (stat->getActiveMenu() == 1){  //Gate length menu
    if(id < 8){ //noteGate encoder
      if(push){
        activeSeq->toggleGate(offset + id) ;
      }
    }
    else{ //menu encoder
    }
  }
}


//utility
seq* controls::getActiveSeqPointer(){
      if( stat->getActiveSeq() == 0){
    return seq1;
  }
  else if(stat->getActiveSeq() == 1){
    return seq2;
  }
  return NULL; //notValid

}
