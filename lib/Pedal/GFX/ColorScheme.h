#pragma once

#include <stdint.h>

class ColorScheme
{
public:
             // DARKGREY
    uint16_t viewBgColor = fromRGB(0x7B, 0x7D, 0x7B),
             // WHITE
             viewFgColor = fromRGB(0xFF, 0xFF, 0xFF),
             // DARKBLUE
             viewHoverColor = fromRGB(0x00, 0x00, 0xFF),
             // DARKGREEN
             viewSelectColor = fromRGB(0x00, 0x7D, 0x00),
             // DARK DARK GREY
             layoutSelectColor = fromRGB(0x4f, 0x4f, 0x4f);

    static uint16_t fromRGB(uint8_t r, uint8_t g, uint8_t b);
};

uint16_t ColorScheme::fromRGB(uint8_t r, uint8_t g, uint8_t b)
{
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
}