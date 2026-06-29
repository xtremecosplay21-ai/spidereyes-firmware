# SpiderEyes Firmware

🕷️ **Smart Eye Shutter Control System** - ESP32-C3 Based Firmware

## Overview

SpiderEyes is a sophisticated ESP32-C3 firmware system for controlling animatronic eye shutters using Hall effect sensors and servo motors. It features real-time motion processing, web-based control interface, and comprehensive calibration tools.

## Features

### Hardware Control
- **Dual Hall Effect Sensors**: Real-time analog input monitoring with filtering
- **Dual Servo Motors**: LEDC PWM-based control with acceleration limiting
- **Soft Startup**: Gradual acceleration to prevent mechanical stress
- **Angle Tracking**: Precise position feedback and control

### Motion Processing
- **Exponential Moving Average Filter**: Smooth sensor data
- **Deadband Processing**: Eliminate noise and jitter
- **Spring-Damper Physics**: Natural motion dynamics
- **Multiple Easing Curves**: Linear, Cubic, SmoothStep, Exponential
- **100 Hz Update Rate**: Responsive real-time motion

### Calibration System
- **Multi-Step Wizard**: Interactive calibration process
- **Three-Point Calibration**: Neutral, Open, Closed positions
- **Persistent Storage**: NVS-based configuration saving
- **Factory Reset**: One-command reset to defaults

### Web Interface
- **Responsive Dashboard**: Real-time sensor and servo monitoring
- **Calibration Manager**: Step-by-step guided calibration
- **Motion Settings**: Sensitivity, deadband, speed adjustment
- **Diagnostics**: System health and performance metrics
- **JSON API**: RESTful endpoints for integration

### Connectivity
- **WiFi Access Point**: Built-in AP for control
- **HTTP Web Server**: Port 80 for web UI
- **Serial Interface**: USB debugging and commands

## System Architecture

```
┌─────────────────────────────────────────────────┐
│              Main Loop (10 ms)                  │
├─────────────────────────────────────────────────┤
│  Hall Sensors → Motion Engine → Servo Control  │
├─────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────────────┐   │
│  │ ADC Sampling │  │ Motion Processing    │   │
│  │ (100 Hz)     │→ │ • EMA Filter         │   │
│  └──────────────┘  │ • Deadband           │   │
│                    │ • Spring-Damper      │→ │ Servo PWM │
│  ┌──────────────┐  │ • Easing Curves      │   │ (50 Hz)   │
│  │ Calibration  │→ │ • Speed Limiting     │   │           │
│  │ Wizard       │  └──────────────────────┘   │           │
│  └──────────────┘                              │           │
└─────────────────────────────────────────────────┘
         ↓
    ┌─────────────┐
    │  WiFi / AP  │
    │  Web Server │
    └─────────────┘
```

## Configuration

All configuration is centralized in `Config.h`:

```cpp
// GPIO Pins
namespace GPIO {
    const uint8_t HALL_LEFT = 0;
    const uint8_t HALL_RIGHT = 1;
    const uint8_t SERVO_LEFT = 10;
    const uint8_t SERVO_RIGHT = 3;
}

// Servo Limits (degrees)
namespace Servo {
    const int16_t ANGLE_MIN = -90;
    const int16_t ANGLE_MAX = 70;
    const int16_t ANGLE_NEUTRAL = 0;
    const int16_t LEFT_OPEN = 70;
    const int16_t LEFT_CLOSED = -60;
    const int16_t RIGHT_OPEN = 70;
    const int16_t RIGHT_CLOSED = -60;
}

// Motion Engine Defaults
namespace Motion {
    const float DEFAULT_SENSITIVITY = 1.0f;
    const Motion::EasingCurve DEFAULT_CURVE = Motion::EasingCurve::SMOOTH_STEP;
}
```

## Compilation

### PlatformIO
```bash
# Build for ESP32-C3
pio run -e esp32-c3-devkitc-02

# Upload firmware
pio run -e esp32-c3-devkitc-02 -t upload

# Monitor serial output
pio run -e esp32-c3-devkitc-02 -t monitor

# Debug build
pio run -e debug

# Release optimized build
pio run -e release
```

### Arduino IDE
1. Install ESP32 board support via Board Manager
2. Select "ESP32-C3 Dev Kit C02" as board
3. Compile and upload

## Web Interface

Access at `http://192.168.4.1` when connected to the WiFi AP.

### Tabs
- **Dashboard**: Real-time sensor and servo values
- **Calibration**: Interactive 3-point calibration wizard
- **Servo Control**: Manual servo positioning
- **Motion Settings**: Sensitivity and filtering parameters
- **Diagnostics**: System health and memory usage

## Serial Commands

Connect via serial (115200 baud) for debugging:

```
help          - Show available commands
status        - Display system status
calibrate     - Start calibration wizard
neutral       - Move servos to neutral
open          - Open eyes fully
close         - Close eyes fully
sensors       - Show current sensor values
reset         - Factory reset all settings
```

## API Endpoints

### GET /api/dashboard
Returns current sensor and servo values:
```json
{
  "hall_left": 0.45,
  "hall_right": 0.52,
  "servo_left": 15,
  "servo_right": -10,
  "motion_update_freq": 100.0,
  "heap_free": 180224
}
```

### POST /api/servo
Control servo position:
```
/api/servo?left_angle=45&speed=180
/api/servo?action=neutral
/api/servo?action=open
/api/servo?action=closed
```

### POST /api/motion
Adjust motion parameters:
```
/api/motion?sensitivity=1.2&deadband=0.05
```

### POST /api/calibration
Manage calibration:
```
/api/calibration?action=start
/api/calibration?action=next
/api/calibration?action=save
/api/calibration?action=reset
```

### GET /api/diagnostics
Returns system diagnostics:
```json
{
  "heap_total": 327680,
  "heap_free": 180224,
  "heap_used": 147456,
  "motion_freq": 100.0,
  "firmware": "1.0.0",
  "build_date": "2026-06-29"
}
```

## Calibration Process

1. **Start**: Initialize calibration mode
2. **Neutral**: Position eyes at center and capture values
3. **Open**: Move eyes fully open and capture values
4. **Closed**: Move eyes fully closed and capture values
5. **Save**: Store calibration to persistent storage

Each step requires ~2 seconds of steady position holding.

## Performance Specifications

- **Motion Update Rate**: 100 Hz
- **Servo Update Rate**: 50 Hz
- **Sensor Sample Rate**: 100 Hz
- **Web Server**: Non-blocking, handles multiple clients
- **Typical Heap Usage**: ~150 KB
- **Build Size**: ~400 KB (FLASH)

## Debugging

Enable performance metrics in `Config.h`:
```cpp
namespace Debug {
    const bool ENABLE_PERFORMANCE_METRICS = true;
}
```

Metrics are printed to serial every 5 seconds.

## File Structure

```
SpiderEyes-Firmware/
├── Config.h              # Central configuration
├── Utilities.h/cpp       # Math, filtering, physics
├── HallSensors.h/cpp     # Hall effect input
├── ServoControl.h/cpp    # PWM servo control
├── MotionEngine.h/cpp    # Motion processing
├── Calibration.h/cpp     # Calibration wizard
├── Storage.h/cpp         # NVS persistence
├── WiFiManager.h/cpp     # WiFi/AP control
├── WebUI.h/cpp           # HTTP server
├── WebPages.h            # HTML/CSS/JavaScript
├── SpiderEyes.ino        # Main sketch
└── platformio.ini        # Build configuration
```

## Development Notes

- All timing is non-blocking using `millis()` deltas
- Motion updates are decoupled from servo updates
- Calibration can be done during operation
- Storage is optimized for minimal writes
- Web server handles concurrent clients

## Future Enhancements

- [ ] OTA firmware updates
- [ ] Profile switching via web UI
- [ ] Advanced motion presets
- [ ] Logging and data export
- [ ] Mobile app integration
- [ ] Multi-eye synchronization

## License

SpiderEyes Firmware © 2026 SpiderEyes Development Team

## Support

For issues or questions, please visit the GitHub repository.
