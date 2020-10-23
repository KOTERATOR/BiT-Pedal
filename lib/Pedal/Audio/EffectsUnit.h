#pragma once

#include "EffectsUnitBase.h"
#include "Views/EffectsUnit/Controls/Options.h"

enum class FootswitchBinding
{
    NONE,
    TOGGLE,
    HOLD,
    UNHOLD
};

class EffectsUnit : public EffectsUnitBase
{
protected:
    size_t fwNum = 0;

public:
    Options<FootswitchBinding> fwBind = Options<FootswitchBinding>(this, "BIND",
                                                                   {OptionsNode<FootswitchBinding>("NONE", FootswitchBinding::NONE),
                                                                    OptionsNode<FootswitchBinding>("TOGGLE", FootswitchBinding::TOGGLE),
                                                                    OptionsNode<FootswitchBinding>("HOLD", FootswitchBinding::HOLD),
                                                                    OptionsNode<FootswitchBinding>("UNHOLD", FootswitchBinding::UNHOLD)});

    EffectsUnit(const String &name, Preset *preset = nullptr) : EffectsUnitBase(name, preset)
    {
    }

    void onFootswitchPress(size_t fwNum) override
    {
        FootswitchBinding *value_ptr = fwBind.getValue();
        if (fwBind.isSelected)
        {
            this->fwNum = fwNum;
            fwBind.label.text = "BINDED FW" + String(fwNum);
        }
        else if (this->fwNum == fwNum)
        {
            if (value_ptr != nullptr)
            {
                FootswitchBinding value = *value_ptr;

                if (value == FootswitchBinding::TOGGLE)
                {
                    setBypass(!getBypass());
                }
                else if (value == FootswitchBinding::HOLD)
                {
                    setBypass(false);
                }
                else if (value == FootswitchBinding::UNHOLD)
                {
                    setBypass(true);
                }
            }
        }
    }

    void onFootswitchRelease(size_t fwNum) override
    {
        if (this->fwNum == fwNum)
        {
            FootswitchBinding *value_ptr = fwBind.getValue();
            if (value_ptr != nullptr)
            {
                FootswitchBinding value = *value_ptr;

                if (value == FootswitchBinding::HOLD)
                {
                    setBypass(true);
                }
                else if (value == FootswitchBinding::UNHOLD)
                {
                    setBypass(false);
                }
            }
        }
    }
};