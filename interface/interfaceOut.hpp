#pragma once

#include <Arduino.h>
#include <SPI.h>

#define DAC1 8
#define DAC2 6
#define TRIGGER1 4
#define TRIGGER2 5
#define GATE1 2
#define GATE2 3
#define CLK1 1
#define CLK2 0

void setVoltage(int dacpin, bool channel, bool gain, unsigned int mV); // channel 0, 1, Gain = 2 (default)
void initOutput();

class channel {
  public:
    channel(byte noteDac, byte noteChannel, byte cvDac, byte cvChannel, byte triggerPin, byte gatePin)
        : noteDac(noteDac), noteChannel(noteChannel), cvDac(cvDac), cvChannel(cvChannel), triggerPin(triggerPin),
          gatePin(gatePin) {}

    void setTuning(float tuning);
    void setGate(byte state);
    void setTrigger(byte state);
    void setNote(byte note);
    void setCV(int value);

  private:
    byte noteDac;
    byte noteChannel;
    float noteTuning = 47.069;

    byte cvDac;
    byte cvChannel;

    byte triggerPin;

    byte gatePin;
};

class clock {
  public:
    clock(byte pin) : pin(pin) {}

    void setClock(byte state);


  private:
    byte pin;
};
