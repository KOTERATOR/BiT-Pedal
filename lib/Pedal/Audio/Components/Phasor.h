#pragma once

/** Generates a normalized signal moving from 0-1 at the specified frequency.
\todo Selecting which channels should be initialized/included in the sequence conversion.
\todo Setup a similar start function for an external mux, but that seems outside the scope of this file.
*/
class Phasor
{
  public:
    Phasor() {}
    ~Phasor() {}
    /** Initializes the Phasor module
    sample rate, and freq are in Hz
    initial phase is in radians
    Additional Init functions have defaults when arg is not specified:
    - phs = 0.0f
    - freq = 1.0f
    */
    inline void Init(float sample_rate, float freq, float initial_phase)
    {
        sample_rate_ = sample_rate;
        phs_         = initial_phase;
        SetFreq(freq);
    }

    /** Initialize phasor with samplerate and freq
    */
    inline void Init(float sample_rate, float freq)
    {
        Init(sample_rate, freq, 0.0f);
    }

    /** Initialize phasor with samplerate
    */
    inline void Init(float sample_rate) { Init(sample_rate, 1.0f, 0.0f); }
    /** processes Phasor and returns current value
    */
    float Process();


    /** Sets frequency of the Phasor in Hz
    */
    void SetFreq(float freq);


    /** Returns current frequency value in Hz
    */
    inline float GetFreq() { return freq_; }

  private:
    float freq_;
    float sample_rate_, inc_, phs_;
};

void Phasor::SetFreq(float freq)
{
    freq_ = freq;
    inc_  = ((float)M_TWOPI * freq_) / sample_rate_;
}

float Phasor::Process()
{
    float out;
    out = phs_ / ((float)M_TWOPI);
    phs_ += inc_;
    if(phs_ > (float)M_TWOPI)
    {
        phs_ -= (float)M_TWOPI;
    }
    if(phs_ < 0.0f)
    {
        phs_ = 0.0f;
    }
    return out;
}