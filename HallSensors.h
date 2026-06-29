/**
 * @file HallSensors.h
 * @brief Hall sensor driver for magnetic field detection
 * @details Manages analog Hall sensors with filtering, calibration, and polarity detection
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_HALL_SENSORS_H
#define SPIDEREYES_HALL_SENSORS_H

#include "Config.h"
#include "Utilities.h"
#include <cstdint>
#include <array>

/**
 * @enum SensorPosition
 * @brief Identifies which sensor (left or right)
 */
enum class SensorPosition : uint8_t {
    LEFT = 0,      ///< Left eyebrow sensor
    RIGHT = 1,     ///< Right eyebrow sensor
};

/**
 * @struct HallCalibration
 * @brief Stores calibration data for a single Hall sensor
 */
struct HallCalibration {
    uint16_t neutralValue;       ///< ADC value at neutral position
    uint16_t openValue;          ///< ADC value at fully open position
    uint16_t closedValue;        ///< ADC value at fully closed position
    bool isCalibrated;           ///< Whether calibration has been performed
    bool isInverted;             ///< Whether magnetic polarity is inverted
};

/**
 * @class HallSensor
 * @brief Single Hall sensor controller with filtering and calibration
 */
class HallSensor {
public:
    /**
     * @brief Construct Hall sensor
     * @param gpioPin GPIO pin number for sensor
     * @param position Sensor position identifier
     */
    HallSensor(uint8_t gpioPin, SensorPosition position);
    
    /// Initialize sensor and configure ADC
    void begin();
    
    /// Read and process sensor data
    void update();
    
    /// Get raw ADC value
    uint16_t getRawValue() const;
    
    /// Get filtered ADC value
    uint16_t getFilteredValue() const;
    
    /// Get normalized value (0.0-1.0)
    float getNormalizedValue() const;
    
    /// Get sensor position identifier
    SensorPosition getPosition() const;
    
    /// Get calibration data
    const HallCalibration& getCalibration() const;
    
    /// Set calibration data
    void setCalibration(const HallCalibration& calibration);
    
    /// Start calibration capture
    void startCalibrationCapture();
    
    /// Get calibration capture progress (0-100)
    uint8_t getCalibrationProgress() const;
    
    /// Check if calibration capture is complete
    bool isCalibrationComplete() const;
    
    /// Get captured calibration value
    uint16_t getCapturedCalibrationValue() const;
    
    /// Detect magnetic polarity (returns true if inverted)
    bool detectPolarity();
    
    /// Check if sensor is connected (has valid signal)
    bool isConnected() const;
    
    /// Get connectivity status
    bool getConnectionStatus() const;
    
    /// Get sensor health status
    bool getHealthStatus() const;
    
private:
    uint8_t gpioPin;                               ///< GPIO pin number
    SensorPosition position;                        ///< Sensor position identifier
    HallCalibration calibration;                    ///< Calibration data
    ExponentialMovingAverage filter;               ///< Noise filter
    uint16_t rawValue;                              ///< Current raw ADC value
    uint16_t filteredValue;                         ///< Current filtered ADC value
    float normalizedValue;                          ///< Normalized value (0.0-1.0)
    
    // Calibration state
    uint32_t calibrationCaptureCount;               ///< Number of samples captured
    uint32_t calibrationAccumulator;                ///< Sum of samples
    bool isCalibrating;                             ///< Calibration in progress
    uint16_t capturedCalibrationValue;              ///< Captured calibration value
    
    // Connectivity tracking
    uint32_t lastValidReadTime;                     ///< Timestamp of last valid read
    bool isConnectedFlag;                           ///< Current connection status
    uint32_t disconnectionTimeoutMs;                ///< Disconnection detection timeout
    
    /// Update normalized value based on calibration
    void updateNormalizedValue();
};

/**
 * @class HallSensorArray
 * @brief Manages both Hall sensors (left and right)
 */
class HallSensorArray {
public:
    /// Initialize Hall sensor array
    HallSensorArray();
    
    /// Initialize both sensors
    void begin();
    
    /// Update both sensors
    void update();
    
    /// Get left sensor
    HallSensor& getLeftSensor();
    
    /// Get right sensor
    HallSensor& getRightSensor();
    
    /// Get sensor by position
    HallSensor& getSensor(SensorPosition position);
    
    /// Get left normalized value
    float getLeftNormalized() const;
    
    /// Get right normalized value
    float getRightNormalized() const;
    
    /// Check if both sensors are calibrated
    bool isFullyCalibrated() const;
    
    /// Check if both sensors are connected
    bool areSensorsConnected() const;
    
    /// Get overall system health
    bool getSystemHealth() const;
    
private:
    HallSensor leftSensor;                          ///< Left eyebrow sensor
    HallSensor rightSensor;                         ///< Right eyebrow sensor
};

#endif // SPIDEREYES_HALL_SENSORS_H
