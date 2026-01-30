/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "spi.h"

#define PIN_NUM_MISO GPIO_NUM_7
#define PIN_NUM_MOSI GPIO_NUM_4
#define PIN_NUM_CLK GPIO_NUM_10

static const char* TAG = "ICM-42688-P";

void spi_init(void) {
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "SPI initialized");
}
