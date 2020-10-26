#pragma once

#include "GFX/View.h"
#include "Views/Generic/TextComponent.h"
#include "Audio/EffectsUnitBase.h"

class ControlView : public View
{
protected:
    Animation scrollTextAnim = Animation();
    bool animationDirection = true;
public:
    TextComponent label;
    Size defaultSize = Size(80, 64), hoverSize = Size(80, 84);

    ControlView(EffectsUnitBase *unit, const String &labelText, bool control = true) : View(nullptr, Size(0, 0)), label(this, labelText, &FreeSansBold9pt7b, Size(0, 5))
    {
        setSize(defaultSize);
        hoverEvent = [&](Container *c) {
            animationResize(hoverSize, 500);
        };

        unhoverEvent = [&](Container *c) {
            animationResize(defaultSize, 500);
        };

        if (control)
            unit->controls.push_back(this);

        label.autoScroll = true;
        label.expandOnHover = true;
    }

    void draw(GFX *gfx)
    {
        label.textColor = gfx->pallete.viewFgColor;
        label.position.y = size.height - label.getBounds(gfx).height - 1;
        onDraw(gfx);
        label.draw(gfx);
    }
};

