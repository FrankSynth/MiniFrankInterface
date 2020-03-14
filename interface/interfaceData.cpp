#include "interfaceData.hpp"

// Debug logging
#define DEBUG 1

#if DEBUG == 1
#define PRINTLN(x) Serial.println(x)
#define PRINTLN2(x, y) Serial.println(x, y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x, y) Serial.print(x, y)
#else
#define PRINTLN(x)
#define PRINTLN2(x, y)
#define PRINT(x)
#define PRINT2(x, y)
#endif

// class PressedNotesList
void PressedNotesList::appendKey(const byte &note, const byte &velocity) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {
        if (listElement->note == note) return;
        while (listElement->next != NULL) {
            listElement = listElement->next;
            if (listElement->note == note) return;
        }

        listElement->next = new PressedNotesElement(note, velocity);
    }

    else {

        pressedNoteElement = new PressedNotesElement(note, velocity);
    }
    size++;
}

void PressedNotesList::deleteKey(const byte &note) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        PressedNotesElement *listElementPrev = listElement;

        while (listElement->note != note) {

            if (listElement->next) {
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
    size--;
}

void PressedNotesList::deleteAllKeys() {
    if (pressedNoteElement) {
        while (pressedNoteElement->next) {
            PressedNotesElement *listElement = pressedNoteElement;
            pressedNoteElement = pressedNoteElement->next;
            delete listElement;
        }

        delete pressedNoteElement;
        pressedNoteElement = NULL;
    }
    size = 0;
}

bool PressedNotesList::containsElements() {
    return pressedNoteElement ? 1 : 0;
}

PressedNotesElement *PressedNotesList::getKeyHighest() {
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
    return nullptr;
}

PressedNotesElement *PressedNotesList::getKeyLowest() {
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
    return nullptr;
}

PressedNotesElement *PressedNotesList::getKeyLatest() {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        while (listElement->next) {
            listElement = listElement->next;
        }

        return listElement;
    }
    return nullptr;
}

PressedNotesElement *PressedNotesList::getElement(const byte &element) {
    byte count = element;
    PressedNotesElement *listElement = pressedNoteElement;

    if (listElement) {

        while (count) {
            if (listElement->next) {
                listElement = listElement->next;
                count--;
            }
            // requested Element not there, return NULL
            else {
                return nullptr;
            }
        }
        // counted till position, return that
        return listElement;
    }

    // no elements there, return NULL
    else {
        return nullptr;
    }
}

// class LiveMidi for all real time midi data for a single midi input
void LiveMidi::keyPressed(const byte &note, const byte &velocity) {
    noteList.appendKey(note, velocity);
    if (arpRetrigger && sustain < 64) {
        arpList.deleteAllKeys();
        arpRetrigger = 0;
    }
    else {
        arpRetrigger = 0;
    }

    arpList.appendKey(note, velocity);
    triggered = 1;
}

void LiveMidi::keyReleased(const byte &note) {

    // if last element will get deleted
    if (noteList.size == 1) {
        lastKey.note = noteList.getElement(0)->note;
        lastKey.velocity = noteList.getElement(0)->velocity;
    }

    noteList.deleteKey(note);
    if (noteList.size == 0) {
        arpRetrigger = 1;
    }
    triggered = 1;
}

// bool, are any keys pressed
bool LiveMidi::keysPressed() {
    return noteList.containsElements() ? 1 : 0;
}

// return highest key pressed
structKey LiveMidi::getKeyHighest() {
    if (keysPressed()) {

        structKey key;
        PressedNotesElement *listElement = noteList.getKeyHighest();
        key.note = listElement->note;
        key.velocity = listElement->velocity;
        return key;
    }
    else {
        return lastKey;
    }
}

// return lowest key pressed
structKey LiveMidi::getKeyLowest() {
    if (keysPressed()) {
        structKey key;
        PressedNotesElement *listElement = noteList.getKeyLowest();
        key.note = listElement->note;
        key.velocity = listElement->velocity;
        return key;
    }
    else {
        return lastKey;
    }
}

// return latest key pressed
structKey LiveMidi::getKeyLatest() {
    if (keysPressed()) {
        structKey key;
        PressedNotesElement *listElement = noteList.getKeyLatest();
        key.note = listElement->note;
        key.velocity = listElement->velocity;
        return key;
    }
    else {
        return lastKey;
    }
}

// reset performance
void LiveMidi::reset() {
    noteList.deleteAllKeys();
    arpList.deleteAllKeys();
    mod = 0;
    pitchbend = 64;
    aftertouch = 0;
    sustain = 0;
    arpRetrigger = 1; // clear arp with next key
    triggered = 1;    // update outputs
}

void LiveMidi::printArray() {
    for (int x = 0; x < arpList.size; x++) {
    }
}

void LiveMidi::updateArpArray(const byte &arpSettings) {
    copyArpListToArray();

    if (arpSettings < 3) qsort(arpArray, arpList.size, sizeof(structKey), sort_asc);
    triggered = 1; // update outputs
}

inline void LiveMidi::copyArpListToArray() {
    for (int x = 0; x < arpList.size; x++) {
        PressedNotesElement *element = arpList.getElement(x);
        structKey key;
        key.note = element->note;
        key.velocity = element->velocity;
        arpArray[x] = key;
    }
}

structKey LiveMidi::getArpKey(const byte &step) {
    if (arpList.containsElements()) return arpArray[step];
    else
        return lastKey;
}

// Sequence data for each sequence
// init sequence to default values
void Seq::init(const byte &note, const byte &gate, const byte &gateLength, const byte &cc, const byte &tuning, const byte &ratchet,
               const byte &gateLengthOffset) {
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = note;  // test
        sequence.gate[i] = i % 2; // test
        sequence.gateLength[i] = gateLength;
        sequence.cc[i] = cc;
    }
    sequence.tuning = tuning;
    sequence.ratchet = ratchet;
    sequence.gateLengthOffset = gateLengthOffset;
}

void Seq::setNote(const byte &index, const byte &note) { // set note value
    sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE);
}

void Seq::setNotes(const byte &note) { // set note value

    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = testByte(note, 0, 88); // set notes
    }
}

void Seq::setCCs(const byte &cc) { // set note value

    for (int i = 0; i < LENGTH; i++) {
        sequence.cc[i] = testByte(cc, 0, 127); // set notes
    }
}
void Seq::setGates(const byte &gate) { // set note value

    for (int i = 0; i < LENGTH; i++) {
        sequence.gate[i] = testByte(gate, 0, 1); // set notes
    }
}

void Seq::increaseNote(const byte &index) { // increase note value and return new note

    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 0) {
        // note up
        sequence.note[index] = increaseByte(note, NOTERANGE);
    }
    else { // tuning active
        if (note < 88) {
            switch (((note + 1) + (12 - (sequence.tuning - 1))) % 12) {
            case 1:
            case 4:
            case 6:
            case 9:
            case 11:
                if (note < 87) {
                    note = note + 2;
                }
                break;

            default: note++; break;
            }
        }
        sequence.note[index] = note;
    }
}

void Seq::decreaseNote(const byte &index) { // decrease  note value and return new note

    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 0) {
        // note down
        sequence.note[index] = decreaseByte(note, 0);
    }
    else { // tuning active
        if (note > 0) {
            switch (((note - 1) + (12 - (sequence.tuning - 1))) % 12) {
            case 1:
            case 4:
            case 6:
            case 9:
            case 11:
                if (note > 1) {
                    note = note - 2;
                }
                break;

            default: note--; break;
            }
        }
        sequence.note[index] = note;
    }
}

void Seq::changeNotes(const int &change) { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], change, 0, NOTERANGE, 1);
    }
}

void Seq::octaveUp() { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], 12, 0, NOTERANGE, 0);
    }
}

void Seq::octaveDown() { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], -12, 0, NOTERANGE, 0);
    }
}

// all gate lengths
void Seq::setGateLengths(const byte &gateLength) { // set all gates at once
    // gateLength = testByte(gateLength, 0, 255); // test gate length
    for (int i = 0; i < LENGTH; i++) {
        sequence.gate[i] = gateLength; // set new gates
    }
}

void Seq::changeGateLengths(const int &change) { // change gate length
    for (int i = 0; i < LENGTH; i++) {
        sequence.gateLength[i] = changeByte(sequence.gateLength[i], change, 0, 100, 1);
    }
}

// Sequence
void Seq::setSequence(structSequence *copySeq) { // set complete suquence struct
    sequence = *copySeq;                         // copy struct
}

structSequence *Seq::getSequence() { // return sequence struct pointer
    return &sequence;
}

int Seq::getSequenceSize() { // return the struct size
    return (int)sizeof(structSequence);
}

// Data Singleton that handles all data, including all sequences, live midi and settings
void FrankData::receivedKeyPressed(const byte &channel, const byte &note, const byte &velocity) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].channel == 0 || config.routing[x].channel == channel) {
            liveMidi[x].keyPressed(note, velocity);
            updateArp(x);
        }
    }
}

void FrankData::receivedKeyReleased(const byte &channel, const byte &note) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].channel == 0 || config.routing[x].channel == channel) {
            liveMidi[x].keyReleased(note);
        }
    }
}

void FrankData::receivedMidiClock() {
    increaseMidiClock();
}
void FrankData::receivedMidiSongPosition(unsigned int spp) {
    setBpm16thCount(spp);
}
void FrankData::receivedStart() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;
    for (byte out = 0; out < OUTPUTS; out++) {
        liveMidi[out].stepSeq = config.routing[out].nbPages * STEPSPERPAGE - 1;
        liveMidi[out].channel16thCount = liveMidi[out].stepSeq * 16 - 1;
    }
    set(play, 1);
}
void FrankData::receivedContinue() {
    set(play, 1);
}
void FrankData::receivedStop() {
    set(play, 0);
}

void FrankData::receivedReset() {}

void FrankData::reset() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;

    for (byte out = 0; out < OUTPUTS; out++) {
        liveMidi[out].stepSeq = config.routing[out].nbPages * STEPSPERPAGE - 1;
        liveMidi[out].channel16thCount = liveMidi[out].stepSeq * 16 - 1;
    }

    set(play, 0);

    for (byte x = 0; x < OUTPUTS; x++) {
        liveMidi[x].reset();
        liveMidi[x].updateArpArray(config.routing[x].arpMode);
        liveMidi[x].stepArp = 0;
        liveMidi[x].stepSeq = 0;
    }
}

void FrankData::increaseMidiClock() {
    stat.midiClockCount++;
    if (stat.midiClockCount == 6) {
        stat.midiClockCount = 0;
        increaseBpm16thCount();
    }
}

void FrankData::increaseBpm16thCount() {
    stat.bpm16thCount++;
    if (stat.bpm16thCount == 32) {
        stat.bpm16thCount = 0;
    }
    calcBPM();
    for (byte out = 0; out < OUTPUTS; out++) {
        if ((int)liveMidi[out].channel16thCount == ((int)config.routing[out].nbPages * STEPSPERPAGE * 16) - 1) {
            liveMidi[out].channel16thCount = 0;
        }
        else {
            liveMidi[out].channel16thCount++;
        }
    }

}

void FrankData::increaseStepCounters(const byte &channel) {

    byte amount = config.routing[channel].stepSpeed;

    for (byte count = 0; count < amount; count++) {

        stat.bpm16thCount++;
        if (stat.bpm16thCount == 32) {
            stat.bpm16thCount = 0;
        }
        for (byte out = 0; out < OUTPUTS; out++) {
            if ((int)liveMidi[out].channel16thCount == 0) {
                liveMidi[out].channel16thCount = (int)config.routing[out].nbPages * STEPSPERPAGE * 16) - 1;
            }
            else {
                liveMidi[out].channel16thCount--;
            }
        }
    }
}

void FrankData::decreaseStepCounters(const byte &channel) {

    byte amount = config.routing[channel].stepSpeed;

    for (byte count = 0; count < amount; count++) {

        if (stat.bpm16thCount == 0) {
            stat.bpm16thCount = 32;
        }
        stat.bpm16thCount--;

        for (byte out = 0; out < OUTPUTS; out++) {
            if ((int)liveMidi[out].channel16thCount == ((int)config.routing[out].nbPages * STEPSPERPAGE * 16) - 1) {
                liveMidi[out].channel16thCount = 0;
            }
            else {
                liveMidi[out].channel16thCount++;
            }
        }

        for (byte out = 0; out < OUTPUTS; out++) {
            if ((int)stat.bpm16thCount % (int)pow(2, (int)(config.routing[out].stepSpeed)) == 0) {
                nextArpStep(out);

                if (stat.play) {
                    if (config.direction) {
                        increaseSeqStep(out);
                    }
                    else {
                        decreaseSeqStep(out);
                    }
                }
            }
        }
    }
}

void FrankData::setBpm16thCount(unsigned int spp) {
    stat.midiClockCount = 5;
    stat.bpm16thCount = (spp % 16) - 1;
}

void FrankData::setBPMPoti(const unsigned &bpmPot) {
    stat.bpmPot = bpmPot;
    stat.bpm = testByte(bpmPot / 4);
}

void FrankData::updateClockCounter() {

    if (!stat.bpmSync) {
        static long timer = 0;
        if (stat.bpmPot == 0) stat.bpmPot = 1;
        if (millis() - timer > 60000 / stat.bpmPot) {
            increaseBpm16thCount();
            timer = millis();
        }
    }
}

inline void FrankData::calcBPM() {
    if (stat.bpmSync) {
        static double bpm16thTimer = 0;
        set(bpm, (int)(((60000 / (millis() - bpm16thTimer)) / 4 + 0.5)));

        bpm16thTimer = millis();
    }
}

inline void FrankData::increaseSeqStep(const byte &array) {
    if (!((liveMidi[array].stepSeq + 1) % STEPSPERPAGE)) {                                     // if we make a pageJump
        if (config.routing[array].nbPages <= ((liveMidi[array].stepSeq + 1) / STEPSPERPAGE)) { // newPage above number of pages
            liveMidi[array].stepSeq = 0;                                                       // set stepSeq[array] 0
        }
        else {                         // new page is valid.
            liveMidi[array].stepSeq++; // increase Step
        }
    }
    else {
        liveMidi[array].stepSeq++; // increase Step
    }
}

inline void FrankData::decreaseSeqStep(const byte &array) {
    if (liveMidi[array].stepSeq == 0) {                                             // we jump to the last page?
        liveMidi[array].stepSeq = config.routing[array].nbPages * STEPSPERPAGE - 1; // set to max stepSeq[array]
    }
    else if (!liveMidi[array].stepSeq % STEPSPERPAGE) {                                 // we make a pageJump?
        if (config.routing[array].nbPages > (liveMidi[array].stepSeq / STEPSPERPAGE)) { // newPage above number of pages
            liveMidi[array].stepSeq = config.routing[array].nbPages * STEPSPERPAGE - 1; // set jump to last stepSeq[array]
        }
        else {                         // new page is valid.
            liveMidi[array].stepSeq--; // decrease Step
        }
    }
    else {
        liveMidi[array].stepSeq--; // decrease Step
    }
}

inline byte FrankData::getCurrentPageNumber(const byte &array) { // number of pages, takes care if page number has changed
    if (config.routing[array].nbPages > (liveMidi[array].stepSeq / 8))
        return config.routing[array].nbPages; // is our stepSeq above the current number of pages?
    return (liveMidi[array].stepSeq / 8 + 1); // return current stepSeq page until the next page jump
}

// Singleton
FrankData *FrankData::mainData = nullptr;
FrankData &FrankData::getDataObj() {
    if (mainData == nullptr) mainData = new FrankData();
    return *mainData;
}

inline structKey FrankData::getLiveKeyEvaluated(const byte &array) {
    switch (config.routing[array].liveMidiMode) {
    case 0: return liveMidi[array].getKeyLatest();
    case 1: return liveMidi[array].getKeyLowest();
    case 2: return liveMidi[array].getKeyHighest();
    default: PRINTLN("FrankData getLiveKeyEvaluated, no case found"); return liveMidi[array].lastKey;
    }
}

structKey FrankData::getArpKeyEvaluated(const byte &array) {

    return liveMidi[array].arpKey;
}

inline void FrankData::increaseArpOct(const byte &array) {
    // change octave for next round
    // no octaving
    if (config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET == 0) {
        liveMidi[array].arpOctave = 0;
    }
    else {

        // eval new min and max
        int newOctMin;
        int newOctMax;

        if (config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET < 0) {
            newOctMin = config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET;
            newOctMax = 0;
        }
        else {
            newOctMin = 0;
            newOctMax = config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET;
        }

        if (config.routing[array].arpMode == 2 || config.routing[array].arpMode == 3) {
            liveMidi[array].arpOctave = changeInt(liveMidi[array].arpOctave, 1, newOctMin, newOctMax, true);
            if (liveMidi[array].arpOctave == newOctMax) liveMidi[array].arpOctaveDirection = 0;
        }
        else {
            liveMidi[array].arpOctave = changeIntReverse(liveMidi[array].arpOctave, 1, newOctMin, newOctMax);
        }
    }
}

inline void FrankData::decreaseArpOct(const byte &array) {
    // no octaving
    if (config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET == 0) {
        liveMidi[array].arpOctave = 0;
    }
    else {
        // eval new min and max
        int newOctMin;
        int newOctMax;

        if (config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET < 0) {
            newOctMin = config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET;
            newOctMax = 0;
        }
        else {
            newOctMin = 0;
            newOctMax = config.routing[array].arpOctaves - ARPOCTAVECENTEROFFSET;
        }
        if (config.routing[array].arpMode == 2 || config.routing[array].arpMode == 3) {
            liveMidi[array].arpOctave = changeInt(liveMidi[array].arpOctave, -1, newOctMin, newOctMax, true);
            if (liveMidi[array].arpOctave == newOctMin) {
                liveMidi[array].arpOctaveDirection = 1;
            }
        }
        else {
            liveMidi[array].arpOctave = changeIntReverse(liveMidi[array].arpOctave, -1, newOctMin, newOctMax);
        }
    }
}

inline void FrankData::nextArpStep(const byte &array) {

    switch (config.routing[array].arpMode) {
    case 1:
        if (liveMidi[array].stepArp == 0) {
            decreaseArpOct(array);
        }
        liveMidi[array].stepArp = changeByteReverse(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
        break;
    case 0:
    case 4:
    case 5:
        if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {
            increaseArpOct(array);
        }
        liveMidi[array].stepArp = changeByteReverse(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
        break;
    case 2:
        // going up
        if (liveMidi[array].arpDirection) {
            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
            if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {

                liveMidi[array].arpDirection = 0;
            }
        }
        // going down
        else {
            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
            if (liveMidi[array].stepArp == 0) {
                liveMidi[array].arpDirection = 1;
                if (liveMidi[array].arpOctaveDirection == 1) {
                    increaseArpOct(array);
                }
                else {
                    decreaseArpOct(array);
                }
            }
        }
        break;
    case 3:
        // going up
        if (liveMidi[array].arpDirection) {
            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
            if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {

                liveMidi[array].arpDirection = 0;
                if (liveMidi[array].arpOctaveDirection == 1) {
                    increaseArpOct(array);
                }
                else {
                    decreaseArpOct(array);
                }
            }
        }
        // going down
        else {
            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
            if (liveMidi[array].stepArp == 0) {
                liveMidi[array].arpDirection = 1;
            }
        }
        break;
    default:;
    }

    byte step = liveMidi[array].stepArp;
    structKey key;

    if (config.routing[array].arpMode == 5) {
        step = random(0, liveMidi[array].arpList.size);
        key = liveMidi[array].getArpKey(step);
    }
    else {
        key = liveMidi[array].getArpKey(step);
    }

    // lower octaves
    if (liveMidi[array].arpOctave < 0) {
        for (int x = liveMidi[array].arpOctave; x < 0; x++) {
            key.note = changeByte(key.note, -12, 0, NOTERANGE, 0);
        }
    }
    // raise octaves
    else if (liveMidi[array].arpOctave > 0) {
        for (int x = liveMidi[array].arpOctave; x > 0; x--) {
            key.note = changeByte(key.note, 12, 0, NOTERANGE, 0);
        }
    }

    liveMidi[array].arpKey = key;
}

inline structKey FrankData::getKeyHighest(const byte &array) {
    return liveMidi[array].getKeyHighest();
}
inline structKey FrankData::getKeyLowest(const byte &array) {
    return liveMidi[array].getKeyLowest();
}
inline structKey FrankData::getKeyLatest(const byte &array) {
    return liveMidi[array].getKeyLatest();
}

inline byte FrankData::getLiveCcEvaluated(const byte &array) {
    switch (config.routing[array].cc) {
    case 0: return config.routing[array].arp ? getArpKeyEvaluated(array).velocity : getLiveKeyEvaluated(array).velocity;
    case 1: return liveMidi[array].mod;
    case 2: return liveMidi[array].pitchbend;
    case 3: return liveMidi[array].aftertouch;
    case 4: return liveMidi[array].sustain;
    default: PRINTLN("FrankData getLiveCcEvaluated, no case found"); return 0;
    }
}

void FrankData::seqSetAllNotes(const byte &array, const byte &data) {
    seq[array].setNotes(data);
}
void FrankData::seqSetAllCC(const byte &array, const byte &data) {
    seq[array].setCCs(data);
}
void FrankData::seqSetAllGates(const byte &array, const byte &data) {
    seq[array].setGates(data);
}
void FrankData::seqSetAllGateLengths(const byte &array, const byte &data) {
    seq[array].setGateLengths(data);
}
void FrankData::seqResetAllGateLengths(const byte &array) {
    seq[array].setGateLengths(50);
}
void FrankData::seqAllOctaveUp(const byte &array) {
    seq[array].octaveUp();
}
void FrankData::seqAllOctaveDown(const byte &array) {
    seq[array].octaveDown();
}

void FrankData::seqResetAllNotes(const byte &array) {
    seq[array].setNotes(0);
}
void FrankData::seqResetAllGates(const byte &array) {
    seq[array].setGateLengths(50);
    seq[array].setGates(1);
}
void FrankData::seqResetAllCC(const byte &array) {
    seq[array].setCCs(64);
}

void FrankData::seqCopy(const byte &source, const byte &destination) {
    seq[destination].setSequence(&seq[source].sequence);
}

void FrankData::resetAllStepCounter() {
    for (byte out = 0; out < OUTPUTS; out++) {
        liveMidi[out].stepSeq = config.routing[out].nbPages * STEPSPERPAGE - 1;
        liveMidi[out].channel16thCount = liveMidi[out].stepSeq * 16 - 1;
    }
}

void FrankData::updateArp(const byte &array) {
    if (config.routing[array].outSource == 0 && config.routing[array].arp) {
        liveMidi[array].updateArpArray(config.routing[array].arpMode);
        liveMidi[array].stepArp = testByte(liveMidi[array].stepArp, 0, liveMidi[array].arpList.size, true);
    }
}

void FrankData::setNoteCal(const byte &data, const byte &array, const byte &note) {
    cal[array].noteCalibration[note] = ((float)data - CALOFFSET) / 1000;
}
byte FrankData::getNoteCal(const byte &array, const byte &note) {
    return (byte)((cal[array].noteCalibration[note] * 1000) + 0.5 + CALOFFSET);
}
void FrankData::setCvCal(const byte &data, const byte &array) {
    cal[array].cvOffset = ((float)data - CALOFFSET) / 1000;
}
byte FrankData::getCvCal(const byte &array) {
    return (byte)((cal[array].cvOffset * 1000) + 0.5 + CALOFFSET);
}

// Screen config
void FrankData::resetSubScreen() {
    stat.screen.subscreen = FrankData::screenNote;
}

inline const byte FrankData::getSubscreenMax() {
    return config.routing[stat.screen.channel].outSource ? stat.screen.subScreenMaxSeq : stat.screen.subScreenMaxLive;
}

// new set and get functions
byte FrankData::get(const frankData &frankDataType) {
    switch (frankDataType) {
    case midiSource: return config.midiSource;

    case direction: return config.direction;
    case displayBrightness: return config.displayBrightness;

    case screenOutputChannel: return stat.screen.channel;
    case screenConfig: return stat.screen.config;
    case screenMainMenu: return stat.screen.mainMenu;
    case screenSubScreen: return stat.screen.subscreen;
    case screenRouting: return stat.screen.routing;
    case screenCal: return stat.screen.calibration;
    case screenCalNote: return stat.screen.calibrateNote;

    case bpm: return stat.bpm;
    case play: return stat.play;
    case rec: return stat.rec;
    case error: return stat.error;
    case bpmSync: return stat.bpmSync;
    case bpmPoti: return stat.bpmPot;
    case bpm16thCount: return stat.bpm16thCount;
    case save:
    case load: return stat.loadSaveSlot;
    case pulseLength: return stat.pulseLength;

    case none: return (byte)0;
    default: PRINTLN("FrankData get(frankData frankDataType), no case found"); return 0;
    }
}

byte FrankData::get(const frankData &frankDataType, const byte &array) {
    switch (frankDataType) {
    case outputSource: return config.routing[array].outSource;
    case outputChannel: return config.routing[array].channel;
    case outputArp: return config.routing[array].arp;
    case outputCc: return config.routing[array].cc;
    case outputCcEvaluated: return getLiveCcEvaluated(array);
    case outputLiveMode: return config.routing[array].liveMidiMode;
    case outputClock: return config.routing[array].clockSpeed;
    case outputArpRatchet: return config.routing[array].arpRatchet;
    case outputArpOctave: return config.routing[array].arpOctaves;
    case outputArpMode: return config.routing[array].arpMode;

    case cvCal: return getCvCal(array);

    case liveMod: return liveMidi[array].mod;
    case livePitchbend: return liveMidi[array].pitchbend;
    case liveAftertouch: return liveMidi[array].aftertouch;
    case liveSustain: return liveMidi[array].sustain;
    case liveTriggered: return liveMidi[array].triggered;
    case liveKeyNoteEvaluated: return getLiveKeyEvaluated(array).note;
    case liveKeyVelEvaluated: return getLiveKeyEvaluated(array).velocity;
    case liveKeyArpNoteEvaluated: return getArpKeyEvaluated(array).note;
    case liveKeyArpVelEvaluated: return getArpKeyEvaluated(array).velocity;
    case liveLatestKey: return liveMidi[array].getKeyLatest().note;
    case liveLowestKey: return liveMidi[array].getKeyLowest().note;
    case liveHighestKey: return liveMidi[array].getKeyHighest().note;

    case nbPages: return config.routing[array].nbPages;
    case stepArp: return liveMidi[array].stepArp;
    case stepSeq: return liveMidi[array].stepSeq;
    case stepSpeed: return config.routing[array].stepSpeed;
    case activePage: return (liveMidi[array].stepSeq / STEPSPERPAGE);
    case stepOnPage: return (liveMidi[array].stepSeq - (get(activePage, array) * STEPSPERPAGE));
    case currentPageNumber: return getCurrentPageNumber(array);

    case seqTuning: return seq[array].sequence.tuning;
    case seqRatchet: return seq[array].sequence.ratchet;
    case seqGateLengthOffset: return seq[array].sequence.gateLengthOffset;

    case none: return (byte)0;

    default: PRINTLN("FrankData get(frankData frankDataType, byte array), no case found"); return 0;
    }
}

byte FrankData::get(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
    case seqNote: return seq[array].sequence.note[step];
    case seqGate: return seq[array].sequence.gate[step];
    case seqGateLength: return seq[array].sequence.gateLength[step];
    case seqCc: return seq[array].sequence.cc[step];
    case seqVelocity: return seq[array].sequence.velocity[step];

    case noteCal: return getNoteCal(array, step);

    case none: return (byte)0;

    default: PRINTLN("FrankData get(frankData frankDataType, byte array, byte step), no case found"); return 0;
    }
}

// set data
void FrankData::set(const frankData &frankDataType, const int &data, const bool &clampChange) {
    switch (frankDataType) {
    case midiSource: config.midiSource = testByte(data, 0, 1, clampChange); break;

    case direction: config.direction = testByte(data, 0, 1, clampChange); break;
    case displayBrightness: config.displayBrightness = testByte(data, 0, 255, clampChange); break; // how high?

    case screenOutputChannel: stat.screen.channel = testByte(data, 0, OUTPUTS - 1, clampChange); break;
    case screenConfig: stat.screen.config = testByte(data, 0, 1, clampChange); break;
    case screenMainMenu: stat.screen.mainMenu = testByte(data, 0, 1, clampChange); break;
    case screenSubScreen: stat.screen.subscreen = testByte(data, 0, getSubscreenMax(), clampChange); break;
    case screenRouting: stat.screen.routing = testByte(data, 0, 1, clampChange); break;
    case screenCal: stat.screen.calibration = testByte(data, 0, 1, clampChange); break;
    case screenCalNote: stat.screen.calibrateNote = testByte(data, 0, 1, clampChange); break;

    case bpm: stat.bpm = testByte(data, 0, 255, clampChange); break;
    case play: stat.play = testByte(data, 0, 1, clampChange); break;
    case rec: stat.rec = testByte(data, 0, 1, clampChange); break;
    case error: stat.error = testByte(data, 0, 1, clampChange); break;
    case bpmSync: stat.bpmSync = testByte(data, 0, 1, clampChange); break;
    case bpmPoti: stat.bpmPot = testByte(data, 0, 255, clampChange); break;
    case load:
    case save: stat.loadSaveSlot = testByte(data, 1, 10, clampChange); break;
    case pulseLength: stat.pulseLength = testByte(data, 0, 255, clampChange); break;
    case none: break;
    default: PRINTLN("FrankData set(frankData frankDataType, byte data, bool clampChange = 0), no case found");
    }
}

// set data, array
void FrankData::set(const frankData &frankDataType, const int &data, const byte &array, const bool &clampChange) {
    switch (frankDataType) {
    case outputSource: config.routing[array].outSource = testByte(data, 0, OUTPUTS, clampChange); break;
    case outputChannel: config.routing[array].channel = testByte(data, 0, 16, clampChange); break;
    case outputArp:
        config.routing[array].arp = testByte(data, 0, 1, clampChange);
        updateArp(array);
        break;
    case outputCc: config.routing[array].cc = testByte(data, 0, 4, clampChange); break;
    case outputLiveMode: config.routing[array].liveMidiMode = testByte(data, 0, 2, clampChange); break;
    case outputClock: config.routing[array].clockSpeed = testByte(data, 0, 5, clampChange); break;
    case outputArpRatchet: config.routing[array].arpRatchet = testByte(data, 0, 3, clampChange); break;
    case outputArpOctave: config.routing[array].arpOctaves = testByte(data, 0, 6, clampChange); break;
    case outputArpMode:
        config.routing[array].arpMode = testByte(data, 0, 5, clampChange);
        updateArp(array);
        break;

    case cvCal: setCvCal(testByte(data, 0, 255, clampChange), array); break;

    case liveMod: liveMidi[array].mod = testByte(data, 0, 127, clampChange); break;
    case livePitchbend: liveMidi[array].pitchbend = testByte(data, 0, 255, clampChange); break;
    case liveAftertouch: liveMidi[array].aftertouch = testByte(data, 0, 127, clampChange); break;
    case liveSustain: liveMidi[array].sustain = testByte(data, 0, 127, clampChange); break;
    case liveTriggered: liveMidi[array].triggered = testByte(data, 0, 1, clampChange); break;

    case stepArp: liveMidi[array].stepArp = testByte(data, 0, NOTERANGE, clampChange); break;
    case stepSeq: liveMidi[array].stepSeq = testByte(data, 0, STEPSPERPAGE * config.routing[array].nbPages - 1, clampChange); break;
    case stepSpeed: config.routing[array].stepSpeed = testByte(data, 0, 5, clampChange); break;
    case nbPages: config.routing[array].nbPages = testByte(data, 1, PAGES, clampChange); break;

    case seqTuning: seq[array].sequence.tuning = testByte(data, 0, 13, clampChange); break;
    case seqRatchet: seq[array].sequence.ratchet = testByte(data, 0, 3, clampChange); break;
    case seqGateLengthOffset: seq[array].sequence.gateLengthOffset = testByte(data, 0, 200, clampChange); break;
    case none: break;
    default: PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, bool clampChange), no case found");
    }
}

// set data, array, step
void FrankData::set(const frankData &frankDataType, const int &data, const byte &array, const byte &step, const bool &clampChange) {
    switch (frankDataType) {
    case seqNote: seq[array].setNote(step, data); break;
    case seqGate: seq[array].sequence.gate[step] = testByte(data, 0, 1, clampChange); break;
    case seqGateLength: seq[array].sequence.gateLength[step] = testByte(data, 0, 100, clampChange); break;
    case seqCc: seq[array].sequence.cc[step] = testByte(data, 0, 127, clampChange); break;
    case seqVelocity: seq[array].sequence.velocity[step] = testByte(data, 0, 127, clampChange); break;

    case noteCal: setNoteCal(testByte(data, 0, 255, clampChange), array, step); break;
    case none: break;
    default:
        PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, byte step, bool clampChange), no case "
                "found");
    }
}

void FrankData::change(const frankData &frankDataType, const int &amount, const bool &clampChange) {
    set(frankDataType, get(frankDataType) + amount, clampChange);
}
void FrankData::change(const frankData &frankDataType, const int &amount, const byte &array, const bool &clampChange) {
    set(frankDataType, get(frankDataType, array) + amount, array, clampChange);
}
void FrankData::change(const frankData &frankDataType, const int &amount, const byte &array, const byte &step, const bool &clampChange) {
    set(frankDataType, get(frankDataType, array, step) + amount, array, step, clampChange);
}

void FrankData::increase(const frankData &frankDataType, const bool &clampChange) {
    change(frankDataType, 1, clampChange);
}
void FrankData::increase(const frankData &frankDataType, const byte &array, const bool &clampChange) {
    switch (frankDataType) {
    case stepArp: nextArpStep(array); break;
    case stepSeq: increaseSeqStep(array); break;
    default: change(frankDataType, 1, array, clampChange);
    }
}
void FrankData::increase(const frankData &frankDataType, const byte &array, const byte &step, const bool &clampChange) {
    switch (frankDataType) {
    case seqNote: seq[array].increaseNote(step); break;
    default: change(frankDataType, 1, array, step, clampChange);
    }
}

void FrankData::decrease(const frankData &frankDataType, const bool &clampChange) {
    change(frankDataType, -1, clampChange);
}
void FrankData::decrease(const frankData &frankDataType, const byte &array, const bool &clampChange) {
    switch (frankDataType) {
    case stepArp: nextArpStep(array); break;
    case stepSeq: decreaseSeqStep(array); break;
    default: change(frankDataType, -1, array, clampChange);
    }
}
void FrankData::decrease(const frankData &frankDataType, const byte &array, const byte &step, const bool &clampChange) {
    switch (frankDataType) {
    case seqNote: seq[array].decreaseNote(step); break;
    default: change(frankDataType, -1, array, step, clampChange);
    }
}

void FrankData::toggle(const frankData &frankDataType) {
    switch (frankDataType) {
    case direction: config.direction = toggleValue(config.direction); break;
    case play: stat.play = toggleValue(stat.play); break;
    case rec: stat.rec = toggleValue(stat.rec); break;
    case bpmSync: stat.bpmSync = toggleValue(stat.bpmSync); break;
    case outputArp:
        config.routing[stat.screen.channel].arp = toggleValue(config.routing[stat.screen.channel].arp);
        updateArp(stat.screen.channel);
        break;
    case screenMainMenu:
        if (stat.screen.mainMenu) {
            stat.screen.mainMenu = 0;
        }
        else if (stat.screen.config || stat.screen.routing || stat.screen.calibration || stat.screen.calibrateNote) {
            stat.screen.mainMenu = 0;
            stat.screen.config = 0;
            stat.screen.routing = 0;
            stat.screen.calibration = 0;
            stat.screen.calibrateNote = 0;
        }
        else {
            stat.screen.mainMenu = 1;
            stat.screen.config = 0;
            stat.screen.routing = 0;
            stat.screen.calibration = 0;
            stat.screen.calibrateNote = 0;
        }
        break;
    case screenConfig: stat.screen.config = toggleValue(stat.screen.config); break;
    case screenRouting:
        stat.screen.mainMenu = 0;
        stat.screen.config = 0;
        stat.screen.routing = !stat.screen.routing;
        stat.screen.calibration = 0;
        stat.screen.calibrateNote = 0;
        break;
    case screenCal:
        stat.screen.mainMenu = 0;
        stat.screen.config = 0;
        stat.screen.routing = 0;
        stat.screen.calibration = 1;
        stat.screen.calibrateNote = 0;
        break;
    case screenCalNote:
        stat.screen.mainMenu = 0;
        stat.screen.config = 0;
        stat.screen.routing = 0;
        stat.screen.calibration = 0;
        stat.screen.calibrateNote = 1;
        break;

    case seqResetGates: seqResetAllGates(config.routing[stat.screen.config].outSource - 1); break;
    case seqResetNotes: seqResetAllNotes(config.routing[stat.screen.config].outSource - 1); break;
    case seqResetGateLengths: seqResetAllGateLengths(config.routing[stat.screen.config].outSource - 1); break;
    case seqResetCC: seqResetAllCC(config.routing[stat.screen.config].outSource - 1); break;
    case seqOctaveUp: seqAllOctaveUp(config.routing[stat.screen.config].outSource - 1); break;
    case seqOctaveDown: seqAllOctaveDown(config.routing[stat.screen.config].outSource - 1); break;

    case resetStepCounters: resetAllStepCounter(); break;

    case none: break;
    default: PRINTLN("FrankData toggle(frankData frankDataType), no case found");
    }
}

void FrankData::toggle(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
    case seqGate: seq[array].sequence.gate[step] = toggleValue(seq[array].sequence.gate[step]); break;
    case none: break;
    default: PRINTLN("FrankData toggle(frankData frankDataType, byte array, byte step), no case found");
    }
}

const char *FrankData::getNameAsStr(const frankData &frankDataType) {
    switch (frankDataType) {
    case seqNote: setStr("Note"); break;
    case seqGate: setStr("Gate"); break;
    case seqGateLength: setStr("GateLn"); break;
    case seqCc: setStr("CC"); break;
    case seqVelocity: setStr("Vel"); break;
    case seqTuning: setStr("Tune"); break;
    case seqSize: setStr("Seq"); break;
    case seqRatchet: setStr("Reps"); break;
    case seqGateLengthOffset: setStr("GL-OF"); break;
    case stepSpeed: setStr("Speed"); break;
    case seqResetNotes: setStr("Rs Nt"); break;
    case seqResetGates: setStr("Rs Gt"); break;
    case seqResetGateLengths: setStr("Rs GL"); break;
    case seqResetCC: setStr("Rs CC"); break;
    case seqOctaveUp: setStr("OctUp"); break;
    case seqOctaveDown: setStr("OctDn"); break;

    case midiSource: setStr("Src"); break;
    case nbPages: setStr("Pages"); break;
    case direction: setStr("Fw/Rv"); break;
    case displayBrightness: setStr("ScBr"); break;

    case outputSource: setStr("Src"); break;
    case outputChannel: setStr("Chan"); break;
    case outputArp: setStr("Arp"); break;
    case outputArpMode: setStr("Mode"); break;
    case outputCc: setStr("CC"); break;
    case outputCcEvaluated:
        switch (config.routing[stat.screen.channel].cc) {
        case 0: setStr("Vel"); break;
        case 1: setStr("Mod"); break;
        case 2: setStr("PB"); break;
        case 3: setStr("AftT"); break;
        case 4: setStr("Sus"); break;
        default: setStr("ERR"); break;
        }
        break;
    case outputArpRatchet: setStr("Reps"); break;
    case outputArpOctave: setStr("Oct"); break;

    case outputLiveMode: setStr("Key"); break;

    case outputClock: setStr("Clock"); break;

    case screenOutputChannel: setStr("Rout"); break;
    case screenConfig: setStr("Conf"); break;
    case screenMainMenu: setStr("Main"); break;
    case screenSubScreen: setStr("Sub"); break;
    case screenCal: setStr("Cal"); break;
    case screenCalNote: setStr("CalNt"); break;
    case screenRouting: setStr("MIDI"); break;

    case stepSeq: setStr("Step"); break;
    case stepArp: setStr("Arp Step"); break;
    case bpm: setStr("BPM"); break;
    case play: setStr("Pl/St"); break;
    case rec: setStr("Rec"); break;
    case error: setStr("Error"); break;
    case bpmSync: setStr("Sync"); break;
    case bpmPoti: setStr("BPM"); break;
    case load: setStr("Load"); break;
    case save: setStr("Save"); break;
    case pulseLength: setStr("PulsL"); break;

    case liveMod: setStr("Mod"); break;
    case livePitchbend: setStr("PB"); break;
    case liveAftertouch: setStr("AftT"); break;
    case liveSustain: setStr("Sus"); break;
    case liveTriggered: setStr("TRIG"); break;
    case liveLatestKey: setStr("Late"); break;
    case liveLowestKey: setStr("Low"); break;
    case liveHighestKey: setStr("High"); break;
    case liveKeyArpNoteEvaluated:
    case liveKeyNoteEvaluated: setStr("Note"); break;
    case liveKeyArpVelEvaluated:
    case liveKeyVelEvaluated: setStr("Vel"); break;

    case noteCal: setStr("NCal"); break;
    case cvCal: setStr("CvCal"); break;

    case resetStepCounters: setStr("RsStep"); break;

    case none: setStr(""); break;

    default: PRINTLN("FrankData getNameAsStr(frankData frankDataType), no case found"); setStr("ERR");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType) {
    byte channel;
    switch (frankDataType) {
    case seqResetGates:
    case seqResetGateLengths:
    case seqResetCC:
    case seqOctaveUp:
    case seqOctaveDown:
    case seqResetNotes:
    case seqNote:
    case seqGate:
    case seqGateLength:
    case seqCc:
    case seqCcEvaluated:
    case seqSize:
    case seqRatchet:
    case seqGateLengthOffset:
    case stepOnPage:
    case currentPageNumber:
    case nbPages:
    case stepSeq:
    case activePage:
    case seqVelocity: channel = config.routing[stat.screen.config].outSource - 1;
    default: channel = stat.screen.channel;
    }
    return valueToStr(frankDataType, channel);
}

const char *FrankData::getValueAsStrChannel(const frankData &frankDataType, const byte &channel) {
    return valueToStr(frankDataType, channel);
}

const char *FrankData::valueToStr(const frankData &frankDataType, const byte &channel) {
    char tempStr[5];

    switch (frankDataType) {
    case seqSize:

    case stepSeq:
    case stepArp:
    case nbPages:

    case outputArpRatchet:
    case seqRatchet:

    case liveMod:
    case livePitchbend:
    case liveAftertouch:
    case liveSustain:

    case liveTriggered: setStr(toStr(get(frankDataType, channel))); break;
    case outputChannel:
        if (config.routing[channel].channel == 0) {
            setStr("ALL");
        }
        else {
            setStr(toStr(get(frankDataType, channel)));
        }
        break;
    case outputArpOctave: setStr(toStr(((int)get(frankDataType, channel)) - ARPOCTAVECENTEROFFSET)); break;
    case seqGateLengthOffset: setStr(toStr(((int)get(frankDataType, channel)) - GATELENGTHOFFSET)); break;
    case seqTuning: setStr(tuningToChar(seq[channel].sequence.tuning)); break;

    case resetStepCounters:
    case screenRouting:
    case screenCal:
    case screenCalNote:
    case seqResetGates:
    case seqResetGateLengths:
    case seqResetCC:
    case seqOctaveUp:
    case seqOctaveDown:
    case seqResetNotes: setStr("@"); break;

    case screenOutputChannel:
    case screenConfig:
    case screenMainMenu:
    case screenSubScreen:

    case displayBrightness:

    case bpm:
    case bpmSync:
    case load:
    case save:
    case pulseLength:
    case bpmPoti: setStr(toStr(get(frankDataType))); break;

    case cvCal: setStr(toStr(((int)get(frankDataType, channel)) - CALOFFSET)); break;

    case outputSource:

        if (config.routing[channel].outSource == 0) {
            setStr("Midi");
        }
        else if (config.routing[channel].outSource <= OUTPUTS) {

            itoa((int)config.routing[channel].outSource, tempStr, 10);

            tempStr[2] = tempStr[0];
            tempStr[0] = 'S';
            tempStr[1] = 'q';
            tempStr[3] = '\0';
            setStr(tempStr);
        }
        else {
            setStr("ERR");
            PRINT("outputSource, received ");
            PRINTLN(config.routing[channel].outSource);
        }
        break;

    case outputArpMode:
        switch (config.routing[channel].arpMode) {
        case 0: setStr("up"); break;
        case 1: setStr("dn"); break;
        case 2: setStr("UD"); break;
        case 3: setStr("DU"); break;
        case 4: setStr("ordr"); break;
        case 5: setStr("rnd"); break;
        default:
            setStr("ERR");
            PRINT("outputArpMode, received ");
            PRINTLN(config.routing[channel].arpMode);
        }
        break;

    case midiSource:
        switch (config.midiSource) {
        case 0: setStr("DIN"); break;
        case 1: setStr("USB"); break;
        default:
            setStr("ERR");
            PRINT("midiSource, received ");
            PRINTLN(config.midiSource);
        }
        break;

    case direction:
        switch (config.direction) {

        case 0: setStr("<-"); break;
        case 1: setStr("->"); break;
        default: setStr("ERR");
        }
        break;

    case outputArp:
        switch (config.routing[channel].arp) {
        case 0: setStr("OFF"); break;
        case 1: setStr("ON"); break;
        default: setStr("ERR");
        }
        break;

    case play:
        switch (stat.play) {
        case 0: setStr("STOP"); break;
        case 1: setStr("PLAY"); break;
        default: setStr("ERR");
        }
        break;

    case rec:
        switch (stat.rec) {
        case 0: setStr("-"); break;
        case 1: setStr("REC"); break;
        default: setStr("ERR");
        }
        break;

    case error:
        switch (stat.error) {
        case 0: setStr(" "); break;
        case 1: setStr("ERR"); break;
        default: setStr("ERR");
        }
        break;

    case liveLatestKey: setStr(toStr(getKeyLatest(channel).note)); break;
    case liveLowestKey: setStr(toStr(getKeyLowest(channel).note)); break;
    case liveHighestKey: setStr(toStr(getKeyHighest(channel).note)); break;
    case liveKeyNoteEvaluated:
        if (liveMidi[channel].keysPressed()) {
            setStr(toStr(getLiveKeyEvaluated(channel).note));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyVelEvaluated:
        if (liveMidi[channel].keysPressed()) {
            setStr(toStr(getLiveKeyEvaluated(channel).velocity));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyArpNoteEvaluated:
        if (liveMidi[channel].keysPressed()) {
            setStr(toStr(getArpKeyEvaluated(channel).note));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyArpVelEvaluated:
        if (liveMidi[channel].keysPressed()) {
            setStr(toStr(getArpKeyEvaluated(channel).velocity));
        }
        else {
            setStr("-");
        }
        break;

    case outputCc:
        switch (config.routing[channel].cc) {
        case 0: setStr("Vel"); break;
        case 1: setStr("Mod"); break;
        case 2: setStr("PB"); break;
        case 3: setStr("AftT"); break;
        case 4: setStr("Sus"); break;
        default: setStr("ERR"); break;
        }
        break;
    case outputCcEvaluated: setStr(toStr(getLiveCcEvaluated(channel))); break;

    case outputLiveMode:
        switch (config.routing[channel].liveMidiMode) {
        case 0: setStr("Late"); break;
        case 1: setStr("Low"); break;
        case 2: setStr("High"); break;
        default: setStr("ERR");
        }
        break;

    case stepSpeed:
        switch (config.routing[channel].stepSpeed) {
        case 0: setStr("1/16"); break;
        case 1: setStr("1/8"); break;
        case 2: setStr("1/4"); break;
        case 3: setStr("1/2"); break;
        case 4: setStr("1/1"); break;
        case 5: setStr("2/1"); break;
        default: setStr("NONE");
        }
        break;
    case outputClock:
        switch (config.routing[channel].clockSpeed) {
        case 0: setStr("1/16"); break;
        case 1: setStr("1/8"); break;
        case 2: setStr("1/4"); break;
        case 3: setStr("1/2"); break;
        case 4: setStr("1/1"); break;
        case 5: setStr("2/1"); break;
        default: setStr("ERR");
        }
        break;
    case none: setStr(""); break;

    default: setStr("ERR"); PRINTLN("FrankData getValueAsStr(const frankData &frankDataType), no case found");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType, const byte &step) {
    switch (frankDataType) {
    case seqNote:
    case seqGate:
    case seqGateLength:
    case seqCc:
    case seqVelocity: setStr(toStr(get(frankDataType, config.routing[stat.screen.config].outSource - 1, step))); break;

    case noteCal: setStr(toStr(((int)get(frankDataType, stat.screen.channel, step)) - CALOFFSET)); break;

    case none: setStr(""); break;

    default: PRINTLN("FrankData getValueAsStr(const frankData &frankDataType, const byte &step), no case found"); setStr("NONE");
    }
    return returnStr;
}

inline void FrankData::setStr(const char *newStr) {
    strncpy(str, newStr, MAXSTRINGSIZE - 1);
    str[MAXSTRINGSIZE] = '\0';
}

// utility
// test byte range and return valid byte
inline byte testByte(const int &value, const byte &minimum, const byte &maximum, const bool &clampChange) {
    if (value > maximum) {
        return maximum;
    }
    else if (value < minimum) {
        return minimum;
    }
    else {
        return (byte)value;
    }
}

// increase byte
inline byte increaseByte(const byte &value, const byte &maximum) {
    if (value == maximum) {
        return value;
    }
    else {
        return value + 1;
    }
}

// decrease byte
inline byte decreaseByte(const byte &value, const byte &minimum) {
    if (value == minimum) {
        return value;
    }
    else {
        return value - 1;
    }
}

// change byte value and check boundaries
inline byte changeByte(const byte &value, const int &change, const byte &minimum, const byte &maximum, const bool &clampChange) {

    if ((int)value + change > maximum) { // test max

        return clampChange ? maximum : value;
    }

    else if ((int)value + change < minimum) { // test min
        return clampChange ? minimum : value;
    }
    else {
        return (byte)((int)value + change); // return new value
    }
}

inline int changeInt(const int &value, const int &change, const int &minimum, const int &maximum, const bool &clampChange) {

    if (value + change > maximum) { // test max

        return clampChange ? maximum : value;
    }
    else if (value + change < minimum) { // test min
        return clampChange ? minimum : value;
    }
    else {
        return (value + change); // return new value
    }
}

inline byte changeByteReverse(const byte &value, const int &change, const byte &minimum, const byte &maximum) {

    if ((int)value + change > maximum) { // test max

        return minimum;
    }
    else if ((int)value + change < minimum) { // test min
        return maximum;
    }
    else {
        return ((byte)value + change); // return new value
    }
}

inline int changeIntReverse(const int &value, const int &change, const int &minimum, const int &maximum) {

    if (value + change > maximum) { // test max

        return minimum;
    }
    else if (value + change < minimum) { // test min
        return maximum;
    }
    else {
        return (value + change); // return new value
    }
}

// toggle byte
template <typename T> inline T toggleValue(const T &data) {
    return !data;
}

// convert number to string
template <typename T> inline char *toStr(const T &data) {
    static char str[4];
    itoa((int)data, str, 10);
    return str;
}

char valueToNote(const byte &noteIn) {

    byte note;
    note = (noteIn + 9) % 12;
    switch (note) {
    case 0:
    case 1: return 'C';
    case 2:
    case 3: return 'D';
    case 4: return 'E';
    case 5:
    case 6: return 'F';
    case 7:
    case 8: return 'G';
    case 9:
    case 10: return 'A';
    case 11: return 'B';
    }
    return '\0';
}

const char *valueToOctave(const byte &noteIn) {

    byte octave;
    octave = (noteIn + 9) / 12;

    switch (octave) {
    case 0: return "-1";
    case 1: return "0";
    case 2: return "1";
    case 3: return "2";
    case 4: return "3";
    case 5: return "4";
    case 6: return "5";
    case 7: return "6";
    case 8: return "7";
    case 9: return "8";
    }
    return '\0';
}

char valueToSharp(const byte &noteIn) {
    byte note;
    note = (noteIn + 9) % 12;

    if (note == 1 || note == 3 || note == 6 || note == 8 || note == 10) {
        return '#';
    }

    return '\0';
}

const char *tuningToChar(const byte &tuning) {

    switch (tuning) {
    case 1: return "C";
    case 2: return "C#";
    case 3: return "D";
    case 4: return "D#";
    case 5: return "E";
    case 6: return "F";
    case 7: return "F#";
    case 8: return "G";
    case 9: return "G#";
    case 10: return "A";
    case 11: return "A#";
    case 12: return "B";
    case 13: return "F";
    default: return "-";
    }
}

inline int sort_desc(const void *cmp1, const void *cmp2) {
    // Need to cast the void * to int *
    byte a = ((structKey *)cmp1)->note;
    byte b = ((structKey *)cmp2)->note;
    // The comparison
    return a > b ? -1 : (a < b ? 1 : 0);
}
inline int sort_asc(const void *cmp1, const void *cmp2) {
    // Need to cast the void * to int *
    byte a = ((structKey *)cmp1)->note;
    byte b = ((structKey *)cmp2)->note;
    // The comparison
    return a > b ? 1 : (a < b ? -1 : 0);
}