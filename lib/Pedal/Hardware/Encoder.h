#pragma once

#include <stdint.h>
#include "Button.h"
#include "Utils/Event.h"

class Encoder
{
protected:
    volatile int rotation = 0;
public:
    unsigned long prevMillis = 0;
    bool A = false, B = false;
    uint8_t pinA = 0, pinB = 0, pinD = 0;
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinD);
    Button button;
    Event<> onCW, onCCW;
    void tick();
    void service();
};

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinD) : button(pinD)
{
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinD = pinD;

    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
}

void Encoder::tick()
{
    noInterrupts();
    int value = rotation;
    rotation = 0;
    interrupts();

    bool dir = value > 0;

    for (int i = 0; i < abs(value); i++)
    {
        dir ? onCW() : onCCW();
    }

    button.tick();
}

void Encoder::service()
{
    bool newA = digitalRead(pinA), newB = digitalRead(pinB);
    if (B && !newB && !newA)
    {
        rotation++;
    }
    else if (A && !newA && !B)
    {
        rotation--;
    }

    A = newA;
    B = newB;
}
