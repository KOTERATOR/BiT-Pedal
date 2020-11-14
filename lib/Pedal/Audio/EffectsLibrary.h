#pragma once

#include "EffectsUnit.h"
#include "Effects/Effects.h"
#include "Utils/Functional.h"
#include <vector>

//typedef EffectsUnit* (*EffectConstructor)();
typedef std::function<EffectsUnit *(void)> EffectConstructor;

class EffectsLibraryNode
{
public:
    String name;
    EffectConstructor constructor;
    EffectsLibraryNode(String name, EffectConstructor ctr)
    {
        this->name = name;
        this->constructor = ctr;
    }
    EffectsLibraryNode() {}
};

class EffectsLibrary
{
protected:
public:
    std::vector<EffectsLibraryNode> library;
    EffectsLibrary()
    {
        add("DELAY", []() { return new Delay(); });
        add("OVERDRIVE", []() { return new Overdrive(); });
        add("PITCH SHIFTER", []() { return new PitchShifter(); });
        add("SOUND SHIFTER", []() { return new SoundShifter(); });
        add("IR", []() { return new IRPlayer(); });
    }

    void add(String name, EffectConstructor ctr)
    {
        library.push_back(EffectsLibraryNode(name, ctr));
    }

    EffectsUnit *get(String name)
    {
        for (int i = 0; i < library.size(); i++)
        {
            if (name == library[i].name)
            {
                return library[i].constructor();
            }
        }
        return nullptr;
    }
} effectsLibrary;