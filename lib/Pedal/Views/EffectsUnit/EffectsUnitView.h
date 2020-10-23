#pragma once

#include "GFX/Screen.h"
#include "Views/EffectsUnit/Controls/ControlView.h"
#include "Audio/EffectsUnitBase.h"

class EffectsUnitView : public ControlView
{
public:
    EffectsUnitBase * unit;
    EffectsUnitView(EffectsUnitBase * unit) : ControlView(unit, unit->name, false), unit(unit)
    {
        //onSelectEvent([=](){ pedal.intent(new EffectActivity(unit)); });
    }

    void onDraw(GFX *gfx)
    {
        gfx->fillCircle(size.width/2, size.height/2, isHovered ? size.height/4 : size.height/8, unit->getBypass() ? gfx->pallete.viewBgColor : gfx->pallete.viewSelectColor);
    }
};

