#pragma once

#include "Activity.h"
#include "GFX.h"


class Screen
{
private:
    Activity *currentActivity = nullptr;
    bool isActivityDisposed = false;
    Sipeed_ST7789 *display_ptr = nullptr;

    unsigned long frameTime = 0, updateMs = 0, prevUpdateMs = 0;

public:
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
        deallocActivity();
        currentActivity = activity;
        if (activity != nullptr)
        {
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
            currentActivity = nullptr;
        }
        isActivityDisposed = false;
    }

    void draw()
    {
        prevUpdateMs = millis();
        //display_ptr->fill();
        gfx.fillScreen(COLOR_BLACK);
        if (currentActivity != nullptr && !isActivityDisposed)
        {
            currentActivity->draw(&gfx);
        }
        display_ptr->drawImage(0, 0, gfx.width(), gfx.height(), gfx.getBuffer());
        display_ptr->setCursor(10, 10);
        display_ptr->print(frameTime);
        frameTime = millis() - prevUpdateMs;
        //prevUpdateMs = updateMs;
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

    void exitActivity()
    {
        currentActivity->onExit();
        if (currentActivity->intentActivity != nullptr)
        {
            //currentActivity->intentActivity->onUnselect();
            setActivity(currentActivity->intentActivity);
            currentActivity->onHover();
            //currentActivity->centerSelectedItem();
        }
    }
};