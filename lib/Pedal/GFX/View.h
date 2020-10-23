#pragma once

#include "Container.h"
#include "GFX.h"

class Layout;

class View : public Container
{
public:
    bool isComplexView = false, clickOnly = false;

    View(Layout *parent = nullptr, Size size = Size(0, 0), Position position = Position(0, 0));
    void draw(GFX *gfx);
    void calculate() {}
    virtual void onDraw(GFX *gfx) {}
    virtual void onHover();
    virtual void onUnhover();
    virtual void onSelect();

    bool isView() { return true; }
};

View::View(Layout *parent, Size size, Position position) : Container(parent, position, size, ContainerMode::NORMAL)
{
    this->parent = parent;
    if (parent != nullptr)
    {
        parent->add(this);
    }
}

void View::draw(GFX *gfx)
{
    /*if(isSelected)
    {
        gfx->drawRect(0, 0, size.width, size.height, COLOR_WHITE);
    }
    else if(drawBorder)
    {
        for(int i = 0; i < size.width; i++)
        {
            for(int j = 0; j < size.height; j++)
            {
                if(j == 0 || j == size.height-1)
                {
                    if(i % 2 == 0)
                    {
                        gfx->drawPixel(i, j, COLOR_WHITE);
                    }
                }
                else if(i == 0 || i == size.width-1)
                {
                    if(j % 2 == 0)
                    {
                        gfx->drawPixel(i, j, COLOR_WHITE);
                    }
                }
            }
        }
    }*/

    onDraw(gfx);
}

void View::onHover()
{
    isHovered = true;
    Container::onHover();
}

void View::onUnhover()
{
    isHovered = false;
    Container::onUnhover();
}

void View::onSelect()
{
    if (!isSelected)
    {
        isSelected = true;
        Container::onSelect();

        if (clickOnly)
        {
            if (parent != nullptr)
                parent->onUnselect();
        }
    }
    else if (!isComplexView)
    {
        if (parent != nullptr)
            parent->onUnselect();
    }
}