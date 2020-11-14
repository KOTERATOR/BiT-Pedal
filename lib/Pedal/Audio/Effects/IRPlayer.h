#pragma once

#include "Audio/EffectsUnit.h"
#include "Views/EffectsUnit/Controls/PotentiometerView.h"
#include "IR/IR.h"
#include "Audio/WavFile.h"

#define SAMPLEFILTER_TAP_NUM 128

typedef struct
{
    float history[SAMPLEFILTER_TAP_NUM];
    unsigned int last_index;
} SampleFilter;

void SampleFilter_init(SampleFilter *f);
void SampleFilter_put(SampleFilter *f, float input);
float SampleFilter_get(SampleFilter *f, float *fir);

void SampleFilter_init(SampleFilter *f)
{
    int i;
    for (i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
        f->history[i] = 0;
    f->last_index = 0;
}

void SampleFilter_put(SampleFilter *f, float input)
{
    f->history[f->last_index++] = input;
    if (f->last_index == SAMPLEFILTER_TAP_NUM)
        f->last_index = 0;
}

float SampleFilter_get(SampleFilter *f, float *fir)
{
    float acc = 0;
    int index = f->last_index, i;
    for (i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
    {
        index = index != 0 ? index - 1 : SAMPLEFILTER_TAP_NUM - 1;
        acc += f->history[index] * fir[i];
    };
    return acc;
}

class IRPlayer : public EffectsUnit
{
public:
    void RunFIRFilter(float *FirCoeff, int NumTaps, float *Signal, float *FilteredSignal, int NumSigPts)
    {
        int j, k, n, Top = 0;
        float y, Reg[256]; // This assumes <= 256 taps.

        for (j = 0; j < NumTaps; j++)
            Reg[j] = 0.0;

        for (j = 0; j < NumSigPts; j++)
        {
            Reg[Top] = Signal[j];
            y = 0.0;
            n = 0;

            // The FirCoeff index increases while the Reg index decreases.
            for (k = Top; k >= 0; k--)
            {
                y += FirCoeff[n++] * Reg[k];
            }
            for (k = NumTaps - 1; k > Top; k--)
            {
                y += FirCoeff[n++] * Reg[k];
            }
            FilteredSignal[j] = y;

            Top++;
            if (Top >= NumTaps)
                Top = 0;
        }
    }

#define FILTER_TAP_NUM 128
    float filter_taps[FILTER_TAP_NUM] = {};

    float filter_read[FILTER_TAP_NUM] = {};

    PotentiometerView gain = PotentiometerView(this, "GAIN", 0.1, 2.0, 0.125);
    Options<int> firV = Options<int>(this, "IR", {});
    int prevIrIndex = -1;
    int prevIrProcessingIndex = -1;
    SampleFilter *sf;

    IRPlayer() : EffectsUnit("IR PLAYER")
    {
        sf = new SampleFilter();
        SampleFilter_init(sf);

        File irFolder = SD.open("/IR");
        if (irFolder)
        {
            int index = 0;
            File ir = irFolder.openNextFile();
            while (ir)
            {
                if (!ir.isDirectory())
                {
                    Serial.println(ir.name());
                    firV.options.push_back(OptionsNode<int>(ir.name(), index++));
                }
                ir.close();
                ir = irFolder.openNextFile();
            }
        }
        else
        {
            Serial.println("IR FOLDER NOT FOUND");
        }
        //firV.options.push_back()
    }

    //float conv = 1.f / 32768.f;
    int maxDelta = 0;
    //float input[1024] = {};
    //float output[1024] = {};
    void update()
    {
        int irnum = *firV.getValue();
        if (irnum != prevIrIndex)
        {
            File irFolder = SD.open("/IR");
            if (irFolder)
            {
                int index = 0;
                File ir = irFolder.openNextFile();
                while (ir)
                {
                    if (!ir.isDirectory())
                    {
                        if (index == irnum)
                        {
                            ir.seek(0);
                            WavInFile file = WavInFile(ir);
                            Serial.println(file.getSampleRate());
                            Serial.println(file.getBytesPerSample());

                            /*int size = ir.read((void*)bytes, (uint16_t)256);
                            for (int i = 0; i < 256; i++)
                            {
                                if (i % 8 == 0)
                                    Serial.println();
                                Serial.print(bytes[i], HEX);
                                Serial.print(" ");  
                            }
                            ir.seek(0);*/
                            int size = FILTER_TAP_NUM * file.getNumChannels();
                            float *read = new float[size];
                            file.read(read, size);
                            for (int i = 0; i < FILTER_TAP_NUM; i++)
                            {
                                filter_read[i] = read[(int)(i * file.getNumChannels())];
                            }
                            delete[] read;
                            ir.close();
                            break;
                        }
                        index++;
                    }
                    ir.close();
                    ir = irFolder.openNextFile();
                }
            }
            prevIrIndex = irnum;
        }
    }

    void processing(int16_t *out, size_t size) override
    {
        if (prevIrIndex != prevIrProcessingIndex)
        {
            prevIrProcessingIndex = prevIrIndex;
            memcpy(filter_taps, filter_read, sizeof(float) * FILTER_TAP_NUM);
        }
        /*unsigned long now = micros();
        for (int i = 0; i < size; i++)
        {
            input[i] = ((float)out[i]) * conv;
        }

        RunFIRFilter(ir2, 19, input, output, size);

        for (int i = 0; i < size; i++)
        {
            out[i] = output[i] * (float)(32768.f);
        }

        int delta = micros() - now;

        if (maxDelta < delta)
        {
            maxDelta = delta;
        }*/
        float gainf = gain.getValue();
        for (int i = 0; i < size; i++)
        {
            //out[i] = Calc_FIR(out[i]/16);
            SampleFilter_put(sf, out[i] * gainf);
            out[i] = SampleFilter_get(sf, filter_taps);
        }
    }
};