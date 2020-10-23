#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"

class Overdrive : public EffectsUnit
{
public:
    PotentiometerView gain = PotentiometerView(this, "GAIN", 0.0001, 0.01, 0.0001);
    PotentiometerView post_gain = PotentiometerView(this, "POST GAIN", 500.0, 2000.0, 1000.0);

    Overdrive() : EffectsUnit("OVERDRIVE")
    {

    }

    void processing_sample(int16_t *sample) override
    {
        double preGain = gain.getValue(), postGain = post_gain.getValue();
        int16_t val = *sample;
        val = atan(val * preGain) * postGain;
        *sample = val;
    }
};