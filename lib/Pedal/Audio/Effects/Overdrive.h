#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"

class Overdrive : public EffectsUnit
{
public:
    PotentiometerView gain = PotentiometerView(this, "GAIN", 0.0001, 0.01, 0.0001);
    PotentiometerView post_gain = PotentiometerView(this, "POST GAIN", 500.0, 2000.0, 1000.0);
    double preGain = 0.0001, postGain = 1000.0;

    Overdrive() : EffectsUnit("OVERDRIVE")
    {

    }

    inline void processing_sample(int16_t *sample) noexcept override
    {
        float s = ((*sample) * preGain);
        *sample = atanf(s) * postGain; //(fabs(s) > 1.6f ? 1.6f * (s < 0.f ? -1.f : 1.f) : s) * postGain;
    }
    void processing(int16_t *out, size_t size) noexcept override
    {
        preGain = gain.getValue(), postGain = post_gain.getValue();
        EffectsUnit::processing(out, size);
    }
};