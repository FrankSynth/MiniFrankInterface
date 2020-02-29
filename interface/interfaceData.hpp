#pragma once

#include <Arduino.h>

//zum Testen vom mapping
#define NONE 0
#define BPM 1
#define NOTE 1
#define GATE 1
#define TGATE 1
#define CGATE 1
#define STEP 1
#define CV 1
#define CONF 1

//Cxxx -> Global Channel data
//Txxx -> Toggle Data



#define LENGTH 64
#define PAGES 8
#define NOTERANGE 88
#define STEPPERPAGE 8
#define OUTPUTS 2 // Number of outputs

#define DATAOBJ FrankData::getDataObj()
// #define GETDATAOBJ FrankData::getDataObj();


class OutputRouting {
    byte out;          // 0 = live, 1 = seq
    byte channel;      // 0 = all, 1 = channel 1, ...
    byte seq;          // 0 = seq0, 1 = seq1
    byte arp;          // 0 = off, 1 = on
    byte cc;           // 0 = vel, 1 = mod, 2 = pitchbend, 3 = aftertouch, 4 = sustain
    byte liveMidiMode; // 0 = latest, 1 = lowest, 2 = highest

  public:
    OutputRouting() {
        this->out = 0;
        this->channel = 0;
        this->seq = 0;
        this->arp = 0;
        this->cc = 0;
        this->liveMidiMode = 0;
    }

    byte getOut();
    byte getChannel();
    byte getSeq();
    byte getArp();
    byte getCc();
    byte getliveMidiMode();

    void setOut(byte data);
    void setChannel(byte data);
    void setSeq(byte data);
    void setArp(byte data);
    void setCc(byte data);
    void setLiveMidiMode(byte data);
};

// Sequence struct holding all values for a sequence
typedef struct {
    byte note[LENGTH];
    byte cv[LENGTH];
    byte gate[LENGTH];
    byte gateLength[LENGTH];
    byte velocity[LENGTH];
    byte tuning; // tuning offset
} structSequence;

// Settings struct for all settings that will get saved permanently
typedef struct {
    byte midiSource = 1;            // active MidiDevice (usb -> 1, din -> 0)
    byte nbPages = 4;               // nb Pages  1 -> 8
    byte direction = 1;             // 0 -> reverse ; 1 -> forward
    byte displayBrightness = 100;   // 0-255;
    OutputRouting routing[OUTPUTS]; // hold settings for that many outputs
    byte clockOut0 = 0;             // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = full, 5 = 8 beats
    byte clockOut1 = 1;             // 0 = 16th, 1 = 8th, 2 = quarter, 3 = half, 4 = full, 5 = 8 beats

} structSettings;

// Sequence struct holding all values for a sequence
typedef struct {
  byte channel = 0;    //active channel, 0-> Channel 1, 1-> Channel 2
  byte config = 0;     //display config, 0-> off, 1-> on
  byte mainMenu = 1;     //display Main Menu, 0-> off, 1-> on
  byte subscreen = 0;  //subscreen -> current displayed screen .. note, gate, cv (seq) ; live, appregiator (live)
} structScreen;

// all Settings that don't need to be saved permanently
typedef struct {
    structScreen screen; //screen status

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

// Key press data
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
    void appendKey(byte note, byte velocity);
    void deleteKey(byte note);
    void deleteAllKeys();
    bool containsElements();
    PressedNotesElement *getKeyHighest();
    PressedNotesElement *getKeyLowest();
    PressedNotesElement *getKeyLatest();
};

// save live midi data
class LiveMidi {
    PressedNotesList noteList;
    byte mod;
    byte pitchbend;
    byte aftertouch;
    byte sustain;
    byte triggered;

  public:
    LiveMidi() {
        this->mod = 0;
        this->pitchbend = 64;
        this->aftertouch = 0;
        this->sustain = 0;
        this->triggered = 0;
    }

    void keyPressed(byte note, byte velocity);
    void keyReleased(byte note);

    bool keysPressed();

    void setMod(byte data);
    void setPitchbend(byte data);
    void setAftertouch(byte data);
    void setSustain(byte data);
    void resetTrigger();

    structKey getKeyHighest();
    structKey getKeyLowest();
    structKey getKeyLatest();

    byte getMod();
    byte getPitchbend();
    byte getAftertouch();
    byte getSustain();
    byte getTriggered();

    void reset();
};

// Sequence class
class Seq {
  public:
    void init(byte note = 12, byte gate = 1, byte gateLength = 50, byte tuning = 10); // init sequence to default values

    // Note
    void setNote(byte index, byte note); // set note value
    void setNotes(byte note);            // set all note values
    byte getNote(byte index);            // return note value

    byte increaseNote(byte index); // increase note value and return new note, function take care of tuning
    byte decreaseNote(byte index); // decrease note value and return new note, function take care of tuning

    byte changeNote(byte index, int change); // change note value and return new note
    void changeNotes(int change);            // change all note values

    void octaveUp();   // All notes one octave down (if possible)
    void octaveDown(); // All notes one octave down (if possible)

    // TUNE
    void setTuning(byte tuning);
    byte getTuning();

    // Gate
    void setGate(byte index, byte gate); // set gate value
    byte getGate(byte index);            // return gate value
    byte toggleGate(byte index);         // toggle gate and return new status;

    // GateLength
    void setGateLength(byte index, byte gateLength); // set gate length
    byte getGateLength(byte index);                  // return gate length
    byte changeGateLength(byte index, int change);   // increase note value and return new note

    void setGateLengths(byte gateLength); // set all gates at once
    void changeGateLengths(int change);   // increase note value and return new note

    // Sequence
    void setSequence(structSequence *copySeq); // set all sequence values at once
    structSequence *getSequence();             // return the sequence struct pointer

    int getSequenceSize(); // return the struct size

  private:
    // Sequence
    structSequence sequence;
};

// data class
class FrankData {
  public:
    structStatus stat;
    structSettings config;

    LiveMidi liveMidi[OUTPUTS];
    Seq seq[OUTPUTS];

    FrankData() {
        for (byte output = 0; output < OUTPUTS; output++) {
            this->seq[output].init();
        }
    }


    // receive MIDI
    void receivedKeyPressed(byte channel, byte note, byte velocity);
    void receivedKeyReleased(byte channel, byte note);
    void receivedMod(byte channel, byte data);
    void receivedPitchbend(byte channel, byte data);
    void receivedAftertouch(byte channel, byte data);
    void receivedSustain(byte channel, byte data);
    void receivedMidiClock();
    void receivedMidiSongPosition(unsigned int spp);
    void receivedStart();
    void receivedContinue();
    void receivedStop();
    void receivedReset();

    // clock
    void increaseMidiClock();
    void increaseBpm16thCount();
    void setBpm16thCount(unsigned int spp);
    byte getBpm16thCount();
    void resetClock();

    // settings

    void setSync(byte bpmSync);
    byte getSync();

    void setRec(byte rec);
    byte getRec();

    void setBPM(int bpm);
    void calcBPM();
    int getBPM();


    void setStep(byte stepSeq);
    byte getStep();
    void increaseStep();
    void decreaseStep();

    byte getActivePage();
    byte getStepOnPage();

    void setPlayStop(byte mode);
    byte getPlayStop();

    void togglePlayStop();


    void setDirection(byte direction);
    byte getDirection();

    void setError(byte error);
    byte getError();

    void setDisplayBrightness(byte brightness);
    byte getDisplayBrightness();

    void setMidiSource(byte midi);
    byte getMidiSource();

    void setNumberPages(byte nbPages);
    byte getNumberPages();
    byte getCurrentNumberPages();
    Seq *getSeqObject();




    //Screen config
    void setSubScreen(byte subScreen , byte max);
    byte getSubScreen();
    void resetSubScreen(); // switch menu max 3 menu pages

    void increaseSubScreen(byte max); // switch menu max 3 menu pages
    void decreaseSubScreen(); // switch menu max 3 menu pages;

    byte getScreenConfig(byte screen);
    void toggleScreenConfig();

    void setScreenChannel(byte channel);
    byte getScreenChannel();

    byte getMainMenuEnabled();
    void toogleMainMenu();

    byte getOutputMode(byte channel);   // Live oder Seq?
    byte getArpModeEnable(byte channel);   // Arp on or off





    //zum testen//

    void setData(byte id, byte index = 0);

    void toggleData(byte id, byte index = 0);

    void changeData(byte id, byte index = 0 , byte direction = 0);

    int getData(byte id, byte index = 0);
    char* getDataName(byte id);



    static FrankData& getDataObj();

    protected:
    static FrankData* mainData;
};


// init all data
// void initData();

// get data objects
// FrankData * getDataObject();

// LiveMidi *getLiveMidiObject();
// structSettings *getSettingsObject();
// structStatus *getStatusObject();

// utility
inline byte testByte(byte value, byte minimum, byte maximum = 255); // test byte range and return valid byte
inline byte increaseByte(byte value, byte maximum);           // increase byte
inline byte decreaseByte(byte value, byte minimum);           // decrease byte
inline byte changeByte(byte value, int change, byte minimum = 0, byte maximum = 255); // change byte
inline byte changeByteNoClampChange(byte value, int change, byte minimum = 0,
                                    byte maximum = 255); // change byte (keeps original value if change not possible)
