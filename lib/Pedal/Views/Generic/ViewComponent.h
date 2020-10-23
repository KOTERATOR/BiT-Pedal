#pragma once

#include "GFX/GFX.h"
#include "GFX/View.h"

class ViewComponent
{
protected:
    View *parent;
public:
    Position position;
    Size size;

    ViewComponent(View *parent) : parent(parent)
    {

    }

    virtual void draw(GFX *gfx) {}
};