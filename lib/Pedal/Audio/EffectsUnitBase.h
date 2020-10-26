#pragma once

#include <stdint.h>
#include "Arduino.h"
#include <vector>
#include "GFX/Container.h"

class Preset;

//#include "Views/EffectsUnit/Controls/Options.h"

class EffectsUnitBase
{
protected:
    Preset *preset;
    bool bypass = false;
public:
    std::vector<Container *> controls;

    String name; 

    virtual void processing_sample(int16_t *out)
    {

    }

    virtual void processing(int16_t *out, size_t length)
    {
        for (int i = 0; i < length; i++)
        {
            processing_sample(&out[i]);
        }
    }

    virtual void proceed(int16_t *out, size_t length) {}

    String &getName()
    {
        return name;
    }

    Preset *getPreset()
    {
        return preset;
    }

    void setPreset(Preset *preset)
    {
        this->preset = preset;
    }

    EffectsUnitBase(const String &name, Preset *preset = nullptr) : preset(preset)
    {
        this->name = name;
    }

    virtual ~EffectsUnitBase()
    {
        Serial.println("EUB DTOR");
    }

    virtual inline void setBypass(bool bypass)
    {
        this->bypass = bypass;
    }

    virtual inline bool getBypass()
    {
        return bypass;
    }

    virtual inline bool getGlobalBypass()
    {
        return bypass;
    }

    virtual void onFootswitchPress(size_t fwNum)
    {
    }

    virtual void onFootswitchRelease(size_t fwNum)
    {
    }
};