#pragma once

#include "Pedal.h"
#include "GFX/Activity.h"
#include "Audio/EffectsLibrary.h"
#include "Views/ButtonView.h"
#include "Audio/Preset.h"
#include <vector>

class AddEffectActivity : public Activity
{
public:
    std::vector<ButtonView*> effects = std::vector<ButtonView*>();

    AddEffectActivity(Preset * currentPreset) : Activity()
    {
        for(int i = 0; i < effectsLibrary.library.size(); i++)
        {
            ButtonView * btn = new ButtonView(effectsLibrary.library[i].name);
            btn->selectEvent = ([=](Container * c)
            { 
                auto effect = effectsLibrary.library[i].constructor();
                currentPreset->add(effect);
                pedal.screen.exitActivity(); 
            });
            effects.push_back(btn);
            add(btn);
        }
    }
 
    ~AddEffectActivity()
    {
        for(int i = 0; i < effects.size(); i++)
        {
            delete effects[i];
        }
    }
};