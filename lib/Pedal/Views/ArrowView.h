#pragma once

#include "GFX/View.h"

class ArrowView : public View
{
  protected:
    Size arrowSize = Size(16, 16), arrowRectSize = Size(8, 8);
  public:
    ArrowView(Size size = Size(16, 48)) : View(nullptr, size, Position(0, 0)) {
      isSelectable = false;
    }

    void onDraw(GFX * gfx)
    {
        int hCenter = size.height/2;
        int rectTopPointY = hCenter - arrowRectSize.height/2;
        gfx->fillRect(0, rectTopPointY, arrowRectSize.width, arrowRectSize.height, gfx->pallete.viewFgColor);
        gfx->fillTriangle(arrowRectSize.width, (size.height - arrowSize.height)/2 + 1, arrowRectSize.width, (size.height + arrowSize.height)/2 - 1, size.width-1, hCenter-1, gfx->pallete.viewFgColor);
    }
};