/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#include "icm-42688-p.h"

#define PIN_NUM_CS GPIO_NUM_1

// ICM-42688 Registers
#define REG_WHO_AM_I 0x75
#define REG_PWR_MGMT_0 0x4E
#define REG_ACCEL_XOUT_H 0x1F
#define REG_REG_BANK_SEL 0x76
#define REG_GYRO_CONFIG0 0x4F
#define REG_ACCEL_CONFIG0 0x50
#define ICMSPI_READ_LEN 12 // ACCEL(6) + GYRO(6)

static const char* TAG = "ICM-42688-P";
spi_device_handle_t icm_handle;

// Sensitivity based on chosen FSR
#define ACCEL_DIV 8192.0f // ±4 g
#define GYRO_DIV 65.5f    // ±500 dps

esp_err_t icm_spi_read(uint8_t reg, uint8_t* data, size_t len) {
    uint8_t tx_buf[1] = {reg | 0x80};
    spi_transaction_t t = {
        .length = 8 * (1 + len),
        .tx_buffer = tx_buf,
        .rxlength = 8 * (1 + len),
    };
    uint8_t rx_buf[1 + len];
    t.rx_buffer = rx_buf;

    esp_err_t ret = spi_device_transmit(icm_handle, &t);
    if (ret == ESP_OK) {
        memcpy(data, &rx_buf[1], len);
    }
    return ret;
}

esp_err_t icm_spi_write(uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg & 0x7F, data};
    spi_transaction_t t = {
        .length = 8 * 2,
        .tx_buffer = tx_buf,
    };
    return spi_device_transmit(icm_handle, &t);
}

// Read accel and gyro, convert to physical units
void icm_read_accel_gyro(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    uint8_t raw[12];
    icm_spi_read(REG_ACCEL_XOUT_H, raw, 12);

    int16_t raw_ax = (raw[0] << 8) | raw[1];
    int16_t raw_ay = (raw[2] << 8) | raw[3];
    int16_t raw_az = (raw[4] << 8) | raw[5];

    int16_t raw_gx = (raw[6] << 8) | raw[7];
    int16_t raw_gy = (raw[8] << 8) | raw[9];
    int16_t raw_gz = (raw[10] << 8) | raw[11];

    *ax = raw_ax / ACCEL_DIV;
    *ay = raw_ay / ACCEL_DIV;
    *az = raw_az / ACCEL_DIV;

    *gx = raw_gx / GYRO_DIV;
    *gy = raw_gy / GYRO_DIV;
    *gz = raw_gz / GYRO_DIV;
}

void icm_init(void) {
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8 * 1000 * 1000, // 8 MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &icm_handle));

    uint8_t who_am_i = 0;
    icm_spi_read(REG_WHO_AM_I, &who_am_i, 1);
    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X", who_am_i);
    if (who_am_i != 0x47) {
        ESP_LOGE(TAG, "ICM-42688 not detected!");
        return;
    }

    // Select Bank 0
    icm_spi_write(REG_REG_BANK_SEL, 0x00);

    // Configure gyro ±500 dps → bits 7:5 = 010
    icm_spi_write(REG_GYRO_CONFIG0, 0x02 << 5);

    // Configure accel ±4 g → bits 7:5 = 010
    icm_spi_write(REG_ACCEL_CONFIG0, 0x02 << 5);

    // Wake up device and enable accelerometer/gyro
    icm_spi_write(REG_PWR_MGMT_0, 0x0F); // enable sensors
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "ICM-42688 initialized (Accel ±4g, Gyro ±500dps)");
}
