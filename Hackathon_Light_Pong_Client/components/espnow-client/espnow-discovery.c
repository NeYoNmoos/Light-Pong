/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "espnow-discovery.h"
#include "espnow-client.h"

static const char* TAG = "ESPNOW_DISCOVERY";

// helper to add a peer
static void add_peer(const uint8_t mac[6]) {
    if (esp_now_is_peer_exist(mac))
        return;

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, mac, 6);
    peer.ifidx = ESP_IF_WIFI_STA;
    peer.channel = 0;
    peer.encrypt = false;

    esp_err_t ret = esp_now_add_peer(&peer);
    if (ret != ESP_OK && ret != ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGW(TAG, "Failed to add peer: %d", ret);
    }
}

// periodically broadcast HELLO until server assigns a player ID
static void hello_task(void* arg) {
    EventGroupHandle_t wifi_ev = espnow_get_wifi_event_group();
    EventGroupHandle_t server_ev = espnow_get_server_event_group();

    // wait for Wi-Fi to be ready
    xEventGroupWaitBits(wifi_ev, WIFI_READY_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    hello_t hello = {.type = 0};
    uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    add_peer(broadcast_mac);

    while (!(xEventGroupGetBits(server_ev) & SERVER_ASSIGNED_BIT)) {
        esp_err_t ret = esp_now_send(broadcast_mac, (uint8_t*)&hello, sizeof(hello));
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Hello broadcast sent");
        } else {
            ESP_LOGW(TAG, "Failed to send hello, err=%d", ret);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    ESP_LOGI(TAG, "Server assigned player, stopping discovery task");
    vTaskDelete(NULL);
}

// public function: start discovery
void espnow_start_discovery(void) { xTaskCreate(hello_task, "hello_task", 2048, NULL, 5, NULL); }
