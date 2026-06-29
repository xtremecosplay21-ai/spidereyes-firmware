/**
 * @file Storage.cpp
 * @brief Implementation of persistent storage manager
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "Storage.h"
#include <Arduino.h>
#include <esp_err.h>

// ============================================================================
// PersistentStorage Implementation
// ============================================================================

PersistentStorage::PersistentStorage()
    : nvsHandle(0),
      initialized(false) {}

bool PersistentStorage::begin() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated, erase it and retry
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    
    if (err != ESP_OK) {
        return false;
    }
    
    // Open NVS namespace
    err = nvs_open(Storage::NAMESPACE_KEY, NVS_READWRITE, &nvsHandle);
    initialized = (err == ESP_OK);
    
    return initialized;
}

void PersistentStorage::saveHallCalibration(SensorPosition position, const HallCalibration& calibration) {
    if (!initialized) return;
    
    const char* keyNeutral = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_NEUTRAL : Storage::KEY_HALL_RIGHT_NEUTRAL;
    const char* keyOpen = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_OPEN : Storage::KEY_HALL_RIGHT_OPEN;
    const char* keyClosed = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_CLOSED : Storage::KEY_HALL_RIGHT_CLOSED;
    
    nvs_set_u16(nvsHandle, keyNeutral, calibration.neutralValue);
    nvs_set_u16(nvsHandle, keyOpen, calibration.openValue);
    nvs_set_u16(nvsHandle, keyClosed, calibration.closedValue);
    nvs_set_u8(nvsHandle, (position == SensorPosition::LEFT ? "h_l_inv" : "h_r_inv"), 
               calibration.isInverted ? 1 : 0);
    nvs_commit(nvsHandle);
}

HallCalibration PersistentStorage::loadHallCalibration(SensorPosition position) {
    HallCalibration calibration;
    calibration.neutralValue = ADC::MAX_VALUE / 2;
    calibration.openValue = ADC::MAX_VALUE;
    calibration.closedValue = 0;
    calibration.isCalibrated = false;
    calibration.isInverted = false;
    
    if (!initialized) return calibration;
    
    const char* keyNeutral = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_NEUTRAL : Storage::KEY_HALL_RIGHT_NEUTRAL;
    const char* keyOpen = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_OPEN : Storage::KEY_HALL_RIGHT_OPEN;
    const char* keyClosed = (position == SensorPosition::LEFT) ? 
        Storage::KEY_HALL_LEFT_CLOSED : Storage::KEY_HALL_RIGHT_CLOSED;
    
    uint16_t neutral = 0, open = 0, closed = 0;
    uint8_t inverted = 0;
    
    if (nvs_get_u16(nvsHandle, keyNeutral, &neutral) == ESP_OK &&
        nvs_get_u16(nvsHandle, keyOpen, &open) == ESP_OK &&
        nvs_get_u16(nvsHandle, keyClosed, &closed) == ESP_OK) {
        
        calibration.neutralValue = neutral;
        calibration.openValue = open;
        calibration.closedValue = closed;
        calibration.isCalibrated = true;
        
        if (nvs_get_u8(nvsHandle, (position == SensorPosition::LEFT ? "h_l_inv" : "h_r_inv"), 
                      &inverted) == ESP_OK) {
            calibration.isInverted = (inverted != 0);
        }
    }
    
    return calibration;
}

void PersistentStorage::saveServoLimits(ServoId servoId, int16_t closedAngle, int16_t openAngle) {
    if (!initialized) return;
    
    const char* keyClosed = (servoId == ServoId::LEFT) ? 
        Storage::KEY_SERVO_LEFT_CLOSED : Storage::KEY_SERVO_RIGHT_CLOSED;
    const char* keyOpen = (servoId == ServoId::LEFT) ? 
        Storage::KEY_SERVO_LEFT_OPEN : Storage::KEY_SERVO_RIGHT_OPEN;
    
    nvs_set_i16(nvsHandle, keyClosed, closedAngle);
    nvs_set_i16(nvsHandle, keyOpen, openAngle);
    nvs_commit(nvsHandle);
}

void PersistentStorage::loadServoLimits(ServoId servoId, int16_t& closedAngle, int16_t& openAngle) {
    closedAngle = (servoId == ServoId::LEFT) ? Servo::LEFT_CLOSED : Servo::RIGHT_CLOSED;
    openAngle = (servoId == ServoId::LEFT) ? Servo::LEFT_OPEN : Servo::RIGHT_OPEN;
    
    if (!initialized) return;
    
    const char* keyClosed = (servoId == ServoId::LEFT) ? 
        Storage::KEY_SERVO_LEFT_CLOSED : Storage::KEY_SERVO_RIGHT_CLOSED;
    const char* keyOpen = (servoId == ServoId::LEFT) ? 
        Storage::KEY_SERVO_LEFT_OPEN : Storage::KEY_SERVO_RIGHT_OPEN;
    
    int16_t closed = 0, open = 0;
    if (nvs_get_i16(nvsHandle, keyClosed, &closed) == ESP_OK) {
        closedAngle = closed;
    }
    if (nvs_get_i16(nvsHandle, keyOpen, &open) == ESP_OK) {
        openAngle = open;
    }
}

void PersistentStorage::saveMotionSettings(float sensitivity, float deadband, float speed,
                                          float acceleration, uint8_t curve) {
    if (!initialized) return;
    
    writeFloat(Storage::KEY_SENSITIVITY, sensitivity);
    writeFloat(Storage::KEY_DEADBAND, deadband);
    writeFloat(Storage::KEY_SPEED, speed);
    writeFloat(Storage::KEY_ACCELERATION, acceleration);
    nvs_set_u8(nvsHandle, Storage::KEY_CURVE, curve);
    nvs_commit(nvsHandle);
}

void PersistentStorage::loadMotionSettings(float& sensitivity, float& deadband, float& speed,
                                          float& acceleration, uint8_t& curve) {
    sensitivity = Motion::DEFAULT_SENSITIVITY;
    deadband = 0.05f;
    speed = Servo::MAX_SPEED_DPS;
    acceleration = Servo::MAX_ACCELERATION_DPS2;
    curve = 0;  // LINEAR
    
    if (!initialized) return;
    
    sensitivity = readFloat(Storage::KEY_SENSITIVITY, Motion::DEFAULT_SENSITIVITY);
    deadband = readFloat(Storage::KEY_DEADBAND, 0.05f);
    speed = readFloat(Storage::KEY_SPEED, Servo::MAX_SPEED_DPS);
    acceleration = readFloat(Storage::KEY_ACCELERATION, Servo::MAX_ACCELERATION_DPS2);
    
    uint8_t curveVal = 0;
    nvs_get_u8(nvsHandle, Storage::KEY_CURVE, &curveVal);
    curve = curveVal;
}

void PersistentStorage::saveProfile(uint8_t profileIndex, const MotionProfile& profile) {
    if (!initialized || profileIndex >= Profile::MAX_PROFILES) return;
    
    char keyName[32];
    snprintf(keyName, sizeof(keyName), "prof_%d_name", profileIndex);
    writeString(keyName, profile.name, Profile::MAX_PROFILE_NAME_LENGTH);
    
    char keySens[32];
    snprintf(keySens, sizeof(keySens), "prof_%d_sens", profileIndex);
    writeFloat(keySens, profile.sensitivity);
    
    char keyDeadband[32];
    snprintf(keyDeadband, sizeof(keyDeadband), "prof_%d_dead", profileIndex);
    writeFloat(keyDeadband, profile.deadband);
    
    char keySpeed[32];
    snprintf(keySpeed, sizeof(keySpeed), "prof_%d_spd", profileIndex);
    writeFloat(keySpeed, profile.speed);
    
    char keyAccel[32];
    snprintf(keyAccel, sizeof(keyAccel), "prof_%d_acc", profileIndex);
    writeFloat(keyAccel, profile.acceleration);
    
    char keyCurve[32];
    snprintf(keyCurve, sizeof(keyCurve), "prof_%d_crv", profileIndex);
    nvs_set_u8(nvsHandle, keyCurve, profile.curve);
    
    char keyUsed[32];
    snprintf(keyUsed, sizeof(keyUsed), "prof_%d_use", profileIndex);
    nvs_set_u8(nvsHandle, keyUsed, profile.used ? 1 : 0);
    
    nvs_commit(nvsHandle);
}

MotionProfile PersistentStorage::loadProfile(uint8_t profileIndex) {
    MotionProfile profile;
    std::memset(&profile, 0, sizeof(profile));
    profile.used = false;
    
    if (!initialized || profileIndex >= Profile::MAX_PROFILES) return profile;
    
    char keyName[32];
    snprintf(keyName, sizeof(keyName), "prof_%d_name", profileIndex);
    readString(keyName, profile.name, Profile::MAX_PROFILE_NAME_LENGTH);
    
    char keySens[32];
    snprintf(keySens, sizeof(keySens), "prof_%d_sens", profileIndex);
    profile.sensitivity = readFloat(keySens, Motion::DEFAULT_SENSITIVITY);
    
    char keyDeadband[32];
    snprintf(keyDeadband, sizeof(keyDeadband), "prof_%d_dead", profileIndex);
    profile.deadband = readFloat(keyDeadband, 0.05f);
    
    char keySpeed[32];
    snprintf(keySpeed, sizeof(keySpeed), "prof_%d_spd", profileIndex);
    profile.speed = readFloat(keySpeed, Servo::MAX_SPEED_DPS);
    
    char keyAccel[32];
    snprintf(keyAccel, sizeof(keyAccel), "prof_%d_acc", profileIndex);
    profile.acceleration = readFloat(keyAccel, Servo::MAX_ACCELERATION_DPS2);
    
    char keyCurve[32];
    snprintf(keyCurve, sizeof(keyCurve), "prof_%d_crv", profileIndex);
    uint8_t curve = 0;
    nvs_get_u8(nvsHandle, keyCurve, &curve);
    profile.curve = curve;
    
    char keyUsed[32];
    snprintf(keyUsed, sizeof(keyUsed), "prof_%d_use", profileIndex);
    uint8_t used = 0;
    if (nvs_get_u8(nvsHandle, keyUsed, &used) == ESP_OK) {
        profile.used = (used != 0);
    }
    
    return profile;
}

std::array<MotionProfile, Profile::MAX_PROFILES> PersistentStorage::listProfiles() {
    std::array<MotionProfile, Profile::MAX_PROFILES> profiles;
    for (uint8_t i = 0; i < Profile::MAX_PROFILES; i++) {
        profiles[i] = loadProfile(i);
    }
    return profiles;
}

void PersistentStorage::deleteProfile(uint8_t profileIndex) {
    if (!initialized || profileIndex >= Profile::MAX_PROFILES) return;
    
    MotionProfile emptyProfile;
    std::memset(&emptyProfile, 0, sizeof(emptyProfile));
    saveProfile(profileIndex, emptyProfile);
}

void PersistentStorage::saveWiFiCredentials(const char* ssid, const char* password) {
    if (!initialized) return;
    
    writeString(Storage::KEY_WIFI_SSID, ssid, 32);
    writeString(Storage::KEY_WIFI_PASS, password, 64);
    nvs_commit(nvsHandle);
}

void PersistentStorage::loadWiFiCredentials(char* ssid, char* password, size_t maxLen) {
    readString(Storage::KEY_WIFI_SSID, ssid, maxLen);
    readString(Storage::KEY_WIFI_PASS, password, maxLen);
}

void PersistentStorage::saveStartupMode(Profile::StartupMode mode) {
    if (!initialized) return;
    nvs_set_u8(nvsHandle, Storage::KEY_STARTUP_MODE, static_cast<uint8_t>(mode));
    nvs_commit(nvsHandle);
}

Profile::StartupMode PersistentStorage::loadStartupMode() {
    if (!initialized) return Profile::StartupMode::RESTORE_LAST;
    
    uint8_t mode = 0;
    nvs_get_u8(nvsHandle, Storage::KEY_STARTUP_MODE, &mode);
    return static_cast<Profile::StartupMode>(mode);
}

void PersistentStorage::saveVersion(const char* version) {
    if (!initialized) return;
    writeString(Storage::KEY_VERSION, version, 16);
    nvs_commit(nvsHandle);
}

void PersistentStorage::loadVersion(char* version, size_t maxLen) {
    readString(Storage::KEY_VERSION, version, maxLen, System::FIRMWARE_VERSION);
}

void PersistentStorage::eraseAll() {
    if (!initialized) return;
    nvs_erase_all(nvsHandle);
    nvs_commit(nvsHandle);
}

bool PersistentStorage::isInitialized() const {
    return initialized;
}

uint32_t PersistentStorage::getAvailableSpace() const {
    return 0;  // ESP32 NVS doesn't provide direct API for this
}

uint32_t PersistentStorage::getUsedSpace() const {
    return 0;  // ESP32 NVS doesn't provide direct API for this
}

void PersistentStorage::writeString(const char* key, const char* value, size_t maxLen) {
    if (value != nullptr && std::strlen(value) > 0) {
        nvs_set_str(nvsHandle, key, value);
    }
}

void PersistentStorage::readString(const char* key, char* value, size_t maxLen, const char* defaultValue) {
    size_t len = maxLen;
    esp_err_t err = nvs_get_str(nvsHandle, key, value, &len);
    if (err != ESP_OK) {
        if (defaultValue != nullptr) {
            std::strncpy(value, defaultValue, maxLen - 1);
            value[maxLen - 1] = '\0';
        } else {
            value[0] = '\0';
        }
    }
}

void PersistentStorage::writeFloat(const char* key, float value) {
    // Store as integer by multiplying by 1000 to preserve precision
    int32_t intValue = static_cast<int32_t>(value * 1000.0f);
    nvs_set_i32(nvsHandle, key, intValue);
}

float PersistentStorage::readFloat(const char* key, float defaultValue) {
    int32_t intValue = 0;
    if (nvs_get_i32(nvsHandle, key, &intValue) == ESP_OK) {
        return static_cast<float>(intValue) / 1000.0f;
    }
    return defaultValue;
}
