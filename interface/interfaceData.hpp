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
#define ARPOCTAVECENTEROFFSET (int)3
#define GATELENGTHOFFSET (int)100
#define CALOFFSET (int)127

#define DATAOBJ FrankData::getDataObj()

typedef struct {
    byte outSource = 0;    // 0 = live, 1 = seq1, 2 = seq2, ...
    byte channel = 0;      // 0 = all, 1 = channel 1, ...
    byte arp = 0;          // 0 = off, 1 = on, 2 = Latch
    byte arpMode = 0;      // 0 = up, 1 = down, 2 = updown, 3= downup, 4 = upRdownR, 5 = downRupR, 6 = order, 7 = random
    byte cc = 0;           // 0 = vel, 1 = mod, 2 = pitchbend, 3 = aftertouch, 4 = sustain
    byte liveMidiMode = 0; // 0 = latest, 1 = lowest, 2 = highest
    byte clockSpeed = 2;   // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = 1 bar, 5 = 2 bars
    byte arpRatchet = 0;   // repeats per step, 1 = 1 repeat (2 notes total), up to 3 repeats
    byte arpOctaves = 3;   // Octaves 0 = -3, 3 = 0, 6 = +3
    byte stepSpeed = 2;    // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = 1 bar, 5 = 2 bars
    byte nbPages = 8;      // nb Pages  1-16
} structOutputRouting;

typedef struct {
    byte noteCalibration[NOTERANGE] = {127};
    int cvOffset = 127;
    byte noteScaleOffset = 127;
} structCalibration;

typedef struct {
    int16_t lowerEnd = 0;
    int16_t upperEnd = 0;
} structCalibrationPitchbend;

// Sequence struct holding all values for a sequence, to save it
typedef struct {
    byte note[LENGTH];
    byte cc[LENGTH];
    byte gate[(LENGTH / 8)]; // optimize data to single bits! middleman seq check, if gate
    byte gateLength[LENGTH]; //
    byte tuning;             // tuning offset
    byte gateLengthOffset;   // 100 = no offset
} structSequence;

// Settings struct for all settings that need to be saved permanently
typedef struct {
    byte midiSource = 1;                  // active MidiDevice (usb -> 1, din -> 0)
    byte direction = 1;                   // 0 -> reverse ; 1 -> forward
    byte displayBrightness = 200;         // 0-255;
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

    byte bpm = 120; // current bpm
    byte play = 1;  // play stop
    byte rec = 0;   // Rec Active
    byte error = 0; // ErrorFlag

    byte pulseLength = 20; // pulse length in ms

    byte noteToCalibrate = 0; // note value that gets calibrated

    byte bpmSync = 0;          // Sync Active
    byte midiClockCount = 5;   // counts incoming midiclock signals (6 ticks per 16th)
    byte bpm16thCount = 0;     // general 16th counter for clock outputs
    unsigned int bpmPot = 120; // sync= 0 ? 0-1023 bpm log : divider /4, /2, 1, *2, *4 ; Range is 0-1023 (not yet implemented)
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
    int size = 0;

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
    int pitchbend = 0;         // live midi pitchbend
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

    byte recModePlayback = 0; // status for rec mode playback (last recorded note will be played instead of current step)

    int arpOctave = 0; // current arp octave being played

    structKey lastKey;             // always holds the last played key, if no keys are pressed
    structKey arpKey;              // always holds the last played key, if no keys are pressed
    PressedNotesList arpList;      // Key List for arpeggiator
    structKey arpArray[NOTERANGE]; // sorted array for arpeggiator

    byte stepArp = 0; // current arp step
    byte stepSeq = 0; // current seq step

    unsigned int channel16thCount = 0; // individual 16th counter for asynchronous playback

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
    inline void copyArpListToArray();
    void printArray();
};

// Sequence class
class Seq {
  public:
    Seq() { init(); }

    void init(const byte &note = 24, const byte &gate = 1, const byte &gateLength = 50, const byte &cc = 64, const byte &tuning = 0,
              const byte &gateLengthOffset = 100); // init sequence to default values

    // Note
    void setNote(const byte &index, const byte &note); // set note value
    void setNotes(const byte &note);                   // set all note values
    void setCCs(const byte &cc);                       // set all note values
    void setGates(const byte &gate);                   // set all note values

    void increaseNote(const byte &index); // increase note value and return new note, function take care of tuning
    void decreaseNote(const byte &index); // decrease note value and return new note, function take care of tuning

    void changeNotes(const int &change); // change all note values
    void octaveUp();                     // All notes one octave down (if possible)
    void octaveDown();                   // All notes one octave down (if possible)

    void setGate(const byte &index, const byte &gate);
    byte getGate(const byte &index);

    // GateLength
    void setGateLengths(const byte &gateLength); // set all gates at once
    void changeGateLengths(const int &change);

    // Sequence
    void setSequence(structSequence *copySeq); // set all sequence values at once
    structSequence *getSequence();             // return the sequence struct pointer

    int getSequenceSize(); // return the struct size

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
        seqSize,

        // calibration, needs value, array, step
        noteCalOffset,

        // calibration, needs value, array
        cvCalOffset,
        noteScaleOffset,
        pitchbendCalUpper,
        pitchbendCalLower,

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
        screenCal,
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
        liveCalNote,
    };

  private:
    FrankData() {}

    structStatus stat;
    structSettings config;
    structCalibration cal[OUTPUTS];
    structCalibrationPitchbend calPitchbend[OUTPUTS];

    LiveMidi liveMidi[OUTPUTS];
    Seq seq[OUTPUTS];
    char str[MAXSTRINGSIZE + 1]; // MAXSTRINGSIZE + escape char

  public:
    const char *returnStr = str;

    void init();

    // receive MIDI
    void receivedKeyPressed(const byte &channel, const byte &note, const byte &velocity);
    void receivedKeyReleased(const byte &channel, const byte &note);
    void receivedPitchbend(const byte &channel, const int &pitchbend);
    void receivedMidiClock();
    void receivedMidiSongPosition(unsigned int spp);
    void receivedStart();
    void receivedContinue();
    void receivedStop();
    void receivedReset();

    // internal helper functions
  private:
    void increaseMidiClock();
    void setBpm16thCount(unsigned int spp);
    byte getBpm16thCount();
    inline void calcBPM();
    inline void increaseSeqStep(const byte &array);
    inline void decreaseSeqStep(const byte &array);
    inline void nextArpStep(const byte &array);
    inline void increaseArpOct(const byte &array);
    inline void decreaseArpOct(const byte &array);

    inline byte getCurrentPageNumber(const byte &array);
    inline const byte getSubscreenMax();
    inline byte getLiveCcEvaluated(const byte &array);
    inline void setStr(const char *newStr);

    const char *valueToStr(const frankData &frankDataType, const byte &channel);

  public:
    void updateClockCounter(const bool newMillis = false);
    void increaseStepCounters(const byte &channel);
    void decreaseStepCounters(const byte &channel);

    byte getPitchbendAsByte(const byte &channel);
    int getPitchbend(const byte &channel);

    int getPitchbendCalUpper(const byte &channel);
    int getPitchbendCalLower(const byte &channel);

    void increaseBpm16thCount();
    inline structKey getLiveKeyEvaluated(const byte &array);
    inline structKey getKeyHighest(const byte &array);
    inline structKey getKeyLowest(const byte &array);
    inline structKey getKeyLatest(const byte &array);

    void setBPMPoti(const unsigned int &bpm);

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

    void loadAllMenuSettings();
    void saveMenuSettings();

    void saveNoteCalibration();
    void loadNoteCalibration();

  public:
    byte get(const frankData &frankDataType);
    byte get(const frankData &frankDataType, const byte &array);
    byte get(const frankData &frankDataType, const byte &array, const byte &step);

    void set(const frankData &frankDataType, const int &data, const bool &clampChange = 0);
    void set(const frankData &frankDataType, const int &data, const byte &array, const bool &clampChange = 0);
    void set(const frankData &frankDataType, const int &data, const byte &array, const byte &step, const bool &clampChange = 0);

    void toggle(const frankData &frankdataType);
    void toggle(const frankData &frankdataType, const byte &array);
    void toggle(const frankData &frankdataType, const byte &array, const byte &step);

    void change(const frankData &frankDataType, const int &amount, const bool &clampChange = 0);
    void change(const frankData &frankDataType, const int &amount, const byte &array, const bool &clampChange = 0);
    void change(const frankData &frankDataType, const int &amount, const byte &array, const byte &step, const bool &clampChange = 0);

    void increase(const frankData &frankDataType, const bool &clampChange = 0);
    void increase(const frankData &frankDataType, const byte &array, const bool &clampChange = 0);
    void increase(const frankData &frankDataType, const byte &array, const byte &step, const bool &clampChange = 0);

    void decrease(const frankData &frankDataType, const bool &clampChange = 0);
    void decrease(const frankData &frankDataType, const byte &array, const bool &clampChange = 0);
    void decrease(const frankData &frankDataType, const byte &array, const byte &step, const bool &clampChange = 0);

    const char *getNameAsStr(const frankData &frankDataType);
    const char *getValueAsStr(const frankData &frankDataType);
    const char *getValueAsStr(const frankData &frankDataType, const byte &step);
    const char *getValueAsStrChannel(const frankData &frankDataType, const byte &channel);

    // singleton
    static FrankData &getDataObj();

  protected:
    static FrankData *mainData;
};

// utility
inline byte testByte(const int &value, const byte &minimum = 0, const byte &maximum = 255,
                     const bool &clampChange = 0); // test byte range and return valid byte
int testInt(const int &value, const int &minimum, const int &maximum);
inline byte increaseByte(const byte &value, const byte &maximum); // increase byte
inline byte decreaseByte(const byte &value, const byte &minimum); // decrease byte
inline byte changeByte(const byte &value, const int &change, const byte &minimum = 0, const byte &maximum = 255,
                       const bool &clampChange = 0); // change byte
inline int changeInt(const int &value, const int &change, const int &minimum, const int &maximum,
                     const bool &clampChange = 0); // change byte
byte changeByteReverse(const byte &value, const int &change, const byte &minimum = 0, const byte &maximum = 255);
inline int changeIntReverse(const int &value, const int &change, const int &minimum, const int &maximum);
template <typename T> inline T toggleValue(const T &data);
template <typename T> inline char *toStr(const T &data);

char valueToNote(const byte &noteIn);
const char *valueToOctave(const byte &noteIn);
char valueToSharp(const byte &noteIn);
const char *tuningToChar(const byte &tuning);

inline int sort_desc(const void *cmp1, const void *cmp2);
inline int sort_asc(const void *cmp1, const void *cmp2);