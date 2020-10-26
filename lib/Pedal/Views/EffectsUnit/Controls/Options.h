#pragma once

#include "ControlView.h"
#include <vector>

template <typename T>
struct OptionsNode
{
    String label;
    T value;

    OptionsNode(const String &label, const T &value) : label(label), value(value)
    {
    }
};

template <typename T>
class Options : public ControlView
{
protected:
    size_t currentOptionIndex = 0;
    TextComponent optionText;

public:
    std::vector<OptionsNode<T>> options;
    Options(EffectsUnitBase *unit, const String &labelText, std::vector<OptionsNode<T>> options) : ControlView(unit, labelText), optionText(this, "", &FreeSansBold12pt7b, Size(5, 5), 5), options(options)
    {
        optionText.autoScroll = true;
        optionText.expandOnHover = true;
    }

    OptionsNode<T> *getCurrentOption()
    {
        if (currentOptionIndex < options.size())
        {
            return &options[currentOptionIndex];
        }
        return nullptr;
    }

    T *getValue()
    {
        OptionsNode<T> *ptr = getCurrentOption();
        if (ptr != nullptr)
        {
            return &ptr->value;
        }
        return nullptr;
    }

    void onDraw(GFX *gfx)
    {
        OptionsNode<T> *ptr = getCurrentOption();
        if (ptr != nullptr)
        {
            Size safeArea = Size(size.width, size.height - label.getBounds(gfx).height);

            optionText.text = ptr->label;
            optionText.textColor = gfx->pallete.viewFgColor;
            optionText.backgroundColor = isSelected ? gfx->pallete.viewSelectColor : gfx->pallete.viewHoverColor;
            optionText.drawBackground = isHovered;

            Size optionTextBounds = optionText.getBounds(gfx);
            optionText.position.y = (safeArea.height - optionTextBounds.height) / 2;

            /*if (isHovered)
            {
                if (currentOptionIndex > 0)
                {
                    optionText.text = "< " + optionText.text;
                }

                if (currentOptionIndex < options.size() - 1)
                {
                    optionText.text = optionText.text + " >";
                }
            }*/
        }
        else
        {
            optionText.text = "NONE";
        }
        optionText.draw(gfx);
    }

    void onNext()
    {
        if (currentOptionIndex < options.size() - 1)
        {
            currentOptionIndex++;
        }
    }

    void onPrev()
    {
        if (currentOptionIndex > 0)
        {
            currentOptionIndex--;
        }
    }
};