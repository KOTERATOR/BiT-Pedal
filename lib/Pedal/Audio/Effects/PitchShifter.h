#pragma once

#include "Audio/EffectsUnit.h"
#include "Audio/Components/PitchShifter.h"
#include "Audio/Components/DelayLine.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"
#include "GFX/Animator.h"

class PitchShifter : public EffectsUnit
{
private:
    static const int bufSize = 3000;
    int overlap = 150;

    int16_t Buf[bufSize] = {};

    int WtrP;
    float Rd_P = 0.0;
    float Shift;
    float CrossFade;
    float a0, a1, a2, b1, b2, hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;

    bool curRd = false;
    bool rising = false;

    int16_t Do_HighPass(int16_t inSample)
    {
        //300Hz high-pass, 96k
        a0 = 0.9862117951198142f;
        a1 = -1.9724235902396283f;
        a2 = 0.9862117951198142f;
        b1 = -1.972233470205696f;
        b2 = 0.9726137102735608f;

        float inSampleF = (float)inSample;
        float outSampleF =
            a0 * inSampleF + a1 * hp_in_z1 + a2 * hp_in_z2 - b1 * hp_out_z1 - b2 * hp_out_z2;
        hp_in_z2 = hp_in_z1;
        hp_in_z1 = inSampleF;
        hp_out_z2 = hp_out_z1;
        hp_out_z1 = outSampleF;

        return (int16_t)outSampleF;
    }
public:
    DSP::PitchShifter shifter = DSP::PitchShifter();
    //DSP::DelayLine<int16_t, 44100> delay = DSP::DelayLine<int16_t, 44100>();
    //PotentiometerView wet = PotentiometerView(this, "WET", 0.0, 1.0, 1.0);
    PotentiometerView wet = PotentiometerView(this, "WETNESS", 0.0, 1.0, 0.5);
    //PotentiometerView pitchAnimation = PotentiometerView(this, "PITCH SPEED", 0.0, 3000.0, 1000.0);
    PotentiometerView pitchPot = PotentiometerView(this, "PITCH", 0.f, 1.f, 0.5);
    PotentiometerView overlapPot = PotentiometerView(this, "OVRLP", 10, 4000, 100);
    ToggleSwitch filter = ToggleSwitch(this, "FLTR", false);

    float semitones = 0.f;


    PitchShifter() : EffectsUnit("PITCH SHIFTER")
    {
        shifter.Init(44100.f);
        shifter.SetDelSize(4000);
        //shifter.SetFun(1.f);
        shifter.SetTransposition(12.f);
        //delay.Init();
    }

    void processing_sample(int16_t *sample) override
    {
        overlap = (int)overlapPot.getValue();
        //dryMix = 1.0 - mix;
        //int16_t original = (int16_t)((*sample)*dryMix);
        
        //float mix = (float)wet.getValue();
        //int16_t pitched = (int16_t)();
    }

    void processing(int16_t *out, size_t size) override
    {
        /*float newSemitones = 22050.f*pitchPot.getValue();
        if (newSemitones != semitones)
        {
            semitones = newSemitones;
            //shifter.SetTransposition(semitones);
            delay.SetDelay(semitones);
        }*/
        for (int i = 0; i < size; i++)
        {
            float in = (float)out[i];
            out[i] = (int16_t)shifter.Process(in);
            //delay.Write(out[i]);
            //out[i] = delay.Read();
        }
    }
};