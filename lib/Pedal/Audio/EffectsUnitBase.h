#pragma once

#include <stdint.h>
#include "Arduino.h"
#include <vector>
#include "GFX/Container.h"

class ControlView;
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

    void proceed(int16_t *out, size_t length);

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
        name.~String();
        controls.~vector();
    }

    virtual void setBypass(bool bypass)
    {
        this->bypass = bypass;
    }

    virtual bool getBypass()
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

void EffectsUnitBase::proceed(int16_t *out, size_t length)
{
    if (!bypass)
    {
        //processing_sample(&out[i]);
        processing(out, length);
        
    }
}