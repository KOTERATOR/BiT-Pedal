#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"
#include "Views/EffectsUnit/Controls/Options.h"
#include "GFX/Animator.h"
//#define SOUNDTOUCH_INTEGER_SAMPLES
#include "../soundtouch/SoundTouch.h"
#include "cmath"

class PitchOptions : public Options<int>
{
public:
    PitchOptions(EffectsUnitBase *unit, const String &label, int defaultPitch = 0) : Options<int>(unit, label, {})
    {
        for (int i = -24; i <= 24; i++)
        {
            options.push_back(OptionsNode<int>(String(i), i));
            if (defaultPitch == i)
            {
                currentOptionIndex = defaultPitch + 24;
            }
        }
    }
};

class SoundShifter : public EffectsUnit
{
private:
    //Animation pitchAnim = Animation();

    bool setBypassAfterAnim = false;
    soundtouch::SoundTouch *st1 = new soundtouch::SoundTouch(),
                           *st2 = new soundtouch::SoundTouch();
    soundtouch::SoundTouch *sts[2] = {st1, st2};

public:
    PotentiometerView dry = PotentiometerView(this, "DRY", 0.0, 1.0, 0.5);
    //PotentiometerView pitch = PotentiometerView(this, "PITCH", 0.5, 2.0, 1.0);

    ToggleSwitch pe1 = ToggleSwitch(this, "P1 ON", true);
    PotentiometerView pv1 = PotentiometerView(this, "P1 VOL", 0.0, 1.0, 0.5);
    PitchOptions po1 = PitchOptions(this, "P1", -12);

    ToggleSwitch pe2 = ToggleSwitch(this, "P2 ON", true);
    PotentiometerView pv2 = PotentiometerView(this, "P2 VOL", 0.0, 1.0, 0.5);
    PitchOptions po2 = PitchOptions(this, "P2", 12);

    int prevPO1, prevPO2;

    SoundShifter() : EffectsUnit("SOUND SHIFTER")
    {
        for (int i = 0; i < 2; i++)
        {
            soundtouch::SoundTouch *st = sts[i];
            st->setSampleRate(44100);
            st->setChannels(1);
            st->setSetting(SETTING_USE_QUICKSEEK, 1);
            //st.setSetting(SETTING_USE_AA_FILTER, 0);
            st->setSetting(SETTING_AA_FILTER_LENGTH, 8);
            st->setSetting(SETTING_SEQUENCE_MS, 50);
            st->setSetting(SETTING_SEEKWINDOW_MS, 15);
            st->setSetting(SETTING_OVERLAP_MS, 8);
        }
        prevPO1 = *po1.getValue();
        prevPO2 = *po2.getValue();
        st1->setPitchSemiTones(prevPO1);
        st2->setPitchSemiTones(prevPO2);
    }
    int maxDelta = -1;
    void processing(int16_t *out, size_t size) override
    {
        unsigned long now = micros();
        int16_t st1Buf[size] = {};
        int16_t st2Buf[size] = {};
        memset(&st1Buf, 0, sizeof(int16_t) * size);
        memset(&st2Buf, 0, sizeof(int16_t) * size);

        int newPO1 = *po1.getValue(), newPO2 = *po2.getValue();

        if (newPO1 != prevPO1)
        {
            st1->clear();
            st1->setPitchSemiTones(newPO1);
            prevPO1 = newPO1;
        }
        if (newPO2 != prevPO2)
        {
            st2->clear();
            st2->setPitchSemiTones(newPO2);
            prevPO2 = newPO2;
        }

        uint nSamples = 0, index = 0;

        st1->putSamples(out, size);
        st2->putSamples(out, size);
        do
        {
            nSamples = st1->receiveSamples(&st1Buf[index], size - index);
            index += nSamples;
        } while (nSamples != 0);

        nSamples = 0, index = 0;
        do
        {
            nSamples = st2->receiveSamples(&st2Buf[index], size - index);
            index += nSamples;
        } while (nSamples != 0);
        //nSamples += st.receiveSamples(out, size);
        float dryC = dry.getValue();
        float wetP1 = pv1.getValue();
        float wetP2 = pv2.getValue();
        bool en1 = pe1.state, en2 = pe2.state;
        for (int i = 0; i < size; i++)
        {
            out[i] *= dryC;
            if (en1)
                out[i] += st1Buf[i] * wetP1;
            if (en2)
                out[i] += st2Buf[i] * wetP2;
        }

        int delta = micros() - now;

        if (maxDelta < delta)
        {
            maxDelta = delta;
        }
    }

    /*void setBypass(bool bypass)
    {
        if (!bypass)
        {
            setBypassAfterAnim = false;
            EffectsUnit::setBypass(bypass);
            pitchAnim.setup(1.0, pitch.getValue(), 500);
            pitchAnim.start();
        }
        else
        {
            setBypassAfterAnim = true;
            pitchAnim.setup(pitch.getValue(), 1.0, 500);
            pitchAnim.start();
        }
    }*/

    void update()
    {
        //Serial.println(maxDelta);
        //maxDelta = -1;
    }

    ~SoundShifter()
    {
        delete st1;
        delete st2;
    }
};