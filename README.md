# P-Sci Arduino Library

A powerful Arduino library for ESP32 that provides a complete web-based control panel with WiFi management, pin control, serial monitoring, and OTA firmware updates.

## Features

- 📡 WiFi connectivity with automatic AP mode fallback
- 🌐 Captive portal setup page at 192.168.4.1
- 💻 Responsive web dashboard accessible via p-sci.local
- 🔌 Dynamic pin control (Digital, PWM, Input)
- 📊 Real-time system monitoring
- 🔄 Over-the-air (OTA) firmware updates from GitHub
- 📝 Serial monitor integration in web dashboard
- 🔐 mDNS support for easy local network access

## Installation

1. Download the P-Sci library folder
2. Copy to Arduino libraries folder: `Documents/Arduino/libraries/`
3. Restart Arduino IDE
4. Library will appear in Sketch > Include Library > P-Sci

## Quick Start

```cpp
#include <PSci.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    psci.setup();  // Initialize library
}

void loop() {
    psci.run();    // Handle all operations
    
    // Your code here
    delay(10);
}
```

## WiFi Setup

1. First boot: ESP32 enters AP mode
2. Connect to WiFi: **P-Sci_Config** (password: 12345678)
3. Open browser: **http://192.168.4.1**
4. Enter your WiFi credentials and click "Save & Connect"
5. Device reboots and connects to your WiFi
6. Access dashboard: **http://p-sci.local** or IP address

## Dependencies

- ArduinoJson
- CircularBuffer

Install via Arduino Library Manager or manually add to libraries folder.

## Usage

All functionality is handled by `psci.setup()` and `psci.run()`. Simply call these in your Arduino sketch.

## Support

For issues or feature requests, visit: [P-Sci GitHub](https://github.com/p-sci/P-Sci)
