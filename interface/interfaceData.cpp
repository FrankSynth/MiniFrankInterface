#include "interfaceData.h"

void seq::init(byte note, byte gate, byte gateLength, byte activePages , byte tuning){  //init sequence to default values
  for(int i = 0 ; i < LENGTH ; i++ ){
    sequence.note[i] = note;
    sequence.gate[i] = gate;
    sequence.gateLength[i] = gateLength;
  }
  sequence.activePages = activePages;
  sequence.tuning = tuning;
}



//Pages
byte seq::getActivePages(){   //return number of active pages
  return sequence.activePages;
}

void seq::setActivePages(byte page){  //set number of active pages
  sequence.activePages = testByte(page, 1, PAGES);
}

//Note
byte seq::getNote(byte index){  //return note value
  return sequence.note[testByte(index, 0, LENGTH)];
}

void seq::setNote(byte index, byte note){ //set note value
  sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE);
}

void seq::setNotes(byte note){ //set note value
  note =  testByte(note, 0, 88); //test note value in range

  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = testByte(note, 0, NOTERANGE);  //set notes
  }
}

byte seq::increaseNote(byte index) {  //increase note value and return new note
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

byte seq::decreaseNote(byte index){ //decrease  note value and return new note
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

byte seq::changeNote(byte index, int change){  //change note
  index = testByte(index, 0, LENGTH); //test index
  sequence.note[index] = changeByte(sequence.note[index], change ,0 , NOTERANGE);

  return sequence.note[index];
}

void seq::changeNotes(int change){  //change note
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.note[i] = changeByte(sequence.note[i], change ,0 , NOTERANGE);
  }
}

//Gate
byte seq::getGate(byte index){  //return gate value
  return sequence.gate[testByte(index, 0, LENGTH)];
}

void seq::setGate(byte index, byte gate) {  //set gate value
  sequence.gate[testByte(index, 0, LENGTH)] = testByte(gate, 0, 1);
}

byte seq::toggleGate(byte index){ //toggle gate and return new status;
  index = testByte(index, 0, LENGTH); //test index
  sequence.gate[index] = !sequence.gate[index]; //toggle gate

  return sequence.gate[index];  //return new gate
}

//one gate length
void seq::setGateLength(byte index, byte gateLength) {  //set gate length
  sequence.gateLength[testByte(index, 0, LENGTH)] = testByte(gateLength, 0, 255);
}

byte seq::getGateLength(byte index){  //return gate length
  return sequence.gateLength[testByte(index, 0, LENGTH)];
}

byte seq::changeGateLength(byte index, int change){  //change gate length
  index = testByte(index, 0, LENGTH); //test index
  sequence.gateLength[index] = changeByte(sequence.gateLength[index], change);

  return sequence.gateLength[index];
}


//all gate lengths
void seq::setGateLengths(byte gateLength){  //set all gates at once
  gateLength =  testByte(gateLength, 0, 255); //test gate length
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.gate[i] = gateLength;  //set new gates
  }
}

void seq::changeGateLengths(int change){  //change gate length
  for(int i = 0; i < LENGTH ; i++ ){
    sequence.gateLength[i] = changeByte(sequence.gateLength[i], change);
  }
}

//Sequence
void seq::setSequence(structSequence *copySeq){ //set complete suquence struct
  sequence = *copySeq;  //copy struct
}

structSequence* seq::getSequence(){ //return sequence struct pointer
  return &sequence;
}

int seq::getSequenceSize(){ //return the struct size
  return (int)sizeof(structSequence);
}


void seq::setTuning(byte tuning){
  sequence.tuning = tuning;
}

byte seq::getTuning(){
  return sequence.tuning;
}

//utility
byte seq::testByte(byte value, byte minimum, byte maximum){  //test byte range and return valid byte
  if(value > maximum){
    return maximum;

    #ifdef DEBUG
    Serial.println("testByte: value was to big");
    Serial.println("INPUT: ");
    Serial.println(value);
    #endif

  }
  else if(value < minimum){
    return minimum;

    #ifdef DEBUG
    Serial.println("testByte: value was to small");
    Serial.println("INPUT: ");
    Serial.println(value);
    #endif

  }
  else{
    return value;
  }
}

byte seq::increaseByte(byte value, byte maximum){  //increase byte
  if(value == maximum){
    return value;
  }
  else{
    return value + 1;
  }
}

byte seq::decreaseByte(byte value, byte minimum){  //decrease byte
  if(value == minimum){
    return value;
  }
  else{
    return value - 1;
  }
}


byte seq::changeByte(byte value, int change ,byte minimum, byte maximum){  //change byte value and check boundaries
  if((int)value + change >= maximum){ //test max
    return maximum;
  }
  else if((int)value - change <= minimum){  //test min
    return minimum;
  }
  else{
    return value + change;  //return new value
  }
}
