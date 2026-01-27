#include "espnow-client.h"

static const char* TAG = "ESPNOW_CLIENT";

uint8_t g_player_id = 0;
static uint8_t current_player_score = 0;
static uint8_t g_server_mac[6] = {0};

static EventGroupHandle_t server_event_group;
static EventGroupHandle_t wifi_event_group;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                               void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        xEventGroupSetBits(wifi_event_group, WIFI_READY_BIT);
    }
}

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

static void on_data_recv(const esp_now_recv_info_t* recv_info, const uint8_t* data, int data_len) {
    if (data_len == sizeof(server_assign_t)) {
        if (g_player_id != 0) {
            return;
        }

        server_assign_t assign;
        memcpy(&assign, data, sizeof(assign));

        ESP_LOGI(TAG, "Server assigned player_id=%d, status=%d", assign.player_id, assign.status);

        if (assign.status == 0 || assign.status == 2) { // accepted
            g_player_id = assign.player_id;
            memcpy(g_server_mac, recv_info->src_addr, 6); // store server MAC dynamically
            add_peer(g_server_mac);                       // Add server as a peer for unicast
            xEventGroupSetBits(server_event_group, SERVER_ASSIGNED_BIT);
        } else {
            ESP_LOGW(TAG, "Server rejected registration, status=%d", assign.status);
        }
    }

    if (data_len == sizeof(game_score_t)) {
        game_score_t score;
        memcpy(&score, data, sizeof(score));

        if (g_player_id == 1) {
            current_player_score = score.score_1;
        } else if (g_player_id == 2) {
            current_player_score = score.score_2;
        } else {
            ESP_LOGW(TAG, "Unknown sender");
        }
    }
}

// -------------------- Public API --------------------
EventGroupHandle_t espnow_get_wifi_event_group(void) { return wifi_event_group; }
EventGroupHandle_t espnow_get_server_event_group(void) { return server_event_group; }

void espnow_send_input_event(input_event_t* data) {
    esp_err_t result = esp_now_send(g_server_mac, (uint8_t*)data, sizeof(*data));
    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Measurement data sent successfully");
    } else {
        ESP_LOGE(TAG, "Error sending measurement data: %d", result);
    }
}

uint8_t espnow_get_display_score(void) { return current_player_score; }

void espnow_client_init(void) {
    // init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();

    // create event groups
    wifi_event_group = xEventGroupCreate();
    server_event_group = xEventGroupCreate();

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);

    // init Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_start();

    // init ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_register_recv_cb(on_data_recv);

    ESP_LOGI(TAG, "ESPNOW client initialized");
}
