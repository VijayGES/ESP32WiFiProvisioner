# ESP32WiFiProvisioner

A lightweight **Wi-Fi captive portal provisioning library for ESP32**, designed for Arduino users.

This library provides a **clean AP + captive portal** that always collects **Wi-Fi SSID and password**, while allowing users to **add any custom configuration fields** without modifying the library code.

---

## Features

- ESP32 Wi-Fi Access Point mode
- True captive portal using DNS redirection
- Mandatory SSID & password provisioning
- User-defined custom fields (any number)
- Wi-Fi network scanning
- Callback-based configuration handling
- No EEPROM, SPIFFS, MQTT, or cloud coupling
- Arduino-style, beginner friendly API

---

## Installation

### Arduino Library Manager
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for **ESP32WiFiProvisioner**
4. Click **Install**

### Manual Installation
1. Download or clone this repository
2. Copy `ESP32WiFiProvisioner` folder to:
