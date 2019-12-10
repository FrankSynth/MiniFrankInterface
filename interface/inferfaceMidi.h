//MIDI input library
//collecting data from USB and din
//Ring Buffer for multiple inputs

#ifndef _MF_MIDI_H_
#define _MF_MIDI_H_

#define KEYS 10
#define BAUD 0

#include <Arduino.h>
#include "interfaceData.h"
#include <midi.h>


class key
{
  puplic:
        key(byte note,byte gate,byte cv1,byte cv2)    //constructor
        :note(note),
        gate(gate),
        cv1(cv1),
        cv2(cv2)
        {}

  private:
    byte note;
    byte gate;
    byte cv1;
    byte cv2;
};

class midi
{
  puplic:
        midi(*status configPointer); //initialize midi connections

        byte updateMidi(){
          if(stat->midiType() == 0){ //USB enabled
            return midiUSB(); //return true if updated
          }
          else if(stat->midiType() == 1){ //DIN enabled
            return midiDIN(); //return true if updated
          }
          return 0;
        }

        byte midiUSB();
        byte midiDIN();


private:

  //midi struct
  byte note[KEYS] = {0}; //notes
  byte gate[KEYS] = {0}; //gate per note

  //RingBuffer
  byte head = 0;
  byte tail = 0;
  status *stat;

};


#endif
