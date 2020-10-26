#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"

class PitchShifter : public EffectsUnit
{
private:
    static const int bufSize = 4192;
    int overlap = 100;

    int16_t Buf[bufSize] = {};

    int WtrP;
    double Rd_P = 0.0;
    double Shift;
    double CrossFade;
    double a0, a1, a2, b1, b2, hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;

    int16_t Do_HighPass(int16_t inSample)
    {
        //300Hz high-pass, 96k
        a0 = 0.9862117951198142f;
        a1 = -1.9724235902396283f;
        a2 = 0.9862117951198142f;
        b1 = -1.972233470205696f;
        b2 = 0.9726137102735608f;

        double inSampleF = (double)inSample;
        double outSampleF =
            a0 * inSampleF + a1 * hp_in_z1 + a2 * hp_in_z2 - b1 * hp_out_z1 - b2 * hp_out_z2;
        hp_in_z2 = hp_in_z1;
        hp_in_z1 = inSampleF;
        hp_out_z2 = hp_out_z1;
        hp_out_z1 = outSampleF;

        return (int16_t)outSampleF;
    }
public:
    //PotentiometerView wet = PotentiometerView(this, "WET", 0.0, 1.0, 1.0);
    PotentiometerView wet = PotentiometerView(this, "WETNESS", 0.0, 1.0, 0.5);
    PotentiometerView pitchPot = PotentiometerView(this, "PITCH", 0.5, 2.0, 0.5);
    PotentiometerView overlapPot = PotentiometerView(this, "OVRLP", 10, 4000, 100);
    
    PotentiometerView overlapPot2 = PotentiometerView(this, "OVRLP", 10, 4000, 100);
    ToggleSwitch filter = ToggleSwitch(this, "FLTR", false);

    int16_t Do_PitchShift(int16_t lSample)
    {
        //sum up and do high-pass
        if (filter.state)
            lSample = Do_HighPass(lSample);

        //write to ringbuffer
        Buf[WtrP] = lSample;

        //read fractional readpointer and generate 0° and 180° read-pointer in integer
        int RdPtr_Int = roundf(Rd_P);
        int RdPtr_Int2 = 0;
        if (RdPtr_Int >= bufSize / 2)
            RdPtr_Int2 = RdPtr_Int - (bufSize / 2);
        else
            RdPtr_Int2 = RdPtr_Int + (bufSize / 2);

        //read the two samples...
        double Rd0 = (double)Buf[RdPtr_Int];
        double Rd1 = (double)Buf[RdPtr_Int2];

        //Check if first readpointer starts overlap with write pointer?
        // if yes -> do cross-fade to second read-pointer
        if (overlap >= (WtrP - RdPtr_Int) && (WtrP - RdPtr_Int) >= 0 && Shift != 1.0f)
        {
            int rel = WtrP - RdPtr_Int;
            CrossFade = ((double)rel) / (double)overlap;
        }
        else if (WtrP - RdPtr_Int == 0)
            CrossFade = 0.0f;

        //Check if second readpointer starts overlap with write pointer?
        // if yes -> do cross-fade to first read-pointer
        if (overlap >= (WtrP - RdPtr_Int2) && (WtrP - RdPtr_Int2) >= 0 && Shift != 1.0f)
        {
            int rel = WtrP - RdPtr_Int2;
            CrossFade = 1.0f - ((double)rel) / (double)overlap;
        }
        else if (WtrP - RdPtr_Int2 == 0)
            CrossFade = 1.0f;

        //do cross-fading and sum up
        lSample = (Rd0 * CrossFade + Rd1 * (1.0f - CrossFade));

        //increment fractional read-pointer and write-pointer
        Rd_P += Shift;
        WtrP++;
        if (WtrP == bufSize)
            WtrP = 0;
        if (roundf(Rd_P) >= bufSize)
            Rd_P = 0.0f;

        return lSample;
    }

    PitchShifter() : EffectsUnit("PITCH SHIFTER")
    {
    }

    void processing_sample(int16_t *sample) override
    {
        overlap = (int)overlapPot.getValue();
        //dryMix = 1.0 - mix;
        //int16_t original = (int16_t)((*sample)*dryMix);
        Shift = pitchPot.getValue();
        //float mix = (float)wet.getValue();
        //int16_t pitched = (int16_t)();
        (*sample) = Do_PitchShift((*sample))*(wet.getValue()) + (*sample)*(1.0 - wet.getValue());// + (int16_t)((*sample)*mix);// + original; //+ (int16_t)(pitched * (1.0 - wet.getValue()));
    }

    /*void processing(int16_t *out, size_t size) override
    {
        auto now = micros();
        double val = pitchPot.getValue();
        if (Pitch != val)
        {
            st.setPitch(val);
            Pitch = val;
        }
        st.putSamples(out, size);
        st.receiveSamples(out, size);
        maxMicros = std::max(maxMicros, micros()-now);
    }*/
};