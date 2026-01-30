# Light Pong Client (ESP32-C3)

Client implementation for the **Light Pong** game using **ESP-NOW**, an **ICM-42688-P IMU**, and the **Espressif LED Strip** library. Developed for the **ESP32-C3** platform.

## Overview

Each client connects to a central game server over **ESP-NOW**.

After startup, the client:

- Discovers the server via broadcast
- Receives a **player ID**
- Sends **input events** (paddle hits and special shots) to the server
- Receives **score updates**, which are displayed on the onboard **5×5 LED matrix**

## Hardware Requirements

- **ESP32-C3 Development Board**

## Pin Configuration

| Function   | GPIO | Notes                        |
| ---------- | ---- | ---------------------------- |
| SPI MISO   | 7    |                              |
| SPI MOSI   | 4    |                              |
| SPI CLK    | 10   |                              |
| ICM CS     | 1    | Chip Select for ICM-42688-P  |
| LED Matrix | 8    | Data line for 5×5 LED matrix |

**Note:** Avoid using **GPIO18** and **GPIO19** (USB D− / D+).

## Software Architecture

The project uses a **component-based structure**:

```
components/
├── button/              # Button input handling
├── espnow-client/       # ESP-NOW communication
├── icm-42688-p/         # IMU handling and motion detection
├── spi/                 # SPI bus abstraction
└── led_matrix/          # LED matrix control

main/
└── client.c             # Application entry point

managed_components/
└── espressif_led_strip  # LED strip driver
```

## Build and Flash

### Prerequisites

- **ESP-IDF v5.5.1** or newer
- **ESP-IDF VS Code Extension** (recommended)

### Commands

```bash
# Set target
idf.py set-target esp32c3

# Build project
idf.py build

# Flash and open serial monitor
idf.py -p PORT flash monitor
```

## Game Features

- **Dynamic Peer Discovery**
  The client sends a broadcast and connects automatically when the server responds.

- **Motion-Based Hits**
  Paddle hit events are triggered when acceleration exceeds a defined threshold.

- **Fireball Mode**
  A special button press sends a different input event, interpreted by the server as a “special shot”.

- **Score Display**
  The current score, received from the server, is shown on the 5×5 LED matrix.

- **Accelerometer Calibration**
  At startup, the ICM-42688-P is sampled multiple times to establish a baseline for motion detection.

## Communication Protocol

Communication between client and server uses **ESP-NOW** for low-latency data exchange.

- **Broadcast MAC Address:** `FF:FF:FF:FF:FF:FF`
- **Paddle Events:** Sent when motion or button conditions are met
- **Score Updates:** Sent by the server and displayed by the client

## License

Developed as part of the **FHV Master’s Program in Embedded Systems (2026)**.

## Author

**Elias Sohm**
