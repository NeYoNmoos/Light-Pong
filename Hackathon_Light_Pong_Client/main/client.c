/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "button.h"
#include "esp_log.h"
#include "espnow-client.h"
#include "espnow-discovery.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "icm-42688-p.h"
#include "led_matrix.h"
#include "math.h"
#include "spi.h"
#include "string.h"

static const char* TAG = "Client";

#define ACCEL_THRESHOLD 2.0f // g
#define SPECIAL_SHOT_COOLDOWN_MS 10000
#define SEND_INTERVAL_MS 1000

led_strip_handle_t g_led_strip;
static TickType_t last_special_shot_tick = 0;
static TickType_t last_log_tick = 0;

static float gravity_mag = 0; // baseline gravity magnitude
static int gravity_samples = 0;
static bool in_motion = false; // motion state
static float prev_mag = 0;

static float total_acceleration(float ax, float ay, float az) {
    return sqrtf(ax * ax + ay * ay + az * az);
}

static bool can_trigger_special_shot(TickType_t now) {
    return (now - last_special_shot_tick >= pdMS_TO_TICKS(SPECIAL_SHOT_COOLDOWN_MS));
}

static void send_input_event(void) {
    if (g_player_id == 0) {
        ESP_LOGW(TAG, "Server not assigned yet, skipping send");
        return;
    }

    input_event_t event;
    icm_read_accel_gyro(&event.ax, &event.ay, &event.az, &event.gx, &event.gy, &event.gz);

    float mag = total_acceleration(event.ax, event.ay, event.az);

    if (gravity_samples < 100) {
        gravity_mag += mag;
        gravity_samples++;
        if (gravity_samples == 100) {
            gravity_mag /= 100.0f;
            ESP_LOGI(TAG, "Gravity baseline learned: %.3f g", gravity_mag);
        }
        return;
    }

    float filtered_mag = 0.8f * prev_mag + 0.2f * mag;
    prev_mag = filtered_mag;

    float dynamic = fabsf(filtered_mag - gravity_mag);

    if (!in_motion && dynamic > 1.0f) {
        in_motion = true;
    } else if (in_motion && dynamic < 0.3f) {
        in_motion = false;
    } else if (!in_motion) {
        return;
    }

    static TickType_t last_send_tick = 0;
    TickType_t now = xTaskGetTickCount();
    if (now - last_send_tick < pdMS_TO_TICKS(SEND_INTERVAL_MS)) {
        return;
    }
    last_send_tick = now;

    refresh_button_values();
    event.id = g_player_id;
    event.type = 1;
    event.btn_left_pressed = btn_left_pressed;

    if (!btn_right_pressed && can_trigger_special_shot(now)) {
        last_special_shot_tick = now;
        event.btn_right_pressed = 0;
    } else {
        event.btn_right_pressed = 1;
    }

    if (now - last_log_tick >= pdMS_TO_TICKS(500)) {
        ESP_LOGI(
            TAG,
            "ID=%d Buttons: L=%d R=%d Accel: ax=%.2f ay=%.2f az=%.2f Gyro: gx=%.2f gy=%.2f gz=%.2f",
            event.id, event.btn_left_pressed, event.btn_right_pressed, event.ax, event.ay, event.az,
            event.gx, event.gy, event.gz);
        last_log_tick = now;
    }

    espnow_send_input_event(&event);
}

void app_main(void) {
    g_led_strip = configure_led_strip();
    spi_init();
    icm_init();
    espnow_client_init();
    espnow_start_discovery();

    while (1) {
        send_input_event();

        // Display score and cooldown
        display_number_with_cooldown(g_led_strip, espnow_get_display_score(),
                                     last_special_shot_tick,
                                     pdMS_TO_TICKS(SPECIAL_SHOT_COOLDOWN_MS));

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
