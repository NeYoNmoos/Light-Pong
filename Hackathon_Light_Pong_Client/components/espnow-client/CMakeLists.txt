idf_component_register(
    SRCS "espnow-client.c"
         "espnow-discovery.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_event
        esp_netif
        esp_wifi
        esp_system 
        nvs_flash
        driver
)
