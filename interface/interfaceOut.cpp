#include "interfaceOut.hpp"

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

void Channel::setTuning(float tuning) { noteTuning = tuning; }

void Channel::setGate(byte state) { digitalWrite(gatePin, state); }

void Channel::setTrigger(byte state) { digitalWrite(triggerPin, state); }

void Channel::setNote(byte note) {
    unsigned int mV = (unsigned int)((float)note * noteTuning + 0.5);
    setVoltage(noteDac, noteChannel, 2, mV);
}

void Channel::setCV(int value) { // 0 - 1024 -> -5V -> 5V
    unsigned int mV = (unsigned int)((float)value * 4);
    setVoltage(cvDac, cvChannel, 2, mV);
}

void initOutput() {
    pinMode(DAC1, OUTPUT);
    digitalWrite(DAC1, HIGH);

    pinMode(DAC2, OUTPUT);
    digitalWrite(DAC2, HIGH);

    pinMode(TRIGGER1, OUTPUT);
    digitalWrite(TRIGGER1, LOW);

    pinMode(TRIGGER2, OUTPUT);
    digitalWrite(TRIGGER2, LOW);

    pinMode(GATE1, OUTPUT);
    digitalWrite(GATE1, LOW);

    pinMode(GATE2, OUTPUT);
    digitalWrite(GATE2, LOW);

    pinMode(CLK1, OUTPUT);
    digitalWrite(CLK1, LOW);

    pinMode(CLK2, OUTPUT);
    digitalWrite(CLK2, LOW);
}

void setVoltage(int dacpin, bool channel, bool gain, unsigned int mV) // channel 0, 1, Gain = 2 (default)
{
    unsigned int command = channel ? 0x9000 : 0x1000;

    command |= gain ? 0x0000 : 0x2000;
    command |= (mV & 0x0FFF);

    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0)); // nochmal SPI speed testen...
    digitalWrite(dacpin, LOW);
    SPI.transfer(command >> 8);
    SPI.transfer(command & 0xFF);
    digitalWrite(dacpin, HIGH);
    SPI.endTransaction();
}


    void clock::setClock(byte state){
        digitalWrite(pin, state);
    }

    void ClkLed::init(byte pin) {
        clkLed = pin;
        pinMode(pin, OUTPUT);
    }

    void ClkLed::setClkLed(byte state) {
        digitalWrite(clkLed, state);
    }
