#ifndef MF_DATA_H_
#define MF_DATA_H_

#include <Arduino.h>


#define LENGTH 64
#define PAGES 8
#define NOTERANGE 88

//Sequence struct holding all values for a sequence
typedef struct{
  byte note[LENGTH];
  byte gate[LENGTH];
  byte gateLength[LENGTH];
  byte activePages;
  byte tuning;
} structSequence;

//Settings struct for all settings
typedef struct{
  byte midiType = 1;            //active MidiDevice (usb = 1, din = 0)
  byte nbPages = 2;
  byte direction = 0;
} structSettings;

typedef struct{
  byte clock = 4;
  byte bpm = 255;
  byte activeSeq = 1;
} structStatus;



class status{
public:
  //Sequence

  void setBPM(byte bpm){stat.bpm = bpm;}  //return MidiType

  byte getActiveSeq(){return stat.activeSeq;}
  void setActiveSeq(byte activeSeq){stat.activeSeq = activeSeq;}


  byte getActivePage(){return  (stat.clock / 8);}


  byte getClock(){return stat.clock;}  //return MidiType
  byte getBPM(){return stat.bpm;}  //return MidiType

private:
  structStatus stat;
};


class setting{
public:
  //Sequence
  byte getDirection(){return config.direction;}
  byte getMidiType(){return config.midiType;}  //return MidiType
  byte getNumberPages(){return config.nbPages;}  //return MidiType


  void setSequence(structSettings *copySet);   //set all sequence values at once
  structSettings* getSettings();               //return the sequence struct pointer

private:
  structSettings config;
};

//Sequence class
class seq
{
public:
  void init(byte note = 0, byte gate = 1, byte gateLength = 50, byte activePages= 2, byte tuning = 255); //init sequence to default values

//Pages
  byte getActivePages();            //return number of active pages
  void setActivePages(byte page);   //set number of active pages

//Note
  byte getNote(byte index);             //return note value
  void setNote(byte index, byte note);  //set note value
  void setNotes( byte note);            //set all note values

  byte increaseNote(byte index);  //increase note value and return new note, function take care of tuning
  byte decreaseNote(byte index);  //decrease note value and return new note, function take care of tuning

  byte changeNote(byte index, int change);  //change note value and return new note
  void changeNotes(int change);             //change all note values

//Gate
  byte getGate(byte index);             //return gate value
  void setGate(byte index, byte gate);  //set gate value
  byte toggleGate(byte index);          //toggle gate and return new status;

//GateLength
  void setGateLength(byte index, byte gateLength);  //set gate length
  byte getGateLength(byte index);                   //return gate length
  byte changeGateLength(byte index, int change);    //increase note value and return new note

  void setGateLengths(byte gateLength);             //set all gates at once
  void changeGateLengths(int change);               //increase note value and return new note

//Sequence
  void setSequence(structSequence *copySeq);   //set all sequence values at once
  structSequence* getSequence();               //return the sequence struct pointer

  int getSequenceSize();                        //return the struct size

  //Tuning
  void setTuning(byte tuning);
  byte getTuning();

private:

  //utility
  byte testByte(byte value, byte minimum, byte maximum);  //test byte range and return valid byte
  byte increaseByte(byte value, byte maximum);  //increase byte
  byte decreaseByte(byte value, byte minimum);  //decrease byte
  byte changeByte(byte value, int change ,byte minimum = 0, byte maximum = 255);  //change byte

  //Sequence
  structSequence sequence;

};

#endif
