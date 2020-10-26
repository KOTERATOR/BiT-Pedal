#pragma once

#include "GFX/View.h"

class ButtonView : public View
{
public:
    TextComponent label;
    HTextAlignment horizontalAlignment = HTextAlignment::Left;
    VTextAlignment verticalAlignment = VTextAlignment::Bottom;

    ButtonView(String text = "", const GFXfont *font = &FreeSansBold12pt7b, int textSize = 1, uint16_t color = COLOR_WHITE, HTextAlignment horizontalAlignment = HTextAlignment::Center, VTextAlignment verticalAlignment = VTextAlignment::Center) : View(nullptr, Size(200, 30),
                                                                                                                                                                                                                Position(0, 0)),
                                                                                                                                                                                                           label(this, text, font)
    {
        this->horizontalAlignment = horizontalAlignment;
        this->verticalAlignment = verticalAlignment;
        this->containerMode = ContainerMode::CENTER;
        label.textColor = color;
        label.autoScroll = true;
    }

    void onDraw(GFX *gfx)
    {
        int16_t x1 = 0, y1 = 0;
        Size bounds = label.getBounds(gfx);
        if (horizontalAlignment == HTextAlignment::Left)
        {
            x1 = 0;
        }
        else if (horizontalAlignment == HTextAlignment::Right)
        {
            x1 = size.width - bounds.width;
        }
        else if (horizontalAlignment == HTextAlignment::Center)
        {
            x1 = (size.width - bounds.width) / 2;
        }

        if (verticalAlignment == VTextAlignment::Top)
        {
            y1 = 0;
        }
        else if (verticalAlignment == VTextAlignment::Bottom)
        {
            y1 = size.height - 1;
        }
        else if (verticalAlignment == VTextAlignment::Center)
        {
            y1 = (size.height - bounds.height) / 2 - 1;
        }

        label.position.x = x1;
        label.position.y = y1;

        gfx->fillRoundRect(0, 0, size.width, size.height, size.height / 2, isSelected ? gfx->pallete.viewSelectColor : (isHovered ? gfx->pallete.viewHoverColor : gfx->pallete.viewBgColor));

        label.draw(gfx);
        //Serial.printf("%s - (%d; %d)\r\n", text.c_str(), bounds.width, bounds.height);
        //Size bounds = gfx.getTextBounds(text);
        //gfx.drawText((size.width - bounds.width) / 2, (size.height - bounds.height) / 2 + 1, text, Color::WHITE);
        //gfx.drawText(0, 10, text, color, textSize);
    }

    void onHover()
    {
        View::onHover();
        animationResize(Size(200, 45), 100);
    }

    void onUnhover()
    {
        View::onUnhover();
        animationResize(Size(200, 30), 100);
    }
};