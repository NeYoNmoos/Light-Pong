# Light Pong Server

ESP32-C3 server implementation for the Light Pong game using DMX512-controlled LED moving head with ESP-NOW wireless communication.

## Overview

The server manages game logic, controls the MH-X25 LED moving head via DMX512, and communicates with wireless ESP-NOW paddle controllers. It implements dynamic peer discovery, fireball effects, and win animations.

## Hardware Requirements

- **ESP32-C3 Development Board**
- **MH-X25 LED Moving Head** (12-channel DMX512)
- **RS485 Transceiver Module** (for DMX communication)

### Pin Configuration

| Function   | GPIO    | Notes                   |
| ---------- | ------- | ----------------------- |
| DMX TX     | GPIO 21 | UART transmit           |
| DMX RX     | GPIO 20 | UART receive            |
| DMX Enable | GPIO 9  | RS485 direction control |

**Note:** Do not use GPIO18/19 (USB D-/D+ pins).

## Software Architecture

The project follows a component-based architecture:

```
components/
├── dmx_driver/          # Low-level DMX512 UART driver
├── mh_x25_driver/       # MH-X25 moving head abstraction
├── light_effects/       # Visual effects (fireball, celebrations)
└── espnow_comm/         # ESP-NOW communication handler

main/
├── game/
│   ├── game_controller.c  # Core game logic
│   └── game_types.h       # Game data structures
└── config/
    ├── hardware_config.h  # Pin definitions
    └── game_config.h      # Game parameters
```

## Build and Flash

### Prerequisites

- ESP-IDF v5.5.1 or later
- ESP-IDF VS Code Extension (recommended)

### Build Commands

```bash
# Set target
idf.py set-target esp32c3

# Build project
idf.py build

# Flash and monitor
idf.py -p PORT flash monitor
```

## Game Features

- **Dynamic Peer Discovery**: Automatically detects and pairs with paddle controllers
- **Fireball Mode**: Special button press creates enhanced effects
- **Win Animations**: Color-coded celebrations for scoring players
- **Timeout Detection**: Automatic ball reset after 3 seconds of inactivity
- **Score Broadcasting**: Real-time score updates to all connected clients

## Configuration

Key parameters in `main/config/game_config.h`:

- `GAME_TIMEOUT_MS`: Ball timeout duration (3000ms)
- `WIN_SCORE`: Points to win (3)
- `BUTTON_FIREBALL`: Button state for fireball (0)
- `BUTTON_NORMAL`: Button state for normal hit (1)

## Communication Protocol

The server uses ESP-NOW for low-latency wireless communication:

- **Broadcast MAC**: `FF:FF:FF:FF:FF:FF`
- **Score Updates**: Sent on every paddle hit
- **Paddle Events**: Received from client controllers

## License

This project was developed as part of the FHV Master's program in Embedded Systems (2026).

## Author

Matthias Hefel
