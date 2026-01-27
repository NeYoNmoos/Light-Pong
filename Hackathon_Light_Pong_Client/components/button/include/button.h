#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BTN_GPIO_RIGHT GPIO_NUM_2
#define BTN_GPIO_LEFT GPIO_NUM_9

void configure_button(gpio_num_t gpioNum);
void refresh_button_values(void);

extern uint8_t btn_left_pressed;
extern uint8_t btn_right_pressed;

#endif