#pragma once

#include "EffectsUnit.h"
#include <vector>

class Preset
{
private:
    std::vector<EffectsUnit *> deleteEffects;
    void deleteEffect(EffectsUnit *unit);
    bool waitRemove = false;

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
        deleteEffects.push_back(unit);
        waitRemove = true;
    }

    int indexOf(EffectsUnit *unit)
    {
        if (unit != nullptr)
        {
            for (int i = 0; i < effects.size(); i++)
            {
                if (unit == effects[i])
                {
                    return i;
                }
            }
        }
        return -1;
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

    bool waitForRemove()
    {
        return waitRemove;
    }
};

void Preset::proceed(int16_t *in, size_t length)
{
    for (int i = 0; i < deleteEffects.size(); i++)
    {
        deleteEffect(deleteEffects.back());
        deleteEffects.pop_back();
    }
    if (waitRemove)
    {
        waitRemove = false;
    }
    for (int i = 0; i < effects.size(); i++)
    {
        effects[i]->proceed(in, length);
    }
}

void Preset::deleteEffect(EffectsUnit *unit)
{
    if (unit != nullptr)
    {
        int index = indexOf(unit);
        if (index != -1)
        {
            effects.erase(effects.begin() + index);
            delete unit;
        }
    }
}