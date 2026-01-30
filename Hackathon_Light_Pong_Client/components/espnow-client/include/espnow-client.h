/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#ifndef ESPNOW_H
#define ESPNOW_H

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "string.h"

// EventGroup bits
#define WIFI_READY_BIT BIT0
#define SERVER_ASSIGNED_BIT BIT0

typedef struct {
    uint8_t score_1;
    uint8_t score_2;
} game_score_t;

typedef struct {
    uint8_t type;
    uint8_t id;
    uint8_t btn_right_pressed;
    uint8_t btn_left_pressed;
    float ax, ay, az;
    float gx, gy, gz;
} input_event_t;

typedef struct {
    uint8_t type;      // MSG_SERVER_ASSIGN
    uint8_t player_id; // Assigned player ID (1 or 2)
    uint8_t status;    // 0=accepted, 1=game_full, 2=already_registered
} server_assign_t;

typedef struct {
    uint8_t type;
} hello_t;

void espnow_client_init(void);
uint8_t espnow_get_display_score(void);
void espnow_send_input_event(input_event_t* data);
EventGroupHandle_t espnow_get_wifi_event_group(void);
EventGroupHandle_t espnow_get_server_event_group(void);

extern uint8_t g_player_id;

#endif