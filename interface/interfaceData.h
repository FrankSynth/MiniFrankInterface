#ifndef MF_DATA_H_
#define MF_DATA_H_

#include <Arduino.h>


#define LENGTH 64
#define PAGES 8
#define NOTERANGE 88

//Sequence struct holding all value for an sequence
typedef struct{
  byte note[LENGTH];
  byte gate[LENGTH];
  byte gateLength[LENGTH];
  byte activePages;
} structSequence;

//Settings struct for all settings
typedef struct{
  byte midi;    //active MidiDevice (usb = 1, din = 0)
  byte defaultPage;

} structSettings;


class set{
public:
  //void init()

private:
  structSettings settings;
};


//Sequence class
class seq
{
public:
  void init(byte note = 0, byte gate = 1, byte gateLength = 50, byte activePages= 2); //init sequence to default values

//Pages
  byte getActivePages();            //return number of active pages
  void setActivePages(byte page);   //set number of active pages

//Note
  byte getNote(byte index);             //return note value
  void setNote(byte index, byte note);  //set note value
  void setNotes( byte note);            //set all note values

  byte increaseNote(byte index);  //increase note value and return new note
  byte decreaseNote(byte index);  //decrease  note value and return new note

  byte changeNote(byte index, int change);  //change note value and return new note
  void changeNotes(int change);  //change all note values



//Gate
  byte getGate(byte index);             //return gate value
  void setGate(byte index, byte gate);  //set gate value
  byte toggleGate(byte index);          //toggle gate and return new status;

//GateLength
  void setGateLength(byte index, byte gateLength);  //set gate length
  byte getGateLength(byte index);                   //return gate length
  byte changeGateLength(byte index, int change);  //increase note value and return new note

  void setGateLengths(byte gateLength);             //set all gates at once
  void changeGateLengths(int change);  //increase note value and return new note

//Sequence
  void setSequence(structSequence *copySeq);   //set all sequence values at once
  structSequence* getSequence();               //return the sequence struct pointer

  int getSequenceSize();                      //return the struct size

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
