#include "interfaceData.hpp"

#define DEBUG



// private data
structStatus stat;
structSettings config;

LiveMidi liveMidi[OUTPUTS];


//class Output Routing
byte OutputRouting::getOut() {
    return out;
}
byte OutputRouting::getChannel() {
    return channel;
}
byte OutputRouting::getSeq() {
    return seq;
}
byte OutputRouting::getArp() {
    return arp;
}
byte OutputRouting::getCc() {
    return cc;
}

void OutputRouting::setOut(byte data) {
    out = data;
}
void OutputRouting::setChannel(byte data) {
    channel = data;
}
void OutputRouting::setSeq(byte data) {
    seq = data;
}
void OutputRouting::setArp(byte data) {
    arp = data;
}
void OutputRouting::setCc(byte data) {
    cc = data;
}






// class PressedNotesList
void PressedNotesList::appendKey(byte note, byte velocity) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {
        while (listElement->next != NULL) {
            listElement = listElement->next;
        }

        listElement->next = new PressedNotesElement(note, velocity);
    }

    else {

        pressedNoteElement = new PressedNotesElement(note, velocity);
    }
}


void PressedNotesList::deleteKey(byte note) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        PressedNotesElement *listElementPrev = listElement;

        while (listElement->note != note) {

            if(listElement->next) {
                listElementPrev = listElement;
                listElement = listElement->next;
            }
            else {
                return;
            }
        }
        if (pressedNoteElement->next == NULL) {
            delete listElement;
            pressedNoteElement = NULL;
        }
        else {
            if (listElementPrev->next == listElement->next) {
                pressedNoteElement = pressedNoteElement->next;
            }
            else {
                listElementPrev->next = listElement->next;
            }
            delete listElement;
        }
    }
}


void PressedNotesList::deleteAllKeys() {
    if (pressedNoteElement) {
        while(pressedNoteElement->next) {
            PressedNotesElement *listElement = pressedNoteElement;
            pressedNoteElement = pressedNoteElement->next;
            delete listElement;
        }

        delete pressedNoteElement;
        pressedNoteElement = NULL;
    }
}


bool PressedNotesList::containsElements() {
    return pressedNoteElement ? 1 : 0;
}


PressedNotesElement* PressedNotesList::getKeyHighest() {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {
        PressedNotesElement *highestElement;
        highestElement = listElement;

        while (listElement->next) {
            listElement = listElement->next;
            if (listElement->note > highestElement->note) {
                highestElement = listElement;
            }

        }
        return highestElement;

    }
    return NULL;
}

PressedNotesElement* PressedNotesList::getKeyLowest() {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {
        PressedNotesElement *lowestElement;
        lowestElement = listElement;

        while (listElement->next) {
            listElement = listElement->next;
            if (listElement->note < lowestElement->note) {
                lowestElement = listElement;
            }

        }
        return lowestElement;

    }
    return NULL;
}

PressedNotesElement* PressedNotesList::getKeyLatest() {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        while (listElement->next) {
            listElement = listElement->next;
        }

        return listElement;

    }
    return NULL;
}


// class LiveMidi
void LiveMidi::keyPressed(byte note, byte velocity) {
    noteList.appendKey(note, velocity);
}

void LiveMidi::keyReleased(byte note) {
    noteList.deleteKey(note);
}

bool LiveMidi::keysPressed() {
    return noteList.containsElements() ? 1 : 0;
}

void LiveMidi::setMod(byte data) {
    mod = data;
}

void LiveMidi::setPitchbend(byte data) {
    pitchbend = data;
}

void LiveMidi::setAftertouch(byte data) {
    aftertouch = data;
}

void LiveMidi::setSustain(byte data) {
    sustain = data;
}

structKey LiveMidi::getKeyHighest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyHighest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

structKey LiveMidi::getKeyLowest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyLowest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

structKey LiveMidi::getKeyLatest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyLatest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

byte LiveMidi::getMod() {
    return mod;
}

byte LiveMidi::getPitchbend() {
    return pitchbend;
}

byte LiveMidi::getAftertouch() {
    return aftertouch;
}

byte LiveMidi::getSustain() {
    return sustain;
}

void LiveMidi::reset() {
    noteList.deleteAllKeys();
    mod = 0;
    pitchbend = 64;
    aftertouch = 0;
    sustain = 0;
}





// receive Midi
void receivedKeyPressed(byte channel, byte note, byte velocity) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].keyPressed(note, velocity);
        }
    }
}


void receivedKeyReleased(byte channel, byte note) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].keyReleased(note);
        }
    }
}

void receivedMod(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setMod(data);
        }
    }
}

void receivedPitchbend(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setPitchbend(data);
        }
    }
}

void receivedAftertouch(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setAftertouch(data);
        }
    }
}

void receivedSustain(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++ ) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setSustain(data);
        }
    }
}




void receivedMidiClock() {
    increaseMidiClock();
}
void receivedMidiSongPosition(unsigned int spp) {
    setBpm16thCount(spp);
}
void receivedStart() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;
    settings::setPlayStop(1);
}
void receivedContinue() {
    settings::setPlayStop(1);
}
void receivedStop() {
    settings::setPlayStop(0);
}

void receivedReset() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;
    settings::setPlayStop(0);

    for (byte x = 0; x < OUTPUTS; x++ ) {
        liveMidi[x].reset();
    }
}


void increaseMidiClock() {
     stat.midiClockCount++;
     if (stat.midiClockCount == 6) {
         stat.midiClockCount = 0;
         increaseBpm16thCount();
     }
}

void increaseBpm16thCount() {
    stat.bpm16thCount++;
    if (stat.bpm16thCount == 32) {
        stat.bpm16thCount = 0;
    }
}

void setBpm16thCount(unsigned int spp) {
    stat.midiClockCount = 5;
    stat.bpm16thCount = spp - 1;
}



//stat
namespace settings {

    byte getSync() {
      return stat.bpmSync;
    }

    void setSync(byte bpmSync) {
      stat.bpmSync = bpmSync;
    }

    void setRec(byte rec) {
      stat.rec = rec;
    }

    byte getRec() {
      return stat.rec;
    }

    void setBPM(int bpm) {
      stat.bpm = bpm;
    }

    void calcBPM() {
      static double bpmTimer = 0;
      setBPM((int)(60000. / (millis() - bpmTimer)));

      bpmTimer = millis();
    }

    int getBPM(){return stat.bpm;}  //return MidiSource

    byte getActiveSeq(){return stat.activeSeq;}
    void setActiveSeq(byte activeSeq){stat.activeSeq = activeSeq;}


    void increaseStep(){
    if(!((stat.stepSeq+1) % STEPPERPAGE)){  //if we make a pageJump
      if( getNumberPages() <= ((stat.stepSeq+1) / STEPPERPAGE)){ //newPage above number of pages
      stat.stepSeq = 0;   //set stepSeq 0
      }
      else{ //new page is valid.
       stat.stepSeq++;    //increase Step
      }
    }
    else{
      stat.stepSeq++;    //increase Step
      }

      calcBPM();
    }

    void decreaseStep(){
      if(stat.stepSeq == 0){ //we jump to the last page?
        stat.stepSeq =  getNumberPages() * STEPPERPAGE -1;    //set to max stepSeq

      }
      else if((!stat.stepSeq % STEPPERPAGE)){  //we make a pageJump?
        if( getNumberPages() > (stat.stepSeq / STEPPERPAGE)){ //newPage above number of pages
        stat.stepSeq = getNumberPages() * STEPPERPAGE -1;   //set jump to last stepSeq
        }
        else{ //new page is valid.
         stat.stepSeq--;    //decrease Step
        }
      }
      else{
        stat.stepSeq--;    //decrease Step
      }
        calcBPM();
    }


    void setStep(byte stepSeq){
      stat.stepSeq = testByte(stepSeq,0, STEPPERPAGE * getNumberPages()-1);
    }



    byte getActivePage(){return  (stat.stepSeq / STEPPERPAGE);}
    byte getStepOnPage(){return (stat.stepSeq - (getActivePage() * STEPPERPAGE)); }
    byte getStep(){return stat.stepSeq;}  //return MidiSource

    byte getPlayStop(){return stat.play;}
    void setPlayStop(byte mode){stat.play = mode;}

    byte getDirection(){return config.direction;}
    void setDirection(byte direction){config.direction =  direction;}

    byte getError(){return stat.error;}
    void setError(byte error){stat.error = error;}



    //menu
    void increasePane(){stat.pane = testByte(stat.pane+1,0,2);}  //switch menu max 3 menu pages
    void decreasePane(){stat.pane = testByte(stat.pane-1,0,2);}  //switch menu max 3 menu pages;
    void setPane(byte pane){stat.pane = testByte(pane,0,2);}
    byte getActivePane(){return stat.pane;};

    byte getActiveMenu(){return getActivePane();} ///nochmal pane und menu auf eins bringen.....


    //config
    byte getDisplayBrightness(){return config.displayBrightness;}
    void setDisplayBrightness(byte brightness){config.displayBrightness = brightness;}

    void setMidiSource(byte midi){config.midiSource = testByte(midi,0,1);}
    byte getMidiSource(){return config.midiSource;}

    void setNumberPages(byte nbPages){ config.nbPages = testByte(nbPages,1,PAGES);}
    byte getNumberPages(){return config.nbPages;}
    byte getCurrentNumberPages(){ //number of pages, takes care if page number has changed
      if(config.nbPages > (getStep()/8)) return config.nbPages; //is our stepSeq above the current number of pages?
      return (getStep()/8 +1);  //return current stepSeq page until the next page jump
    }
}

//Sequence
void Seq::init(byte note, byte gate, byte gateLength, byte tuning){  //init sequence to default values
  for(int i = 0 ; i < LENGTH ; i++ ){
    sequence.note[i] = note; //test
    sequence.gate[i] = i%2; //test
    sequence.gateLength[i] = gateLength;
  }
  sequence.tuning = tuning;
}



//Note
byte Seq::getNote(byte index){  //return note value
  return sequence.note[testByte(index, 0, LENGTH)];
}

void Seq::setNote(byte index, byte note){ //set note value
  sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE);
}

void Seq::setNotes(byte note){ //set note value
  note =  testByte(note, 0, 88); //test note value in range

  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = testByte(note, 0, NOTERANGE);  //set notes
  }
}

byte Seq::increaseNote(byte index) {  //increase note value and return new note


  index = testByte(index, 0, LENGTH); //testIndex

  byte note = sequence.note[index];

  // no tuning
  if (sequence.tuning == 255) {
    // note up
    sequence.note[index] = increaseByte(note, NOTERANGE );
  }
  else {  // tuning active
    if (note < 88) {
      switch (((note+1)+(12-sequence.tuning)) % 12) {
        case 1:
        case 4:
        case 6:
        case 9:
        case 11:
        if (note < 87) {
          note = note + 2;
        }
        break;

        default:
        note++;
        break;
      }
    }
  }
  sequence.note[index] = note;
  return sequence.note[index];  //return note

}

byte Seq::decreaseNote(byte index){ //decrease  note value and return new note
  index = testByte(index, 0, LENGTH); //testIndex
  byte note = sequence.note[index];

  // no tuning
  if (sequence.tuning == 255) {
    // note down
    sequence.note[index] = decreaseByte(note, NOTERANGE );
  }
  else {  // tuning active
    if (note > 0) {
      switch (((note-1)+(12-sequence.tuning)) % 12) {
        case 1:
        case 4:
        case 6:
        case 9:
        case 11:
        if (note > 1) {
          note = note - 2;
        }
        break;

        default:
        note--;
        break;
      }
    }
  }

  sequence.note[index] = note;
  return sequence.note[index];  ///return new note
}

byte Seq::changeNote(byte index, int change){  //change note
  index = testByte(index, 0, LENGTH); //test index
  sequence.note[index] = changeByte(sequence.note[index], change ,0 , NOTERANGE);

  return sequence.note[index];
}

void Seq::changeNotes(int change){  //change note
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = changeByte(sequence.note[i], change ,0 , NOTERANGE);
  }
}


void Seq::octaveUp(){  //change note
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = changeByte2(sequence.note[i], 12 ,0 , NOTERANGE);
  }
}

void Seq::octaveDown(){  //change note
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = changeByte2(sequence.note[i], -12 ,0 , NOTERANGE);
  }
}




//Gate
byte Seq::getGate(byte index){  //return gate value
  return sequence.gate[testByte(index, 0, LENGTH)];
}

void Seq::setGate(byte index, byte gate) {  //set gate value
  sequence.gate[testByte(index, 0, LENGTH)] = testByte(gate, 0, 1);
}

byte Seq::toggleGate(byte index){ //toggle gate and return new stat;
  index = testByte(index, 0, LENGTH); //test index
  sequence.gate[index] = !sequence.gate[index]; //toggle gate

  return sequence.gate[index];  //return new gate
}

//one gate length
void Seq::setGateLength(byte index, byte gateLength) {  //set gate length
  sequence.gateLength[testByte(index, 0, LENGTH)] = testByte(gateLength, 0, 255);
}

byte Seq::getGateLength(byte index){  //return gate length
  return sequence.gateLength[testByte(index, 0, LENGTH)];
}

byte Seq::changeGateLength(byte index, int change){  //change gate length
  index = testByte(index, 0, LENGTH); //test index
  sequence.gateLength[index] = changeByte(sequence.gateLength[index], change,0,100);

  return sequence.gateLength[index];
}


//all gate lengths
void Seq::setGateLengths(byte gateLength){  //set all gates at once
  gateLength =  testByte(gateLength, 0, 255); //test gate length
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.gate[i] = gateLength;  //set new gates
  }
}

void Seq::changeGateLengths(int change){  //change gate length
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.gateLength[i] = changeByte(sequence.gateLength[i], change);
  }
}

//Sequence
void Seq::setSequence(structSequence *copySeq){ //set complete suquence struct
  sequence = *copySeq;  //copy struct
}

structSequence* Seq::getSequence(){ //return sequence struct pointer
  return &sequence;
}

int Seq::getSequenceSize(){ //return the struct size
  return (int)sizeof(structSequence);
}


void Seq::setTuning(byte tuning){
  sequence.tuning = tuning;
}

byte Seq::getTuning(){
  return sequence.tuning;
}










//utility
byte testByte(byte value, byte minimum, byte maximum){  //test byte range and return valid byte
  if(value > maximum){

    #ifdef DEBUG
    Serial.println("testByte: value was to big");
    Serial.println("INPUT: ");
    Serial.println(value);
    #endif
    return maximum;


  }
  else if(value < minimum){

    #ifdef DEBUG
    Serial.println("testByte: value was to small");
    Serial.println("INPUT: ");
    Serial.println(value);
    #endif

    return minimum;


  }
  else{
    return value;
  }
}

byte increaseByte(byte value, byte maximum){  //increase byte
  if(value == maximum){
    return value;
  }
  else{
    return value + 1;
  }
}

byte decreaseByte(byte value, byte minimum){  //decrease byte
  if(value == minimum){
    return value;
  }
  else{
    return value - 1;
  }
}


byte changeByte(byte value, int change ,byte minimum, byte maximum){  //change byte value and check boundaries
  if((int)value + change >= maximum){ //test max
    return maximum;
  }
  else if((int)value + change <= minimum){  //test min
    return minimum;
  }
  else{
    return (byte)((int)value + change);  //return new value
  }
}


byte changeByte2(byte value, int change ,byte minimum, byte maximum){  //change byte (keeps original value if change not possible)
  if((int)value + change >= maximum){ //test max
    return value;
  }
  else if((int)value + change <= minimum){  //test min
    return value;
  }
  else{
    return (byte)((int)value + change);  //return new value
  }
}
