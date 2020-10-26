#pragma once

#include "Views/EffectsUnit/Controls/ControlView.h"
#include "Controls/VirtualPotentiometer.h"
#include "Audio/EffectsUnitBase.h"

class PotentiometerView : public ControlView, public VirtualPotentiometer
{
protected:
    uint16_t steps;

public:
    PotentiometerView(EffectsUnitBase *unit, const String &name, double from = 0.0, double to = 1.0, double value = 0, uint16_t steps = 100) : ControlView(unit, name), VirtualPotentiometer(from, to, value)
    {
        this->steps = steps;
        setValue(value);
    }

    void onDraw(GFX *gfx)
    {
        int r = (size.height - 32) / 2;
        int x, y = (size.height - 16) / 2;
        x = size.width / 2;

        gfx->fillCircle(x, y, r, gfx->pallete.viewFgColor);

        gfx->fillCircle(x, y, r - 3, isSelected ? gfx->pallete.viewHoverColor : gfx->pallete.viewBgColor);

        int angle = -getRangedValue() * 270 - 135;

        float rad = radians(angle - 90);
        int x1 = (x - (sin(rad) * r));
        int y1 = (y - (cos(rad) * r));

        gfx->drawLine(x, y, x1, y1, gfx->pallete.viewFgColor);
        String val(getValue());
        gfx->drawText(0, 20, val, gfx->pallete.dangerColor, &FreeSansBold9pt7b);
        //gfx->drawText(scrollTextAnim.getValue() /* (size.width - bounds.width) / 2 */, size.height - 2, name, gfx->pallete.viewFgColor);
    }

    void onNext()
    {
        setValue(getValue() + getRange() / steps);
    }

    void onPrev()
    {
        setValue(getValue() - getRange() / steps);
    }
};