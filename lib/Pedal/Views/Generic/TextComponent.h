#pragma once

#include "ViewComponent.h"

class TextComponent : public ViewComponent
{
protected:
    Animation scrollAnimation = Animation();
    bool animationDirection = true;

public:
    String text;
    uint16_t textColor = 0, backgroundColor = 0;
    const GFXfont *font;
    uint8_t textSize = 1;

    bool drawBackground = false;
    bool autoScroll = false;
    bool expandOnHover = true;
    Size boundsOffset = Size(0, 0);
    int16_t backgroundRadius = 0;

    TextComponent(View *parent, const String &text, const GFXfont *font, Size boundsOffset = Size(0, 0), int16_t backgroundRadius = 0, uint8_t textSize = 1) : ViewComponent(parent), text(text),
                                                                                                                                                               font(font), textSize(textSize),
                                                                                                                                                               boundsOffset(boundsOffset),
                                                                                                                                                               backgroundRadius(backgroundRadius)
    {
    }

    Size getBounds(GFX *gfx)
    {
        gfx->setFont(font);
        size = gfx->getTextBounds(text, textSize) + boundsOffset + boundsOffset;
        return size;
    }

    void draw(GFX *gfx)
    {
        bool prevOverlap = gfx->getOverlap();

        if (parent != nullptr)
        {
            Size bounds = getBounds(gfx);
            int parentWidth = parent->getSize().width;
            position.x = (parentWidth - bounds.width) / 2;
            if (bounds.width > parentWidth)
            {
                if (parent->isHovered && expandOnHover)
                {
                    gfx->setOverlap(true);

                    Position absPos = parent->getPosition() + position;

                    if (absPos.x < 0)
                    {
                        position.x = 0;
                    }
                    else if (absPos.x + bounds.width > gfx->width())
                    {
                        position.x = parentWidth - bounds.width;
                    }
                }
                else if (autoScroll)
                {
                    if (scrollAnimation.isFinished())
                    {
                        animationDirection = !animationDirection;
                        int delta = bounds.width - parentWidth;
                        if (animationDirection)
                            scrollAnimation.setup(0, -(delta), 50 * delta);
                        else
                            scrollAnimation.setup(-(delta), 0, 50 * delta);
                        scrollAnimation.start();
                    }
                    position.x = scrollAnimation.getValue();
                }
                else
                {
                    position.x = 0;
                }
            }
        }
        if (drawBackground)
        {
            if (backgroundRadius != 0)
                gfx->fillRoundRect(position.x, position.y, size.width, size.height, backgroundRadius, backgroundColor);
            else
                gfx->fillRect(position.x, position.y, size.width, size.height, backgroundColor);
        }
        gfx->drawText(position.x + boundsOffset.width, position.y + size.height - boundsOffset.height - 1, text, textColor, font, textSize);
        gfx->setOverlap(prevOverlap);
    }
};