#ifndef MF_IN_H_
#define MF_IN_H_

#include <Arduino.h>
#include "interfaceData.h"


//Controls object for the incoming UART messages
class controls
{
  public:
    void encode(byte message);  //encode message
    void rotate(byte id ,byte dir); // is a rotate message
    void push( byte id,byte push);  //is a switch message


    void setPointer(seq *seq1Pointer, seq *seq2Pointer, status *statPointer){ //set pointer
      seq1 = seq1Pointer;
      seq2 = seq2Pointer;
      stat = statPointer;
    }

private:
    seq *seq1;
    seq *seq2;
    status *stat;

};

#endif
