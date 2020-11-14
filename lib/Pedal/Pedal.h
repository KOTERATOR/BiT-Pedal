#pragma once

#include <SPI.h>
#include <Sipeed_ST7789.h>
#include <SD.h>

#include "GFX/Screen.h"
#include "Hardware/Encoder.h"

#include "Audio/Preset.h"

#include "timer.h"

int enc_timer_irq(void *ctx);
void onNext(), onPrev(), onSelect(), onUnselect();

class Pedal
{
protected:
    SPIClass lcd_spi = SPIClass(SPI0);
    Sipeed_ST7789 lcd = Sipeed_ST7789(320, 240, lcd_spi);

public:
    Encoder enc = Encoder(31, 32, 30);
    Button fw1 = Button(23, INPUT_PULLUP),
           fw2 = Button(22, INPUT_PULLUP),
           fw3 = Button(24, INPUT_PULLUP);
    Screen screen = Screen();

    Preset *preset = nullptr;

    Pedal()
    {
    }

    void footswitch(size_t num, bool press)
    {
        if (preset != nullptr)
        {
            if (press)
                preset->onFootswitchPress(num);
            else
                preset->onFootswitchRelease(num);
        }
    }

    void init()
    {
        lcd.begin(20000000U, 0x0000);
        lcd.setRotation(2);
        lcd.fillScreen(COLOR_BLACK);

        screen.setDisplay(&lcd);

        enc.onCW = ([]() {
            onNext();
        });
        enc.onCCW = ([]() {
            onPrev();
        });
        enc.button.onOnceClick = ([]() {
            onSelect();
        });
        enc.button.onHold = ([]() {
            onUnselect();
        });

        fw1.onPress = [&]() {
            footswitch(1, true);
        };
        fw1.onRelease = [&]() {
            footswitch(1, false);
        };

        fw2.onPress = [&]() {
            footswitch(2, true);
        };
        fw2.onRelease = [&]() {
            footswitch(2, false);
        };

        fw3.onPress = [&]() {
            footswitch(3, true);
        };
        fw3.onRelease = [&]() {
            footswitch(3, false);
        };

        timer_init(timer_device_number_t::TIMER_DEVICE_0);
        timer_set_interval(timer_device_number_t::TIMER_DEVICE_0, timer_channel_number_t::TIMER_CHANNEL_0, 3000000);
        timer_irq_register(timer_device_number_t::TIMER_DEVICE_0, timer_channel_number_t::TIMER_CHANNEL_0, 0, 1, enc_timer_irq, nullptr);
        timer_set_enable(timer_device_number_t::TIMER_DEVICE_0, timer_channel_number_t::TIMER_CHANNEL_0, 1);

        if (SD.begin())
        {
            Serial.println("SD INIT SUCCESS");
        }
        else
        {
            Serial.println("SD INIT FAILED");
        }
    }

    void proceedInputL(int16_t *buf, size_t length)
    {
        if (preset != nullptr)
        {
            preset->proceed(buf, length);
        }
    }

    void tick()
    {
        enc.tick();
        fw1.tick();
        fw2.tick();
        fw3.tick();
    }

    void update()
    {
        if (preset != nullptr)
        {
            preset->update();
        }
    }
} pedal;

void onNext()
{
    pedal.screen.onNext();
}

void onPrev()
{
    pedal.screen.onPrev();
}

void onSelect()
{
    pedal.screen.onSelect();
}

void onUnselect()
{
    pedal.screen.onUnselect();
}

int enc_timer_irq(void *ctx)
{
    pedal.enc.service();
    return 0;
}