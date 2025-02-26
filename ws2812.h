#ifndef WS2812_H
#define WS2812_H

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define NUM_PIXELS 25      // Matriz 5x5
#define LED_WHITE 0x010101 // Valor mínimo recomendado para ainda ser visível

static inline void ws2812_init(uint pin)
{
    // Configure o PIO para WS2812
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, pin, 800000, false);
}

void ws2812_show_number(int num)
{
    uint32_t pixels[NUM_PIXELS] = {0};

    switch (num)
    {
    case 0:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[5] = pixels[9] = LED_WHITE;
        pixels[10] = pixels[14] = LED_WHITE;
        pixels[15] = pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 1:

        pixels[2] = LED_WHITE;
        pixels[6] = pixels[7] = LED_WHITE;
        pixels[12] = LED_WHITE;
        pixels[17] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 2:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[9] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[15] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 3:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[9] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 4:

        pixels[1] = pixels[3] = LED_WHITE;
        pixels[6] = pixels[8] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[18] = LED_WHITE;
        pixels[23] = LED_WHITE;
        break;

    case 5:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[5] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 6:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[5] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[15] = pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 7:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[8] = LED_WHITE;
        pixels[13] = LED_WHITE;
        pixels[18] = LED_WHITE;
        pixels[23] = LED_WHITE;
        break;

    case 8:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[5] = pixels[9] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[15] = pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;

    case 9:

        pixels[1] = pixels[2] = pixels[3] = LED_WHITE;
        pixels[5] = pixels[9] = LED_WHITE;
        pixels[11] = pixels[12] = pixels[13] = LED_WHITE;
        pixels[19] = LED_WHITE;
        pixels[21] = pixels[22] = pixels[23] = LED_WHITE;
        break;
    }

    // Envie os dados para a matriz
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio0, 0, pixels[i] << 8u);
    }
}

#endif