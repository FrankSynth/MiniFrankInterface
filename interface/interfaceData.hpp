#pragma once

#include <Arduino.h>

#define LENGTH 64
#define PAGES 8
#define NOTERANGE 88
#define STEPPERPAGE 8
#define OUTPUTS 2 // Number of outputs
#define MAXSTRINGSIZE 8

#define DATAOBJ FrankData::getDataObj()

class OutputRouting {
  public:
    byte outSource;    // 0 = live, 1 = seq
    byte channel;      // 0 = all, 1 = channel 1, ...
    byte seq;          // 0 = seq0, 1 = seq1
    byte arp;          // 0 = off, 1 = on
    byte cc;           // 0 = vel, 1 = mod, 2 = pitchbend, 3 = aftertouch, 4 = sustain
    byte liveMidiMode; // 0 = latest, 1 = lowest, 2 = highest
    byte clock;        // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = full, 5 = 8 beats

    OutputRouting() {
        this->outSource = 0;
        this->channel = 0;
        this->seq = 0;
        this->arp = 0;
        this->cc = 0;
        this->liveMidiMode = 0;
        this->clock = 2;
    }

};

// Sequence struct holding all values for a sequence, to save it
typedef struct {
    byte note[LENGTH];
    byte cc[LENGTH];
    byte gate[LENGTH];
    byte gateLength[LENGTH];
    byte velocity[LENGTH];
    byte tuning; // tuning offset
} structSequence;

// Settings struct for all settings that need to be saved permanently
typedef struct {
    byte midiSource = 1;            // active MidiDevice (usb -> 1, din -> 0)
    byte nbPages = 4;               // nb Pages  1 -> 8
    byte direction = 1;             // 0 -> reverse ; 1 -> forward
    byte displayBrightness = 100;   // 0-255;
    OutputRouting routing[OUTPUTS]; // hold settings for that many outputs

} structSettings;

// possible screen status
typedef struct {
    byte channel = 0;   // active channel, 0-> Channel 1, 1-> Channel 2
    byte config = 0;    // display config, 0-> off, 1-> on
    byte mainMenu = 1;  // display Main Menu, 0-> off, 1-> on
    byte subscreen = 0; // subscreen -> current displayed screen .. note, gate, cv (seq) ; live, appregiator (live)
    const byte subScreenMaxSeq = 2;  // Number of subscreens for seq mode
    const byte subScreenMaxLive = 1; // Number of subscreens for live mode
} structScreen;

// all Settings that don't need to be saved permanently
typedef struct {
    structScreen screen; // screen status

    byte stepSeq = 0; // current Step
    byte stepArp = 0;

    int bpm = 0;    // current bpm
    byte play = 0;  // play stop
    byte rec = 0;   // Rec Active
    byte error = 0; // ErrorFlag

    byte bpmSync = 0; // Sync Active
    byte midiClockCount = 5;
    byte bpm16thCount = 31;
    uint16_t bpmPoti = 0; // sync= 0 ? 0-1023 bpm log : divider /4, /2, 1, *2, *4 ; Range is 0-1023
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
    PressedNotesElement *pressedNoteElement = NULL;
    void appendKey(const byte &note, const byte &velocity);
    void deleteKey(const byte &note);
    void deleteAllKeys();
    bool containsElements();
    PressedNotesElement *getKeyHighest();
    PressedNotesElement *getKeyLowest();
    PressedNotesElement *getKeyLatest();
};

// save live midi data
class LiveMidi {
  public:
    PressedNotesList noteList;
    byte mod;
    byte pitchbend;
    byte aftertouch;
    byte sustain;
    byte triggered;

    LiveMidi() {
        this->mod = 0;
        this->pitchbend = 64;
        this->aftertouch = 0;
        this->sustain = 0;
        this->triggered = 0;
    }

    void keyPressed(const byte &note, const byte &velocity);
    void keyReleased(const byte &note);

    bool keysPressed();

    // void setMod(byte data);
    // void setPitchbend(byte data);
    // void setAftertouch(byte data);
    // void setSustain(byte data);
    // void resetTrigger();

    structKey getKeyHighest();
    structKey getKeyLowest();
    structKey getKeyLatest();

    // byte getMod();
    // byte getPitchbend();
    // byte getAftertouch();
    // byte getSustain();
    byte getTriggered();

    void reset();
};

// Sequence class
class Seq {
  public:
    void init(const byte &note = 12, const byte &gate = 1, const byte &gateLength = 50,
              const byte &tuning = 10); // init sequence to default values

    // Note
    void setNote(const byte &index, const byte &note); // set note value
    void setNotes(const byte &note);                   // set all note values
    byte getNote(const byte &index);                   // return note value

    byte increaseNote(const byte &index); // increase note value and return new note, function take care of tuning
    byte decreaseNote(const byte &index); // decrease note value and return new note, function take care of tuning

    byte changeNote(const byte &index, const int &change); // change note value and return new note
    void changeNotes(const int &change);                   // change all note values

    void octaveUp();   // All notes one octave down (if possible)
    void octaveDown(); // All notes one octave down (if possible)

    // TUNE
    void setTuning(const byte &tuning);
    byte getTuning();

    // Gate
    void setGate(const byte &index, const byte &gate); // set gate value
    byte getGate(const byte &index);                   // return gate value
    byte toggleGate(const byte &index);                // toggle gate and return new status;

    // GateLength
    void setGateLength(const byte &index, const byte &gateLength); // set gate length
    byte getGateLength(const byte &index);                         // return gate length
    byte changeGateLength(const byte &index, const int &change);   // increase note value and return new note

    void setGateLengths(const byte &gateLength); // set all gates at once
    void changeGateLengths(const int &change);   // increase note value and return new note

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
        seqCcEvaluated,
        seqVelocity,
        seqTuning,
        seqSize,

        // general Settings, needs value
        midiSource,
        nbPages,
        direction,
        displayBrightness,

        // Output Routing Settings, needs value, array
        outputSource,
        outputChannel,
        outputSeq,
        outputArp,
        outputCc,
        outputCcEvaluated,
        outputLiveMode,
        // outputLiveModeEvaluated,
        outputClock,
        // outputClockEvaluated,

        // Screen Settings, needs value
        screenOutputChannel,
        screenConfig,
        screenMainMenu,
        screenSubScreen,

        // structStatus, needs value
        stepSeq,
        stepArp,
        activePage,
        stepOnPage,
        currentPageNumber,
        bpm,
        play,
        rec,
        error,
        bpmSync,
        // midiClockCount,
        bpm16thCount,
        bpmPoti,

        // liveMidi, needs value, array
        liveMod,
        livePitchbend,
        liveAftertouch,
        liveSustain,
        liveTriggered,
        liveKeyNoteEvaluated,
        liveKeyVelEvaluated,
        liveLatestKey,
        liveHighestKey,
        liveLowestKey,
        // liveKeysPressed,
    };

    // idea for further enumerators
    enum subscreenStates : byte { screenNote, screenGate, screenCv, screenLive, screenArp };
    enum midiSourceStates : byte { din, usb };
    enum directionStates : byte { reverse, forward };
    enum beatStates : byte { sixteenth, eighth, quarter, half, bar, doublebar };

  private:
    FrankData() {
        for (byte output = 0; output < OUTPUTS; output++) {
            this->seq[output].init();
        }

    }

    structStatus stat;
    structSettings config;

    LiveMidi liveMidi[OUTPUTS];
    Seq seq[OUTPUTS];

    char str[MAXSTRINGSIZE+1]; // MAXSTRINGSIZE + escape char

  public:
    const char* returnStr = str;

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
    void increaseBpm16thCount();
    void setBpm16thCount(unsigned int spp);
    byte getBpm16thCount();
    // inline void resetClock();
    inline void calcBPM();
    inline void increaseStep();
    inline void decreaseStep();
    inline byte getCurrentPageNumber();
    inline const byte getSubscreenMax();
    inline byte getLiveCcEvaluated(const byte &array);
    inline byte getOutputLiveModeEvaluated(const byte &array);
    inline byte getOutputClockEvaluated(const byte &array);
    inline void setStr(const char *newStr);

  public:
    inline structKey getLiveKeyEvaluated(const byte &array);
    inline structKey getKeyHighest(const byte &array);
    inline structKey getKeyLowest(const byte &array);
    inline structKey getKeyLatest(const byte &array);
    void resetSubScreen(); // switch menu max 3 menu pages

    // settings

  public:
    // get single type value
    byte get(const frankData &frankDataType);
    // get value that is part of an array, e.g. output, seq current step, ...
    byte get(const frankData &frankDataType, const byte &array);
    // get value for certain step
    byte get(const frankData &frankDataType, const byte &array, const byte &step);


    // set single type value
    void set(const frankData &frankDataType, const byte &data, const bool &clampChange = 0);
    // set value prat of an array
    void set(const frankData &frankDataType, const byte &data, const byte &array, const bool &clampChange = 0);
    // set value for certain step
    void set(const frankData &frankDataType, const byte &data, const byte &array, const byte &step,
             const bool &clampChange = 0);
    // toggle what can be toggled
    void toggle(const frankData &frankdataType);
    void toggle(const frankData &frankdataType, const byte &array, const byte &step);

    void change(const frankData &frankDataType, const byte &amount, const bool &clampChange = 0);
    void change(const frankData &frankDataType, const byte &amount, const byte &array,
                       const bool &clampChange = 0);
    void change(const frankData &frankDataType, const byte &amount, const byte &array, const byte &step,
                       const bool &clampChange = 0);

    void increase(const frankData &frankDataType, const bool &clampChange = 0);
    void increase(const frankData &frankDataType, const byte &array, const bool &clampChange = 0);
    void increase(const frankData &frankDataType, const byte &array, const byte &step,
                         const bool &clampChange = 0);

    void decrease(const frankData &frankDataType, const bool &clampChange = 0);
    void decrease(const frankData &frankDataType, const byte &array, const bool &clampChange = 0);
    void decrease(const frankData &frankDataType, const byte &array, const byte &step,
                         const bool &clampChange = 0);

    const char *getNameAsStr(const frankData &frankDataType);
    const char *getValueAsStr(const frankData &frankDataType);
    const char *getValueAsStr(const frankData &frankDataType, const byte &step);

    // singleton
    static FrankData &getDataObj();

  protected:
    static FrankData *mainData;

};

// utility
inline byte testByte(const byte &value, const byte &minimum, const byte &maximum = 255,
                     const bool &clampChange = 0);                // test byte range and return valid byte
inline byte increaseByte(const byte &value, const byte &maximum); // increase byte
inline byte decreaseByte(const byte &value, const byte &minimum); // decrease byte
inline byte changeByte(const byte &value, const int &change, const byte &minimum = 0, const byte &maximum = 255,
                       const bool &clampChange = 0); // change byte
template <typename T> inline T toggleValue(const T &data);
template <typename T> inline char *toStr(const T &data);

char valueToNote(const byte &noteIn);
char valueToOctave(const byte &noteIn);
char valueToSharp(const byte &noteIn);
const char *tuningToChar(const byte &tuning);