/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "button.h"

#define BTN_GPIO_RIGHT GPIO_NUM_2
#define BTN_GPIO_LEFT GPIO_NUM_9

uint8_t btn_left_pressed;
uint8_t btn_right_pressed;

static uint8_t debounced_read(gpio_num_t pin) {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += gpio_get_level(pin);
        vTaskDelay(1);
    }
    return (sum >= 3);
}

void refresh_button_values(void) {
    btn_left_pressed = debounced_read(BTN_GPIO_LEFT);
    btn_right_pressed = debounced_read(BTN_GPIO_RIGHT);
}

void configure_button(gpio_num_t gpioNum) {
    gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << gpioNum),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&btn_config);
}