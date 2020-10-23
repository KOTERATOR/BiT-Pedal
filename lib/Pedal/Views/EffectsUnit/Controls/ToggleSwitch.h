#pragma once

#include "Views/EffectsUnit/Controls/ControlView.h"

class ToggleSwitch : ControlView
{
protected:
    Animation switchAnimation;
    bool prevState = false;

public:
    bool state;

    ToggleSwitch(EffectsUnitBase *unit, const String &labelText, bool state = false) : ControlView(unit, labelText)
    {
        clickOnly = true;
        this->state = prevState = state;
        selectEvent = [&](Container *c) { this->state = !this->state; };
    }

    void onDraw(GFX *gfx)
    {
        int length = size.width * 0.8;
        int height = (size.height - label.size.height) / 2;
        height += height % 2 + 1;
        int x = (size.width - length) / 2,
            y = (size.height - height - label.size.height) / 2;
        
        int r = (height - 6) / 2;

        int offX = x + 3 + r,
            onX = x + length - 3 - r - 1;


        gfx->fillRoundRect(x, y, length, height, height / 2, gfx->pallete.viewFgColor);
        gfx->fillRoundRect(x + 3, y + 3, length - 6, height - 6, (height - 6) / 2, state ? gfx->pallete.viewSelectColor : gfx->pallete.viewBgColor);

        gfx->fillCircle(state ? onX : offX, y + height / 2, r - 1, gfx->pallete.viewFgColor);
    }
};