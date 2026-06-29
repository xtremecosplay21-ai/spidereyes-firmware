/**
 * @file Storage.h
 * @brief Persistent storage manager using ESP32 NVS (Non-Volatile Storage)
 * @details Manages configuration, calibration, and profile storage
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_STORAGE_H
#define SPIDEREYES_STORAGE_H

#include "Config.h"
#include "HallSensors.h"
#include <cstdint>
#include <cstring>
#include <array>
#include <nvs_flash.h>
#include <nvs.h>

/**
 * @struct MotionProfile
 * @brief Saved motion profile with all settings
 */
struct MotionProfile {
    char name[Profile::MAX_PROFILE_NAME_LENGTH];    ///< Profile name
    float sensitivity;                               ///< Motion sensitivity
    float deadband;                                  ///< Deadband threshold
    float speed;                                     ///< Servo speed
    float acceleration;                              ///< Servo acceleration
    uint8_t curve;                                   ///< Easing curve type
    bool used;                                       ///< Whether profile slot is used
};

/**
 * @class PersistentStorage
 * @brief Manages all persistent storage operations
 */
class PersistentStorage {
public:
    /// Initialize NVS storage
    PersistentStorage();
    
    /// Initialize storage subsystem
    bool begin();
    
    /// Save Hall sensor calibration
    void saveHallCalibration(SensorPosition position, const HallCalibration& calibration);
    
    /// Load Hall sensor calibration
    HallCalibration loadHallCalibration(SensorPosition position);
    
    /// Save servo limits
    void saveServoLimits(ServoId servoId, int16_t closedAngle, int16_t openAngle);
    
    /// Load servo limits
    void loadServoLimits(ServoId servoId, int16_t& closedAngle, int16_t& openAngle);
    
    /// Save motion settings
    void saveMotionSettings(float sensitivity, float deadband, float speed, 
                          float acceleration, uint8_t curve);
    
    /// Load motion settings
    void loadMotionSettings(float& sensitivity, float& deadband, float& speed,
                          float& acceleration, uint8_t& curve);
    
    /// Save motion profile
    void saveProfile(uint8_t profileIndex, const MotionProfile& profile);
    
    /// Load motion profile
    MotionProfile loadProfile(uint8_t profileIndex);
    
    /// List all saved profiles
    std::array<MotionProfile, Profile::MAX_PROFILES> listProfiles();
    
    /// Delete profile
    void deleteProfile(uint8_t profileIndex);
    
    /// Save WiFi credentials
    void saveWiFiCredentials(const char* ssid, const char* password);
    
    /// Load WiFi credentials
    void loadWiFiCredentials(char* ssid, char* password, size_t maxLen);
    
    /// Save startup mode preference
    void saveStartupMode(Profile::StartupMode mode);
    
    /// Load startup mode preference
    Profile::StartupMode loadStartupMode();
    
    /// Save firmware version
    void saveVersion(const char* version);
    
    /// Load firmware version
    void loadVersion(char* version, size_t maxLen);
    
    /// Erase all data (factory reset)
    void eraseAll();
    
    /// Check if storage is initialized
    bool isInitialized() const;
    
    /// Get available storage space (bytes)
    uint32_t getAvailableSpace() const;
    
    /// Get used storage space (bytes)
    uint32_t getUsedSpace() const;
    
private:
    nvs_handle_t nvsHandle;                         ///< NVS handle
    bool initialized;                               ///< Initialization flag
    
    /// Helper function to safely write string to NVS
    void writeString(const char* key, const char* value, size_t maxLen);
    
    /// Helper function to safely read string from NVS
    void readString(const char* key, char* value, size_t maxLen, const char* defaultValue = "");
    
    /// Helper function to safely write float to NVS (as integer with scaling)
    void writeFloat(const char* key, float value);
    
    /// Helper function to safely read float from NVS (from integer)
    float readFloat(const char* key, float defaultValue = 0.0f);
};

#endif // SPIDEREYES_STORAGE_H
