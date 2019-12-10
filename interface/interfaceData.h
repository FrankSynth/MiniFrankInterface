#ifndef MF_DATA_H_
#define MF_DATA_H_

#include <Arduino.h>


#define LENGTH 64
#define PAGES 8
#define NOTERANGE 88
#define STEPPERPAGE 8

//Sequence struct holding all values for a sequence
typedef struct{
  byte note[LENGTH];
  byte gate[LENGTH];
  byte gateLength[LENGTH];
  byte tuning;
} structSequence;

//Settings struct for all settings
typedef struct{
  byte midiType = 1;              // active MidiDevice (usb -> 1, din -> 0)
  byte nbPages = 4;               // nb Pages  1 -> 8
  byte direction = 0;             // 0 -> reverse ; 1 -> forward
  byte displayBrightness = 150;   // 0-255;
} structSettings;

typedef struct{
  byte step = 0;                  //current Step
  int bpm = 0;                    //current bpm
  byte activeSeq = 0;             //0 -> seq 1,  1 -> seq2
  byte play = 0;                  //play stop
  byte pane = 0;                  //active menu 0-> Note; 1->Gate; 2->Replay; 2->Settings
} structStatus;

//utility
byte testByte(byte value, byte minimum, byte maximum);  //test byte range and return valid byte
byte increaseByte(byte value, byte maximum);  //increase byte
byte decreaseByte(byte value, byte minimum);  //decrease byte
byte changeByte(byte value, int change ,byte minimum = 0, byte maximum = 255);  //change byte


class status{
public:

//Status
  void setBPM(int bpm){stat.bpm = bpm;}
  void calcBPM();
  int getBPM(){return stat.bpm;}  //return MidiType

  byte getActiveSeq(){return stat.activeSeq;}
  void setActiveSeq(byte activeSeq){stat.activeSeq = activeSeq;}

  void increaseStep();
  void decreaseStep();
  void setStep(byte step);

  byte getActivePage(){return  (stat.step / STEPPERPAGE);}
  byte getStepOnPage(){return (stat.step - (getActivePage() * STEPPERPAGE)); }
  byte getStep(){return stat.step;}  //return MidiType

  byte getPlayStop(){return stat.play;}
  void setPlayStop(byte mode){stat.play = mode;}

  byte getDirection(){return config.direction;}
  void setDirection(byte direction){config.direction =  direction;}


//menu
  void increasePane(){stat.pane = testByte(stat.pane+1,0,2);}  //switch menu max 3 menu pages
  void decreasePane(){stat.pane = testByte(stat.pane-1,0,2);}  //switch menu max 3 menu pages;
  void setPane(byte pane){stat.pane = testByte(pane,0,2);}
  byte getActivePane(){return stat.pane;};

  byte getActiveMenu(){return getActivePane();} ///nochmal pane und menu auf eins bringen.....


//config
  byte getDisplayBrightness(){return display.brightness;}
  void setDisplayBrightness(byte brightness){display.brightness = brightness;}

  void setMidiType(byte midi){config.midiType = testByte(midi,0,1);}
  byte getMidiType(){return config.midiType;}

  void setNumberPages(byte nbPages){ config.nbPages = testByte(nbPages,1,PAGES);}
  byte getNumberPages(){return config.nbPages;}
  byte getCurrentNumberPages(){ //number of pages, takes care if page number has changed
    if(config.nbPages > (getStep()/8)) return config.nbPages; //is our step above the current number of pages?
    return (getStep()/8 +1);  //return current step page until the next page jump
  }

  void setSequence(structSettings *copySet);   //set all sequence values at once
  structSettings* getSettings();               //return the sequence struct pointer

private:
  structStatus stat;
  structSettings config;
};

//Sequence class
class seq
{
public:
  void init(byte note = 12, byte gate = 1, byte gateLength = 50, byte tuning = 10); //init sequence to default values

//Note
  void setNote(byte index, byte note);  //set note value
  void setNotes( byte note);            //set all note values
  byte getNote(byte index);             //return note value

  byte increaseNote(byte index);  //increase note value and return new note, function take care of tuning
  byte decreaseNote(byte index);  //decrease note value and return new note, function take care of tuning

  byte changeNote(byte index, int change);  //change note value and return new note
  void changeNotes(int change);             //change all note values

//TUNE
  void setTuning(byte tuning);
  byte getTuning();

//Gate
  void setGate(byte index, byte gate);  //set gate value
  byte getGate(byte index);             //return gate value
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


private:
  //Sequence
  structSequence sequence;
};


#endif
