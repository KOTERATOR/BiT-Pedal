#pragma once

#include "Activity.h"
#include "GFX.h"

class Screen
{
private:
    Activity *currentActivity = nullptr;
    bool isActivityDisposed = true;
    Sipeed_ST7789 *display_ptr = nullptr;

    unsigned long frameTime = 0, updateMs = 0, prevUpdateMs = 0;

public:
    unsigned long processingTime = 0;
    unsigned long maxProcessingTime = 0, prevProcessingUpdateMs = 0;
    GFX gfx;

    Screen() : gfx(320, 240)
    {
    }

    void setDisplay(Sipeed_ST7789 *display)
    {
        this->display_ptr = display;
    }

    void setActivity(Activity *activity)
    {
        if (!isActivityDisposed)
            deallocActivity();

        currentActivity = activity;
        if (activity != nullptr)
        {
            isActivityDisposed = false;
            currentActivity->update();
        }
    }

    Activity *getCurrentActivity()
    {
        return currentActivity;
    }

    void intent(Activity *activity)
    {
        if (activity != nullptr)
        {
            activity->intentActivity = currentActivity;

            currentActivity = activity;
            currentActivity->update();
        }
    }

    void deallocActivity()
    {
        isActivityDisposed = true;
        if (currentActivity != nullptr)
        {
            delete currentActivity;
            //currentActivity = nullptr;
        }
    }

    void exitActivity()
    {
        currentActivity->onExit();
        if (currentActivity->intentActivity != nullptr)
        {
            //currentActivity->intentActivity->onUnselect();
            setActivity(currentActivity->intentActivity);
            //currentActivity->onHover();
            //currentActivity->centerSelectedItem();
        }
    }

    void draw()
    {
        prevUpdateMs = millis();
        //display_ptr->fill();
        gfx.fillScreen(COLOR_BLACK);
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            currentActivity->draw(&gfx);
            gfx.setCurrentContainer(nullptr);
            String heap(get_free_heap_size());
            String proc(processingTime/1000.0);
            String frame(frameTime);
            gfx.drawText(5, 15, /*frame*/ heap, COLOR_WHITE, &FreeSansBold9pt7b);
            gfx.drawText(5, 30, proc, COLOR_WHITE, &FreeSansBold9pt7b);
            //gfx.drawText(5, 1, proc, COLOR_WHITE, &FreeSansBold9pt7b);
        }

        display_ptr->drawImage(0, 0, gfx.width(), gfx.height(), gfx.getBuffer());

        frameTime = millis() - prevUpdateMs;
        //prevUpdateMs = updateMs;
    }

    void update()
    {
        if (currentActivity != nullptr)
        {
            currentActivity->update();
        }
    }

    void onNext()
    {
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            currentActivity->onNext();
        }
    }

    void onPrev()
    {
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            currentActivity->onPrev();
        }
    }

    void onSelect()
    {
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            currentActivity->onSelect();
        }
    }

    void onUnselect()
    {
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            if (currentActivity->onUnselect())
            {
                exitActivity();
            }
        }
    }
};