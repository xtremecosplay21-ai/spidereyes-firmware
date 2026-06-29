/**
 * @file Config.h
 * @brief Central configuration file for SpiderEyes firmware
 * @details Contains all compile-time and runtime constants, enums, and configuration structures
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_CONFIG_H
#define SPIDEREYES_CONFIG_H

#include <cstdint>
#include <cstddef>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

/// Hall Sensor GPIO pins
namespace GPIO {
    constexpr uint8_t HALL_LEFT = 4;      ///< Left eyebrow Hall sensor input
    constexpr uint8_t HALL_RIGHT = 5;     ///< Right eyebrow Hall sensor input
    constexpr uint8_t SERVO_LEFT = 2;     ///< Left eye servo PWM output
    constexpr uint8_t SERVO_RIGHT = 3;    ///< Right eye servo PWM output
}

// ============================================================================
// ADC CONFIGURATION
// ============================================================================

namespace ADC {
    constexpr uint8_t BITS = 12;                           ///< 12-bit ADC resolution
    constexpr uint16_t MAX_VALUE = (1 << BITS) - 1;        ///< 4095 for 12-bit
    constexpr uint32_t SAMPLE_RATE_HZ = 1000;              ///< ADC sampling frequency
    constexpr uint32_t OVERSAMPLE_COUNT = 16;              ///< Oversampling for noise reduction
    constexpr float FILTER_ALPHA = 0.15f;                  ///< EMA filter coefficient
    constexpr uint16_t DEADBAND_THRESHOLD = 50;            ///< ADC counts deadband
}

// ============================================================================
// SERVO CONFIGURATION
// ============================================================================

namespace Servo {
    constexpr uint32_t PWM_FREQUENCY = 50;                 ///< Standard servo frequency (Hz)
    constexpr uint32_t PWM_RESOLUTION_BITS = 16;           ///< PWM resolution bits (0-65535)
    constexpr uint16_t PULSE_MIN_US = 500;                 ///< Minimum pulse width (microseconds)
    constexpr uint16_t PULSE_MAX_US = 2500;                ///< Maximum pulse width (microseconds)
    constexpr uint16_t PULSE_CENTER_US = 1500;             ///< Center pulse width (microseconds)
    constexpr uint32_t PWM_PERIOD_US = 1000000 / PWM_FREQUENCY;  ///< Period in microseconds (20ms)
    
    // Angle limits (mechanical constraints)
    constexpr int16_t ANGLE_MIN = -90;                     ///< Minimum servo angle (degrees)
    constexpr int16_t ANGLE_MAX = 90;                      ///< Maximum servo angle (degrees)
    constexpr int16_t ANGLE_NEUTRAL = 0;                   ///< Neutral position (degrees)
    
    // Left eye specific angles
    constexpr int16_t LEFT_CLOSED = -90;                   ///< Left eye fully closed
    constexpr int16_t LEFT_OPEN = 70;                      ///< Left eye fully open
    
    // Right eye specific angles (mirrored)
    constexpr int16_t RIGHT_CLOSED = 90;                   ///< Right eye fully closed
    constexpr int16_t RIGHT_OPEN = -70;                    ///< Right eye fully open
    
    // Motion constraints
    constexpr float MAX_SPEED_DPS = 180.0f;                ///< Maximum speed (degrees per second)
    constexpr float MAX_ACCELERATION_DPS2 = 360.0f;        ///< Maximum acceleration (degrees per second squared)
    constexpr float SPRING_STIFFNESS = 0.85f;              ///< Spring-damper stiffness (0.0-1.0)
    constexpr float SPRING_DAMPING = 0.7f;                 ///< Spring-damper damping (0.0-1.0)
}

// ============================================================================
// MOTION ENGINE CONFIGURATION
// ============================================================================

namespace Motion {
    /// Easing curve types for smooth motion
    enum class EasingCurve {
        LINEAR,                                             ///< Linear motion
        CUBIC_IN_OUT,                                       ///< Cubic ease-in-out
        EXPONENTIAL,                                        ///< Exponential curve
        SMOOTH_STEP,                                        ///< Smooth step function
    };
    
    constexpr EasingCurve DEFAULT_CURVE = EasingCurve::CUBIC_IN_OUT;  ///< Default easing curve
    constexpr float DEFAULT_SENSITIVITY = 1.0f;            ///< Default response sensitivity (0.1-2.0)
    constexpr float SENSITIVITY_MIN = 0.1f;                ///< Minimum sensitivity
    constexpr float SENSITIVITY_MAX = 2.0f;                ///< Maximum sensitivity
}

// ============================================================================
// CALIBRATION CONFIGURATION
// ============================================================================

namespace Calibration {
    /// Calibration states for the wizard
    enum class State {
        IDLE,                                               ///< No calibration in progress
        CAPTURING_NEUTRAL,                                  ///< Capturing neutral position
        CAPTURING_OPEN,                                     ///< Capturing fully open position
        CAPTURING_CLOSED,                                   ///< Capturing fully closed position
        COMPLETE,                                           ///< Calibration complete
    };
    
    constexpr uint32_t CAPTURE_DURATION_MS = 2000;         ///< Duration to capture sample (ms)
    constexpr uint16_t CAPTURE_SAMPLES = 200;              ///< Number of samples to average
}

// ============================================================================
// STORAGE (NVS) CONFIGURATION
// ============================================================================

namespace Storage {
    constexpr const char* NAMESPACE_KEY = "spidereyes";    ///< NVS namespace
    
    // Calibration storage keys
    constexpr const char* KEY_HALL_LEFT_NEUTRAL = "h_l_neut";
    constexpr const char* KEY_HALL_LEFT_OPEN = "h_l_open";
    constexpr const char* KEY_HALL_LEFT_CLOSED = "h_l_clos";
    constexpr const char* KEY_HALL_RIGHT_NEUTRAL = "h_r_neut";
    constexpr const char* KEY_HALL_RIGHT_OPEN = "h_r_open";
    constexpr const char* KEY_HALL_RIGHT_CLOSED = "h_r_clos";
    
    // Servo limits storage keys
    constexpr const char* KEY_SERVO_LEFT_CLOSED = "s_l_clos";
    constexpr const char* KEY_SERVO_LEFT_OPEN = "s_l_open";
    constexpr const char* KEY_SERVO_RIGHT_CLOSED = "s_r_clos";
    constexpr const char* KEY_SERVO_RIGHT_OPEN = "s_r_open";
    
    // Motion settings
    constexpr const char* KEY_SENSITIVITY = "sens";
    constexpr const char* KEY_DEADBAND = "deadb";
    constexpr const char* KEY_SPEED = "speed";
    constexpr const char* KEY_ACCELERATION = "accel";
    constexpr const char* KEY_CURVE = "curve";
    constexpr const char* KEY_INVERT_LEFT = "inv_l";
    constexpr const char* KEY_INVERT_RIGHT = "inv_r";
    
    // System settings
    constexpr const char* KEY_STARTUP_MODE = "startup";
    constexpr const char* KEY_WIFI_SSID = "wifi_ssid";
    constexpr const char* KEY_WIFI_PASS = "wifi_pass";
    constexpr const char* KEY_VERSION = "version";
}

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

namespace WiFi {
    constexpr const char* AP_SSID = "SpiderEyes_Setup";    ///< Access point SSID
    constexpr const char* AP_PASSWORD = "spiderman";       ///< Access point password
    constexpr const char* AP_IP_ADDRESS = "192.168.4.1";   ///< Access point IP address
    constexpr uint16_t MAX_CLIENTS = 4;                     ///< Maximum simultaneous clients
    constexpr uint32_t RECONNECT_INTERVAL_MS = 10000;       ///< WiFi reconnect interval (ms)
}

// ============================================================================
// WEB UI CONFIGURATION
// ============================================================================

namespace WebUI {
    constexpr uint16_t SERVER_PORT = 80;                   ///< HTTP server port
    constexpr uint32_t UPDATE_INTERVAL_MS = 100;            ///< Dashboard update interval (ms)
    constexpr uint16_t MAX_CONCURRENT_CLIENTS = 4;          ///< Maximum concurrent HTTP connections
    constexpr size_t MAX_JSON_SIZE = 2048;                  ///< Maximum JSON response size
}

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================

namespace Timing {
    constexpr uint32_t MAIN_LOOP_FREQUENCY_HZ = 100;        ///< Main loop frequency (Hz)
    constexpr uint32_t MAIN_LOOP_PERIOD_MS = 1000 / MAIN_LOOP_FREQUENCY_HZ;  ///< Loop period (ms)
    constexpr uint32_t SENSOR_UPDATE_FREQUENCY_HZ = 100;    ///< Sensor update frequency (Hz)
    constexpr uint32_t SERVO_UPDATE_FREQUENCY_HZ = 50;      ///< Servo update frequency (Hz)
    constexpr uint32_t DIAGNOSTICS_UPDATE_HZ = 1;           ///< Diagnostics update frequency (Hz)
}

// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================

namespace System {
    constexpr const char* FIRMWARE_VERSION = "1.0.0";       ///< Firmware version string
    constexpr const char* BUILD_DATE = __DATE__;            ///< Build date
    constexpr const char* BUILD_TIME = __TIME__;            ///< Build time
    constexpr uint32_t BOOTLOADER_TIMEOUT_MS = 5000;        ///< Bootloader timeout (ms)
}

// ============================================================================
// PROFILE CONFIGURATION
// ============================================================================

namespace Profile {
    constexpr uint8_t MAX_PROFILES = 5;                     ///< Maximum saved profiles
    constexpr uint8_t MAX_PROFILE_NAME_LENGTH = 32;         ///< Max profile name length
    
    /// Startup profile selection
    enum class StartupMode {
        RESTORE_LAST,                                       ///< Restore last used profile
        USE_DEFAULT,                                        ///< Use default profile
        MANUAL,                                             ///< Wait for manual selection
    };
}

// ============================================================================
// DEBUGGING CONFIGURATION
// ============================================================================

namespace Debug {
    constexpr bool ENABLE_SERIAL_DEBUG = true;              ///< Enable serial debug output
    constexpr uint32_t SERIAL_BAUD_RATE = 115200;           ///< Serial baud rate
    constexpr bool ENABLE_PERFORMANCE_METRICS = true;       ///< Enable performance tracking
}

#endif // SPIDEREYES_CONFIG_H
