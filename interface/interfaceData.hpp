#pragma once

#include "src/EEPROM/EEPROM.h"
#include <Arduino.h>

#define SAVESLOTS 20 // number of possible save slots
#define LENGTH 128   // max Seq length
#define PAGES 16
#define NOTERANGE 88
#define STEPSPERPAGE 8
#define OUTPUTS 2 // Number of output lanes
#define MAXSTRINGSIZE 8

#define MIDIARPUPDATEDELAY 3 // in millis

#define DATAOBJ FrankData::getDataObj()
#define CHANNEL DATAOBJ.get(FrankData::screenOutputChannel)
#define SEQCHANNEL (DATAOBJ.get(FrankData::outputSource, DATAOBJ.get(FrankData::screenOutputChannel)) - 1)

typedef struct {
    byte outSource = 0;    // 0 = live, 1 = seq1, 2 = seq2, ...
    byte channel = 0;      // 0 = all, 1 = channel 1, ...
    byte arp = 0;          // 0 = off, 1 = on, 2 = Latch
    byte arpMode = 0;      // 0 = up, 1 = down, 2 = updown, 3= downup, 4 = upRdownR, 5 = downRupR, 6 = order, 7 = random
    byte cc = 0;           // 0 = vel, 1 = mod, 2 = pitchbend, 3 = aftertouch, 4 = sustain
    byte liveMidiMode = 0; // 0 = latest, 1 = lowest, 2 = highest
    byte clockSpeed = 2;   // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = 1 bar, 5 = 2 bars
    byte arpRatchet = 0;   // repeats per step, 1 = 1 repeat (2 notes total), up to 3 repeats
    int8_t arpOctaves = 0; // Octaves -3 ... 0 ... 3
    byte stepSpeed = 2;    // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = 1 bar, 5 = 2 bars
    byte nbPages = 8;      // nb Pages  1-16
} structOutputRouting;

typedef struct {
    int8_t noteCalibration[NOTERANGE] = {0};
    int16_t cvOffset = 0;
    int16_t noteScaleOffset = 0;
    int16_t cvLowerLimit = 0;
    int16_t cvUpperLimit = 0;
    int16_t cvPitchbendLowerLimit = 0;
    int16_t cvPitchbendUpperLimit = 0;
} structCalibration;

// Sequence struct holding all values for a sequence, to save it
typedef struct {
    byte note[LENGTH];
    byte cc[LENGTH];
    byte gate[(LENGTH / 8)]; // optimize data to single bits! middleman seq check, if gate
    byte gateLength[LENGTH]; //
    byte tuning;             // tuning offset
    int8_t gateLengthOffset; // 100 = no offset
} structSequence;

// Settings struct for all settings that need to be saved permanently
typedef struct {
    byte midiSource = 1;         // active MidiDevice (usb -> 1, din -> 0)
    byte direction = 1;          // 0 -> reverse ; 1 -> forward
    byte displayBrightness = 70; // 0-100;
    uint16_t pulseLength = 20;   // pulse length in ms

    structOutputRouting routing[OUTPUTS]; // hold settings for that many outputs
} structSettings;

// possible screen status
typedef struct {
    byte channel = 0;                // active channel, 0-> Channel 1, 1-> Channel 2
    byte config = 0;                 // display config, 0-> off, 1-> on
    byte mainMenu = 0;               // display Main Menu, 0-> off, 1-> on
    byte subscreen = 0;              // subscreen -> current displayed screen .. note, gate, cv (seq) ; live, appregiator (live)
    byte calibration = 0;            // calibration screen
    byte calibrateNote = 0;          // Note calibration screen
    byte calibrationCv = 0;          // Note calibration screen
    byte routing = 0;                // routing screen
    const byte subScreenMaxSeq = 2;  // Number of subscreens for seq mode
    const byte subScreenMaxLive = 0; // Number of subscreens for live mode
} structScreen;

// all Settings that don't need to be saved permanently
typedef struct {
    structScreen screen; // screen status

    byte loadSaveSlot = 0; // laod save 1-10

    uint16_t bpm = 120; // current bpm
    byte play = 1;      // play stop
    byte rec = 0;       // Rec Active
    byte error = 0;     // ErrorFlag

    byte noteToCalibrate = 0; // note value that gets calibrated
    int8_t cvToCalibrate = 0; // cv value * 2048 that gets calibrated

    byte bpmSync = 0;        // Sync Active
    byte midiClockCount = 5; // counts incoming midiclock signals (6 ticks per 16th)
    byte bpm16thCount = 0;   // general 16th counter for clock outputs

    // byte receivedNewMidiDataArp = 0;

    // uint32_t last16thTime = 0;
    uint16_t bpmPot = 120; // sync= 0 ? 0-1023 bpm log : divider /4, /2, 1, *2, *4 ; Range is 0-1023 (not yet implemented)
} structStatus;

// Midi Key data
typedef struct {
    byte note = 0;
    byte velocity = 0;
} structKey;

// pressed Notes
class PressedNotesElement {
  public:
    PressedNotesElement(byte note, byte velocity) {
        this->note = note;
        this->velocity = velocity;
        this->next = NULL;
    }
    byte note;
    byte velocity;
    PressedNotesElement *next;
};

class PressedNotesList {
  public:
    int16_t size = 0;

    PressedNotesElement *pressedNoteElement = NULL;
    void appendKey(const byte &note, const byte &velocity);
    void deleteKey(const byte &note);
    void deleteAllKeys();
    bool containsElements();

    PressedNotesElement *getKeyHighest();
    PressedNotesElement *getKeyLowest();
    PressedNotesElement *getKeyLatest();

    PressedNotesElement *getElement(const byte &element);
};

// save live midi data
class LiveMidi {
  public:
    PressedNotesList noteList; // Key List for Live Midi
    byte mod = 0;              // live midi mod
    int16_t pitchbend = 0;     // live midi pitchbend
    byte aftertouch = 0;       // live midi aftertouch
    byte sustain = 0;          // live midi sustain
    byte triggered = 0;        // key was pressed
    byte released = 0;         // key was released

    byte arpDirection = 1;        // arp direction for updown etc, 1 = up
    byte arpOctaveDirection = 1;  // arp octave direction for updown etc, 1 = up
    byte arpRetrigger = 1;        // clears Arp Array on next iteration
    byte arpTriggeredNewNote = 0; // Arp has a new step to send out via middleman
    byte arpStepRepeat = 1;       // arp repeats step, for upRdownR, etc
    byte arpRestarted = 1;        // arp was reset
    uint32_t arpOffsetTime = 0;
    byte arp16thCount = 0;

    byte recModePlayback = 0; // status for rec mode playback (last recorded note will be played instead of current step)

    int16_t arpOctave = 0; // current arp octave being played

    structKey lastKey;             // always holds the last played key, if no keys are pressed
    structKey arpKey;              // always holds the last played key, if no keys are pressed
    PressedNotesList arpList;      // Key List for arpeggiator
    structKey arpArray[NOTERANGE]; // sorted array for arpeggiator
    elapsedMillis midiUpdateWaitTimer = 0;

    byte stepArp = 0; // current arp step
    byte stepSeq = 0; // current seq step

    uint16_t channel16thCount = 0; // individual 16th counter for asynchronous playback

    LiveMidi() {
        // initialize with a default key
        keyPressed(24, 0);
        keyReleased(24);
    }

    void keyPressed(const byte &note, const byte &velocity);
    void keyReleased(const byte &note);

    bool keysPressed();

    void updateArpArray(const byte &arpSettings);

    structKey getKeyHighest();
    structKey getKeyLowest();
    structKey getKeyLatest();

    structKey getArpKey(const byte &step);

    void reset();

  private:
    void copyArpListToArray();
};

// Sequence class
class Seq {
  public:
    Seq() { init(); }

    void init(const byte &note = 24, const byte &gate = 1, const byte &gateLength = 50, const byte &cc = 64, const byte &tuning = 0,
              const int16_t &gateLengthOffset = 0); // init sequence to default values

    // Note
    void setNote(const byte &index, const byte &note); // set note value
    void setNotes(const byte &note);                   // set all note values
    void setCCs(const byte &cc);                       // set all note values
    void setGates(const byte &gate);                   // set all note values

    void increaseNote(const byte &index); // increase note value and return new note, function take care of tuning
    void decreaseNote(const byte &index); // decrease note value and return new note, function take care of tuning

    void changeNotes(const int16_t &change); // change all note values
    void octaveUp();                         // All notes one octave down (if possible)
    void octaveDown();                       // All notes one octave down (if possible)

    void setGate(const byte &index, const byte &gate);
    byte getGate(const byte &index);

    // GateLength
    void setGateLengths(const byte &gateLength); // set all gates at once
    void changeGateLengths(const int16_t &change);

    // Sequence
    void setSequence(structSequence *copySeq); // set all sequence values at once
    structSequence *getSequence();             // return the sequence struct pointer

    int16_t getSequenceSize(); // return the struct size

    // Sequence
    structSequence sequence;
};

// data class
class FrankData {

  public:
    // storage enumerator
    enum frankData : byte {
        // frankData = 0 is referred to none, so if-statements will work as expected
        none,

        // Seq, needs value, array, step
        seqNote,
        seqGate,
        seqGateLength,
        seqCc,

        // calibration, needs value, array, step
        noteCalOffset,

        // calibration, needs value, array
        cvCalOffset,
        noteScaleOffset,
        liveCalNote,
        liveCalCv,
        cvCalUpper,
        cvCalLower,
        cvPitchbendCalUpper,
        cvPitchbendCalLower,

        // Seq, needs value, array
        seqTuning,
        seqGateLengthOffset,
        stepSpeed,
        nbPages,
        stepSeq,
        activePage,
        seqResetNotes,
        seqResetGates,
        seqResetGateLengths,
        seqResetCC,
        seqOctaveUp,
        seqOctaveDown,
        copySeq,
        stepOnPage,
        currentPageNumber,

        // general Settings, needs value
        midiSource,
        direction,
        displayBrightness,
        resetStepCounters,
        saveCal,

        // Output Routing Settings, needs value, array
        stepArp,
        outputSource,
        outputChannel,
        outputArp,
        outputArpMode,
        outputRatchet,
        outputArpOctave,
        outputCc,
        outputCcEvaluated,
        outputLiveMode,
        outputClock,

        // Screen Settings, needs value
        screenOutputChannel,
        screenConfig,
        screenMainMenu,
        screenSubScreen,
        screenCalNote,
        screenCalCv,
        screenRouting,

        // structStatus, needs value
        bpm,
        play,
        rec,
        error,
        bpmSync,
        bpm16thCount,
        bpmPoti,
        load,
        save,
        pulseLength,

        // liveMidi, needs value, array
        liveMod,
        livePitchbend,
        liveAftertouch,
        liveSustain,
        liveTriggered,
        liveArpTriggeredNewNote,
        liveReleased,
        liveRecModePlayback,
        liveKeyArpNoteEvaluated,
        liveKeyArpVelEvaluated,
        liveKeyNoteEvaluated,
        liveKeyVelEvaluated,
        liveKeysPressed,
        liveLatestKey,
        liveHighestKey,
        liveLowestKey,
        // liveNewMidiDataArp,
        liveMidiUpdateWaitTimer,

    };

  private:
    FrankData() {}

    structStatus stat;
    structSettings config;
    structCalibration cal[OUTPUTS];

    LiveMidi liveMidi[OUTPUTS];
    Seq seq[OUTPUTS];
    char str[MAXSTRINGSIZE + 1]; // MAXSTRINGSIZE + escape char

  public:
    const char *returnStr = str;

    void init();

    // receive MIDI
    void receivedKeyPressed(const byte &channel, const byte &note, const byte &velocity);
    void receivedKeyReleased(const byte &channel, const byte &note);
    void receivedMidiClock();
    void receivedMidiSongPosition(unsigned int spp);
    void receivedStart();
    void receivedContinue();
    void receivedStop();
    void receivedReset();

    // internal helper functions
  private:
    void increaseMidiClock();
    void calcBPM();
    void increaseSeqStep(const byte &array);
    void decreaseSeqStep(const byte &array);
    void nextArpStep(const byte &array);
    void increaseArpOct(const byte &array);
    void decreaseArpOct(const byte &array);

    byte getCurrentPageNumber(const byte &array);
    const byte getSubscreenMax();
    int16_t getLiveCcEvaluated(const byte &array);

    void setStr(const char *newStr);

  public:
    void updateClockCounter(const bool restartCounter = false);
    void increaseStepCounters(const byte &channel);
    void decreaseStepCounters(const byte &channel);

    byte getPitchbendAsByte(const byte &channel);

    void increaseBpm16thCount();
    structKey getLiveKeyEvaluated(const byte &array);
    structKey getKeyHighest(const byte &array);
    structKey getKeyLowest(const byte &array);
    structKey getKeyLatest(const byte &array);

    void seqSetAllNotes(const byte &array, const byte &data);
    void seqSetAllGates(const byte &array, const byte &data);
    void seqSetAllCC(const byte &array, const byte &data);
    void seqSetAllGateLengths(const byte &array, const byte &data);
    void seqResetAllGateLengths(const byte &array);
    void seqResetAllNotes(const byte &array);
    void seqResetAllGates(const byte &array);
    void seqResetAllCC(const byte &array);
    void seqAllOctaveUp(const byte &array);
    void seqAllOctaveDown(const byte &array);
    void seqCopy(const byte &source, const byte &destination);

    void updateArp(const byte &array);

    void resetAllStepCounter();

    void reset();

    void loadSequence(const byte &saveSlot, const byte &sequence);
    void saveSequence(const byte &saveSlot, const byte &sequence);

    void loadMenuSettings();
    void saveMenuSettings();

    void saveNoteCalibration();
    void loadNoteCalibration();

  public:
    int16_t get(const frankData &frankDataType);
    int16_t get(const frankData &frankDataType, const byte &array);
    int16_t get(const frankData &frankDataType, const byte &array, const byte &step);

    void set(const frankData &frankDataType, const int16_t &data);
    void set(const frankData &frankDataType, const int16_t &data, const byte &array);
    void set(const frankData &frankDataType, const int16_t &data, const byte &array, const byte &step);

    void toggle(const frankData &frankdataType);
    void toggle(const frankData &frankdataType, const byte &array);
    void toggle(const frankData &frankdataType, const byte &array, const byte &step);

    void change(const frankData &frankDataType, const int16_t &amount);
    void change(const frankData &frankDataType, const int16_t &amount, const byte &array);
    void change(const frankData &frankDataType, const int16_t &amount, const byte &array, const byte &step);

    void increase(const frankData &frankDataType);
    void increase(const frankData &frankDataType, const byte &array);
    void increase(const frankData &frankDataType, const byte &array, const byte &step);

    void decrease(const frankData &frankDataType);
    void decrease(const frankData &frankDataType, const byte &array);
    void decrease(const frankData &frankDataType, const byte &array, const byte &step);

    const char *getNameAsStr(const frankData &frankDataType);

    const char *getValueAsStr(const frankData &frankDataType);
    const char *getValueAsStr(const frankData &frankDataType, const byte &channel);
    const char *getValueAsStr(const frankData &frankDataType, const byte &channel, const byte &step);

    // singleton
    static FrankData &getDataObj();

  protected:
    static FrankData *mainData;
};

// utility
byte testByte(const int16_t &value, const byte &minimum = 0, const byte &maximum = 255); // test byte range and return valid byte
int16_t testInt(const int16_t &value, const int16_t &minimum, const int16_t &maximum);
byte increaseByte(const byte &value, const byte &maximum); // increase byte
byte decreaseByte(const byte &value, const byte &minimum); // decrease byte

byte changeByte(const byte &value, const int16_t &change, const byte &minimum = 0, const byte &maximum = 255,
                const bool &clampChange = 0); // change byte
int16_t changeInt(const int16_t &value, const int16_t &change, const int16_t &minimum, const int16_t &maximum,
                  const bool &clampChange = 0); // change byte

byte changeByteReverse(const byte &value, const int16_t &change, const byte &minimum = 0, const byte &maximum = 255);
int16_t changeIntReverse(const int16_t &value, const int16_t &change, const int16_t &minimum, const int16_t &maximum);

template <typename T> T toggleValue(const T &data);
template <typename T> char *toStr(const T &data);

char valueToNote(const byte &noteIn);
const char *valueToOctave(const byte &noteIn);
char valueToSharp(const byte &noteIn);
const char *tuningToChar(const byte &tuning);

int sort_desc(const void *cmp1, const void *cmp2);
int sort_asc(const void *cmp1, const void *cmp2);

class DebugTimer {
    elapsedMicros timer;
    const char *name;

  public:
    DebugTimer(const char *string) {
        timer = 0;
        name = string;
    }
    ~DebugTimer() {
        Serial.print("perf ");
        Serial.print(name);
        Serial.print(": ");
        Serial.print(timer);
        Serial.println(" micros");
    }
};