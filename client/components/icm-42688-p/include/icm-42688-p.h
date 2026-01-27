#ifndef ICM_42688_P_H
#define ICM_42688_P_H

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

void icm_init(void);
void icm_read_accel_gyro(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);

#endif