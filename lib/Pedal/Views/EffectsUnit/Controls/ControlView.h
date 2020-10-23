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

    ControlView(EffectsUnitBase *unit, const String &labelText, bool control = true) : View(nullptr, Size(0, 0)), label(this, labelText, &FreeSansBold9pt7b, Size(5, 5))
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
        /*gfx->setOverlap(isHovered);

        Size bounds = label.getBounds(gfx);

        Position textPos = Position((size.width - bounds.width) / 2, size.height - bounds.height - 1);
        if (bounds.width > size.width)
        {
            if (isHovered)
            {
                Position absPos = getPosition() + textPos;

                if (absPos.x < 0)
                {
                    textPos.x = 0;
                }
                else if (absPos.x + bounds.width > gfx->width())
                {
                    textPos.x = size.width - bounds.width;
                }
            }
            else
            {
                if (scrollTextAnim.isFinished())
                {
                    animationDirection = !animationDirection;
                    int delta = bounds.width - size.width;
                    if (animationDirection)
                        scrollTextAnim.setup(0, -(delta), 50 * delta);
                    else
                        scrollTextAnim.setup(-(delta), 0, 50 * delta);
                    scrollTextAnim.start();
                }
                textPos.x = scrollTextAnim.getValue();
            }
        }*/
        label.textColor = gfx->pallete.viewFgColor;
        label.position.y = size.height - label.getBounds(gfx).height - 1;
        label.draw(gfx);

        onDraw(gfx);
    }
};

