#pragma once

#include "Dimensions.h"
#include "Animator.h"
#include "Utils/Event.h"
#include "Utils/Functional.h"

class GFX;

typedef std::function<void(void)> ContainerEvent;

enum class ContainerMode
{
    NORMAL,
    ABSOLUTE,
    CENTER
};

class Container
{
protected:
    Container *parent;

    ContainerMode containerMode;

    Position position;
    Size size;

    Animation resizeWidthAnimation = Animation(),
              resizeHeightAnimation = Animation();

public:
    int alphaContourSize = 5;
    bool alphaContour = false;
    bool isSelectable = true, isSelected = false, isHovered = false;
    size_t index;

    Container(Container *parent = nullptr, Position position = Position(0, 0), Size size = Size(0, 0), ContainerMode containerMode = ContainerMode::NORMAL);
    Event<Container *> selectEvent, unselectEvent, nextEvent, prevEvent, hoverEvent, unhoverEvent, updateEvent, exitEvent, enterEvent;

    Position getPosition();
    Position getAbsolutePosition();
    void setPosition(Position position);
    void setX(int x) { position.x = x; }
    void setY(int y) { position.y = y; }
    void setAbsolutePosition(Position position);

    ContainerMode getContainerMode() { return containerMode; }

    Size getSize();
    Size &getReferenceSize();
    virtual void setSize(Size size);
    void setWidth(int width) { setSize(Size(width, size.height)); }
    void setHeight(int height) { setSize(Size(size.width, height)); }

    virtual void animation();
    virtual void animationResize(Size size, unsigned long ms);

    Container *getParent();
    Container *getAbsoluteParent();
    void setParent(Container *parent);
    virtual void update()
    {
        animation();
        updateEvent(this);
    }
    virtual void draw(GFX *gfx);

    virtual void onNext()
    {
        nextEvent(this);
    }
    virtual void onPrev()
    {
        prevEvent(this);
    }
    virtual void onSelect()
    {
        if (isSelectable)
        {
            isSelected = true;
        }
        selectEvent(this);
    }
    virtual bool onUnselect()
    {
        if (isSelectable)
        {
            isSelected = false;
        }
        unselectEvent(this);
        return true;
    }
    virtual void onHover()
    {
        if (isSelectable)
        {
            isHovered = true;
        }
        hoverEvent(this);
    }
    virtual void onUnhover()
    {
        if (isSelectable)
        {
            isHovered = false;
        }
        unhoverEvent(this);
    }

    virtual void onDraw(GFX *gfx) {}

    virtual ~Container()
    {
    }
};

Container::Container(Container *parent, Position position, Size size, ContainerMode containerMode)
{
    this->parent = parent;
    this->position = position;
    this->size = size;
    this->containerMode = containerMode;
}

inline void Container::draw(GFX *gfx)
{
    onDraw(gfx);
    //gfx.draw();
}

inline Position Container::getPosition()
{
    return position;
}

Position Container::getAbsolutePosition()
{
    Position pos(0, 0);
    if (parent != nullptr)
    {
        pos = parent->getAbsolutePosition();
    }
    pos = pos + this->position;
    return pos;
}

inline void Container::setPosition(Position position)
{
    this->position = position;
}

inline void Container::setAbsolutePosition(Position position)
{
    Position absolute = getAbsolutePosition();
    this->position = position - absolute;
}

inline Size Container::getSize()
{
    return size;
}

Size &Container::getReferenceSize()
{
    return size;
}

inline void Container::setSize(Size size)
{
    this->size = size;
}

Container *Container::getParent()
{
    return parent;
}

Container *Container::getAbsoluteParent()
{
    if (parent != nullptr)
    {
        return parent->getAbsoluteParent();
    }
    else
    {
        return this;
    }
}

inline void Container::setParent(Container *parent)
{
    this->parent = parent;
}

void Container::animation()
{
    resizeWidthAnimation.tick();
    resizeHeightAnimation.tick();

    if (!resizeWidthAnimation.isFinished())
        setWidth(resizeWidthAnimation.getValue());
    if (!resizeHeightAnimation.isFinished())
        setHeight(resizeHeightAnimation.getValue());
}

void Container::animationResize(Size size, unsigned long ms)
{
    resizeWidthAnimation.setup(this->size.width, size.width, ms);
    resizeHeightAnimation.setup(this->size.height, size.height, ms);
    resizeWidthAnimation.start();
    resizeHeightAnimation.start();
}
