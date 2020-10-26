#pragma once

#include "math.h"

class VirtualPotentiometer
{
private:
    double value, from, to;
public:

    VirtualPotentiometer(double from = 0.0, double to = 1.0, double value = 0.0)
    {
        this->from = from;
        this->to = to;
        setValue(value);
    }

    double getValue();
    double getRangedValue();
    double getRange();
    void setValue(double value);

    virtual void tick() {}
};

double VirtualPotentiometer::getValue()
{
    return value;
}

void VirtualPotentiometer::setValue(double value)
{
    if(value > to)
    {
        value = to;
    }
    else if(value < from)
    {
        value = from;
    }
    if (value != this->value)
    {
        this->value = value;
    }
}

double VirtualPotentiometer::getRangedValue()
{
    return abs(value - from)/abs(from - to);
}

double VirtualPotentiometer::getRange()
{
    return abs(from - to);
}