#pragma once

#include "Dimensions.h"
#include "Container.h"
#include "GFX.h"
#include "Animator.h"
#include <vector>

enum class LayoutMode
{
    HORIZONTAL,
    VERTICAL,
    ABSOLUTE
};

class Layout : public Container
{
protected:
    Position defaultPosition;
    LayoutMode mode;
    size_t selectedItemIndex = 0, nextItemIndex = 0;
    Container *selectedItem = nullptr;
    Container *prevSelectedItem = nullptr;
    int16_t spacing = 0;
    Size childrenBounds, prevChildrenBounds;

    Position scrollPosition = Position(0, 0), fromScrollPosition = Position(0, 0);

    Animation xScrollAnimation = Animation(),
              yScrollAnimation = Animation();

public:
    std::vector<Container *> children;

    Layout(Layout *parent, LayoutMode layoutMode, Position position, Size size, ContainerMode containerMode);

    void add(Container *container);

    void animation();
    void animationScrollX(int scrollX, long ms);
    void animationScrollY(int scrollY, long ms);

    void clear();
    void update();
    void draw(GFX *gfx);
    void onDraw(GFX *gfx);
    Size getChildrenBounds();

    Container *getHoveredItem()
    {
        if (selectedItemIndex < children.size())
        {
            return children[selectedItemIndex];
        }
        else
        {
            return nullptr;
        }
    }
    Container *getSelectedItem() { return selectedItem; }

    void setSpacing(int16_t spacing) { this->spacing = spacing; }

    virtual void onNext();
    virtual void onPrev();
    virtual void onSelect();
    virtual bool onUnselect();
    virtual void onHover();
    virtual void onUnhover();

    void selectItem(size_t index);
    void centerSelectedItem();

    virtual ~Layout()
    {
    }
};

Layout::Layout(Layout *parent, LayoutMode layoutMode, Position position = Position(0, 0), Size size = Size(0, 0), ContainerMode containerMode = ContainerMode::NORMAL) : Container(parent, position, size, containerMode), defaultPosition(position)
{
    this->parent = parent;
    if (parent != nullptr)
    {
        parent->add(this);
    }
    this->mode = layoutMode;

    //animator.add(&xScrollAnimation);
    //animator.add(&yScrollAnimation);
}

void Layout::update()
{
    animation();

    int w = scrollPosition.x, h = scrollPosition.y;
    int maxW = 0, maxH = 0;
    for (int i = 0; i < children.size(); i++)
    {
        Container *c = children[i];
        c->index = i;

        if (isSelected)
        {
            if (i != selectedItemIndex && !c->isHovered)
            {
                c->isHovered = false;
            }
            else if (i == selectedItemIndex && c->isHovered)
            {
                c->isHovered = true;
            }
        }

        c->update();

        Size s = c->getSize();
        int cH = s.height, cW = s.width;

        ContainerMode childMode = c->getContainerMode();
        if (childMode != ContainerMode::ABSOLUTE)
        {
            if (mode == LayoutMode::HORIZONTAL)
            {
                c->setX(w);
                w += s.width + spacing;
                h = s.height;
            }
            else if (mode == LayoutMode::VERTICAL)
            {
                c->setY(h);
                h += s.height + spacing;
                w = s.width;
            }
            if (maxH < h)
            {
                maxH = h;
            }
            if (maxW < w)
            {
                maxW = w;
            }
        }

        if (childMode == ContainerMode::CENTER)
        {
            int x = (size.width - cW) / 2,
                y = (size.height - cH) / 2;
            Position childPos = c->getPosition();
            if (mode == LayoutMode::HORIZONTAL)
            {
                x = childPos.x;
            }
            else if (mode == LayoutMode::VERTICAL)
            {
                y = childPos.y;
            }

            c->setPosition(Position(x, y));
        }
    }
    childrenBounds.width = maxW - scrollPosition.x;
    childrenBounds.height = maxH - scrollPosition.y;

    if (selectedItem != nullptr && childrenBounds != prevChildrenBounds)
    {

        prevChildrenBounds = childrenBounds;
    }
    centerSelectedItem();

    updateEvent.invoke(this);
}

void Layout::add(Container *container)
{
    container->setParent(this);
    children.push_back(container);
}

void Layout::clear()
{
    selectedItem = nullptr;
    selectedItemIndex = 0;
    children.clear();
}

Size Layout::getChildrenBounds()
{
    return childrenBounds;
}

void Layout::draw(GFX *gfx)
{
    if (isHovered)
    {
        gfx->setCurrentContainer(this);
        gfx->fillRect(0, 0, size.width, size.height, gfx->pallete.layoutSelectColor);
    }

    for (int i = 0; i < children.size(); i++)
    {
        if (i != selectedItemIndex)
        {
            Size childSize = children[i]->getSize();
            Position childPos = children[i]->getPosition();
            if (childPos.x + childSize.width < 0 || childPos.y + childSize.height < 0 || childPos.x > size.width || childPos.y > size.height)
                continue;

            gfx->setCurrentContainer(children[i]);
            children[i]->draw(gfx);
        }
    }
    if (selectedItemIndex < children.size())
    {
        gfx->setCurrentContainer(children[selectedItemIndex]);
        children[selectedItemIndex]->draw(gfx);
    }
    this->onDraw(gfx);
}

void Layout::onDraw(GFX *gfx)
{
    gfx->setCurrentContainer(this);
    // scroll bar
    Size childrenSize = getChildrenBounds();
    if (mode == LayoutMode::VERTICAL)
    {
        if (childrenSize.height > size.height)
        {
            int scroll = -round((scrollPosition.y * size.height) / (double)childrenSize.height);
            int scrollHeight = round((size.height * size.height) / (double)childrenSize.height);

            gfx->fillRoundRect(size.width - 10, 0, 10, size.height, 5, gfx->pallete.viewBgColor);
            gfx->fillRoundRect(size.width - 10, scroll, 10, scrollHeight, 5, gfx->pallete.viewFgColor);
        }
    }
    else if (mode == LayoutMode::HORIZONTAL)
    {
        if (childrenSize.width > size.width)
        {
            int scroll = -round((scrollPosition.x * size.width) / (double)childrenSize.width);
            int scrollWidth = round((size.width * size.width) / (double)childrenSize.width);

            gfx->fillRoundRect(0, size.height - 10, size.width, 10, 5, gfx->pallete.viewBgColor);
            gfx->fillRoundRect(scroll, size.height - 10, scrollWidth, 10, 5, gfx->pallete.viewFgColor);
        }
    }
}

void Layout::onSelect()
{
    if (selectedItem != nullptr)
    {
        selectedItem->onSelect();
    }
    else
    {
        if (!isSelected)
        {
            if (children.size() > 0)
            {
                bool isHoveredAny = false;
                for (int i = 0; i < children.size(); i++)
                {
                    if (children[i]->isHovered)
                    {
                        isHoveredAny = true;
                        break;
                    }
                }

                if (!isHoveredAny)
                {
                    selectedItemIndex = 0;
                    children[0]->onHover();
                }
                //centerSelectedItem();
                if (children.size() == 1)
                {
                    selectedItem = children[0];
                    selectedItem->enterEvent(selectedItem);
                    selectedItem->onSelect();
                }
            }
            isSelected = true;

            selectEvent.invoke(this);
        }
        else
        {
            if (selectedItemIndex < children.size())
            {
                selectedItem = children[selectedItemIndex];
                selectedItem->enterEvent(selectedItem);
                selectedItem->onSelect();
            }
        }
    }
}

bool Layout::onUnselect()
{
    if (selectedItem != nullptr)
    {
        if (selectedItem->onUnselect())
        {
            //selectedItem->onUnhover();
            Container *selectedItemPtr = selectedItem;
            selectedItem = nullptr;
            selectedItemPtr->exitEvent.invoke(selectedItemPtr);
            if (children.size() == 1)
            {
                return this->onUnselect();
            }
        }
        return false;
    }
    else
    {
        children[selectedItemIndex]->onUnhover();
        selectedItemIndex = 0;
        nextItemIndex = 0;
        //centerSelectedItem();
        selectedItem = nullptr;
        isSelected = false;
        //setAbsolutePosition(defaultPosition);
        unselectEvent.invoke(this);
        return true;
    }
}

void Layout::onNext()
{
    if (selectedItem != nullptr)
    {
        selectedItem->onNext();
    }
    else
    {

        if (selectedItemIndex < children.size() - 1 && nextItemIndex < children.size() - 1)
        {
            if (nextItemIndex < selectedItemIndex)
                nextItemIndex = selectedItemIndex;
            nextItemIndex += 1;
            if (children[nextItemIndex]->isSelectable)
            {
                children[selectedItemIndex]->onUnhover();
                selectedItemIndex = nextItemIndex;
                children[selectedItemIndex]->onHover();
                //centerSelectedItem();
            }
            else
            {

                onNext();
            }
        }
        nextEvent.invoke(this);
    }
}

void Layout::onPrev()
{
    if (selectedItem != nullptr)
    {
        selectedItem->onPrev();
    }
    else
    {

        if (selectedItemIndex > 0 && nextItemIndex > 0)
        {
            if (nextItemIndex > selectedItemIndex)
                nextItemIndex = selectedItemIndex;
            nextItemIndex -= 1;
            if (children[nextItemIndex]->isSelectable)
            {
                children[selectedItemIndex]->onUnhover();
                selectedItemIndex = nextItemIndex;
                children[selectedItemIndex]->onHover();
                //centerSelectedItem();
            }
            else
            {
                onPrev();
            }
        }
        prevEvent.invoke(this);
    }
}

void Layout::onHover()
{
    if (selectedItem != nullptr)
    {
        selectedItem->onHover();
    }
    else
    {
        isHovered = true;
        hoverEvent.invoke(this);
    }
}

void Layout::onUnhover()
{
    if (selectedItem != nullptr)
    {
        selectedItem->onUnhover();
    }
    else
    {
        isHovered = false;
        unhoverEvent.invoke(this);
    }
}

void Layout::selectItem(size_t index)
{
    if (children.size() > index)
    {
        //children[selectedItemIndex]->onUnselect();
        if (selectedItem != nullptr)
        {
            selectedItem->isHovered = false;
            selectedItem->isSelected = false;
        }
        else if (selectedItemIndex < children.size())
        {
            children[selectedItemIndex]->isHovered = false;
        }
        selectedItemIndex = index;
        selectedItem = children[selectedItemIndex];
        selectedItem->isHovered = true;
        selectedItem->isSelected = false;
        selectedItem->onSelect();
    }
}

void Layout::centerSelectedItem()
{
    if (selectedItemIndex < children.size())
    {
        Container *child = children[selectedItemIndex];
        Container *absoluteParent = getAbsoluteParent();
        Position scroll = scrollPosition;
        if (mode == LayoutMode::HORIZONTAL)
        {
            //setX(64 - children[selectedItemIndex]->getPosition().x - children[selectedItemIndex]->getSize().width / 2);
            if (child->getPosition().x + child->getSize().width > size.width)
            {
                scroll.x -= child->getPosition().x + child->getSize().width - size.width;
            }
            else if (child->getPosition().x < 0)
            {
                scroll.x -= child->getPosition().x;
            }
        }
        else if (mode == LayoutMode::VERTICAL)
        {
            //setY(32 - children[selectedItemIndex]->getPosition().y - children[selectedItemIndex]->getSize().height / 2);
            if (child->getPosition().y + child->getSize().height > size.height)
            {
                scroll.y -= child->getPosition().y + child->getSize().height - size.height;
            }
            else if (child->getPosition().y < 0)
            {
                scroll.y -= child->getPosition().y;
            }
        }
        scrollPosition = scroll;
    }
}

void Layout::animation()
{
    Container::animation();

    xScrollAnimation.tick();
    yScrollAnimation.tick();

    if (!xScrollAnimation.isFinished())
        scrollPosition.x = xScrollAnimation.getValue();
    if (!yScrollAnimation.isFinished())
        scrollPosition.y = yScrollAnimation.getValue();
}

void Layout::animationScrollX(int scrollX, long ms)
{
    if (xScrollAnimation.isFinished())
    {
        xScrollAnimation.setup(scrollPosition.x, scrollX, abs(ms));
        xScrollAnimation.start();
    }
    else
    {
        xScrollAnimation.changeTo(scrollX, ms);
    }
}

void Layout::animationScrollY(int scrollY, long ms)
{
    if (yScrollAnimation.isFinished())
    {
        yScrollAnimation.setup(scrollPosition.y, scrollY, abs(ms));
        yScrollAnimation.start();
    }
    else
    {
        yScrollAnimation.changeTo(scrollY, ms);
    }
}