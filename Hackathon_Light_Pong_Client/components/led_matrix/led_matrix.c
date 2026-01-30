/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "led_matrix.h"

#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)
#define LED_GPIO GPIO_NUM_8
#define MAX_LEDS 25

const uint8_t number_map_hex[10][5] = {
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x02, 0x04, 0x1F}, // 2
    {0x1F, 0x02, 0x04, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x1F, 0x02}, // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x1E}, // 5
    {0x06, 0x08, 0x1E, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08}, // 7
    {0x0E, 0x11, 0x0E, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x0F, 0x01, 0x0E}  // 9
};

// void display_number_with_cooldown(led_strip_handle_t strip, int num, TickType_t last_tick,
//                                   TickType_t cooldown_ticks) {
//     TickType_t now = xTaskGetTickCount();
//     TickType_t elapsed = now - last_tick;
//     if (elapsed > cooldown_ticks)
//         elapsed = cooldown_ticks;

//     float progress = (float)elapsed / cooldown_ticks; // 0 = just shot, 1 = ready
//     bool ready = (progress >= 1.0f);

//     // Small pulsing effect when ready
//     int value = random() % 10; // number between 0 and 9

//     uint8_t pulse = ready ? (5 + (value % 10)) : 0;

//     for (int row = 0; row < 5; row++) {
//         for (int col = 0; col < 5; col++) {
//             int idx = row * 5 + col;

//             bool num_on = number_map_hex[num][row] & (1 << (4 - col));
//             uint8_t r = 0, g = 0, b = 0;

//             float row_progress = (float)(5 - row) / 5.0f; // bottom-up fill

//             if (num_on) {
//                 // Number pixel: bright white, pulsing if ready
//                 r = g = b = 20 + pulse;
//             } else {
//                 if (progress >= row_progress) {
//                     // Filled cooldown bar: green
//                     g = 20;
//                 } else {
//                     // Empty cooldown bar: dark
//                     r = g = b = 0;
//                 }
//             }

//             led_strip_set_pixel(strip, idx, r, g, b);
//         }
//     }

//     led_strip_refresh(strip);
// }

// VERSION 2
void display_number_with_cooldown(led_strip_handle_t strip, int num, TickType_t last_tick,
                                  TickType_t cooldown_ticks) {
    TickType_t now = xTaskGetTickCount();
    TickType_t elapsed = now - last_tick;
    if (elapsed > cooldown_ticks)
        elapsed = cooldown_ticks;
    float progress = (float)elapsed / cooldown_ticks;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            int idx = row * 5 + col;

            // Get number pixel
            bool num_on = number_map_hex[num][row] & (1 << (4 - col));

            uint8_t r = 0, g = 0, b = 0;

            // Flip the row index so progress fills from bottom up
            float row_progress = (float)(5 - row) / 5.0f; // <-- flipped

            // Cooldown color
            if (progress >= row_progress) {
                g = 20; // fully green
            } else {
                r = (uint8_t)(20 * (1.0f - progress / row_progress));
                g = (uint8_t)(20 * (progress / row_progress));
            }

            // If number pixel is on, override to bright white
            if (num_on) {
                r = 0;
                g = 0;
                b = 20;
            }

            led_strip_set_pixel(strip, idx, r, g, b);
        }
    }
    led_strip_refresh(strip);
}

// // VERSION 1
// void display_cooldown(led_strip_handle_t strip, TickType_t last_tick, TickType_t cooldown_ticks)
// {
//     TickType_t now = xTaskGetTickCount();
//     TickType_t elapsed = now - last_tick;

//     // Clamp elapsed
//     if (elapsed > cooldown_ticks)
//         elapsed = cooldown_ticks;

//     // Calculate progress 0..1
//     float progress = (float)elapsed / cooldown_ticks;

//     // We'll fill the matrix from bottom to top
//     // Assume a 5x5 matrix
//     for (int row = 0; row < 5; row++) {
//         for (int col = 0; col < 5; col++) {
//             int idx = row * 5 + col; // Linear index
//             float row_progress = (float)(row + 1) / 5.0f;
//             uint8_t r = 0, g = 0;

//             if (progress >= row_progress) {
//                 // Fully green
//                 g = 50;
//             } else {
//                 // Interpolate color: red -> green
//                 r = (uint8_t)(50 * (1.0f - progress / row_progress));
//                 g = (uint8_t)(50 * (progress / row_progress));
//             }

//             led_strip_set_pixel(strip, idx, r, g, 0);
//         }
//     }

//     led_strip_refresh(strip);
// }

void display_score(led_strip_handle_t strip, int num) {
    if (num < 0 || num > 9)
        return;

    for (int row = 0; row < 5; row++) {
        uint8_t bits = number_map_hex[num][row];

        for (int col = 0; col < 5; col++) {
            int i = row * 5 + col;
            bool on = bits & (1 << (4 - col));
            led_strip_set_pixel(strip, i, 0, 0, on ? 50 : 0);
        }
    }
    led_strip_refresh(strip);
}

led_strip_handle_t configure_led_strip(void) {
    led_strip_handle_t led_strip;

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = MAX_LEDS,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .flags.with_dma = false,
    };

    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    led_strip_clear(led_strip);

    return led_strip;
}