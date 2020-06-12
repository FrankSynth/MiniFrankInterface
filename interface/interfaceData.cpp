#include "interfaceData.hpp"
#include "interfaceMidi.hpp"

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

// class PressedNotesList
void PressedNotesList::appendKey(const byte &note, const byte &velocity) {
    PressedNotesElement *listElement = pressedNoteElement;
    if (listElement) {
        if (listElement->note == note)
            return;
        while (listElement->next != NULL) {
            listElement = listElement->next;
            if (listElement->note == note)
                return;
        }

        listElement->next = new PressedNotesElement(note, velocity);
        size++;
    }
    else {
        pressedNoteElement = new PressedNotesElement(note, velocity);
        size++;
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
            }
            else {
                return;
            }
        }
        if (pressedNoteElement->next == NULL) {
            delete listElement;
            size--;

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
            size--;
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
        // PRINTLN("retrigger");
        arpList.deleteAllKeys();
        arpRetrigger = 0;
        arpStepRepeat = 1;
        arpRestarted = 1;
    }
    else {
        arpRetrigger = 0;
    }

    arpList.appendKey(note, velocity);
}

void LiveMidi::keyReleased(const byte &note) {

    // if last element will get deleted
    if (noteList.size == 1) {
        lastKey.note = noteList.getElement(0)->note;
        lastKey.velocity = noteList.getElement(0)->velocity;
        arpRetrigger = 1;
    }

    noteList.deleteKey(note);
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
    stepArp = 0;
    stepSeq = 0;
    mod = 0;
    pitchbend = 64;
    aftertouch = 0;
    sustain = 0;
    arpRetrigger = 1; // clear arp with next key
    triggered = 1;    // update outputs
    arpOctave = 0;
    arpDirection = 0;
    arpStepRepeat = 1;
}

void LiveMidi::updateArpArray(const byte &arpSettings) {

    copyArpListToArray();

    if (arpSettings < 6)
        qsort(arpArray, arpList.size, sizeof(structKey), sort_asc);
}

void LiveMidi::copyArpListToArray() {
    for (int x = 0; x < arpList.size; x++) {
        PressedNotesElement *element = arpList.getElement(x);
        structKey key;
        key.note = element->note;
        key.velocity = element->velocity;
        arpArray[x] = key;
    }
}

structKey LiveMidi::getArpKey(const byte &step) {
    if (arpList.containsElements())
        return arpArray[step];
    else
        return lastKey;
}

// Sequence data for each sequence
// init sequence to default values
void Seq::init(const byte &note, const byte &gate, const byte &gateLength, const byte &cc, const byte &tuning, const int16_t &gateLengthOffset) {
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = note; // test
        setGate(i, 1);           // test
        sequence.gateLength[i] = gateLength;
        sequence.cc[i] = cc;
    }
    sequence.tuning = tuning;
    sequence.gateLengthOffset = gateLengthOffset;
}

void Seq::setNote(const byte &index, const byte &note) { // set note value
    sequence.note[testByte(index, 0, LENGTH)] = testByte(note, 0, NOTERANGE - 1);
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
        setGate(i, gate); // set gates
    }
}

byte Seq::getGate(const byte &index) {
    return bitRead(sequence.gate[index >> 3], index & 0x07);
}

void Seq::setGate(const byte &index, const byte &gate) {
    bitWrite(sequence.gate[index >> 3], index & 0x07, testByte(gate, 0, 1));
}

void Seq::increaseNote(const byte &index) { // increase note value and return new note

    byte note = sequence.note[index];

    // no tuning
    if (sequence.tuning == 0) {
        // note up
        sequence.note[index] = increaseByte(note, NOTERANGE - 1);
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

void Seq::changeNotes(const int16_t &change) { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], change, 0, NOTERANGE - 1, 1);
    }
}

void Seq::octaveUp() { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], 12, 0, NOTERANGE - 1, 0);
    }
}

void Seq::octaveDown() { // change note
    for (int i = 0; i < LENGTH; i++) {
        sequence.note[i] = changeByte(sequence.note[i], -12, 0, NOTERANGE - 1, 0);
    }
}

// all gate lengths
void Seq::setGateLengths(const byte &gateLength) { // set all gates at once
    // gateLength = testByte(gateLength, 0, 255); // test gate length
    for (int i = 0; i < LENGTH; i++) {
        sequence.gateLength[i] = gateLength; // set new gates
    }
}

void Seq::changeGateLengths(const int16_t &change) { // change gate length
    for (int i = 0; i < LENGTH; i++) {
        sequence.gateLength[i] = changeByte(sequence.gateLength[i], change, 0, 100, 1);
    }
}

// Sequence
void Seq::setSequence(structSequence *copySeq) { // set complete suquence struct
    sequence = *copySeq;                         // copy struct
}

// Data Singleton that handles all data, including all sequences, live midi and settings
void FrankData::receivedKeyPressed(const byte &channel, const byte &note, const byte &velocity) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].channel == 0 || config.routing[x].channel == channel) {
            liveMidi[x].keyPressed(note, velocity);
            updateArp(x);

            // seq rec
            if (stat.rec && x == stat.screen.channel && config.routing[x].outSource > 0) {
                seq[config.routing[x].outSource - 1].sequence.note[liveMidi[x].stepSeq] = note;

                // if output cc is velocity, take new velocity
                if (config.routing[x].cc == 0) {
                    seq[config.routing[x].outSource - 1].sequence.cc[liveMidi[x].stepSeq] = velocity;
                }
                // else take the momentary cc values depending on outputCC
                else if (config.routing[x].cc == 2) {
                    seq[config.routing[x].outSource - 1].sequence.cc[liveMidi[x].stepSeq] = getPitchbendAsByte(x);
                }
                else {
                    seq[config.routing[x].outSource - 1].sequence.cc[liveMidi[x].stepSeq] = getLiveCcEvaluated(x);
                }

                if (!stat.play) {
                    increaseStepCounters(x);
                    liveMidi[x].recModePlayback = 1;
                }
            }

            // live midi
            if (config.routing[x].outSource == 0) {
                liveMidi[x].triggered = 1;
                liveMidi[x].midiUpdateWaitTimer = 0;
            }
        }
    }
}

void FrankData::receivedKeyReleased(const byte &channel, const byte &note) {
    for (byte x = 0; x < OUTPUTS; x++) {
        if (config.routing[x].channel == 0 || config.routing[x].channel == channel) {
            liveMidi[x].keyReleased(note);
        }

        if (config.routing[x].outSource == 0 && config.routing[x].arp == 0) {
            liveMidi[x].triggered = 1;
            liveMidi[x].released = 1;
        }
        if (config.routing[x].arp == 1 && liveMidi[x].sustain < 64) {

            liveMidi[x].arpList.deleteKey(note);
            liveMidi[x].updateArpArray(config.routing[x].arpMode);
            liveMidi[x].midiUpdateWaitTimer = 0;
        }
    }
}

void FrankData::init() {
    byte testByte = 0;
    byte checkByte = 2; // change this to cause a memory reset
    if (EEPROM.get(0, testByte) == checkByte) {
        loadNoteCalibration();
        loadMenuSettings();
    }
    else {
        loadNoteCalibration();
        saveMenuSettings();
        EEPROM.update(0, checkByte);
    }
}

void FrankData::receivedMidiClock() {
    if (stat.bpmSync) {
        increaseBpmCount();
        if (!(stat.bpmClockCounter % 6)) {
            calcBPM();
        }
    }
}

void FrankData::receivedMidiSongPosition(unsigned int spp) {
    // DebugTimer debug("spp");
    if (stat.bpmSync) {
        stat.bpmClockCounter = 0;
        for (byte out = 0; out < OUTPUTS; out++) {
            liveMidi[out].arpRestarted = 1;
            liveMidi[out].arpRetrigger = 1;
        }
        PRINT(millis());

        PRINT(": ssp ");
        PRINTLN(spp);

        // return;

        uint16_t stepcounter[OUTPUTS];
        uint16_t doNotCheckClocksCounter[OUTPUTS];
        uint16_t stepClockCount[OUTPUTS];
        uint16_t clockClockCount[OUTPUTS];

        for (byte out = 0; out < OUTPUTS; out++) {
            stepcounter[out] = 0;
            stepClockCount[out] = clockSteppingCounts[config.routing[out].stepSpeed];
            doNotCheckClocksCounter[out] = (stat.bpmClockCounter + 1) % stepClockCount[out];
            if (doNotCheckClocksCounter[out])
                doNotCheckClocksCounter[out] = stepClockCount[out] - doNotCheckClocksCounter[out];

            if (config.routing[out].polyRhythm > 1) {
                clockClockCount[out] = clockSteppingCounts[config.routing[out].clockSpeed];
                uint16_t clockPoly = (stat.bpmClockCounter + 1) % clockClockCount[out];
                if (clockPoly)
                    clockPoly = clockClockCount[out] - clockPoly;
                doNotCheckClocksCounter[out] = min(doNotCheckClocksCounter[out], clockPoly);
            }
        }

        for (unsigned int i = 0; i < (spp * 6); i++) {
            stat.bpmClockCounter++;
            if (stat.bpmClockCounter > MAXBPMCOUNT) {
                stat.bpmClockCounter = 0;
            }

            for (byte out = 0; out < OUTPUTS; out++) {
                if (config.routing[out].outSource) {
                    if (doNotCheckClocksCounter[out] == 0) {
                        stepcounter[out]++;
                        // liveMidi[out].midiUpdateWaitTimer = 0;

                        doNotCheckClocksCounter[out] = (stat.bpmClockCounter + 1) % stepClockCount[out];
                        if (doNotCheckClocksCounter[out])
                            doNotCheckClocksCounter[out] = stepClockCount[out] - doNotCheckClocksCounter[out];

                        if (config.routing[out].polyRhythm > 1) {
                            uint16_t clockPoly = (stat.bpmClockCounter + 1) % clockClockCount[out];
                            if (clockPoly)
                                clockPoly = clockClockCount[out] - clockPoly;
                            doNotCheckClocksCounter[out] = min(doNotCheckClocksCounter[out], clockPoly);
                        }
                    }
                    else {
                        doNotCheckClocksCounter[out]--;
                    }
                }
            }
        }
        stat.receivedSPPclockCount = stat.bpmClockCounter;

        for (byte out = 0; out < OUTPUTS; out++) {

            // liveMidi[out].midiUpdateWaitTimer = 0;

            if (config.routing[out].outSource) {
                liveMidi[out].stepSeq = 0;

                stepcounter[out] = stepcounter[out] % (config.routing[out].nbPages * (STEPSPERPAGE - seq[out].sequence.pageEndOffset));

                if (config.direction) {
                    for (unsigned int i = 0; i < stepcounter[out]; i++)
                        increaseSeqStep(out);
                }
                else {
                    for (unsigned int i = 0; i < stepcounter[out]; i++)
                        decreaseSeqStep(out);
                }
            }
            liveMidi[out].midiUpdateWaitTimer = 0;
        }
        stat.receivedNewSPP = 1;
    }
}

void FrankData::receivedStart() {
    PRINTLN("start");

    if (stat.bpmSync) {

        stat.receivedNewSPP = 1;
        // stat.midiClockCount = 0;
        // stat.bpm16thCount = 0;
        stat.bpmClockCounter = 0;
        for (byte out = 0; out < OUTPUTS; out++) {
            // liveMidi[out].stepSeq = config.routing[out].nbPages * STEPSPERPAGE;
            // liveMidi[out].stepArp = 0;
            liveMidi[out].arpRetrigger = 1;
            liveMidi[out].arpRestarted = 1;
            liveMidi[out].midiUpdateWaitTimer = 0;
            // updateArp(out);
        }
        stat.play = 1;
    }
}
void FrankData::receivedContinue() {
    PRINT(millis());
    PRINTLN(": continue");
    if (stat.bpmSync) {
        // sendMidiAllNotesOff();

        stat.receivedNewSPP = 1;

        for (byte out = 0; out < OUTPUTS; out++) {
            // updateArp(out);
            // liveMidi[out].stepArp = 0;
            // liveMidi[out].arpRetrigger = 1;
            // liveMidi[out].arpRestarted = 1;
            liveMidi[out].midiUpdateWaitTimer = 0;
        }
        stat.bpmClockCounter = stat.receivedSPPclockCount;

        stat.play = 1;
    }
}
void FrankData::receivedStop() {
    PRINT(millis());
    PRINTLN(": stop");

    if (stat.bpmSync) {
        stat.play = 0;
        // for (byte out = 0; out < OUTPUTS; out++) {
        //     liveMidi[out].arpRetrigger = 1;
        // }
        for (byte out = 0; out < OUTPUTS; out++)
            liveMidi[out].midiUpdateWaitTimer = 0;
    }
    // sendMidiAllNotesOff();
}

void FrankData::receivedReset() {
    reset();
}

void FrankData::reset() {
    // stat.midiClockCount = 0;
    // stat.bpm16thCount = 0;
    // stat.bpmClockCounter = 0;

    for (byte out = 0; out < OUTPUTS; out++) {
        liveMidi[out].reset();
        liveMidi[out].updateArpArray(config.routing[out].arpMode);
    }
    // sendMidiAllNotesOff();
}

void FrankData::increaseBpmCount() {
    // PRINT(millis());

    if (stat.receivedNewSPP) {

        // PRINTLN(": increaseBPM receivednewSPP");

        stat.receivedNewSPP = 0;
        stat.doNotCalcBpm = 1;
        for (byte out = 0; out < OUTPUTS; out++) {
            if (checkClockStepping(out, stepSpeed)) {
                liveMidi[out].triggered = 1;
                liveMidi[out].arpTriggeredNewNote = 1;
                // liveMidi[out].arpRestarted = 1;
            }
        }
        return;
    }

    // PRINTLN(": increaseBPM");

    // LCM of all timings is 2304
    stat.bpmClockCounter++;
    if (stat.bpmClockCounter > MAXBPMCOUNT) {
        stat.bpmClockCounter = 0;
    }

    for (byte out = 0; out < OUTPUTS; out++) {
        liveMidi[out].midiUpdateWaitTimer = 0;
        if (checkClockStepping(out, stepSpeed)) {

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

void FrankData::increaseStepCounters(const byte &channel) {

    if (stat.editMode) {
        if (config.routing[channel].nbPages <= ((stat.editStep + STEPSPERPAGE) / STEPSPERPAGE)) { // newPage above number of pages
            stat.editStep = 0;                                                                    // set stepSeq[array] 0
        }
        else {                             // new page is valid.
            stat.editStep += STEPSPERPAGE; // increase Step
        }
    }
    else {

        bool steppingDone = false;

        while (!steppingDone) {

            stat.bpmClockCounter++;
            if (stat.bpmClockCounter > MAXBPMCOUNT) {
                stat.bpmClockCounter = 0;
            }

            for (byte out = 0; out < OUTPUTS; out++) {
                if (checkClockStepping(out, stepSpeed)) {

                    increaseSeqStep(out);
                    if (channel == out) {
                        steppingDone = true;
                    }
                }
            }
        }
    }
}

void FrankData::decreaseStepCounters(const byte &channel) {

    if (stat.editMode) {
        if (stat.editStep == 0) {                                                 // we jump to the last page?
            stat.editStep = (config.routing[channel].nbPages - 1) * STEPSPERPAGE; // set to max stepSeq[array]
        }
        else {                                                                                       // we make a pageJump?
            if (config.routing[channel].nbPages < ((stat.editStep - STEPSPERPAGE) / STEPSPERPAGE)) { // newPage above number of pages
                stat.editStep = (config.routing[channel].nbPages - 1) * STEPSPERPAGE;                // set jump to last stepSeq[array]
            }
            else {                             // new page is valid.
                stat.editStep -= STEPSPERPAGE; // decrease Step
            }
        }
    }
    else {

        bool steppingDone = false;

        while (!steppingDone) {

            stat.bpmClockCounter--;
            if (stat.bpmClockCounter < 0) {
                stat.bpmClockCounter = MAXBPMCOUNT;
            }

            for (byte out = 0; out < OUTPUTS; out++) {
                if (checkClockStepping(out, stepSpeed)) {

                    decreaseSeqStep(out);
                    if (channel == out) {
                        steppingDone = true;
                    }
                }
            }
        }
    }
}

// call with argument bool "true" to restart time counter
void FrankData::updateClockCounter(const bool restartCounter) {

    if (!stat.bpmSync) {
        static elapsedMicros timer;
        if (restartCounter) {
            timer = 0;
        }
        else {
            if (stat.bpm != 0) {
                // count time for MidiTicks, 1 bpm would have 24 ticks/seconds
                uint32_t bpmTiming = 60000000 / ((uint32_t)stat.bpm * 24);
                if (timer >= bpmTiming) {
                    timer -= bpmTiming;
                    increaseBpmCount();
                }
            }
        }
    }

    for (byte out = 0; out < OUTPUTS; out++) {

        if (config.routing[out].arp && config.routing[out].outSource == 0) {

            static byte checkArpStep[OUTPUTS] = {0, 0};
            static int16_t lastBpmClockCount[OUTPUTS] = {3000, 3000};

            if (lastBpmClockCount[out] != stat.bpmClockCounter && !stat.receivedNewSPP) {

                lastBpmClockCount[out] = stat.bpmClockCounter;

                if (checkClockStepping(out, stepSpeed)) {
                    checkArpStep[out] = 1;
                }
            }

            if (liveMidi[out].midiUpdateWaitTimer > MIDIARPUPDATEDELAY && checkArpStep[out]) {
                PRINT(millis());

                PRINT(": midi clock: ");
                PRINT(stat.bpmClockCounter);

                PRINT(", new Arp step ");

                nextArpStep(out);
                PRINT(liveMidi[out].stepArp);
                PRINT(", octave ");
                PRINTLN(liveMidi[out].arpOctave);
                checkArpStep[out] = 0;
            }
        }
    }
}

void FrankData::calcBPM() {
    static elapsedMicros bpm16thTimer;
    static elapsedMillis averagingStartTime;
    static uint32_t averageTimer = 0;
    static byte counter = 0;

    averageTimer += 60000000.0 / bpm16thTimer;
    bpm16thTimer = 0;
    counter++;

    if (averagingStartTime > 1000) {

        // avoid updating bpm when last update is too long ago
        if (averagingStartTime > 2000 || stat.doNotCalcBpm) {
            averagingStartTime = 0;
            averageTimer = 0;
            counter = 0;
            stat.doNotCalcBpm = 0;
            return;
        }
        averageTimer = averageTimer / 4.0;
        stat.bpm = testInt((int)(averageTimer / (int)counter + 0.5), 1, 1000);
        averageTimer = 0;
        counter = 0;
        averagingStartTime = 0;
    }
}

bool FrankData::checkClockStepping(const byte &array, const frankData &clockSource) {
    uint16_t checkClock = stat.bpmClockCounter;

    if (config.routing[array].clockingOffset) {
        uint16_t clockStepsOffset = clockSteppingCounts[config.routing[array].clockingOffset - 1];
        if (clockStepsOffset > stat.bpmClockCounter) {
            clockStepsOffset -= stat.bpmClockCounter;
            checkClock = MAXBPMCOUNT + 1 - clockStepsOffset;
        }
        else {
            checkClock -= clockStepsOffset;
        }
    }

    if (clockSource == stepSpeed || config.routing[array].polyRhythm == 1 || config.routing[array].polyRhythm == 3) {
        if (!(checkClock % clockSteppingCounts[config.routing[array].stepSpeed]))
            return true;
    }
    if (clockSource == outputClock || config.routing[array].polyRhythm > 1) {
        if (!(checkClock % clockSteppingCounts[config.routing[array].clockSpeed]))
            return true;
    }
    return false;
}

void FrankData::increaseSeqStep(const byte &array) {
    do {
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
    } while (STEPSPERPAGE - 1 + seq[SEQCHANNEL].sequence.pageEndOffset < liveMidi[array].stepSeq % STEPSPERPAGE);

    if (config.routing[array].outSource)
        liveMidi[array].triggered = 1;
    liveMidi[array].recModePlayback = 0;
}

void FrankData::decreaseSeqStep(const byte &array) {
    do {
        if (liveMidi[array].stepSeq == 0) {                                             // we jump to the last page?
            liveMidi[array].stepSeq = config.routing[array].nbPages * STEPSPERPAGE - 1; // set to max stepSeq[array]
        }
        else if (!(liveMidi[array].stepSeq % STEPSPERPAGE)) {                                     // we make a pageJump?
            if (config.routing[array].nbPages < ((liveMidi[array].stepSeq - 1) / STEPSPERPAGE)) { // newPage above number of pages
                liveMidi[array].stepSeq = config.routing[array].nbPages * STEPSPERPAGE - 1;       // set jump to last stepSeq[array]
            }
            else {                         // new page is valid.
                liveMidi[array].stepSeq--; // decrease Step
            }
        }
        else {
            liveMidi[array].stepSeq--; // decrease Step
        }

    } while (STEPSPERPAGE - 1 + seq[SEQCHANNEL].sequence.pageEndOffset < liveMidi[array].stepSeq % STEPSPERPAGE);

    if (config.routing[array].outSource)
        liveMidi[array].triggered = 1;
    liveMidi[array].recModePlayback = 0;
}

byte FrankData::getCurrentPageNumber(const byte &array) { // number of pages, takes care if page number has changed
    if (config.routing[array].nbPages > (liveMidi[array].stepSeq / 8))
        return config.routing[array].nbPages; // is our stepSeq above the current number of pages?
    return (liveMidi[array].stepSeq / 8 + 1); // return current stepSeq page until the next page jump
}

// Singleton
FrankData &FrankData::getDataObj() {
    static FrankData mainData;
    return mainData;
}

structKey FrankData::getLiveKeyEvaluated(const byte &array) {
    switch (config.routing[array].liveMidiMode) {
        case 0: return liveMidi[array].getKeyLatest();
        case 1: return liveMidi[array].getKeyLowest();
        case 2: return liveMidi[array].getKeyHighest();
        default: PRINTLN("FrankData getLiveKeyEvaluated, no case found"); return liveMidi[array].lastKey;
    }
}

void FrankData::increaseArpOct(const byte &array) {
    // change octave for next round
    // no octaving
    if (config.routing[array].arpOctaves == 0) {
        liveMidi[array].arpOctave = 0;
    }
    else {

        // eval new min and max
        int newOctMin;
        int newOctMax;

        if (config.routing[array].arpOctaves < 0) {
            newOctMin = config.routing[array].arpOctaves;
            newOctMax = 0;
        }
        else {
            newOctMin = 0;
            newOctMax = config.routing[array].arpOctaves;
        }
        if (liveMidi[array].arpRestarted) {
            liveMidi[array].arpOctave = newOctMin;
        }
        else {

            if (config.routing[array].arpMode == 2 || config.routing[array].arpMode == 3 || config.routing[array].arpMode == 4 ||
                config.routing[array].arpMode == 5) {
                liveMidi[array].arpOctave = changeInt(liveMidi[array].arpOctave, 1, newOctMin, newOctMax, true);
                if (liveMidi[array].arpOctave == newOctMax)
                    liveMidi[array].arpOctaveDirection = 0;
            }
            else {
                liveMidi[array].arpOctave = changeIntReverse(liveMidi[array].arpOctave, 1, newOctMin, newOctMax);
            }
        }
    }
}

void FrankData::decreaseArpOct(const byte &array) {
    // no octaving
    if (config.routing[array].arpOctaves == 0) {
        liveMidi[array].arpOctave = 0;
    }
    else {
        // eval new min and max
        int newOctMin;
        int newOctMax;

        if (config.routing[array].arpOctaves < 0) {
            newOctMin = config.routing[array].arpOctaves;
            newOctMax = 0;
        }
        else {
            newOctMin = 0;
            newOctMax = config.routing[array].arpOctaves;
        }

        if (liveMidi[array].arpRestarted) {
            liveMidi[array].arpOctave = newOctMax;
        }
        else {
            if (config.routing[array].arpMode == 2 || config.routing[array].arpMode == 3 || config.routing[array].arpMode == 4 ||
                config.routing[array].arpMode == 5) {
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
}

void FrankData::nextArpStep(const byte &array) {

    if (config.routing[array].outSource == 0 && config.routing[array].arp) {
        if (config.routing[array].arp == 1 && !liveMidi[array].keysPressed() && liveMidi[array].sustain < 64) {
            return;
        }
        if (liveMidi[array].arpRestarted) {
            PRINT("restarted arp ");
            PRINTLN(array);
        }

        static byte randomCounter = 0;

        switch (config.routing[array].arpMode) {
            case 1: // down

                if (liveMidi[array].stepArp == 0) {
                    decreaseArpOct(array);
                }
                if (liveMidi[array].arpRestarted) {
                    decreaseArpOct(array);
                    liveMidi[array].stepArp = liveMidi[array].arpList.size - 1;
                    liveMidi[array].arpRestarted = 0;
                }
                else {
                    // if arp size increased by one, a note would be repeated, so instead, we increase further
                    do {
                        liveMidi[array].stepArp = changeByteReverse(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1);
                }
                break;
            case 0: // up
            case 6: // order
                if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {
                    increaseArpOct(array);
                }
                if (liveMidi[array].arpRestarted) {
                    increaseArpOct(array);
                    liveMidi[array].stepArp = 0;
                    liveMidi[array].arpRestarted = 0;
                }
                else {
                    // if arp size increased by one, a note would be repeated, so instead, we increase further
                    do {
                        liveMidi[array].stepArp = changeByteReverse(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1);
                }
                break;
            case 7: // random
                if (randomCounter == liveMidi[array].arpList.size - 1) {
                    increaseArpOct(array);
                }
                if (liveMidi[array].arpRestarted) {
                    increaseArpOct(array);
                    randomCounter = 0;
                    liveMidi[array].arpRestarted = 0;
                }
                else {
                    randomCounter = changeByteReverse(randomCounter, 1, 0, liveMidi[array].arpList.size - 1);
                }
                liveMidi[array].stepArp = random(0, liveMidi[array].arpList.size);
                break;
            case 2: // updown
                if (liveMidi[array].arpRestarted) {
                    increaseArpOct(array);
                    liveMidi[array].arpDirection = 1;
                    liveMidi[array].arpOctaveDirection = 1;
                    liveMidi[array].stepArp = 0;
                    liveMidi[array].arpRestarted = 0;
                }
                else {
                    // if arp size increased by one, a note would be repeated, so instead, we increase further

                    do {

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
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1);
                }
                break;
            case 4: // upRdownR
                if (liveMidi[array].arpRestarted) {
                    increaseArpOct(array);
                    liveMidi[array].arpDirection = 1;
                    liveMidi[array].arpOctaveDirection = 1;
                    liveMidi[array].stepArp = 0;
                    liveMidi[array].arpRestarted = 0;
                    liveMidi[array].arpStepRepeat = 1;
                }
                else {

                    do {

                        // going up
                        if (liveMidi[array].arpDirection) {
                            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
                            if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {
                                if (!liveMidi[array].arpStepRepeat) {
                                    liveMidi[array].arpStepRepeat = 1;
                                    liveMidi[array].arpDirection = 0;
                                }
                                else {
                                    liveMidi[array].arpStepRepeat = 0;
                                }
                            }
                        }
                        // going down
                        else {
                            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
                            if (liveMidi[array].stepArp == 0) {
                                if (!liveMidi[array].arpStepRepeat) {
                                    liveMidi[array].arpStepRepeat = 1;
                                    liveMidi[array].arpDirection = 1;
                                    if (liveMidi[array].arpOctaveDirection == 1) {
                                        increaseArpOct(array);
                                    }
                                    else {
                                        decreaseArpOct(array);
                                    }
                                }
                                else {
                                    liveMidi[array].arpStepRepeat = 0;
                                }
                            }
                        }
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1 && !liveMidi[array].arpStepRepeat);
                }
                break;
            case 3: // downup
                if (liveMidi[array].arpRestarted) {
                    decreaseArpOct(array);
                    liveMidi[array].arpDirection = 0;
                    liveMidi[array].arpOctaveDirection = 0;
                    liveMidi[array].stepArp = liveMidi[array].arpList.size - 1;
                    liveMidi[array].arpRestarted = 0;
                }
                else {

                    do {

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
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1);
                }
                break;
            case 5: // downRupR
                if (liveMidi[array].arpRestarted) {
                    decreaseArpOct(array);
                    liveMidi[array].arpDirection = 0;
                    liveMidi[array].arpOctaveDirection = 0;
                    liveMidi[array].stepArp = liveMidi[array].arpList.size - 1;
                    liveMidi[array].arpRestarted = 0;
                    liveMidi[array].arpStepRepeat = 1;
                }
                else {

                    do {

                        // going up
                        if (liveMidi[array].arpDirection) {
                            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, 1, 0, liveMidi[array].arpList.size - 1);
                            if (liveMidi[array].stepArp == liveMidi[array].arpList.size - 1) {
                                if (!liveMidi[array].arpStepRepeat) {
                                    liveMidi[array].arpStepRepeat = 1;

                                    liveMidi[array].arpDirection = 0;
                                    if (liveMidi[array].arpOctaveDirection == 1) {
                                        increaseArpOct(array);
                                    }
                                    else {
                                        decreaseArpOct(array);
                                    }
                                }
                                else {
                                    liveMidi[array].arpStepRepeat = 0;
                                }
                            }
                        }
                        // going down
                        else {
                            liveMidi[array].stepArp = changeByte(liveMidi[array].stepArp, -1, 0, liveMidi[array].arpList.size - 1);
                            if (liveMidi[array].stepArp == 0) {
                                if (!liveMidi[array].arpStepRepeat) {
                                    liveMidi[array].arpStepRepeat = 1;
                                    liveMidi[array].arpDirection = 1;
                                }
                                else {
                                    liveMidi[array].arpStepRepeat = 0;
                                }
                            }
                        }
                    } while (liveMidi[array].getArpKey(liveMidi[array].stepArp).note == liveMidi[array].arpKey.note &&
                             liveMidi[array].arpList.size > 1 && !liveMidi[array].arpStepRepeat);
                }
                break;
            default:;
        }

        structKey key;

        key = liveMidi[array].getArpKey(liveMidi[array].stepArp);

        // lower octaves
        if (liveMidi[array].arpOctave < 0) {
            for (int x = liveMidi[array].arpOctave; x < 0; x++) {
                key.note = changeByte(key.note, -12, 0, NOTERANGE - 1, 0);
            }
        }
        // raise octaves
        else if (liveMidi[array].arpOctave > 0) {
            for (int x = liveMidi[array].arpOctave; x > 0; x--) {
                key.note = changeByte(key.note, 12, 0, NOTERANGE - 1, 0);
            }
        }

        liveMidi[array].arpKey = key;
        liveMidi[array].triggered = 1;
        liveMidi[array].arpTriggeredNewNote = 1;
    }
}

structKey FrankData::getKeyHighest(const byte &array) {
    return liveMidi[array].getKeyHighest();
}
structKey FrankData::getKeyLowest(const byte &array) {
    return liveMidi[array].getKeyLowest();
}
structKey FrankData::getKeyLatest(const byte &array) {
    return liveMidi[array].getKeyLatest();
}

int16_t FrankData::getLiveCcEvaluated(const byte &array) {
    switch (config.routing[array].cc) {
        case 0: return config.routing[array].arp ? liveMidi[array].arpKey.velocity : getLiveKeyEvaluated(array).velocity;
        case 1: return liveMidi[array].mod;
        case 2: return liveMidi[array].pitchbend;
        case 3: return liveMidi[array].aftertouch;
        case 4: return liveMidi[array].sustain;
        default: PRINTLN("FrankData getLiveCcEvaluated, no case found"); return 0;
    }
}

byte FrankData::getPitchbendAsByte(const byte &channel) {
    return (byte)(liveMidi[channel].pitchbend / 128 + 64);
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
    seq[array].setNotes(24);
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
        liveMidi[out].stepSeq = 0;
    }
}

void FrankData::updateArp(const byte &array) {
    if (config.routing[array].outSource == 0 && config.routing[array].arp) {
        liveMidi[array].updateArpArray(config.routing[array].arpMode);
        liveMidi[array].stepArp = testByte(liveMidi[array].stepArp, 0, liveMidi[array].arpList.size);
    }
}

// Screen config
const byte FrankData::getSubscreenMax() {
    return config.routing[stat.screen.channel].outSource ? stat.screen.subScreenMaxSeq : stat.screen.subScreenMaxLive;
}

// new set and get functions
int16_t FrankData::get(const frankData &frankDataType) {
    switch (frankDataType) {
        case midiSource: return config.midiSource;

        case direction: return config.direction;
        case displayBrightness: return config.displayBrightness;

        case screenOutputChannel: return stat.screen.channel;
        case screenConfig: return stat.screen.config;
        case screenMainMenu: return stat.screen.mainMenu;
        case screenSubScreen: return stat.screen.subscreen;
        case screenRouting: return stat.screen.routing;
        case screenCalNote: return stat.screen.calibration;
        case screenCalCv: return stat.screen.calibrationCv;

        case liveCalNote: return stat.noteToCalibrate;
        case liveCalCv: return stat.cvToCalibrate;
        case bpm: return stat.bpm;
        case play: return stat.play;
        case rec: return stat.rec;
        case error: return stat.error;
        case bpmSync: return stat.bpmSync;
        case bpmPoti: return stat.bpmPot;
        case bpmClockCount: return stat.bpmClockCounter;
        case save:
        case load: return stat.loadSaveSlot;
        case editMode: return stat.editMode;
        case editStep: return stat.editStep;
        case activeEditPage: return (stat.editStep >> 3);
        case activeGateByte:
            if (stat.editMode) {
                return seq[config.routing[stat.screen.channel].outSource - 1].sequence.gate[stat.editStep >> 3];
            }
            else {
                return seq[config.routing[stat.screen.channel].outSource - 1].sequence.gate[liveMidi[stat.screen.channel].stepSeq >> 3];
            }
        case activeStepOnPage: return liveMidi[stat.screen.channel].stepSeq & 0x07;

        case stepOnEditPage: return (stat.editStep - (get(activeEditPage) * STEPSPERPAGE));

        case pulseLength:
            return config.pulseLength;
            // case liveNewMidiDataArp: return stat.receivedNewMidiDataArp;

        case none: return (int16_t)0;
        default: PRINTLN("FrankData get(frankData frankDataType), no case found"); return 0;
    }
}

int16_t FrankData::get(const frankData &frankDataType, const byte &array) {
    switch (frankDataType) {
        case outputSource: return config.routing[array].outSource;
        case outputChannel: return config.routing[array].channel;
        case outputArp: return config.routing[array].arp;
        case outputCc: return config.routing[array].cc;
        case outputCcEvaluated: return getLiveCcEvaluated(array);
        case outputLiveMode: return config.routing[array].liveMidiMode;
        case outputClock: return config.routing[array].clockSpeed;
        case outputRatchet: return config.routing[array].arpRatchet;
        case outputArpOctave: return config.routing[array].arpOctaves;
        case outputArpMode: return config.routing[array].arpMode;
        case outputPolyRhythm: return config.routing[array].polyRhythm;
        case outputMidiNotes: return config.routing[array].midiNoteOut;
        case outputClockingOffset: return config.routing[array].clockingOffset;

        case liveKeysPressed: return liveMidi[array].keysPressed();

        case cvCalOffset: return cal[array].cvOffset;
        case cvCalLower: return cal[array].cvLowerLimit;
        case cvCalUpper: return cal[array].cvUpperLimit;
        case cvPitchbendCalLower: return cal[array].cvPitchbendLowerLimit;
        case cvPitchbendCalUpper: return cal[array].cvPitchbendUpperLimit;
        case noteScaleOffset: return cal[array].noteScaleOffset;

        case liveMod: return liveMidi[array].mod;
        case livePitchbend: return liveMidi[array].pitchbend;
        case liveAftertouch: return liveMidi[array].aftertouch;
        case liveSustain: return liveMidi[array].sustain;
        case liveTriggered: return liveMidi[array].triggered;
        case liveArpTriggeredNewNote: return liveMidi[array].arpTriggeredNewNote;
        case liveReleased: return liveMidi[array].released;
        case liveRecModePlayback: return liveMidi[array].recModePlayback;
        case liveKeyNoteEvaluated: return getLiveKeyEvaluated(array).note;
        case liveKeyVelEvaluated: return getLiveKeyEvaluated(array).velocity;
        case liveKeyArpNoteEvaluated: return liveMidi[array].arpKey.note;
        case liveKeyArpVelEvaluated: return liveMidi[array].arpKey.velocity;
        case liveLatestKey: return liveMidi[array].getKeyLatest().note;
        case liveLowestKey: return liveMidi[array].getKeyLowest().note;
        case liveHighestKey: return liveMidi[array].getKeyHighest().note;
        case liveMidiUpdateWaitTimer: return liveMidi[array].midiUpdateWaitTimer;

        case nbPages: return config.routing[array].nbPages;
        case stepArp: return liveMidi[array].stepArp;
        case stepSeq: return liveMidi[array].stepSeq;
        case stepSpeed: return config.routing[array].stepSpeed;
        case activePage: return (liveMidi[array].stepSeq >> 3);

        case stepOnPage: return (liveMidi[array].stepSeq & 0x07);

        case currentPageNumber: return getCurrentPageNumber(array);

        case seqTuning: return seq[array].sequence.tuning;
        case seqPageEndOffset: return seq[array].sequence.pageEndOffset;
        case seqGateLengthOffset: return seq[array].sequence.gateLengthOffset;

        case seqOctaveOffset: return config.routing[array].seqOctaves;
        case seqNoteOffset: return config.routing[array].seqNotes;

        case liveCalNote: return stat.noteToCalibrate;
        case liveCalCv: return stat.cvToCalibrate;

        case none: return (int16_t)0;

        default: PRINTLN("FrankData get(frankData frankDataType, byte array), no case found"); return 0;
    }
}

int16_t FrankData::get(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
        case seqNote: return seq[array].sequence.note[step];
        case seqGate: return seq[array].getGate(step);
        case seqGateLength: return seq[array].sequence.gateLength[step];
        case seqCc:
            return seq[array].sequence.cc[step];

            // case seqVelocity: return seq[array].sequence.velocity[step];

        case noteCalOffset: return cal[array].noteCalibration[step];

        case none: return (int16_t)0;

        default: PRINTLN("FrankData get(frankData frankDataType, byte array, byte step), no case found"); return 0;
    }
}

// set data
void FrankData::set(const frankData &frankDataType, const int16_t &data) {
    switch (frankDataType) {
        case midiSource: config.midiSource = testByte(data, 0, 1); break;

        case direction: config.direction = testByte(data, 0, 1); break;
        case displayBrightness: config.displayBrightness = testByte(data, 0, 100); break; // how high?

        case screenOutputChannel:
            stat.screen.channel = testByte(data, 0, OUTPUTS - 1);
            if (config.routing[stat.screen.channel].nbPages < (stat.editStep >> 3)) {
                stat.editStep = (config.routing[stat.screen.channel].nbPages - 1) << 3;
            }
            break;
        case screenConfig: stat.screen.config = testByte(data, 0, 1); break;
        case screenMainMenu: stat.screen.mainMenu = testByte(data, 0, 1); break;
        case screenSubScreen: stat.screen.subscreen = testByte(data, 0, getSubscreenMax()); break;
        case screenRouting: stat.screen.routing = testByte(data, 0, 1); break;
        case screenCalNote: stat.screen.calibration = testByte(data, 0, 1); break;
        case screenCalCv: stat.screen.calibrationCv = testByte(data, 0, 1); break;

        case liveCalNote: stat.noteToCalibrate = testByte(data, 0, NOTERANGE - 1); break;
        case liveCalCv: stat.cvToCalibrate = testInt(data, -1, 2); break;
        case bpm: stat.bpm = testByte(data, 0, 255); break;
        case play:
            stat.play = testByte(data, 0, 1);
            if (stat.play == 0)
                sendMidiAllNotesOff();
            break;
        case rec: stat.rec = testByte(data, 0, 1); break;
        case error: stat.error = testByte(data, 0, 1); break;
        case bpmSync:
            stat.bpmSync = testByte(data, 0, 1);
            updateClockCounter(true);
            break;
        case bpmPoti:
            stat.bpmPot = testInt(data, 0, 1023);
            if (!stat.bpmSync) {

                if (stat.bpmPot < 512)
                    stat.bpm = map(stat.bpmPot, 0, 511, 0, 119);
                else
                    stat.bpm = map(stat.bpmPot, 512, 1023, 120, 280);
            }
            break;
        case load:
        case save: stat.loadSaveSlot = testByte(data, 0, SAVESLOTS - 1); break;
        case editMode:
            stat.editMode = testByte(data, 0, 1);
            stat.editStep = get(activePage, stat.screen.channel) * 8;
            break;

        case editStep: stat.editStep = testByte(data, 0, (config.routing[stat.screen.channel].nbPages - 1) << 3); break;

        case pulseLength: config.pulseLength = testInt(data, 0, 1000); break;
        case none: break;
        default: PRINTLN("FrankData set(frankData frankDataType, byte data  ), no case found");
    }
}

// set data, array
void FrankData::set(const frankData &frankDataType, const int16_t &data, const byte &array) {
    switch (frankDataType) {
        case outputSource: config.routing[array].outSource = testByte(data, 0, OUTPUTS); break;
        case outputChannel: config.routing[array].channel = testByte(data, 0, 16); break;
        case outputArp:
            config.routing[array].arp = testByte(data, 0, 2);
            updateArp(array);
            break;
        case outputCc: config.routing[array].cc = testByte(data, 0, 4); break;
        case outputLiveMode:
            config.routing[array].liveMidiMode = testByte(data, 0, 2);
            liveMidi[array].triggered = 1;
            break;
        case outputClock: config.routing[array].clockSpeed = testByte(data, 0, 22); break;
        case outputClockingOffset: config.routing[array].clockingOffset = testByte(data, 0, 23); break;
        case outputRatchet: config.routing[array].arpRatchet = testByte(data, 0, 3); break;
        case outputArpOctave: config.routing[array].arpOctaves = testInt(data, -3, 3); break;
        case outputArpMode:
            config.routing[array].arpMode = testByte(data, 0, 7);
            updateArp(array);
            break;
        case outputPolyRhythm: config.routing[array].polyRhythm = testByte(data, 0, 3); break;
        case outputMidiNotes:
            config.routing[array].midiNoteOut = testByte(data, 0, 3);
            sendMidiAllNotesOff();
            break;

        case cvCalOffset: cal[array].cvOffset = testInt(data, -500, 500); break;
        case cvCalLower: cal[array].cvLowerLimit = testInt(data, 0, 1000); break;
        case cvCalUpper: cal[array].cvUpperLimit = testInt(data, -1000, 0); break;
        case cvPitchbendCalLower: cal[array].cvPitchbendLowerLimit = testInt(data, 0, 1000); break;
        case cvPitchbendCalUpper: cal[array].cvPitchbendUpperLimit = testInt(data, -1000, 0); break;
        case noteScaleOffset: cal[array].noteScaleOffset = testInt(data, -100, 100); break;

        case liveMod: liveMidi[array].mod = testByte(data, 0, 127); break;
        case livePitchbend: liveMidi[array].pitchbend = testInt(data, -8192, 8191); break;
        case liveAftertouch: liveMidi[array].aftertouch = testByte(data, 0, 127); break;
        case liveSustain: liveMidi[array].sustain = testByte(data, 0, 127); break;
        case liveTriggered: liveMidi[array].triggered = testByte(data, 0, 1); break;
        case liveArpTriggeredNewNote: liveMidi[array].arpTriggeredNewNote = testByte(data, 0, 1); break;
        case liveReleased: liveMidi[array].released = testByte(data, 0, 1); break;
        case liveRecModePlayback: liveMidi[array].recModePlayback = testByte(data, 0, 1); break;

        case stepArp: liveMidi[array].stepArp = testByte(data, 0, NOTERANGE - 1); break;
        case stepSeq: liveMidi[array].stepSeq = testByte(data, 0, (config.routing[stat.screen.channel].nbPages - 1) << 3); break;
        case stepSpeed: config.routing[array].stepSpeed = testByte(data, 0, 22); break;
        case nbPages:
            config.routing[array].nbPages = testByte(data, 1, PAGES);
            if (stat.editMode) {
                if (stat.editStep / STEPSPERPAGE > config.routing[stat.screen.channel].nbPages - 1)
                    stat.editStep = (config.routing[stat.screen.channel].nbPages - 1) << 3;
            }
            break;

        case liveCalNote: stat.noteToCalibrate = testByte(data, 0, NOTERANGE - 1); break;
        case liveCalCv: stat.cvToCalibrate = testInt(data, -1, 2); break;
        case liveMidiUpdateWaitTimer: liveMidi[array].midiUpdateWaitTimer = data; break;

        case seqOctaveOffset: config.routing[array].seqOctaves = testInt(data, -5, 5); break;
        case seqNoteOffset: config.routing[array].seqNotes = testInt(data, -12, 12); break;

        case seqTuning: seq[array].sequence.tuning = testByte(data, 0, 13); break;
        case seqPageEndOffset: seq[array].sequence.pageEndOffset = testInt(data, -STEPSPERPAGE + 1, 0); break;

        case seqGateLengthOffset: seq[array].sequence.gateLengthOffset = testInt(data, -100, 100); break;
        case copySeq: break;
        case none: break;
        default: PRINTLN("FrankData set(frankData frankDataType, byte data, byte array), no case found");
    }
}

// set data, array, step
void FrankData::set(const frankData &frankDataType, const int16_t &data, const byte &array, const byte &step) {
    switch (frankDataType) {
        case seqNote: seq[array].setNote(step, data); break;
        case seqGate: seq[array].setGate(step, data); break;
        case seqGateLength: seq[array].sequence.gateLength[step] = testByte(data, 0, 100); break;
        case seqCc: seq[array].sequence.cc[step] = testByte(data, 0, 127); break;

        case noteCalOffset: cal[array].noteCalibration[step] = testInt(data, -100, 100); break;
        case none: break;
        default:
            PRINTLN("FrankData set(frankData frankDataType, byte data, byte array, byte step), no case "
                    "found");
    }
}

void FrankData::change(const frankData &frankDataType, const int16_t &amount) {
    set(frankDataType, get(frankDataType) + amount);
}
void FrankData::change(const frankData &frankDataType, const int16_t &amount, const byte &array) {
    set(frankDataType, get(frankDataType, array) + amount, array);
}
void FrankData::change(const frankData &frankDataType, const int16_t &amount, const byte &array, const byte &step) {
    set(frankDataType, get(frankDataType, array, step) + amount, array, step);
}

void FrankData::increase(const frankData &frankDataType) {
    change(frankDataType, 1);
}
void FrankData::increase(const frankData &frankDataType, const byte &array) {
    switch (frankDataType) {
        case stepArp: nextArpStep(array); break;
        case stepSeq: increaseSeqStep(array); break;
        default: change(frankDataType, 1, array);
    }
}
void FrankData::increase(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
        case seqNote:
            seq[array].increaseNote(step);
            if (!stat.play) {
                for (byte x = 0; x < OUTPUTS; x++) {
                    if (config.routing[x].outSource) {
                        liveMidi[x].triggered = 1;
                        liveMidi[x].recModePlayback = 0;
                    }
                }
            }
            break;
        default: change(frankDataType, 1, array, step);
    }
}

void FrankData::decrease(const frankData &frankDataType) {
    change(frankDataType, -1);
}
void FrankData::decrease(const frankData &frankDataType, const byte &array) {
    switch (frankDataType) {
        case stepArp: nextArpStep(array); break;
        case stepSeq: decreaseSeqStep(array); break;
        default: change(frankDataType, -1, array);
    }
}
void FrankData::decrease(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
        case seqNote:
            seq[array].decreaseNote(step);
            if (!stat.play) {
                for (byte x = 0; x < OUTPUTS; x++) {
                    if (config.routing[x].outSource) {
                        liveMidi[x].triggered = 1;
                        liveMidi[x].recModePlayback = 0;
                    }
                }
            }
            break;
        default: change(frankDataType, -1, array, step);
    }
}

void FrankData::toggle(const frankData &frankDataType) {
    // save toggle values, instead of simply resetting to default
    static int16_t bufferedRatchet[OUTPUTS] = {0, 0};
    static int16_t bufferedStepSpeed[OUTPUTS] = {10, 10};
    static int16_t bufferedClockSpeed[OUTPUTS] = {10, 10};
    static int16_t bufferedArpOctaves[OUTPUTS] = {0, 0};
    static int16_t bufferedArpMode[OUTPUTS] = {0, 0};
    static int16_t bufferedGateLengthOffset[OUTPUTS] = {0, 0};
    static int16_t bufferedLiveMode[OUTPUTS] = {0, 0};
    static int16_t bufferedOutputSource[OUTPUTS] = {0, 0};
    static int16_t bufferedSeqNotes[OUTPUTS] = {0, 0};
    static int16_t bufferedSeqOctaves[OUTPUTS] = {0, 0};
    static int16_t bufferedClockingOffset[OUTPUTS] = {0, 0};
    int16_t temp;

    switch (frankDataType) {
        case direction: config.direction = toggleValue(config.direction); break;
        case play: stat.play = toggleValue(stat.play); break;
        case rec: stat.rec = toggleValue(stat.rec); break;
        case editMode:
            stat.editMode = toggleValue(stat.editMode);
            stat.editStep = get(activePage, stat.screen.channel) * 8;

            break;
        case bpmSync:
            stat.bpmSync = toggleValue(stat.bpmSync);
            updateClockCounter(true);
            break;
        case outputArp:
            config.routing[stat.screen.channel].arp = toggleValue(config.routing[stat.screen.channel].arp);
            updateArp(stat.screen.channel);
            break;
        case screenMainMenu: stat.screen.mainMenu = toggleValue(stat.screen.mainMenu); break;
        case screenConfig:
            stat.screen.config = toggleValue(stat.screen.config);
            if (stat.screen.config == 0)
                saveMenuSettings();
            break;
        case screenRouting:
            if (stat.screen.mainMenu || stat.screen.routing || stat.screen.calibration || stat.screen.calibrationCv) {
                stat.screen.mainMenu = 0;
                stat.screen.routing = 0;
                stat.screen.calibration = 0;
                stat.screen.calibrationCv = 0;
                saveMenuSettings();
            }
            else {
                stat.screen.mainMenu = 0;
                stat.screen.routing = 1;
                stat.screen.calibration = 0;
                stat.screen.calibrationCv = 0;
            }
            break;
        case screenCalNote:
            stat.screen.mainMenu = 0;
            stat.screen.routing = 0;
            stat.screen.calibration = 1;
            stat.screen.calibrationCv = 0;
            break;
        case screenCalCv:
            stat.screen.mainMenu = 0;
            stat.screen.routing = 0;
            stat.screen.calibration = 0;
            stat.screen.calibrationCv = 1;
            break;

        case outputPolyRhythm: config.routing[stat.screen.channel].polyRhythm = 0;
        case outputRatchet:
            temp = config.routing[stat.screen.channel].arpRatchet;
            config.routing[stat.screen.channel].arpRatchet = bufferedRatchet[stat.screen.channel];
            bufferedRatchet[stat.screen.channel] = temp;
            break;
        case outputArpMode:
            temp = config.routing[stat.screen.channel].arpMode;
            config.routing[stat.screen.channel].arpMode = bufferedArpMode[stat.screen.channel];
            bufferedArpMode[stat.screen.channel] = temp;
            break;
        case outputArpOctave:
            temp = config.routing[stat.screen.channel].arpOctaves;
            config.routing[stat.screen.channel].arpOctaves = bufferedArpOctaves[stat.screen.channel];
            bufferedArpOctaves[stat.screen.channel] = temp;
            break;
        case outputLiveMode:
            temp = config.routing[stat.screen.channel].liveMidiMode;
            config.routing[stat.screen.channel].liveMidiMode = bufferedLiveMode[stat.screen.channel];
            bufferedLiveMode[stat.screen.channel] = temp;
            break;
        case stepSpeed:
            temp = config.routing[stat.screen.channel].stepSpeed;
            config.routing[stat.screen.channel].stepSpeed = bufferedStepSpeed[stat.screen.channel];
            bufferedStepSpeed[stat.screen.channel] = temp;
            break;
        case outputClock:
            temp = config.routing[stat.screen.channel].clockSpeed;
            config.routing[stat.screen.channel].clockSpeed = bufferedClockSpeed[stat.screen.channel];
            bufferedClockSpeed[stat.screen.channel] = temp;
            break;
        case outputClockingOffset:
            temp = config.routing[stat.screen.channel].clockingOffset;
            config.routing[stat.screen.channel].clockingOffset = bufferedClockingOffset[stat.screen.channel];
            bufferedClockingOffset[stat.screen.channel] = temp;
            break;
        case outputSource:
            temp = config.routing[stat.screen.channel].outSource;
            config.routing[stat.screen.channel].outSource = bufferedOutputSource[stat.screen.channel];
            bufferedOutputSource[stat.screen.channel] = temp;
            break;
        case seqOctaveOffset:
            temp = config.routing[stat.screen.channel].seqOctaves;
            config.routing[stat.screen.channel].seqOctaves = bufferedSeqOctaves[stat.screen.channel];
            bufferedSeqOctaves[stat.screen.channel] = temp;
            break;
        case seqNoteOffset:
            temp = config.routing[stat.screen.channel].seqNotes;
            config.routing[stat.screen.channel].seqNotes = bufferedSeqNotes[stat.screen.channel];
            bufferedSeqNotes[stat.screen.channel] = temp;
            break;
        case seqGateLengthOffset:
            temp = seq[config.routing[stat.screen.channel].outSource - 1].sequence.gateLengthOffset;
            seq[config.routing[stat.screen.channel].outSource - 1].sequence.gateLengthOffset = bufferedGateLengthOffset[stat.screen.channel];
            bufferedGateLengthOffset[stat.screen.channel] = temp;
            break;

        case seqTuning: seq[config.routing[stat.screen.channel].outSource - 1].sequence.tuning = 0; break;
        case seqPageEndOffset: seq[config.routing[stat.screen.channel].outSource - 1].sequence.pageEndOffset = 0; break;

        case nbPages: config.routing[stat.screen.channel].nbPages = 8; break;
        case pulseLength: config.pulseLength = 20; break;

        case seqResetGates: seqResetAllGates(config.routing[stat.screen.channel].outSource - 1); break;
        case seqResetNotes:
            seqResetAllNotes(config.routing[stat.screen.channel].outSource - 1);
            config.routing[stat.screen.channel].seqNotes = 0;
            config.routing[stat.screen.channel].seqOctaves = 0;
            break;
        case seqResetGateLengths: seqResetAllGateLengths(config.routing[stat.screen.channel].outSource - 1); break;
        case seqResetCC: seqResetAllCC(config.routing[stat.screen.channel].outSource - 1); break;
        case seqOctaveUp: seqAllOctaveUp(config.routing[stat.screen.channel].outSource - 1); break;
        case seqOctaveDown: seqAllOctaveDown(config.routing[stat.screen.channel].outSource - 1); break;
        case copySeq:
            seqCopy(config.routing[stat.screen.channel].outSource - 1, !(config.routing[stat.screen.channel].outSource - 1));
            break; // works only for two seq objects

        case resetStepCounters: resetAllStepCounter(); break;

        case saveCal: saveNoteCalibration(); break;

        case load: loadSequence(stat.loadSaveSlot, config.routing[stat.screen.channel].outSource - 1); break;
        case save: saveSequence(stat.loadSaveSlot, config.routing[stat.screen.channel].outSource - 1); break;

        case cvCalOffset: cal[stat.screen.channel].cvOffset = 0; break;
        case noteScaleOffset: cal[stat.screen.channel].noteScaleOffset = 0; break;
        case cvCalLower: cal[stat.screen.channel].cvLowerLimit = 0; break;
        case cvCalUpper: cal[stat.screen.channel].cvUpperLimit = 0; break;
        case cvPitchbendCalLower: cal[stat.screen.channel].cvPitchbendLowerLimit = 0; break;
        case cvPitchbendCalUpper: cal[stat.screen.channel].cvPitchbendUpperLimit = 0; break;

        case none: break;
        default: PRINTLN("FrankData toggle(frankData frankDataType), no case found");
    }
}

void FrankData::toggle(const frankData &frankdataType, const byte &array) {
    switch (frankdataType) {
        case seqResetGates: seqResetAllGates(array); break;
        case seqResetNotes: seqResetAllNotes(array); break;
        case seqResetGateLengths: seqResetAllGateLengths(array); break;
        case seqResetCC: seqResetAllCC(array); break;
        case copySeq: seqCopy(array, !array); break; // works only for two seq objects
        case resetStepCounters: resetAllStepCounter(); break;
        case seqOctaveOffset: setStr("Oct+-"); break;
        case seqNoteOffset: setStr("Not+-"); break;
        default: PRINTLN("FrankData toggle(frankData frankDataType, const byte &array), no case found");
    }
}

void FrankData::toggle(const frankData &frankDataType, const byte &array, const byte &step) {
    switch (frankDataType) {
        case seqGate: seq[array].setGate(step, toggleValue(seq[array].getGate(step))); break;
        case noteCalOffset: cal[array].noteCalibration[step] = 0;
        case none: break;
        default: PRINTLN("FrankData toggle(frankData frankDataType, byte array, byte step), no case found");
    }
}

const char *FrankData::getNameAsStr(const frankData &frankDataType) {
    switch (frankDataType) {
        case copySeq: setStr("CopSq"); break;
        case seqNote: setStr("Note"); break;
        case seqGate: setStr("Gate"); break;
        case seqGateLength: setStr("GateLn"); break;
        case seqCc: setStr("CC"); break;
        case seqTuning: setStr("Tune"); break;
        case seqPageEndOffset: setStr("PgEnd"); break;
        case seqGateLengthOffset: setStr("GL-OF"); break;
        case stepSpeed: setStr("Step"); break;
        case seqResetNotes: setStr("Rs Nt"); break;
        case seqResetGates: setStr("Rs Gt"); break;
        case seqResetGateLengths: setStr("Rs GL"); break;
        case seqResetCC: setStr("Rs CC"); break;
        case seqOctaveUp: setStr("OctUp"); break;
        case seqOctaveDown: setStr("OctDn"); break;
        case seqOctaveOffset: setStr("Oct+-"); break;
        case seqNoteOffset: setStr("Not+-"); break;

        case midiSource: setStr("Midi"); break;
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

        case outputPolyRhythm: setStr("Poly"); break;

        case outputRatchet: setStr("Reps"); break;
        case outputArpOctave: setStr("Oct"); break;

        case outputLiveMode: setStr("Key"); break;

        case outputClock: setStr("Clock"); break;
        case outputClockingOffset: setStr("ClOff"); break;
        case outputMidiNotes: setStr("NtOut"); break;

        case screenOutputChannel: setStr("Rout"); break;
        case screenConfig: setStr("Conf"); break;
        case screenMainMenu: setStr("Main"); break;
        case screenSubScreen: setStr("Sub"); break;
        case screenCalNote: setStr("calN"); break;
        case screenCalCv: setStr("calCv"); break;
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
        case editMode: setStr("EditM"); break;
        case editStep: setStr("EditS"); break;

        case liveMod: setStr("Mod"); break;
        case livePitchbend: setStr("PB"); break;
        case liveAftertouch: setStr("AftT"); break;
        case liveSustain: setStr("Sus"); break;
        case liveTriggered: setStr("TRIG"); break;
        case liveLatestKey: setStr("Late"); break;
        case liveLowestKey: setStr("Low"); break;
        case liveHighestKey: setStr("High"); break;
        case liveKeyArpNoteEvaluated:
        case liveCalNote:
        case liveKeyNoteEvaluated: setStr("Note"); break;
        case liveCalCv: setStr("CV"); break;
        case liveKeyArpVelEvaluated:
        case liveKeyVelEvaluated: setStr("Vel"); break;
        case saveCal: setStr("Save"); break;

        case noteCalOffset: setStr("NCal"); break;
        case cvCalOffset: setStr("CvCal"); break;
        case noteScaleOffset: setStr("NSCal"); break;
        case cvCalLower: setStr("CV Lo"); break;
        case cvCalUpper: setStr("CV Hi"); break;
        case cvPitchbendCalLower: setStr("PB Lo"); break;
        case cvPitchbendCalUpper: setStr("PB Hi"); break;

        case resetStepCounters: setStr("RsStep"); break;

        case none: setStr(""); break;

        default: PRINTLN("FrankData getNameAsStr(frankData frankDataType), no case found"); setStr("ERR");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType) {
    switch (frankDataType) {
        case screenOutputChannel:
        case screenConfig:
        case screenSubScreen:

        case displayBrightness:

        case liveCalNote:

        case bpm:
        case bpmSync:
        case pulseLength:
        case bpmPoti: setStr(toStr(get(frankDataType))); break;
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
        case none: setStr(""); break;
        default: setStr("ERR"); PRINTLN("FrankData getValueAsStr(const frankData &frankDataType), no case found");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType, const byte &channel) {
    char tempStr[5];

    switch (frankDataType) {
        case stepSeq:
        case stepArp:
        case nbPages:
        case seqGateLengthOffset:
        case seqPageEndOffset:

        case seqOctaveOffset:
        case seqNoteOffset:

        case outputRatchet:
        case outputArpOctave:

        case liveMod:
        case livePitchbend:
        case liveAftertouch:
        case liveSustain:
        case liveTriggered:

        case cvCalLower:
        case cvCalUpper:
        case cvPitchbendCalLower:
        case cvPitchbendCalUpper:
        case noteScaleOffset:
        case cvCalOffset: setStr(toStr(get(frankDataType, channel))); break;

        case outputChannel:
            if (config.routing[channel].channel == 0) {

                setStr("ALL");
            }
            else {
                setStr(toStr(get(frankDataType, channel)));
            }
            break;

        case outputPolyRhythm:
            switch (config.routing[stat.screen.channel].polyRhythm) {
                case 0: setStr("-"); break;
                case 1: setStr("Cl"); break;
                case 2: setStr("Step"); break;
                case 3: setStr("C+S"); break;
                default: setStr("ERR"); break;
            }
            break;
        case outputMidiNotes:
            switch (config.routing[stat.screen.channel].midiNoteOut) {
                case 0: setStr("-"); break;
                case 1: setStr("DIN"); break;
                case 2: setStr("USB"); break;
                case 3: setStr("U+D"); break;
                default: setStr("ERR"); break;
            }
            break;

        case seqTuning: setStr(tuningToChar(seq[channel].sequence.tuning)); break;

        case copySeq:
        case resetStepCounters:
        case screenRouting:
        case screenCalNote:
        case screenCalCv:
        case screenMainMenu:
        case seqResetGates:
        case seqResetGateLengths:
        case seqResetCC:
        case seqOctaveUp:
        case seqOctaveDown:
        case saveCal:
        case seqResetNotes: setStr("@"); break;
        case noteCalOffset: setStr(toStr(get(frankDataType, channel, stat.noteToCalibrate))); break;
        case liveCalCv:
            switch (stat.cvToCalibrate) {
                case -1: setStr("-5V"); break;
                case 0: setStr("0V"); break;
                case 1: setStr("5V"); break;
                case 2: setStr("PB"); break;
            }
            break;

        case screenOutputChannel:
        case screenConfig:
        case screenSubScreen:

        case displayBrightness:

        case liveCalNote:

        case bpm:
        case bpmSync:
        case pulseLength:
        case bpmPoti: setStr(toStr(get(frankDataType))); break;
        case load:
        case save: setStr(toStr(get(frankDataType) + 1)); break;

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
                case 2: setStr("ud"); break;
                case 3: setStr("du"); break;
                case 4: setStr("urdr"); break;
                case 5: setStr("drur"); break;
                case 6: setStr("ordr"); break;
                case 7: setStr("rnd"); break;
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
                case 2: setStr("Ltch"); break;
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

        case editMode:
            switch (stat.editMode) {
                case 0: setStr("OFF"); break;
                case 1: setStr("ON"); break;
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
        case liveKeyNoteEvaluated: setStr(toStr(getLiveKeyEvaluated(channel).note)); break;
        case liveKeyVelEvaluated: setStr(toStr(getLiveKeyEvaluated(channel).velocity)); break;
        case liveKeyArpNoteEvaluated: setStr(toStr(liveMidi[channel].arpKey.note)); break;
        case liveKeyArpVelEvaluated: setStr(toStr(liveMidi[channel].arpKey.velocity)); break;

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
        case outputCcEvaluated:
            if (config.routing[channel].cc == 2) {
                setStr(toStr(getLiveCcEvaluated(channel) / 64));
            }
            else {
                setStr(toStr(getLiveCcEvaluated(channel)));
            }
            break;

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
                case 0: setStr("/64T"); break;
                case 1: setStr("/32T"); break;
                case 2: setStr("/32"); break;
                case 3: setStr("/16T"); break;
                case 4: setStr("/16"); break;
                case 5: setStr("1/8T"); break;
                case 6: setStr("/16."); break;
                case 7: setStr("1/8"); break;
                case 8: setStr("1/4T"); break;
                case 9: setStr("1/8."); break;
                case 10: setStr("1/4"); break;
                case 11: setStr("1/2T"); break;
                case 12: setStr("1/4."); break;
                case 13: setStr("1/2"); break;
                case 14: setStr("1/1T"); break;
                case 15: setStr("1/2."); break;
                case 16: setStr("1/1"); break;
                case 17: setStr("2/1T"); break;
                case 18: setStr("1/1."); break;
                case 19: setStr("2/1"); break;
                case 20: setStr("4/1T"); break;
                case 21: setStr("2/1."); break;
                case 22: setStr("4/1"); break;
                default: setStr("ERR");
            }
            break;
        case outputClock:
            switch (config.routing[channel].clockSpeed) {
                case 0: setStr("/64T"); break;
                case 1: setStr("/32T"); break;
                case 2: setStr("/32"); break;
                case 3: setStr("/16T"); break;
                case 4: setStr("/16"); break;
                case 5: setStr("1/8T"); break;
                case 6: setStr("/16."); break;
                case 7: setStr("1/8"); break;
                case 8: setStr("1/4T"); break;
                case 9: setStr("1/8."); break;
                case 10: setStr("1/4"); break;
                case 11: setStr("1/2T"); break;
                case 12: setStr("1/4."); break;
                case 13: setStr("1/2"); break;
                case 14: setStr("1/1T"); break;
                case 15: setStr("1/2."); break;
                case 16: setStr("1/1"); break;
                case 17: setStr("2/1T"); break;
                case 18: setStr("1/1."); break;
                case 19: setStr("2/1"); break;
                case 20: setStr("4/1T"); break;
                case 21: setStr("2/1."); break;
                case 22: setStr("4/1"); break;
                default: setStr("ERR");
            }
            break;
        case outputClockingOffset:
            switch (config.routing[channel].clockingOffset) {
                case 0: setStr("-"); break;
                case 1: setStr("/64T"); break;
                case 2: setStr("/32T"); break;
                case 3: setStr("/32"); break;
                case 4: setStr("/16T"); break;
                case 5: setStr("/16"); break;
                case 6: setStr("1/8T"); break;
                case 7: setStr("/16."); break;
                case 8: setStr("1/8"); break;
                case 9: setStr("1/4T"); break;
                case 10: setStr("1/8."); break;
                case 11: setStr("1/4"); break;
                case 12: setStr("1/2T"); break;
                case 13: setStr("1/4."); break;
                case 14: setStr("1/2"); break;
                case 15: setStr("1/1T"); break;
                case 16: setStr("1/2."); break;
                case 17: setStr("1/1"); break;
                case 18: setStr("2/1T"); break;
                case 19: setStr("1/1."); break;
                case 20: setStr("2/1"); break;
                case 21: setStr("4/1T"); break;
                case 22: setStr("2/1."); break;
                case 23: setStr("4/1"); break;
                default: setStr("ERR");
            }
            break;
        case none: setStr(""); break;

        default: setStr("ERR"); PRINTLN("FrankData getValueAsStr(const frankData &frankDataType, const byte &channel), no case found");
    }
    return returnStr;
}

const char *FrankData::getValueAsStr(const frankData &frankDataType, const byte &channel, const byte &step) {
    switch (frankDataType) {
        case seqNote:
        case seqGate:
        case seqGateLength:
        case seqCc:

        case none: setStr(""); break;

        default: PRINTLN("FrankData getValueAsStr(const frankData &frankDataType, const byte &step), no case found"); setStr("NONE");
    }
    return returnStr;
}

void FrankData::setStr(const char *newStr) {
    strncpy(str, newStr, MAXSTRINGSIZE - 1);
    str[MAXSTRINGSIZE] = '\0';
}

void FrankData::loadSequence(const byte &saveSlot, const byte &sequence) {
    Serial.print("Load Slot ");
    Serial.print(saveSlot);
    Serial.print(" into Sequence ");
    Serial.println(sequence);

    if (sequence > OUTPUTS - 1 || saveSlot > SAVESLOTS - 1) {
        Serial.println("received wrong settings");
        return;
    }

    int memory = saveSlot * sizeof(structSequence) + 300; // leave space for menu and calibration

    EEPROM.get(memory, seq[sequence].sequence);
}

void FrankData::saveSequence(const byte &saveSlot, const byte &sequence) {
    Serial.print("Store Sequence ");
    Serial.print(sequence);
    Serial.print(" into Slot ");
    Serial.println(saveSlot);

    if (sequence > OUTPUTS - 1 || saveSlot > SAVESLOTS - 1) {
        Serial.println("received wrong settings");
        return;
    }

    int memory = saveSlot * sizeof(structSequence) + 300; // leave space for menu and calibration

    EEPROM.put(memory, seq[sequence].sequence);
}

void FrankData::loadMenuSettings() {
    Serial.println("Load Menu settings");
    int memory = 1;

    EEPROM.get(memory, config);
}

void FrankData::saveMenuSettings() {
    Serial.println("Save Menu settings");
    int memory = 1;

    EEPROM.put(memory, config);
}

void FrankData::saveNoteCalibration() {
    Serial.println("Store Note Calibration ");
    int memory = 50; // leave space for menu

    EEPROM.put(memory, cal);
}

void FrankData::loadNoteCalibration() {
    Serial.println("Get Note Calibration ");
    int memory = 50; // leave space for menu

    EEPROM.get(memory, cal);
}

// utility
// test byte range and return valid byte
byte testByte(const int16_t &value, const byte &minimum, const byte &maximum) {
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

int16_t testInt(const int16_t &value, const int16_t &minimum, const int16_t &maximum) {
    if (value > maximum) {
        return maximum;
    }
    else if (value < minimum) {
        return minimum;
    }
    else {
        return value;
    }
}

// increase byte
byte increaseByte(const byte &value, const byte &maximum) {
    if (value == maximum) {
        return value;
    }
    else {
        return value + 1;
    }
}

// decrease byte
byte decreaseByte(const byte &value, const byte &minimum) {
    if (value == minimum) {
        return value;
    }
    else {
        return value - 1;
    }
}

// change byte value and check boundaries
byte changeByte(const byte &value, const int16_t &change, const byte &minimum, const byte &maximum, const bool &clampChange) {

    if ((int16_t)value + change > maximum) { // test max

        return clampChange ? maximum : value;
    }

    else if ((int16_t)value + change < minimum) { // test min
        return clampChange ? minimum : value;
    }
    else {
        return (byte)((int16_t)value + change); // return new value
    }
}

int16_t changeInt(const int16_t &value, const int16_t &change, const int16_t &minimum, const int16_t &maximum, const bool &clampChange) {

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

byte changeByteReverse(const byte &value, const int16_t &change, const byte &minimum, const byte &maximum) {

    if ((int16_t)value + change > maximum) { // test max

        return minimum;
    }
    else if ((int16_t)value + change < minimum) { // test min
        return maximum;
    }
    else {
        return ((byte)value + change); // return new value
    }
}

int16_t changeIntReverse(const int16_t &value, const int16_t &change, const int16_t &minimum, const int16_t &maximum) {

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
template <typename T> T toggleValue(const T &data) {
    return !data;
}

// convert number to string
template <typename T> char *toStr(const T &data) {
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

int sort_asc(const void *cmp1, const void *cmp2) {
    // Need to cast the void * to int *
    byte a = ((structKey *)cmp1)->note;
    byte b = ((structKey *)cmp2)->note;
    // The comparison
    return a > b ? 1 : (a < b ? -1 : 0);
}