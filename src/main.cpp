#include <Arduino.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i2s.h"
#include "bsp.h"
#include "fft.h"
#include "Pedal.h"
#include "syscalls.h"

#include "Activities/PresetActivity.h"

#include "Audio/Preset.h"

bool canCopy = false;
bool serialReady = false;
int32_t buf1[512], buf2[512], readCopy[512];
int32_t *proccessing = buf2, *reading = buf1;

uint64_t procTime = 0;

int displayTask(void *arg)
{
    Serial.println("HELLO FROM TASK");
    //lcd.println("BiT-Pedal");
    //lcd.fillRect(0, 0, 100, 100, COLOR_BLUE);
    pedal.init();

    Preset *preset = new Preset("Hello World!");
    pedal.preset = preset;
    pedal.screen.setActivity(new PresetActivity(preset));

    unsigned long prevUpdateTime = millis(), nowMillis = 0;
    while (1)
    {
        nowMillis = millis();
        if (nowMillis - prevUpdateTime > 50)
        {
            prevUpdateTime = nowMillis;
            pedal.screen.update();
            pedal.screen.draw();
            //preset->update();
            //Serial.println(get_free_heap_size());
            //Serial.println(procTime);
            //Serial.println(preset->effects.size());
        }
        pedal.tick();
        //effect->setBypass(!fw1.isDown());
    }
}

void io_mux_init(void)
{
    // Kendryte K210 Dock:
    fpioa_set_function(35, FUNC_I2S1_WS);
    fpioa_set_function(34, FUNC_I2S1_OUT_D1);
    fpioa_set_function(33, FUNC_I2S1_SCLK);

    //fpioa_set_function(10, FUNC_I2S0_)
    fpioa_set_function(9, FUNC_I2S0_IN_D0);
    fpioa_set_function(15, FUNC_I2S0_WS);
    fpioa_set_function(10, FUNC_I2S0_SCLK);
    fpioa_set_function(17, FUNC_I2S0_MCLK);

    fpioa_set_function(7, FUNC_GPIOHS7);
    fpioa_set_function(8, FUNC_GPIOHS8);
    gpiohs_set_drive_mode(7, gpio_drive_mode_t::GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(8, gpio_drive_mode_t::GPIO_DM_OUTPUT);
    gpiohs_set_pin(7, gpio_pin_value_t::GPIO_PV_HIGH);
    gpiohs_set_pin(8, gpio_pin_value_t::GPIO_PV_HIGH);
}

void setup()
{
    Serial.begin(115200);
    //file.init("6.wav");
    //lcd.begin(15000000, COLOR_RED);
    //lcd.invertDisplay(true); // comment this out when camera is on the backside
    //lcd.fillScreen(COLOR_BLACK);
    Serial.println(current_coreid());

    pll_init();

    io_mux_init();
    plic_init();
    sysctl_enable_irq();

    register_core1(displayTask, NULL);

    i2s_init(I2S_DEVICE_0, I2S_RECEIVER, 0x3 << (I2S_CHANNEL_0 * 2));
    i2s_init(I2S_DEVICE_1, I2S_TRANSMITTER, 0x3 << (I2S_CHANNEL_1 * 2));

    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, LEFT_JUSTIFYING_MODE);
    i2s_tx_channel_config(I2S_DEVICE_1, I2S_CHANNEL_1, RESOLUTION_16_BIT, SCLK_CYCLES_16, TRIGGER_LEVEL_4, STANDARD_MODE);

    i2s_set_sample_rate(I2S_DEVICE_0, 44100);
    i2s_set_sample_rate(I2S_DEVICE_1, 44100);
}

float f_buf[256] = {}, f_out[256] = {};

int16_t l_buf[256] = {}, r_buf[256] = {};

void loop()
{
    //Serial.println("LOOP");
    //Serial.println(current_coreid());
    i2s_receive_data_dma(I2S_DEVICE_0, (uint32_t *)reading, 512, DMAC_CHANNEL1);

    /*if (canCopy)
    {
        memcpy(readCopy, proccessing, sizeof(readCopy));
        canCopy = false;
    }*/
    /*int index = 0;
    for (int i = 0; i < 512; i+=2)
    {
        f_buf[index++] = reading[i];
    }

    index = 0;
    for (int i = 0; i < 512; i+=2)
    {
        proccessing[i] = f_out[index++];
    }*
    /*for (int i = 1; i < 512; i += 2)
    {
        sDelayBuffer0[DelayCounter] = proccessing[i - 1] + sDelayBuffer0[DelayCounter]/4;
        sDelayBuffer1[DelayCounter] = proccessing[i] + sDelayBuffer1[DelayCounter]/4;

        //Adjust Delay Depth based in pot2 position.

        //Increse/reset delay counter.
        DelayCounter++;
        if (DelayCounter >= Delay_Depth)
            DelayCounter = 0;

        proccessing[i - 1] = ((sDelayBuffer0[DelayCounter]) + proccessing[i - 1]) / 2;
        proccessing[i] = ((sDelayBuffer1[DelayCounter]) + proccessing[i]) / 2;
    }*/

    //while(!transferComplete);
    /*i2s_play(I2S_DEVICE_2,
                 DMAC_CHANNEL0, (uint8_t *)(buffer_pp), BUFFER_LENGTH * 4, 512, 16, 2);*/
    /*if (!bypass)
    {
        float preGain = 0.002f, postGain = 1000.f;
        for (int i = 0; i < 512; i++)
        {
            int16_t val = proccessing[i];
            val = atan(val * preGain) * postGain;
            proccessing[i] = val;
        }
    }*/
    auto prev = micros();

    for (int i = 0; i < 256; i++)
    {
        r_buf[i] = proccessing[i * 2 + 1];
        l_buf[i] = proccessing[i * 2];
    }

    pedal.proceedInputL(l_buf, 256);

    for (int i = 0; i < 256; i++)
    {
        proccessing[i * 2 + 1] = l_buf[i];
        proccessing[i * 2] = r_buf[i];
    }

    procTime = micros() - prev;
    if (procTime < 5000)
    {
        dmac_wait_idle(DMAC_CHANNEL0);
        i2s_send_data_dma(I2S_DEVICE_1, (uint32_t *)proccessing, 512, DMAC_CHANNEL0);
    }

    int32_t *temp = proccessing;
    proccessing = reading;
    reading = temp;

    pedal.screen.processingTime = procTime;

    dmac_wait_idle(DMAC_CHANNEL1);
}