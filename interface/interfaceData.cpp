#include "interfaceData.hpp"

// Debug logging
#define DEBUG 0

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

// class Output Routing
// byte OutputRouting::getOut() { return outSource; }
// byte OutputRouting::getChannel() { return channel; }
// byte OutputRouting::getSeq() { return seq; }
// byte OutputRouting::getArp() { return arp; }
// byte OutputRouting::getCc() { return cc; }
// byte OutputRouting::getliveMidiMode() { return liveMidiMode; }

// void OutputRouting::setOut(byte data) { outSource = data; }
// void OutputRouting::setChannel(byte data) { channel = data; }
// void OutputRouting::setSeq(byte data) { seq = data; }
// void OutputRouting::setArp(byte data) { arp = data; }
// void OutputRouting::setCc(byte data) { cc = data; }
// void OutputRouting::setLiveMidiMode(byte data) { liveMidiMode = data; }

// class PressedNotesList
void PressedNotesList::appendKey(const byte &note, const byte &velocity) {
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

void PressedNotesList::deleteKey(const byte &note) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        PressedNotesElement *listElementPrev = listElement;

        while (listElement->note != note) {

            if (listElement->next) {
                listElementPrev = listElement;
                listElement = listElement->next;
            } else {
                return;
            }
        }
        if (pressedNoteElement->next == NULL) {
            delete listElement;
            pressedNoteElement = NULL;
        } else {
            if (listElementPrev->next == listElement->next) {
                pressedNoteElement = pressedNoteElement->next;
            } else {
                listElementPrev->next = listElement->next;
            }
            delete listElement;
        }
    }
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
}

bool PressedNotesList::containsElements() { return pressedNoteElement ? 1 : 0; }

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
    return NULL;
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
    return NULL;
}

PressedNotesElement *PressedNotesList::getKeyLatest() {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {

        while (listElement->next) {
            listElement = listElement->next;
        }

        return listElement;
    }
    return NULL;
}

// class LiveMidi for all real time midi data for a single midi input
void LiveMidi::keyPressed(const byte &note, const byte &velocity) {
    noteList.appendKey(note, velocity);
    triggered = 1;
}

void LiveMidi::keyReleased(const byte &note) {
    noteList.deleteKey(note);
    triggered = 1;
}

// bool, are any keys pressed
bool LiveMidi::keysPressed() { return noteList.containsElements() ? 1 : 0; }

// void LiveMidi::setMod(byte data) { mod = data; }

// void LiveMidi::setPitchbend(byte data) { pitchbend = data; }

// void LiveMidi::setAftertouch(byte data) { aftertouch = data; }

// void LiveMidi::setSustain(byte data) { sustain = data; }

// return highest key pressed
structKey LiveMidi::getKeyHighest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyHighest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

// return lowest key pressed
structKey LiveMidi::getKeyLowest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyLowest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

// return latest key pressed
structKey LiveMidi::getKeyLatest() {
    structKey key;
    PressedNotesElement *listElement = noteList.getKeyLatest();
    key.note = listElement->note;
    key.velocity = listElement->velocity;
    return key;
}

// byte LiveMidi::getMod() { return mod; }

// byte LiveMidi::getPitchbend() { return pitchbend; }

// byte LiveMidi::getAftertouch() { return aftertouch; }

// byte LiveMidi::getSustain() { return sustain; }

// new LiveMidi data trigger flag
byte LiveMidi::getTriggered() {
    if (triggered) {
        triggered = 0;
        return 1;
    } else {
        return 0;
    }
}

// reset performance
void LiveMidi::reset() {
    noteList.deleteAllKeys();
    mod = 0;
    pitchbend = 64;
    aftertouch = 0;
    sustain = 0;

    triggered = 1;
}

// Sequence data for each sequence
void Seq::init(const byte &note, const byte &gate, const byte &gateLength,
               const byte &tuning) { // init sequence to default values
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = note;  // test
        sequence.gate[i] = i % 2; // test
        sequence.gateLength[i] = gateLength;
    }
    sequence.tuning = tuning;
}

// Note
byte Seq::getNote(const byte &index) { // return note value
    return sequence.note[testByte(index, 0, LENGTH)];
}

void Seq::setNote(const byte &index, const byte &note) { // set note value
    sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE);
}

void Seq::setNotes(const byte &note) { // set note value

    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = testByte(testByte(note, 0, 88), 0, NOTERANGE); // set notes
    }
}

byte Seq::increaseNote(const byte &index) { // increase note value and return new note

    // index = testByte(index, 0, LENGTH); // testIndex

    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 255) {
        // note up
        sequence.note[index] = increaseByte(note, NOTERANGE);
    } else { // tuning active
        if (note < 88) {
            switch (((note + 1) + (12 - sequence.tuning)) % 12) {
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
    return sequence.note[index]; // return note
}

byte Seq::decreaseNote(const byte &index) { // decrease  note value and return new note
    // index = testByte(index, 0, LENGTH); // testIndex
    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 255) {
        // note down
        sequence.note[index] = decreaseByte(note, 0);
    } else { // tuning active
        if (note > 0) {
            switch (((note - 1) + (12 - sequence.tuning)) % 12) {
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
    return sequence.note[index]; /// return new note
}

byte Seq::changeNote(const byte &index, const int &change) { // change note
    // index = testByte(index, 0, LENGTH);        // test index
    sequence.note[index] = changeByte(sequence.note[index], change, 0, NOTERANGE, 1);

    return sequence.note[index];
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

// Gate
byte Seq::getGate(const byte &index) { // return gate value
    return sequence.gate[testByte(index, 0, LENGTH)];
}

void Seq::setGate(const byte &index, const byte &gate) { // set gate value
    sequence.gate[testByte(index, 0, LENGTH)] = testByte(gate, 0, 1);
}

byte Seq::toggleGate(const byte &index) { // toggle gate and return new stat;
    // index = testByte(index, 0, LENGTH);           // test index
    sequence.gate[index] = !sequence.gate[index]; // toggle gate

    return sequence.gate[index]; // return new gate
}

// one gate length
void Seq::setGateLength(const byte &index, const byte &gateLength) { // set gate length
    sequence.gateLength[testByte(index, 0, LENGTH)] = testByte(gateLength, 0, 255);
}

byte Seq::getGateLength(const byte &index) { // return gate length
    return sequence.gateLength[testByte(index, 0, LENGTH)];
}

byte Seq::changeGateLength(const byte &index, const int &change) { // change gate length
    // index = testByte(index, 0, LENGTH);              // test index
    sequence.gateLength[index] = changeByte(sequence.gateLength[index], change, 0, 100, 1);

    return sequence.gateLength[index];
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

void Seq::setTuning(const byte &tuning) { sequence.tuning = tuning; }

byte Seq::getTuning() { return sequence.tuning; }

// Data Singleton that handles all data, including all sequences, live midi and settings
void FrankData::receivedKeyPressed(const byte &channel, const byte &note, const byte &velocity) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].channel == 0 || config.routing[x].channel == channel) {
            liveMidi[x].keyPressed(note, velocity);
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

void FrankData::receivedMidiClock() { increaseMidiClock(); }
void FrankData::receivedMidiSongPosition(unsigned int spp) { setBpm16thCount(spp); }
void FrankData::receivedStart() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;
    set(play, 1);
}
void FrankData::receivedContinue() { set(play, 1); }
void FrankData::receivedStop() { set(play, 0); }

void FrankData::receivedReset() {
    stat.midiClockCount = 5;
    stat.bpm16thCount = 31;
    set(play, 0);

    for (byte x = 0; x < OUTPUTS; x++) {
        liveMidi[x].reset();
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
}

void FrankData::setBpm16thCount(unsigned int spp) {
    stat.midiClockCount = 5;
    stat.bpm16thCount = spp - 1;
}

inline void FrankData::calcBPM() {
    if (bpmSync) {
        static double bpmTimer = 0;
        set(bpm, (int)(60000. / (millis() - bpmTimer)));

        bpmTimer = millis();
    }
}

// int FrankData::getBPM() { return stat.bpm; } // return MidiSource

inline void FrankData::increaseStep() {
    if (!((stat.stepSeq + 1) % STEPPERPAGE)) {                      // if we make a pageJump
        if (config.nbPages <= ((stat.stepSeq + 1) / STEPPERPAGE)) { // newPage above number of pages
            stat.stepSeq = 0;                                       // set stepSeq 0
        } else {                                                    // new page is valid.
            stat.stepSeq++;                                         // increase Step
        }
    } else {
        stat.stepSeq++; // increase Step
    }

    calcBPM();
}

inline void FrankData::decreaseStep() {
    if (stat.stepSeq == 0) {                             // we jump to the last page?
        stat.stepSeq = config.nbPages * STEPPERPAGE - 1; // set to max stepSeq

    } else if ((!stat.stepSeq % STEPPERPAGE)) {              // we make a pageJump?
        if (config.nbPages > (stat.stepSeq / STEPPERPAGE)) { // newPage above number of pages
            stat.stepSeq = config.nbPages * STEPPERPAGE - 1; // set jump to last stepSeq
        } else {                                             // new page is valid.
            stat.stepSeq--;                                  // decrease Step
        }
    } else {
        stat.stepSeq--; // decrease Step
    }

    calcBPM();
}

inline byte FrankData::getCurrentPageNumber() { // number of pages, takes care if page number has changed
    if (config.nbPages > (stat.stepSeq / 8))
        return config.nbPages;     // is our stepSeq above the current number of pages?
    return (stat.stepSeq / 8 + 1); // return current stepSeq page until the next page jump
}

// Seq *FrankData::getSeqObject() { return seq; }

// Singleton
FrankData *FrankData::mainData = nullptr;
FrankData &FrankData::getDataObj() {
    if (mainData == nullptr)
        // mainData = new FrankData();
        static FrankData mainData;
    return *mainData;
}

inline structKey FrankData::getLiveKeyEvaluated(const byte &array) {
    switch (config.routing[array].liveMidiMode) {
    case 0: return liveMidi[array].getKeyLatest();
    case 1: return liveMidi[array].getKeyLowest();
    case 2: return liveMidi[array].getKeyHighest();
    default:
        PRINTLN("FrankData getLiveKeyEvaluated, no case found");
        structKey emptyKey;
        emptyKey.note = 0;
        emptyKey.velocity = 0;
        return emptyKey;
    }
}

inline byte FrankData::getLiveCcEvaluated(const byte &array) {
    switch (config.routing[array].cc) {
    case 0: return getLiveKeyEvaluated(array).velocity;
    case 1: return liveMidi[array].mod;
    case 2: return liveMidi[array].pitchbend;
    case 3: return liveMidi[array].aftertouch;
    case 4: return liveMidi[array].sustain;
    }
}

inline byte FrankData::getOutputLiveModeEvaluated(const byte &array) {
    switch (config.routing[array].liveMidiMode) {
    case 0: return getLiveKeyEvaluated(array).velocity;
    case 1: return liveMidi[array].mod;
    case 2: return liveMidi[array].pitchbend;
    case 3: return liveMidi[array].aftertouch;
    case 4: return liveMidi[array].sustain;
    }
}

// TO DO
inline byte FrankData::getOutputClockEvaluated(const byte &array) { return config.routing[array].clock + 1; }

// Screen config
// void FrankData::setSubScreen(byte subScreen, byte max) { stat.screen.subscreen = testByte(subScreen, 0, max); }
void FrankData::resetSubScreen() { stat.screen.subscreen = FrankData::screenNote; }

inline const byte FrankData::getSubscreenMax() {
    return config.routing[stat.screen.channel].outSource ? stat.screen.subScreenMaxSeq : stat.screen.subScreenMaxLive;
}

// new set and get functions
byte FrankData::get(const frankData &frankDataType) {
    switch (frankDataType) {
    case midiSource: return config.midiSource;
    case nbPages: return config.nbPages;
    case direction: return config.direction;
    case displayBrightness: return config.displayBrightness;

    case screenOutputChannel: return stat.screen.channel;
    case screenConfig: return stat.screen.config;
    case screenMainMenu: return stat.screen.mainMenu;
    case screenSubScreen: return stat.screen.subscreen;

    case stepArp: return stat.stepArp;
    case stepSeq: return stat.stepSeq;
    case activePage: return (stat.stepSeq / STEPPERPAGE);
    case stepOnPage: return (stat.stepSeq - (get(activePage) * STEPPERPAGE));
    case currentPageNumber: return getCurrentPageNumber();
    case bpm: return stat.bpm;
    case play: return stat.play;
    case rec: return stat.rec;
    case error: return stat.error;
    case bpmSync: return stat.bpmSync;
    case bpmPoti: return stat.bpmPoti;

    default: PRINTLN("FrankData get(frankData frankDataType), no case found"); return 0;
    }
}

byte FrankData::get(const frankData &frankDataType, const byte &array) {
    switch (frankDataType) {
    case outputSource: return config.routing[array].outSource;
    case outputChannel: return config.routing[array].channel;
    case outputSeq: return config.routing[array].seq;
    case outputArp: return config.routing[array].arp;
    case outputCc: return config.routing[array].cc;
    case outputCcEvaluated: return getLiveCcEvaluated(array);
    case outputLiveMode: return config.routing[array].liveMidiMode;
    case outputClock: return config.routing[array].clock;

    case liveMod: return liveMidi[array].mod;
    case livePitchbend: return liveMidi[array].pitchbend;
    case liveAftertouch: return liveMidi[array].aftertouch;
    case liveSustain: return liveMidi[array].sustain;
    case liveTriggered: return liveMidi[array].triggered;
    case liveKeyNoteEvaluated: return getLiveKeyEvaluated(array).note;
    case liveKeyVelEvaluated: return getLiveKeyEvaluated(array).velocity;
    case liveLatestKey: return liveMidi[array].getKeyLatest().note;
    case liveLowestKey: return liveMidi[array].getKeyLowest().note;
    case liveHighestKey: return liveMidi[array].getKeyHighest().note;

    case seqTuning: return seq[array].sequence.tuning;

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

    default: PRINTLN("FrankData get(frankData frankDataType, byte array, byte step), no case found"); return 0;
    }
}

inline char *FrankData::getAsStr(const frankData &frankDataType) { return toStr(get(frankDataType)); }
inline char *FrankData::getAsStr(const frankData &frankDataType, const byte &array) {
    return toStr(get(frankDataType, array));
}
inline char *FrankData::getAsStr(const frankData &frankDataType, const byte &array, const byte &step) {
    return toStr(get(frankDataType, array, step));
}

void FrankData::set(const frankData &frankDataType, const byte &data, const bool &clampChange) {
    switch (frankDataType) {
    case midiSource: config.midiSource = testByte(data, 0, 1, clampChange);
    case nbPages: config.nbPages = testByte(data, 1, PAGES, clampChange);
    case direction: config.direction = testByte(data, 0, 1, clampChange);
    case displayBrightness: config.displayBrightness = testByte(data, 0, 100, clampChange); // how high?

    case screenOutputChannel: stat.screen.channel = testByte(data, 0, OUTPUTS - 1, clampChange);
    case screenConfig: stat.screen.config = testByte(data, 0, 1, clampChange);
    case screenMainMenu: stat.screen.mainMenu = testByte(data, 0, 1, clampChange);
    case screenSubScreen: stat.screen.subscreen = testByte(data, 0, getSubscreenMax(), clampChange);

    case stepArp: stat.stepArp = testByte(data, 0, NOTERANGE, clampChange);
    case stepSeq: stat.stepSeq = testByte(data, 0, STEPPERPAGE * config.nbPages - 1, clampChange);
    case bpm: stat.bpm = testByte(data, 0, 255, clampChange);
    case play: stat.play = testByte(data, 0, 1, clampChange);
    case rec: stat.rec = testByte(data, 0, 1, clampChange);
    case error: stat.error = testByte(data, 0, 1, clampChange);
    case bpmSync: stat.bpmSync = testByte(data, 0, 1, clampChange);
    case bpmPoti: stat.bpmPoti = testByte(data, 0, 255, clampChange);

    default: PRINTLN("FrankData set(frankData frankDataType, byte data, bool clampChange = 0), no case found");
    }
}

void FrankData::set(const frankData &frankDataType, const byte &data, const byte &array, const bool &clampChange) {
    switch (frankDataType) {
    case outputSource: config.routing[array].outSource = testByte(data, 0, 1, clampChange);
    case outputChannel: config.routing[array].channel = testByte(data, 0, 16, clampChange);
    case outputSeq: config.routing[array].seq = testByte(data, 0, OUTPUT, clampChange);
    case outputArp: config.routing[array].arp = testByte(data, 0, 1, clampChange);
    case outputCc: config.routing[array].cc = testByte(data, 0, 4, clampChange);
    case outputLiveMode: config.routing[array].liveMidiMode = testByte(data, 0, 2, clampChange);
    case outputClock: config.routing[array].clock = testByte(data, 0, 5, clampChange);

    case liveMod: liveMidi[array].mod = testByte(data, 0, 127, clampChange);
    case livePitchbend: liveMidi[array].pitchbend = testByte(data, 0, 255, clampChange);
    case liveAftertouch: liveMidi[array].aftertouch = testByte(data, 0, 127, clampChange);
    case liveSustain: liveMidi[array].sustain = testByte(data, 0, 127, clampChange);
    case liveTriggered: liveMidi[array].triggered = testByte(data, 0, 1, clampChange);

    case seqTuning: seq[array].sequence.tuning = testByte(data, 0, 255, clampChange);

    default: PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, bool clampChange), no case found");
    }
}
void FrankData::set(const frankData &frankDataType, const byte &data, const byte &array, const byte &step,
                    const bool &clampChange) {
    switch (frankDataType) {
    case seqNote: seq[array].sequence.note[step] = testByte(data, 0, 127, clampChange);
    case seqGate: seq[array].sequence.gate[step] = testByte(data, 0, 1, clampChange);
    case seqGateLength: seq[array].sequence.gateLength[step] = testByte(data, 0, 100, clampChange);
    case seqCc: seq[array].sequence.cc[step] = testByte(data, 0, 127, clampChange);
    case seqVelocity: seq[array].sequence.velocity[step] = testByte(data, 0, 127, clampChange);

    default:
        PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, byte step, bool clampChange), no case "
                "found");
    }
}

inline void FrankData::change(const frankData &frankDataType, const byte &amount, const bool &clampChange) {
    set(frankDataType, get(frankDataType) + amount, clampChange);
}
inline void FrankData::change(const frankData &frankDataType, const byte &amount, const byte &array,
                              const bool &clampChange) {
    set(frankDataType, get(frankDataType, array) + amount, array, clampChange);
}
inline void FrankData::change(const frankData &frankDataType, const byte &amount, const byte &array, const byte &step,
                              const bool &clampChange) {
    set(frankDataType, get(frankDataType, array, step) + amount, array, step, clampChange);
}

inline void FrankData::increase(const frankData &frankDataType, const bool &clampChange) {
    switch (frankDataType) {
    case stepSeq: increaseStep(); break;
    default: change(frankDataType, 1, clampChange);
    }
}
inline void FrankData::increase(const frankData &frankDataType, const byte &array, const bool &clampChange) {
    change(frankDataType, 1, array, clampChange);
}
inline void FrankData::increase(const frankData &frankDataType, const byte &array, const byte &step,
                                const bool &clampChange) {
    change(frankDataType, 1, array, step, clampChange);
}

inline void FrankData::decrease(const frankData &frankDataType, const bool &clampChange) {
    switch (frankDataType) {
    case stepSeq: decreaseStep(); break;
    default: change(frankDataType, -1, clampChange);
    }
}
inline void FrankData::decrease(const frankData &frankDataType, const byte &array, const bool &clampChange) {
    change(frankDataType, -1, array, clampChange);
}
inline void FrankData::decrease(const frankData &frankDataType, const byte &array, const byte &step,
                                const bool &clampChange) {
    change(frankDataType, -1, array, step, clampChange);
}

void FrankData::toggle(const frankData &frankDataType) {
    switch (frankDataType) {
    case direction: config.direction = toggleByte(config.direction); break;
    case play: stat.play = toggleByte(stat.play); break;
    case rec: stat.rec = toggleByte(stat.rec); break;
    case bpmSync: stat.bpmSync = toggleByte(stat.bpmSync); break;
    case screenMainMenu: stat.screen.mainMenu = toggleByte(stat.screen.mainMenu); break;
    case screenConfig: stat.screen.config = toggleByte(stat.screen.config); break;

    default: PRINTLN("FrankData toggle(frankData frankDataType), no case found");
    }
}

const char *FrankData::getName(const frankData &frankDataType) {
    switch (frankDataType) {
    case seqNote: setStr("Note"); break;
    case seqGate: setStr("Gate"); break;
    case seqGateLength: setStr("Gate Ln"); break;
    case seqCc: setStr("MidiCC"); break;
    case seqVelocity: setStr("Vel"); break;
    case seqTuning: setStr("Tuning"); break;
    case seqSize: setStr("Seq Size"); break;

    case midiSource: setStr("Midi Src"); break;
    case nbPages: setStr("Pages"); break;
    case direction: setStr("Fw/Rev"); break;
    case displayBrightness: setStr("Screen Br"); break;

    case outputSource: setStr("Out Src"); break;
    case outputChannel: setStr("Midi Ch"); break;
    case outputSeq: setStr("Seq"); break;
    case outputArp: setStr("Arp"); break;
    case outputCc: setStr("MidiCC"); break;
    case outputLiveMode: setStr("Key sel"); break;
    case outputClock: setStr("Clock");break;

    case screenOutputChannel: setStr("Routing"); break;
    case screenConfig: setStr("Config"); break;
    case screenMainMenu: setStr("Main"); break;
    case screenSubScreen: setStr("Sub"); break;

    case stepSeq: setStr("Step"); break;
    case stepArp: setStr("Arp Step"); break;
    case bpm: setStr("BPM"); break;
    case play: setStr("Play"); break;
    case rec: setStr("Rec"); break;
    case error: setStr("Error"); break;
    case bpmSync: setStr("Sync"); break;
    case bpmPoti: setStr("BPM Pot"); break;

    case liveMod: setStr("Mod"); break;
    case livePitchbend: setStr("PB"); break;
    case liveAftertouch: setStr("AFT"); break;
    case liveSustain: setStr("SUS"); break;
    case liveTriggered: setStr("TRIG"); break;
    case liveLatestKey: setStr("La Key"); break;
    case liveLowestKey: setStr("Lo Key"); break;
    case liveHighestKey: setStr("Hi Key"); break;

    default: PRINTLN("FrankData getName(frankData frankDataType), no case found"); setStr("NONE");
    }
    return returnStr;
}
const char *FrankData::getValueName(const frankData &frankDataType) {
    switch (frankDataType) {
    case seqTuning: setStr("Tuning"); break;
    case seqSize: setStr("seqSize"); break;

    case midiSource: setStr("Midi Src"); break;
    case nbPages: setStr("Pages"); break;
    case direction: setStr("Direction"); break;
    case displayBrightness: setStr("Brightness"); break;

    case outputSource: setStr("Out Src"); break;
    case outputChannel: setStr("Midi Ch"); break;
    case outputSeq: setStr("Seq"); break;
    case outputArp: setStr("Arp"); break;

    case outputCc:
        switch (config.routing[stat.screen.channel].cc) {
        case 0: setStr("Vel"); break;
        case 1: setStr("Mod"); break;
        case 2: setStr("Pitchbend"); break;
        case 3: setStr("Aftertouch"); break;
        case 4: setStr("Sustain"); break;
        default: setStr("NONE"); break;
        }

    case outputLiveMode:
        switch (config.routing[stat.screen.channel].liveMidiMode) {
        case 0: setStr("latest"); break;
        case 1: setStr("lowest"); break;
        case 2: setStr("highest"); break;
        default: setStr("NONE"); break;
        }

    case outputClock:
        switch (config.routing[stat.screen.channel].clock) {
        case 0: setStr("16th"); break;
        case 1: setStr("8th"); break;
        case 2: setStr("quarter"); break;
        case 3: setStr("half"); break;
        case 4: setStr("1 bar"); break;
        case 5: setStr("2 bar"); break;
        default: setStr("NONE"); break;
        }

    case screenOutputChannel: setStr("Output Routing"); break;
    case screenConfig: setStr("Config"); break;
    case screenMainMenu: setStr("Main"); break;
    case screenSubScreen: setStr("Sub"); break;

    case stepSeq: setStr("Step"); break;
    case stepArp: setStr("Arp Step"); break;
    case bpm: setStr("BPM"); break;
    case play: setStr("Play"); break;
    case rec: setStr("Rec"); break;
    case error: setStr("Error"); break;
    case bpmSync: setStr("Sync"); break;
    case bpmPoti: setStr("internal BPM"); break;

    case liveMod: setStr("Mod"); break;
    case livePitchbend: setStr("PitchBend"); break;
    case liveAftertouch: setStr("Aftertouch"); break;
    case liveSustain: setStr("Sustain"); break;
    case liveTriggered: setStr("Triggered"); break;
    case liveLatestKey: setStr("Latest Key"); break;
    case liveLowestKey: setStr("Lowest Key"); break;
    case liveHighestKey: setStr("Highest Key"); break;

    default:
        PRINTLN("FrankData getValueName(frankData frankDataType), no case found");
        setStr("NONE");
    }
    return returnStr;
}
const char *FrankData::getValueName(const frankData &frankDataType, const byte &step) {
    switch (frankDataType) {
    case seqNote: setStr("Note"); break;
    case seqGate: setStr("Gate"); break;
    case seqGateLength: setStr("Gate Length"); break;
    case seqCc: setStr("MidiCC"); break;
    case seqVelocity: setStr("Vel"); break;

    default:
        PRINTLN("FrankData getValueName(const frankData &frankDataType, const byte &step), no case found");
        setStr("NONE");
    }
    return returnStr;
}

char FrankData::valueToNote(const byte &noteIn) {

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

char FrankData::valueToOctave(const byte &noteIn) {

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

char FrankData::valueToSharp(const byte &noteIn) {
    byte note;
    note = (noteIn + 9) % 12;

    if (note == 1 or note == 3 or note == 6 or note == 8 or note == 10) {
        return '#';
    }

    return '\0';
}

char *FrankData::tuningToChar(const byte &tuning) {

    switch (tuning) {
    case 0: return "C";
    case 1: return "C#";
    case 2: return "D";
    case 3: return "D#";
    case 4: return "E";
    case 5: return "F";
    case 6: return "F#";
    case 7: return "G";
    case 8: return "G#";
    case 9: return "A";
    case 10: return "A#";
    case 11: return "B";
    case 12: return "F";
    default: return "-";
    }
}

inline void FrankData::setStr(const char *newStr) {
    strncpy(str, newStr, MAXSTRINGSIZE - 1);
    str[MAXSTRINGSIZE-1] = '\0';
}

// utility
// test byte range and return valid byte
inline byte testByte(const byte &value, const byte &minimum, const byte &maximum, const bool &clampChange) {
    if (value > maximum) {

        PRINTLN("testByte: value was to big");
        PRINTLN("INPUT: ");
        PRINTLN(value);

        return maximum;

    } else if (value < minimum) {

        PRINTLN("testByte: value was to small");
        PRINTLN("INPUT: ");
        PRINTLN(value);

        return minimum;

    } else {
        return value;
    }
}

// increase byte
inline byte increaseByte(const byte &value, const byte &maximum) {
    if (value == maximum) {
        return value;
    } else {
        return value + 1;
    }
}

// decrease byte
inline byte decreaseByte(const byte &value, const byte &minimum) {
    if (value == minimum) {
        return value;
    } else {
        return value - 1;
    }
}

// change byte value and check boundaries
inline byte changeByte(const byte &value, const int &change, const byte &minimum, const byte &maximum,
                       const bool &clampChange) {

    if ((int)value + change >= maximum) { // test max

        return clampChange ? maximum : value;
    } else if ((int)value + change <= minimum) { // test min
        return clampChange ? minimum : value;
    } else {
        return (byte)((int)value + change); // return new value
    }
}

// toggle byte
template <typename T> inline T toggleByte(const T &data) { return !data; }

// convert number to string
template <typename T> inline char *toStr(const T &data) {
    static char str[4];
    itoa((int)data, str, 10);
    return str;
}
