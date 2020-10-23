#pragma once

#include "GFX/View.h"

class AddButtonView : public View
{
public:
    AddButtonView(Size size = Size(48, 48)) : View(nullptr, size, Position(0, 0))
    {

    }

    void onDraw(GFX * gfx)
    {
        Size plusSize(32, 32);
        int plusLineWidth = 4;

        gfx->fillCircle(size.width/2 , size.height/2 , size.width/2 - 4, isHovered ? gfx->pallete.viewSelectColor : gfx->pallete.viewFgColor);
        uint16_t plusColor = isHovered ? gfx->pallete.viewFgColor : gfx->pallete.viewBgColor;
        gfx->fillRect((size.width - plusLineWidth)/2, (size.height - plusSize.height)/2, plusLineWidth, plusSize.height, plusColor);
        gfx->fillRect((size.width - plusSize.width)/2, (size.height - plusLineWidth)/2, plusSize.width, plusLineWidth, plusColor);
    }
};