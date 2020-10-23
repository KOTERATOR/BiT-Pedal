#pragma once

#include "GFX/Activity.h"
#include "Audio/EffectsUnit.h"
#include "Views/ButtonView.h"
#include "GFX/Layout.h"
#include "Audio/Preset.h"
#include "Views/EffectsUnit/EffectsUnitView.h"
#include "Views/ArrowView.h"
#include "Views/AddButtonView.h"
#include "Activities/AddEffectActivity.h"
#include <vector>

class PresetActivity : public Activity
{
private:
    Preset *preset;
    ButtonView *presetHeader;
    AddButtonView *addEffectButton = new AddButtonView();
    Layout *effectsLayout,
        *controlsLayout;
    std::vector<EffectsUnitView *> unitsView = std::vector<EffectsUnitView *>();
    EffectsUnitView *currentEffect = nullptr;
    //List<ArrowView> arrows = List<ArrowView>();

public:
    PresetActivity(Preset *preset) : Activity()
    {
        this->preset = preset;

        presetHeader = new ButtonView(preset->name);
        add(presetHeader);
        effectsLayout = new Layout(this, LayoutMode::HORIZONTAL, Position(0, 0), Size(320, 104), ContainerMode::NORMAL);
        controlsLayout = new Layout(this, LayoutMode::HORIZONTAL, Position(0, 0), Size(320, 96), ContainerMode::NORMAL);
        effectsLayout->setSpacing(5);
        controlsLayout->setSpacing(5);
        controlsLayout->isSelectable = false;
        //controlsLayout->setSpacing(10);

        for (int i = 0; i < preset->effects.size(); i++)
        {
            EffectsUnitView *ptr = new EffectsUnitView(preset->effects[i]);
            ArrowView arrow;
            unitsView.push_back(ptr);
            //arrows.add(arrow);
            effectsLayout->add((Container *)ptr);
            //effectsLayout->add(arrows[i]);
        }
        addEffectButton->selectEvent = ([preset](Container *c) { pedal.screen.intent(new AddEffectActivity(preset)); });
        effectsLayout->add(addEffectButton);
    }

    void onDraw(GFX *gfx)
    {
        if (preset->effects.size() != unitsView.size())
        {
            effectsLayout->clear();
            for (int i = 0; i < unitsView.size(); i++)
            {
                EffectsUnitView *view = unitsView[i];
                delete view;
            }
            unitsView.clear();
            //arrows.clear();
            for (int i = 0; i < preset->effects.size(); i++)
            {
                EffectsUnitView *ptr = new EffectsUnitView(preset->effects[i]);
                //ArrowView arrow;
                unitsView.push_back(ptr);
                //arrows.add(arrow);
                effectsLayout->add((Container *)ptr);
                //effectsLayout.add(arrows[i]);
            }
            effectsLayout->add(addEffectButton);
            update();
        }

        bool effect = false;
        for (int i = 0; i < unitsView.size(); i++)
        {
            if (unitsView[i] == ((EffectsUnitView *)effectsLayout->getHoveredItem()))
            {
                effect = true;
                EffectsUnitView *ptr = (EffectsUnitView *)effectsLayout->getHoveredItem();
                if (currentEffect != ptr)
                {
                    currentEffect = ptr;
                    controlsLayout->clear();
                    for (int i = 0; i < currentEffect->unit->controls.size(); i++)
                    {
                        auto ptr = currentEffect->unit->controls.at(i);
                        controlsLayout->add((Container *)ptr);
                    }
                    currentEffect->selectEvent = ([&](Container *c) { selectItem(controlsLayout->index); effectsLayout->onUnhover(); });
                    controlsLayout->exitEvent = [&](Container *c) {
                        controlsLayout->onUnhover();
                        effectsLayout->onUnhover();
                        selectedItemIndex = effectsLayout->index;
                        selectedItem = effectsLayout;
                        effectsLayout->onHover();
                        //onSelect();
                    };
                    //selectItem(controlsLayout.index);
                }
            }
        }

        if (!effect)
        {
            currentEffect = nullptr;
            controlsLayout->clear();
        }
        Activity::onDraw(gfx);
    }

    ~PresetActivity()
    {
        for (int i = 0; i < unitsView.size(); i++)
        {
            EffectsUnitView *view = unitsView[i];
            //unitsView.removeAt(i);
            delete view;
        }
    }
};