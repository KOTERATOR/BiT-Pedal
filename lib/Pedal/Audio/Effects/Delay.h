#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"
#include "Views/EffectsUnit/Controls/ToggleSwitch.h"
#include "Views/EffectsUnit/Controls/Options.h"

class Delay : public EffectsUnit
{
private:
    const static uint16_t MAX_DELAY = 22050;
    int16_t delaybuffer1[MAX_DELAY] = {};
    int counter = 0, delay_depth = 1;
    //PotentiometerView feedbacker = PotentiometerView(this, "FEEDBACKFEEDBACK");
    ToggleSwitch drySwitch = ToggleSwitch(this, "DRY", true);
    PotentiometerView delay = PotentiometerView(this, "DELAY");
    PotentiometerView feedback = PotentiometerView(this, "FEEDBACK");
    PotentiometerView level = PotentiometerView(this, "LEVEL");

    Options<int> options = Options<int>(this, "MODE",
                                        {OptionsNode<int>("DIGITAL", 0),
                                         OptionsNode<int>("ANALOG", 1)});
    //PotentiometerView feedbacker2 = PotentiometerView(this, "FEEDBACKFEEDBACK222");
public:
    Delay() : EffectsUnit("DELAY")
    {
        for (int i = 0; i < MAX_DELAY; i++)
        {
            delaybuffer1[i] = 0;
        }
    }

    void setBypass(bool bypass)
    {
        if (bypass != this->bypass)
        {
            for (int i = 0; i < MAX_DELAY; i++)
            {
                delaybuffer1[i] = 0;
            }
        }
        this->bypass = bypass;
    }

    void processing(int16_t *out, size_t length) noexcept override
    {
        //Serial.println(in);
        float feedback_level = feedback.getRangedValue();
        delay_depth = delay.getRangedValue() * MAX_DELAY - 1;
        float levelL = level.getRangedValue();
        for (int i = 0; i < length; i++)
        {
            int16_t in = out[i];
            if (feedback_level == 0.0)
            {
                delaybuffer1[counter] = in;
            }
            else
            {
                delaybuffer1[counter] = in + ((float)delaybuffer1[counter]) * feedback_level;
            }
            counter++;
            if (counter >= delay_depth)
            {
                counter = 0;
            }

            out[i] = (delaybuffer1[counter]) * levelL;
            if (drySwitch.state)
                out[i] += in;
        }
    }

    ~Delay()
    {
    }
};
