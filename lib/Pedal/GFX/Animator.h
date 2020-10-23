#pragma once

#include "Arduino.h"

class Animation
{
protected:
    double from, to, currentValue = 0.0, delta = 0.0;
    long ms = 0, startTickMs = 0, updateTickMs = 0, deltaMs = 0;
    bool finished = false, finishedAnimation = false, running = false, incremention = false;

public:
    Animation()
    {
        finishedAnimation = false;
        running = false;
        finished = true;
    }

    void setup(double from, double to, unsigned long ms)
    {
        finishedAnimation = false;
        running = false;
        finished = true;
        this->from = currentValue = from;
        this->to = to;
        this->ms = ms;
        incremention = to > from;
        if (ms == 0.0)
        {
            currentValue = to;
            finishedAnimation = true;
        }
        else
        {
            delta = (to - from);

            /*if (abs(minDelta) > abs(delta))
            {
                delta = incremention ? abs(minDelta) : -abs(minDelta);
            }*/
        }
    }

    void changeTo(double to, long addMs)
    {
        double prevTo = this->to;
        unsigned long prevMs = ms;


        this->ms = ms + addMs;

        this->to = to;
        double percentage1 = ((double)deltaMs) / ((double)prevMs),
               percentage2 = ((double)deltaMs) / ((double)ms);
        double timingValue1 = timingFunction(percentage1),
               timingValue2 = timingFunction(percentage2);

        from = (to * timingValue2 + (from - prevTo) * timingValue1 - from) / (timingValue2 - 1);

        bool newInc = to > from;
        //if (newInc != incremention)
            //stop();
        incremention = newInc;
    }

    void start()
    {
        finished = finishedAnimation = false;
        running = true;
        startTickMs = millis();
    }

    void stop()
    {
        running = false;
        finishedAnimation = finished = true;
    }

    double getValue()
    {
        tick();
        if (finishedAnimation)
            finished = true;
        return currentValue;
    }

    double timingFunction(double t)
    {
        return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
        //return t;
    }
    double linear(double t) { return t; }

    void tick()
    {
        updateTickMs = millis();
        if (running)
        {
            deltaMs = updateTickMs - startTickMs;
            double percentage = ((double)deltaMs) / ((double)ms);
            if (percentage > 1.0)
            {
                percentage = 1.0;
            }
            currentValue = (to - from) * timingFunction(percentage) + from;

            if ((incremention && currentValue >= to) || deltaMs >= ms)
            {
                currentValue = to;
                finishedAnimation = true;
                stop();
            }
            else if ((!incremention && currentValue <= to) || deltaMs >= ms)
            {
                currentValue = to;
                finishedAnimation = true;
                stop();
            }
        }
    }

    bool isRunning()
    {
        return running;
    }

    bool isFinished()
    {
        return finished;
    }
};
