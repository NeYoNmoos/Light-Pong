/**
 * Project: RISC-V Disassembler/Simulator
 * Author: Elias Sohm
 * Date: 30.01.2026
 * Copyright (c) 2026 Elias Sohm
 */
#ifndef SPI_H
#define SPI_H

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

void spi_init(void);

#endif