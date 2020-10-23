#pragma once

#include "EffectsUnit.h"
#include <vector>

class Preset
{
private:
public:
    String name = "Preset";
    std::vector<EffectsUnit *> effects;

    Preset(String name)
    {
        this->name = name;
    }

    void add(EffectsUnit *unit)
    {
        Serial.println("ADD UNIT");
        effects.push_back(unit);
        unit->setPreset(this);
    }

    void remove(EffectsUnit *unit)
    {
        if (unit != nullptr)
        {
            //effects.remove(unit);
            delete unit;
        }
    }

    void proceed(int16_t *in, size_t length);

    void onFootswitchPress(size_t fwNum) 
    {
        for (int i = 0; i < effects.size(); i++)
        {
            effects[i]->onFootswitchPress(fwNum);
        }
    }

    void onFootswitchRelease(size_t fwNum) 
    {
        for (int i = 0; i < effects.size(); i++)
        {
            effects[i]->onFootswitchRelease(fwNum);
        }
    }
};

void Preset::proceed(int16_t *in, size_t length)
{
    for (int i = 0; i < effects.size(); i++)
    {
        effects[i]->proceed(in, length);
    }
}