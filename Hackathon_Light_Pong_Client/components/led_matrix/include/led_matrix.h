#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "led_strip.h"

led_strip_handle_t configure_led_strip(void);
void display_number_with_cooldown(led_strip_handle_t strip, int num, TickType_t last_tick,
                                  TickType_t cooldown_ticks);

#endif