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
byte OutputRouting::getOut() { return outSource; }
byte OutputRouting::getChannel() { return channel; }
byte OutputRouting::getSeq() { return seq; }
byte OutputRouting::getArp() { return arp; }
byte OutputRouting::getCc() { return cc; }
byte OutputRouting::getliveMidiMode() { return liveMidiMode; }

void OutputRouting::setOut(byte data) { outSource = data; }
void OutputRouting::setChannel(byte data) { channel = data; }
void OutputRouting::setSeq(byte data) { seq = data; }
void OutputRouting::setArp(byte data) { arp = data; }
void OutputRouting::setCc(byte data) { cc = data; }
void OutputRouting::setLiveMidiMode(byte data) { liveMidiMode = data; }

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
void LiveMidi::keyPressed(byte note, byte velocity) {
    noteList.appendKey(note, velocity);
    triggered = 1;
}

void LiveMidi::keyReleased(byte note) {
    noteList.deleteKey(note);
    triggered = 1;
}

// bool, are any keys pressed
bool LiveMidi::keysPressed() { return noteList.containsElements() ? 1 : 0; }

void LiveMidi::setMod(byte data) { mod = data; }

void LiveMidi::setPitchbend(byte data) { pitchbend = data; }

void LiveMidi::setAftertouch(byte data) { aftertouch = data; }

void LiveMidi::setSustain(byte data) { sustain = data; }

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

byte LiveMidi::getMod() { return mod; }

byte LiveMidi::getPitchbend() { return pitchbend; }

byte LiveMidi::getAftertouch() { return aftertouch; }

byte LiveMidi::getSustain() { return sustain; }

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
void Seq::init(byte note, byte gate, byte gateLength, byte tuning) { // init sequence to default values
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = note;  // test
        sequence.gate[i] = i % 2; // test
        sequence.gateLength[i] = gateLength;
    }
    sequence.tuning = tuning;
}

// Note
byte Seq::getNote(byte index) { // return note value
    return sequence.note[testByte(index, 0, LENGTH)];
}

void Seq::setNote(byte index, byte note) { // set note value
    sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE);
}

void Seq::setNotes(byte note) {   // set note value
    note = testByte(note, 0, 88); // test note value in range

    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = testByte(note, 0, NOTERANGE); // set notes
    }
}

byte Seq::increaseNote(byte index) { // increase note value and return new note

    index = testByte(index, 0, LENGTH); // testIndex

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

byte Seq::decreaseNote(byte index) {    // decrease  note value and return new note
    index = testByte(index, 0, LENGTH); // testIndex
    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 255) {
        // note down
        sequence.note[index] = decreaseByte(note, NOTERANGE);
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

byte Seq::changeNote(byte index, int change) { // change note
    index = testByte(index, 0, LENGTH);        // test index
    sequence.note[index] = changeByte(sequence.note[index], change, 0, NOTERANGE, 1);

    return sequence.note[index];
}

void Seq::changeNotes(int change) { // change note
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
byte Seq::getGate(byte index) { // return gate value
    return sequence.gate[testByte(index, 0, LENGTH)];
}

void Seq::setGate(byte index, byte gate) { // set gate value
    sequence.gate[testByte(index, 0, LENGTH)] = testByte(gate, 0, 1);
}

byte Seq::toggleGate(byte index) {                // toggle gate and return new stat;
    index = testByte(index, 0, LENGTH);           // test index
    sequence.gate[index] = !sequence.gate[index]; // toggle gate

    return sequence.gate[index]; // return new gate
}

// one gate length
void Seq::setGateLength(byte index, byte gateLength) { // set gate length
    sequence.gateLength[testByte(index, 0, LENGTH)] = testByte(gateLength, 0, 255);
}

byte Seq::getGateLength(byte index) { // return gate length
    return sequence.gateLength[testByte(index, 0, LENGTH)];
}

byte Seq::changeGateLength(byte index, int change) { // change gate length
    index = testByte(index, 0, LENGTH);              // test index
    sequence.gateLength[index] = changeByte(sequence.gateLength[index], change, 0, 100, 1);

    return sequence.gateLength[index];
}

// all gate lengths
void Seq::setGateLengths(byte gateLength) {    // set all gates at once
    gateLength = testByte(gateLength, 0, 255); // test gate length
    for (int i = 0; i < LENGTH; i++) {
        sequence.gate[i] = gateLength; // set new gates
    }
}

void Seq::changeGateLengths(int change) { // change gate length
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

void Seq::setTuning(byte tuning) { sequence.tuning = tuning; }

byte Seq::getTuning() { return sequence.tuning; }

// Data Singleton that handles all data, including all sequences, live midi and settings
void FrankData::receivedKeyPressed(byte channel, byte note, byte velocity) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].keyPressed(note, velocity);
        }
    }
}

void FrankData::receivedKeyReleased(byte channel, byte note) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].keyReleased(note);
        }
    }
}

void FrankData::receivedMod(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].mod = data;
        }
    }
}

void FrankData::receivedPitchbend(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].pitchbend = data;
        }
    }
}

void FrankData::receivedAftertouch(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setAftertouch(data);
        }
    }
}

void FrankData::receivedSustain(byte channel, byte data) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].getChannel() == 0 || config.routing[x].getChannel() == channel) {
            liveMidi[x].setSustain(data);
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

byte FrankData::getBpm16thCount() { return stat.bpm16thCount; }

// stat
byte FrankData::getSync() { return stat.bpmSync; }

void FrankData::setSync(byte bpmSync) { stat.bpmSync = bpmSync; }

void FrankData::setRec(byte rec) { stat.rec = rec; }

byte FrankData::getRec() { return stat.rec; }

void FrankData::setBPM(int bpm) { stat.bpm = bpm; }

void FrankData::calcBPM() {
    static double bpmTimer = 0;
    setBPM((int)(60000. / (millis() - bpmTimer)));

    bpmTimer = millis();
}

int FrankData::getBPM() { return stat.bpm; } // return MidiSource

void FrankData::increaseStep() {
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

void FrankData::decreaseStep() {
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

void FrankData::setStep(byte stepSeq) { stat.stepSeq = testByte(stepSeq, 0, STEPPERPAGE * config.nbPages - 1); }

byte FrankData::getActivePage() { return (stat.stepSeq / STEPPERPAGE); }
byte FrankData::getStepOnPage() { return (stat.stepSeq - (getActivePage() * STEPPERPAGE)); }
byte FrankData::getStep() { return stat.stepSeq; } // return MidiSource

// byte FrankData::getPlayStop() { return stat.play; }
// void FrankData::setPlayStop(byte mode) { stat.play = mode; }

// void FrankData::togglePlayStop() { stat.play = !stat.play; }

// byte FrankData::getDirection() { return config.direction; }
// void FrankData::setDirection(byte direction) { config.direction = direction; }

// byte FrankData::getError() { return stat.error; }
// void FrankData::setError(byte error) { stat.error = error; }

// config
// byte FrankData::getDisplayBrightness() { return config.displayBrightness; }
// void FrankData::setDisplayBrightness(byte brightness) { config.displayBrightness = brightness; }

// void FrankData::setMidiSource(byte midi) { config.midiSource = testByte(midi, 0, 1); }
// byte FrankData::getMidiSource() { return config.midiSource; }

// void FrankData::setNumberPages(byte nbPages) { config.nbPages = testByte(nbPages, 1, PAGES); }
// byte FrankData::getNumberPages() { return config.nbPages; }

byte FrankData::getCurrentNumberPages() { // number of pages, takes care if page number has changed
    if (config.nbPages > (getStep() / 8))
        return config.nbPages;  // is our stepSeq above the current number of pages?
    return (getStep() / 8 + 1); // return current stepSeq page until the next page jump
}

Seq *FrankData::getSeqObject() { return seq; }

FrankData *FrankData::mainData = nullptr;

FrankData &FrankData::getDataObj() {
    if (mainData == nullptr)
        mainData = new FrankData();
    return *mainData;
}

// Testclass *Testclass::mainData = nullptr;

// Screen config
void FrankData::setSubScreen(byte subScreen, byte max) { stat.screen.subscreen = testByte(subScreen, 0, max); }
byte FrankData::getSubScreen() { return stat.screen.subscreen; }
void FrankData::resetSubScreen() { stat.screen.subscreen = FrankData::screenNote; }

void FrankData::increaseSubScreen(byte max) { stat.screen.subscreen = testByte(stat.screen.subscreen + 1, 0, max); }
void FrankData::decreaseSubScreen() { stat.screen.subscreen = testByte(stat.screen.subscreen - 1, 0); }

byte FrankData::getScreenConfig(byte screen) { return stat.screen.config; }
void FrankData::toggleScreenConfig() { stat.screen.config = !stat.screen.config; }

byte FrankData::getScreenChannel() { return stat.screen.channel; }
void FrankData::setScreenChannel(byte channel) { stat.screen.channel = testByte(channel, 0, OUTPUTS - 1); }

byte FrankData::getMainMenuEnabled() { return stat.screen.mainMenu; }
void FrankData::toogleMainMenu() { stat.screen.mainMenu = !stat.screen.mainMenu; }

/*get, change und toggleData...
 wäre cool wenn wir die daten über ne funktion auch zum nem string convertieren könnten, dann muss ich das nicht im
display immer machen. bin mir gerade aber grunsätzlich etwas unschlüssig wie man das mit den listen baut. haben ja nicht
nur daten die 1mal vorliegen sondern auch teilweise mehrfach in den jeweiligen channels... mal mit index mal ohne ...
 müsste die funktion dann jeweils entscheiden welcher der aktuelle channel ist und dann aus wlcher liste sie die daten
holt?

Index kann ich ja übergeben.. und bei index 0 ließt er ja bei ner normalen variable einfach die Variable aus also das
könnte eventuell von alleine funktionieren.

Für die einzelnen funktionen müssen wie warscheinlich einzelne Listen anlegen:

Also für toggle, get und change.

Toggle können wir ja mit TXXX angeben
Change mit CXXX und get einfach XXX.

haben ja variablen wie GATE die nur getoggelt werden mit den push auf den Encoder.

 Wenn wir die Daten zu string konvertieren haben wir auch noch das Problem das dann ein char pointer übergeben wird der
auf die lokale variable der Funktion zeit, muss dann statisch angelegt werden.. und deshalb jedesamal resettet werden,
auch nicht so richtig geil.


Alle funktionen hier drunter sind erstmal nur als dummy geschrieben, damit ich den display kram testen konnte:
*/

// new set and get functions
byte FrankData::get(frankData frankDataType) {
    switch (frankDataType) {
    case midiSource: return config.midiSource;
    case nbPages: return config.nbPages;
    case direction: return config.direction;
    case displayBrightness: return config.displayBrightness;

    case screenChannel: return stat.screen.channel;
    case screenConfig: return stat.screen.config;
    case screenMainMenu: return stat.screen.mainMenu;
    case screenSubScreen: return stat.screen.subscreen;

    case stepArp: return stat.stepArp;
    case stepSeq: return stat.stepSeq;
    case bpm: return stat.bpm;
    case play: return stat.play;
    case rec: return stat.rec;
    case error: return stat.error;
    case bpmSync: return stat.bpmSync;
    case bpmPoti: return stat.bpmPoti;

    default: PRINTLN("FrankData get(frankData frankDataType), no case found"); return 0;
    }
}

byte FrankData::get(frankData frankDataType, byte array) {
    switch (frankDataType) {
    case outputSource: return config.routing[array].outSource;
    case outputChannel: return config.routing[array].channel;
    case outputSeq: return config.routing[array].seq;
    case outputArp: return config.routing[array].arp;
    case outputCc: return config.routing[array].cc;
    case outputLiveMode: return config.routing[array].liveMidiMode;
    case outputClock: return config.routing[array].clock;

    case liveMod: return liveMidi[array].mod;
    case livePitchbend: return liveMidi[array].pitchbend;
    case liveAftertouch: return liveMidi[array].aftertouch;
    case liveSustain: return liveMidi[array].sustain;
    case liveTriggered: return liveMidi[array].triggered;

    case seqTuning: return seq[array].sequence.tuning;

    default: PRINTLN("FrankData get(frankData frankDataType, byte array), no case found"); return 0;
    }
}

byte FrankData::get(frankData frankDataType, byte array, byte step) {
    switch (frankDataType) {
    case seqNote: return seq[array].sequence.note[step];
    case seqGate: return seq[array].sequence.gate[step];
    case seqGateLength: return seq[array].sequence.gateLength[step];
    case seqCc: return seq[array].sequence.cc[step];
    case seqVelocity: return seq[array].sequence.velocity[step];

    default: PRINTLN("FrankData get(frankData frankDataType, byte array, byte step), no case found"); return 0;
    }
}

void FrankData::set(frankData frankDataType, byte data, bool clampChange) {
    switch (frankDataType) {
    case midiSource: config.midiSource = testByte(data, 0, 1, clampChange);
    case nbPages: config.nbPages = testByte(data, 1, PAGES, clampChange);
    case direction: config.direction = testByte(data, 0, 1, clampChange);
    case displayBrightness: config.displayBrightness = testByte(data, 0, 100, clampChange); // how high?

    case screenChannel: stat.screen.channel = testByte(data, 0, 1, clampChange);
    case screenConfig: stat.screen.config = testByte(data, 0, 1, clampChange);
    case screenMainMenu: stat.screen.mainMenu = testByte(data, 0, 1, clampChange);
    case screenSubScreen: stat.screen.subscreen = testByte(data, 0, 4, clampChange);

    case stepArp: stat.stepArp = testByte(data, 0, NOTERANGE, clampChange);
    case stepSeq: stat.stepSeq = testByte(data, 0, LENGTH, clampChange);
    case bpm: stat.bpm = testByte(data, 0, 255, clampChange);
    case play: stat.play = testByte(data, 0, 1, clampChange);
    case rec: stat.rec = testByte(data, 0, 1, clampChange);
    case error: stat.error = testByte(data, 0, 1, clampChange);
    case bpmSync: stat.bpmSync = testByte(data, 0, 1, clampChange);
    case bpmPoti: stat.bpmPoti = testByte(data, 0, 255, clampChange);

    default: PRINTLN("FrankData set(frankData frankDataType, byte data, bool clampChange = 0), no case found");
    }
}

void FrankData::set(frankData frankDataType, byte data, byte array, bool clampChange) {
    switch (frankDataType) {
    case outputSource: config.routing[array].outSource = testByte(data, 0, 1, clampChange);
    case outputChannel: config.routing[array].channel = testByte(data, 0, 16, clampChange);
    case outputSeq: config.routing[array].seq = testByte(data, 0, OUTPUT, clampChange);
    case outputArp: config.routing[array].arp = testByte(data, 0, 1, clampChange);
    case outputCc: config.routing[array].cc = testByte(data, 0, 4, clampChange);
    case outputLiveMode: config.routing[array].liveMidiMode = testByte(data, 0, 2, clampChange);
    case outputClock: config.routing[array].clock = testByte(data, 0,5, clampChange);

    case liveMod: liveMidi[array].mod = testByte(data, 0, 127, clampChange);
    case livePitchbend: liveMidi[array].pitchbend = testByte(data, 0, 255, clampChange);
    case liveAftertouch: liveMidi[array].aftertouch = testByte(data, 0, 127, clampChange);
    case liveSustain: liveMidi[array].sustain = testByte(data, 0, 127, clampChange);
    case liveTriggered: liveMidi[array].triggered = testByte(data, 0, 1, clampChange);

    case seqTuning: seq[array].sequence.tuning = testByte(data, 0, 255, clampChange);

    default:
        PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, bool clampChange), no case found");
    }
}
void FrankData::set(frankData frankDataType, byte data, byte array, byte step, bool clampChange) {
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

inline void FrankData::change(frankData frankDataType, byte amount, bool clampChange) {
    set(frankDataType, get(frankDataType) + amount, clampChange);
}
inline void FrankData::change(frankData frankDataType, byte amount, byte array, bool clampChange) {
    set(frankDataType, get(frankDataType, array) + amount, array, clampChange);
}
inline void FrankData::change(frankData frankDataType, byte amount, byte array, byte step, bool clampChange) {
    set(frankDataType, get(frankDataType, array, step) + amount, array, step, clampChange);
}

inline void FrankData::increase(frankData frankDataType, bool clampChange) { change(frankDataType, 1, clampChange); }
inline void FrankData::increase(frankData frankDataType, byte array, bool clampChange) {
    change(frankDataType, 1, array, clampChange);
}
inline void FrankData::increase(frankData frankDataType, byte array, byte step, bool clampChange) {
    change(frankDataType, 1, array, step, clampChange);
}

inline void FrankData::decrease(frankData frankDataType, bool clampChange) { change(frankDataType, -1, clampChange); }
inline void FrankData::decrease(frankData frankDataType, byte array, bool clampChange) {
    change(frankDataType, -1, array, clampChange);
}
inline void FrankData::decrease(frankData frankDataType, byte array, byte step, bool clampChange) {
    change(frankDataType, -1, array, step, clampChange);
}

void FrankData::toggle(frankData frankDataType) {
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

// Jacobs test
void FrankData::setData(byte data, byte index) {} // brauchen wir warscheinlich nicht?
char *FrankData::getDataString(byte data, byte index) {
    int value = 100;
    int channel = getScreenChannel();

    //  data = (int)*list[channel][data][index];

    static char string[4];
    itoa(value, string, 10);

    return string;
}

int FrankData::getData(byte data, byte index) {
    int value = 100;
    int channel = getScreenChannel();

    //  data = (int)*list[channel][data][index];

    return data;
}

char *FrankData::getDataName(byte data) {
    static char string[] = "test";

    return string;
}

void FrankData::toggleData(byte id, byte index) {}

void FrankData::changeData(byte id, byte index, byte direction) {}

///////////////

byte FrankData::getOutputMode(byte channel) { // Live oder Seq?
    return config.routing[channel].getOut();
}

byte FrankData::getArpModeEnable(byte channel) { // Live oder Seq?
    return config.routing[channel].getArp();
}

// utility

// test byte range and return valid byte
inline byte testByte(byte value, byte minimum, byte maximum, bool clampChange) {
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
inline byte increaseByte(byte value, byte maximum) { 
    if (value == maximum) {
        return value;
    } else {
        return value + 1;
    }
}

// decrease byte
inline byte decreaseByte(byte value, byte minimum) { 
    if (value == minimum) {
        return value;
    } else {
        return value - 1;
    }
}

// change byte value and check boundaries
inline byte changeByte(byte value, int change, byte minimum, byte maximum,
                       bool clampChange) { 

    if ((int)value + change >= maximum) { // test max

        return clampChange ? maximum : value;
    } else if ((int)value + change <= minimum) { // test min
        return clampChange ? minimum : value;
    } else {
        return (byte)((int)value + change); // return new value
    }
}

// toggle byte
template <typename T> inline T toggleByte(T data) { return !data; }