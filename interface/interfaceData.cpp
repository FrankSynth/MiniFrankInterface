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
    if (arpRetrigger) {
        arpList.deleteAllKeys();
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
    if (noteList.size == 0 && sustain < 64) {
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
    mod = 0;
    pitchbend = 64;
    aftertouch = 0;
    sustain = 0;
    arpRetrigger = 1; // clear arp with next key
    triggered = 1;    // update outputs
}

void LiveMidi::printArray() {
    for (int x = 0; x < arpList.size; x++) {
        PRINT("Arp Array ");
        PRINT(x);
        PRINT(": ");
        PRINTLN(arpArray[x].note);
    }
}

void LiveMidi::updateArpArray(const byte &arpSettings) {
    copyArpListToArray();

    switch (arpSettings) {
    case 0:
    case 2:
    case 1: sortList(0); break;

    default:;
    }
    triggered = 1; // update outputs
}

void LiveMidi::copyArpListToArray() {
    for (int x = 0; x < arpList.size; x++) {
        PressedNotesElement *element = arpList.getElement(x);
        structKey key = {element->note, element->velocity};
        arpArray[x] = key;
    }
}

void LiveMidi::sortList(const byte &order) {
    if (order) {
        qsort(arpArray, arpList.size, sizeof(structKey), sort_asc);
    }
    else {
        qsort(arpArray, arpList.size, sizeof(structKey), sort_desc);
    }
}

structKey LiveMidi::getArpKey(const byte &step) {

    return arpArray[step];
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
    }
    sequence.note[index] = note;
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
    }

    sequence.note[index] = note;
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

    set(play, 0);

    for (byte x = 0; x < OUTPUTS; x++) {
        liveMidi[x].reset();
        liveMidi[x].updateArpArray(config.routing[x].arpMode);
        stat.stepArp[x] = 0;
        stat.stepSeq[x] = 0;
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
}

void FrankData::setBpm16thCount(unsigned int spp) {
    stat.midiClockCount = 5;
    stat.bpm16thCount = spp - 1;
}

inline void FrankData::calcBPM() {
    if (bpmSync) {
        static double bpm16thTimer = 0;
        set(bpm, (int)((60000. / (millis() - bpm16thTimer)) * 4 + 0.5));

        bpm16thTimer = millis();
    }
}

inline void FrankData::increaseStepSeq(const byte &array) {
    if (!((stat.stepSeq[array] + 1) % STEPSPERPAGE)) {                                     // if we make a pageJump
        if (config.routing[array].nbPages <= ((stat.stepSeq[array] + 1) / STEPSPERPAGE)) { // newPage above number of pages
            stat.stepSeq[array] = 0;                                                       // set stepSeq[array] 0
        }
        else {                     // new page is valid.
            stat.stepSeq[array]++; // increase Step
        }
    }
    else {
        stat.stepSeq[array]++; // increase Step
    }
}

inline void FrankData::decreaseStepSeq(const byte &array) {
    if (stat.stepSeq[array] == 0) {                                             // we jump to the last page?
        stat.stepSeq[array] = config.routing[array].nbPages * STEPSPERPAGE - 1; // set to max stepSeq[array]
    }
    else if ((!stat.stepSeq[array] % STEPSPERPAGE)) {                               // we make a pageJump?
        if (config.routing[array].nbPages > (stat.stepSeq[array] / STEPSPERPAGE)) { // newPage above number of pages
            stat.stepSeq[array] = config.routing[array].nbPages * STEPSPERPAGE - 1; // set jump to last stepSeq[array]
        }
        else {                     // new page is valid.
            stat.stepSeq[array]--; // decrease Step
        }
    }
    else {
        stat.stepSeq[array]--; // decrease Step
    }
}

inline byte FrankData::getCurrentPageNumber(const byte &array) { // number of pages, takes care if page number has changed
    if (config.routing[array].nbPages > (stat.stepSeq[array] / 8))
        return config.routing[array].nbPages; // is our stepSeq above the current number of pages?
    return (stat.stepSeq[array] / 8 + 1);     // return current stepSeq page until the next page jump
}

// Seq *FrankData::getSeqObject() { return seq; }

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

    return liveMidi[array].getArpKey(0);
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
    case 0: return getLiveKeyEvaluated(array).velocity;
    case 1: return liveMidi[array].mod;
    case 2: return liveMidi[array].pitchbend;
    case 3: return liveMidi[array].aftertouch;
    case 4: return liveMidi[array].sustain;
    default: PRINTLN("FrankData getLiveCcEvaluated, no case found"); return 0;
    }
}

inline byte FrankData::getOutputLiveModeEvaluated(const byte &array) {
    switch (config.routing[array].liveMidiMode) {
    case 0: return getLiveKeyEvaluated(array).velocity;
    case 1: return liveMidi[array].mod;
    case 2: return liveMidi[array].pitchbend;
    case 3: return liveMidi[array].aftertouch;
    case 4: return liveMidi[array].sustain;
    default: PRINTLN("FrankData getOutputLiveModeEvaluated, no case found"); return 0;
    }
}

// TO DO
inline byte FrankData::getOutputClockEvaluated(const byte &array) {
    return config.routing[array].clockSpeed + 1;
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
void FrankData::seqResetGateLengths(const byte &array) {
    seq[array].setGateLengths(50);
}
void FrankData::seqOctaveUp(const byte &array) {
    seq[array].octaveUp();
}
void FrankData::seqOctaveDown(const byte &array) {
    seq[array].octaveDown();
}

void FrankData::seqResetNote(const byte &array) {
    seq[array].setNotes(0);
}
void FrankData::seqResetGate(const byte &array) {
    seq[array].setGateLengths(50);
    seq[array].setGates(1);
}
void FrankData::seqResetCC(const byte &array) {
    seq[array].setCCs(64);
}

void FrankData::seqCopy(const byte &source, const byte &destination) {
    seq[destination].setSequence(&seq[source].sequence);
}

void FrankData::resetAllStepCounter() {
    for (byte i = 1; i < OUTPUTS; i++) {
        stat.stepSeq[i] = 0;
    }
}

void FrankData::updateArp(const byte &array) {
    if (config.routing[array].outSource == 0 && config.routing[array].arp) {
        liveMidi[array].updateArpArray(config.routing[array].arpMode);
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
    case bpmPoti: return stat.bpmPoti;
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
    case stepArp: return stat.stepArp[array];
    case stepSeq: return stat.stepSeq[array];
    case stepSpeed: return config.routing[array].stepSpeed;
    case activePage: return (byte)((float)stat.stepSeq[array] / (float)STEPSPERPAGE);
    case stepOnPage: return (stat.stepSeq[array] - (get(activePage, array) * STEPSPERPAGE));
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
    case bpmPoti: stat.bpmPoti = testByte(data, 0, 255, clampChange); break;
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
    case outputArpRatchet: config.routing[array].arpRatchet = testByte(data, 0, 2, clampChange); break;
    case outputArpOctave: config.routing[array].arpOctaves = testByte(data, 0, 6, clampChange); break;
    case outputArpMode:
        config.routing[array].arpMode = testByte(data, 0, 4, clampChange);
        updateArp(array);
        break;

    case cvCal: setCvCal(testByte(data, 0, 255, clampChange), array); break;

    case liveMod: liveMidi[array].mod = testByte(data, 0, 127, clampChange); break;
    case livePitchbend: liveMidi[array].pitchbend = testByte(data, 0, 255, clampChange); break;
    case liveAftertouch: liveMidi[array].aftertouch = testByte(data, 0, 127, clampChange); break;
    case liveSustain: liveMidi[array].sustain = testByte(data, 0, 127, clampChange); break;
    case liveTriggered: liveMidi[array].triggered = testByte(data, 0, 1, clampChange); break;

    case stepArp: stat.stepArp[array] = testByte(data, 0, NOTERANGE, clampChange); break;
    case stepSeq: stat.stepSeq[array] = testByte(data, 0, STEPSPERPAGE * config.routing[array].nbPages - 1, clampChange); break;
    case stepSpeed: config.routing[array].stepSpeed = testByte(data, 0, 5, clampChange); break;
    case nbPages: config.routing[array].nbPages = testByte(data, 1, PAGES, clampChange); break;

    case seqTuning: seq[array].sequence.tuning = testByte(data, 0, 13, clampChange); break;
    case seqRatchet: seq[array].sequence.ratchet = testByte(data, 0, 2, clampChange); break;
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
    case stepSeq: increaseStepSeq(array); break;
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
    case stepSeq: decreaseStepSeq(array); break;
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
        else {
            stat.screen.mainMenu = 1;
            stat.screen.routing = 0;
            stat.screen.calibration = 0;
            stat.screen.calibrateNote = 0;
        }
        break;
    case screenConfig: stat.screen.config = toggleValue(stat.screen.config); break;
    case screenRouting:
        stat.screen.mainMenu = 0;
        stat.screen.routing = 1;
        break;
    case screenCal:
        stat.screen.mainMenu = 0;
        stat.screen.calibration = 1;
        break;
    case screenCalNote:
        stat.screen.mainMenu = 0;
        stat.screen.calibrateNote = 1;
        break;
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

    case outputLiveMode: setStr("Key sel"); break;

    case outputClock: setStr("Clock"); break;

    case screenOutputChannel: setStr("Rout"); break;
    case screenConfig: setStr("Conf"); break;
    case screenMainMenu: setStr("Main"); break;
    case screenSubScreen: setStr("Sub"); break;
    case screenCal: setStr("Cal"); break;
    case screenCalNote: setStr("CalNt"); break;
    case screenRouting: setStr("Route"); break;

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

    case none: setStr(""); break;

    default: PRINTLN("FrankData getNameAsStr(frankData frankDataType), no case found"); setStr("ERR");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType) {
    byte channel = stat.screen.channel;
    return valueToStr(frankDataType, channel);
}

const char *FrankData::getValueAsStrChannel(const frankData &frankDataType, const byte &channel) {
    return valueToStr(frankDataType, channel);
}

const char *FrankData::valueToStr(const frankData &frankDataType, const byte &channel) {
    char tempStr[5];

    switch (frankDataType) {
    case seqTuning:
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

    case liveTriggered: setStr(toStr(get(frankDataType, stat.screen.channel))); break;
    case outputChannel:
        if (outputChannel == 0) {
            setStr("All");
        }
        else {
            setStr(toStr(get(frankDataType, stat.screen.channel)));
        }
        break;
    case outputArpOctave: setStr(toStr(((int)get(frankDataType, stat.screen.channel)) - ARPOCTAVECENTEROFFSET)); break;
    case seqGateLengthOffset: setStr(toStr(((int)get(frankDataType, stat.screen.channel)) - GATELENGTHOFFSET)); break;

    case screenRouting:
    case screenCal:
    case screenCalNote:
    case seqResetNotes:
    case seqResetGates: setStr("@"); break;

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

        if (config.routing[stat.screen.channel].outSource == 0) {
            setStr("Midi");
        }
        else if (config.routing[stat.screen.channel].outSource <= OUTPUTS) {

            itoa((int)config.routing[stat.screen.channel].outSource, tempStr, 10);

            tempStr[3] = tempStr[0];
            tempStr[0] = 'S';
            tempStr[1] = 'e';
            tempStr[2] = 'q';
            tempStr[4] = '\0';
            setStr(tempStr);
        }
        else {
            setStr("ERR");
        }
        break;

    case outputArpMode:
        switch (config.routing[channel].arpMode) {
        case 0: setStr("up"); break;
        case 1: setStr("down"); break;
        case 2: setStr("updn"); break;
        case 3: setStr("ordr"); break;
        case 4: setStr("rnd"); break;
        default: setStr("ERR");
        }
        break;

    case midiSource:
        switch (config.midiSource) {
        case 0: setStr("DIN"); break;
        case 1: setStr("USB"); break;
        default: setStr("ERR");
        }
        break;

    case direction:
        switch (config.direction) {

        case 0: setStr("->"); break;
        case 1: setStr("<-"); break;
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

    case liveLatestKey: setStr(toStr(getKeyLatest(stat.screen.channel).note)); break;
    case liveLowestKey: setStr(toStr(getKeyLowest(stat.screen.channel).note)); break;
    case liveHighestKey: setStr(toStr(getKeyHighest(stat.screen.channel).note)); break;
    case liveKeyNoteEvaluated:
        if (liveMidi[stat.screen.channel].keysPressed()) {
            setStr(toStr(getLiveKeyEvaluated(stat.screen.channel).note));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyVelEvaluated:
        if (liveMidi[stat.screen.channel].keysPressed()) {
            setStr(toStr(getLiveKeyEvaluated(stat.screen.channel).velocity));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyArpNoteEvaluated:
        if (liveMidi[stat.screen.channel].keysPressed()) {
            setStr(toStr(getArpKeyEvaluated(stat.screen.channel).note));
        }
        else {
            setStr("-");
        }
        break;
    case liveKeyArpVelEvaluated:
        if (liveMidi[stat.screen.channel].keysPressed()) {
            setStr(toStr(getArpKeyEvaluated(stat.screen.channel).velocity));
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
        switch (config.routing[stat.screen.channel].liveMidiMode) {
        case 0: setStr("Late"); break;
        case 1: setStr("Low"); break;
        case 2: setStr("High"); break;
        default: setStr("ERR");
        }
        break;

    case stepSpeed:
        switch (config.routing[stat.screen.channel].stepSpeed) {
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
        switch (config.routing[stat.screen.channel].clockSpeed) {
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
    case seqVelocity: setStr(toStr(get(frankDataType, stat.screen.channel, step))); break;

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
inline byte changeByte(const int &value, const byte &change, const byte &minimum, const byte &maximum, const bool &clampChange) {

    if ((int)value + change >= maximum) { // test max

        return clampChange ? maximum : value;
    }
    else if ((int)value + change <= minimum) { // test min
        return clampChange ? minimum : value;
    }
    else {
        return (byte)((int)value + change); // return new value
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

char valueToOctave(const byte &noteIn) {

    byte octave;
    octave = (noteIn + 9) / 12;

    switch (octave) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
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

int sort_desc(const void *cmp1, const void *cmp2) {
    // Need to cast the void * to int *
    byte a = ((structKey *)cmp1)->note;
    byte b = ((structKey *)cmp2)->note;
    // The comparison
    return a > b ? -1 : (a < b ? 1 : 0);
    // A simpler, probably faster way:
    // return b - a;
}
int sort_asc(const void *cmp1, const void *cmp2) {
    // Need to cast the void * to int *
    byte a = ((structKey *)cmp1)->note;
    byte b = ((structKey *)cmp2)->note;
    // The comparison
    return a > b ? 1 : (a < b ? -1 : 0);
    // A simpler, probably faster way:
    // return b - a;
}