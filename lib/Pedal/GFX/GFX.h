#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "Sipeed_ST7789.h"
#include "Dimensions.h"
#include "Container.h"
#include "ColorScheme.h"
#include "Fonts/FreeSansBold24pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"

enum class HTextAlignment
{
    Left,
    Right,
    Center
};

enum class VTextAlignment
{
    Top,
    Bottom,
    Center
};

class GFX : public GFXcanvas16
{
protected:
    Position offset, containerOffset;
    Size containerSize;
    uint16_t *buffer;

    bool overlap = false;

public:
    Container *currentContainer;
    ColorScheme pallete;

    GFX(uint16_t w, uint16_t h) : GFXcanvas16(w, h)
    {
        //setFont(&FreeSansBold24pt7b);
        buffer = getBuffer();
    }

    ~GFX()
    {
        //GFXcanvas16::~GFXcanvas16(0);
    }

    void setOffset(Position offset) { this->offset = offset; }

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawText(int16_t x, int16_t y, String &str, int16_t color = 0xffff, const GFXfont *font = nullptr, uint8_t textSize = 1);
    void drawTextWithInvertedChar(int16_t x, int16_t y, String &str, int16_t color, int8_t textSize, size_t invertCharIndex, int16_t invertColor = 0x0000);
    void drawAlignedText(HTextAlignment hmode, VTextAlignment vmode, String &str, int16_t color, int8_t textSize = 1);
    Size getTextBounds(String &str, uint8_t textSize = 1);

    void setCurrentContainer(Container *c)
    {
        currentContainer = c;
        if (currentContainer != nullptr)
        {
            containerOffset = currentContainer->getAbsolutePosition();
            containerSize = currentContainer->getSize();
        }
        overlap = false;
    }

    void setOverlap(bool overlap) { this->overlap = overlap; }
    bool getOverlap() { return overlap; }
};

void GFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (currentContainer != nullptr)
    {
        Container *parent = currentContainer->getParent();
        Position position = currentContainer->getPosition();
        Size parentSize = parent->getSize();
        Position parentRelativePosition = Position(position.x + x, position.y + y);

        if (!overlap && (x < 0 || y < 0 ||
            x >= currentContainer->getSize().width || y >= currentContainer->getSize().height ||
            parentRelativePosition.x < 0 || parentRelativePosition.y < 0 ||
            parentRelativePosition.x >= parentSize.width || parentRelativePosition.y >= parentSize.height))
            return;

        x += containerOffset.x;
        y += containerOffset.y;
    }

    if (buffer)
    {
        if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
            return;

        int16_t t;
        switch (rotation)
        {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
        }

        buffer[x + y * WIDTH] = ((color >> 8 & 0xff) | (color << 8));
    }
}

void GFX::drawText(int16_t x, int16_t y, String &str, int16_t color, const GFXfont *font, uint8_t textSize)
{
    if (font != nullptr)
    {
        setFont(font);
    }
    setCursor(x, y);
    setTextSize(textSize);
    setTextColor(color);
    /*for (int i = 0; i < str.length(); i++)
        {
            write(str[i]);
        }*/
    print(str.c_str());
}

void GFX::drawTextWithInvertedChar(int16_t x, int16_t y, String &str, int16_t color, int8_t textSize, size_t invertCharIndex, int16_t invertColor)
{
    setCursor(x, y);
    setTextSize(textSize);
    setTextColor(color);
    uint16_t invertbgcolor = textcolor, invertcolor = invertColor;

    for (int i = 0; i < str.length(); i++)
    {
        if (i == invertCharIndex)
        {
            textcolor = invertColor;
            textbgcolor = invertbgcolor;
            write(str[i]);
            setTextColor(color);
        }
        else
        {
            write(str[i]);
        }
    }
}

void GFX::drawAlignedText(HTextAlignment hmode, VTextAlignment vmode, String &str, int16_t color, int8_t textSize)
{
    int16_t x1 = 0, y1 = 0;
    Size bounds = getTextBounds(str, textSize);
    Size size = Size(width(), height());
    if (currentContainer != nullptr)
    {
        size = currentContainer->getSize();
    }
    if (hmode == HTextAlignment::Left)
    {
        x1 = 0;
    }
    else if (hmode == HTextAlignment::Right)
    {
        x1 = size.width - bounds.width;
    }
    else if (hmode == HTextAlignment::Center)
    {
        x1 = (size.width - bounds.width) / 2;
    }

    if (vmode == VTextAlignment::Top)
    {
        y1 = 0;
    }
    else if (vmode == VTextAlignment::Bottom)
    {
        y1 = size.height - bounds.height;
    }
    else if (vmode == VTextAlignment::Center)
    {
        y1 = (size.height - bounds.height) / 2;
    }
    drawText(x1, y1, str, color, &FreeSansBold12pt7b, textSize);
}

Size GFX::getTextBounds(String &str, uint8_t textSize)
{
    int16_t x1, y1;
    uint16_t w, h;
    int len = str.length() + 1;
    char *text = new char[len];
    str.toCharArray(text, len);
    Adafruit_GFX::setTextSize(textSize);
    Adafruit_GFX::getTextBounds((char *)text, 0, 0, &x1, &y1, &w, &h);
    delete[] text;
    return Size(w, h);
}