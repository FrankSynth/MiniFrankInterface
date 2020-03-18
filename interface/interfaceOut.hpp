#pragma once

#include "interfaceMiddleman.hpp"
#include <Arduino.h>
#include <SPI.h>

#define DAC1 8
#define DAC2 6
#define TRIGGER1 4
#define TRIGGER2 5
#define GATE1 2
#define GATE2 3
#define CLK1 0
#define CLK2 1
#define CLKLED 21

#define NOTESCALING 46.54f

void setVoltage(int dacpin, bool channel, bool gain, unsigned int mV); // channel 0, 1, Gain = 2 (default)
void initOutput();

class Channel {

    byte outputChannel;
    byte noteDac;
    byte noteDacChannel;

    byte cvDac;
    byte cvDacChannel;

    byte triggerPin;

    byte gatePin;


  public:
    Channel(byte outputChannel, byte noteDac, byte noteDacChannel, byte cvDac, byte cvDacChannel, byte triggerPin, byte gatePin)
        : outputChannel(outputChannel), noteDac(noteDac), noteDacChannel(noteDacChannel), cvDac(cvDac), cvDacChannel(cvDacChannel),
          triggerPin(triggerPin), gatePin(gatePin) {}

    void setTuning(float tuning);
    void setGate(byte state);
    void setTrigger(byte state);
    void setNote(byte note);
    void setCV(int value);
};

class clock {
  public:
    clock(byte pin) : pin(pin) {}
    void setClock(byte state);

  private:
    byte pin;
};

class ClkLed {
  public:
    void init(byte pin);
    void setClkLed(byte state);

  private:
    byte clkLed;
};
