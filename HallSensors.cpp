/**
 * @file HallSensors.cpp
 * @brief Implementation of Hall sensor driver
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "HallSensors.h"
#include <Arduino.h>
#include <esp_adc_cal.h>
#include <driver/adc.h>

// ============================================================================
// HallSensor Implementation
// ============================================================================

HallSensor::HallSensor(uint8_t gpioPin, SensorPosition position)
    : gpioPin(gpioPin),
      position(position),
      filter(ADC::FILTER_ALPHA),
      rawValue(0),
      filteredValue(0),
      normalizedValue(0.0f),
      calibrationCaptureCount(0),
      calibrationAccumulator(0),
      isCalibrating(false),
      capturedCalibrationValue(0),
      lastValidReadTime(millis()),
      isConnectedFlag(true),
      disconnectionTimeoutMs(1000) {
    
    // Initialize calibration with defaults
    calibration.neutralValue = ADC::MAX_VALUE / 2;
    calibration.openValue = ADC::MAX_VALUE;
    calibration.closedValue = 0;
    calibration.isCalibrated = false;
    calibration.isInverted = false;
}

void HallSensor::begin() {
    // Configure ADC for this pin
    // ESP32-C3 has 12-bit ADC
    analogSetAttenuation(ADC_11db);  // Full range 0-3.3V
    analogSetWidth(ADC::BITS);       // 12-bit resolution
    
    // Take initial reading
    update();
}

void HallSensor::update() {
    // Read multiple samples and average them
    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC::OVERSAMPLE_COUNT; i++) {
        sum += analogRead(gpioPin);
        delayMicroseconds(10);
    }
    
    rawValue = sum / ADC::OVERSAMPLE_COUNT;
    
    // Apply exponential moving average filter
    filteredValue = static_cast<uint16_t>(filter.filter(static_cast<float>(rawValue)));
    
    // Update normalized value
    updateNormalizedValue();
    
    // Check connectivity
    if (rawValue > 0 && rawValue < ADC::MAX_VALUE) {
        lastValidReadTime = millis();
        isConnectedFlag = true;
    } else if (millis() - lastValidReadTime > disconnectionTimeoutMs) {
        isConnectedFlag = false;
    }
    
    // Handle calibration capture
    if (isCalibrating && calibrationCaptureCount < ADC::OVERSAMPLE_COUNT) {
        calibrationAccumulator += filteredValue;
        calibrationCaptureCount++;
        
        if (calibrationCaptureCount >= ADC::OVERSAMPLE_COUNT) {
            capturedCalibrationValue = calibrationAccumulator / ADC::OVERSAMPLE_COUNT;
            isCalibrating = false;
        }
    }
}

uint16_t HallSensor::getRawValue() const {
    return rawValue;
}

uint16_t HallSensor::getFilteredValue() const {
    return filteredValue;
}

float HallSensor::getNormalizedValue() const {
    return normalizedValue;
}

SensorPosition HallSensor::getPosition() const {
    return position;
}

const HallCalibration& HallSensor::getCalibration() const {
    return calibration;
}

void HallSensor::setCalibration(const HallCalibration& newCalibration) {
    calibration = newCalibration;
}

void HallSensor::startCalibrationCapture() {
    isCalibrating = true;
    calibrationCaptureCount = 0;
    calibrationAccumulator = 0;
    capturedCalibrationValue = 0;
}

uint8_t HallSensor::getCalibrationProgress() const {
    if (!isCalibrating) return 100;
    return static_cast<uint8_t>((calibrationCaptureCount * 100) / ADC::OVERSAMPLE_COUNT);
}

bool HallSensor::isCalibrationComplete() const {
    return !isCalibrating && capturedCalibrationValue > 0;
}

uint16_t HallSensor::getCapturedCalibrationValue() const {
    return capturedCalibrationValue;
}

bool HallSensor::detectPolarity() {
    // Read current value
    uint16_t lowValue = filteredValue;
    delay(100);
    update();
    uint16_t highValue = filteredValue;
    
    // If value increased, polarity is normal; if decreased, inverted
    calibration.isInverted = (highValue < lowValue);
    return calibration.isInverted;
}

bool HallSensor::isConnected() const {
    return isConnectedFlag;
}

bool HallSensor::getConnectionStatus() const {
    return isConnectedFlag;
}

bool HallSensor::getHealthStatus() const {
    return isConnectedFlag && calibration.isCalibrated;
}

void HallSensor::updateNormalizedValue() {
    if (!calibration.isCalibrated) {
        normalizedValue = MathUtils::normalizeAdcValue(filteredValue);
        return;
    }
    
    // Map filtered value to normalized range based on calibration
    uint16_t minVal = std::min(calibration.openValue, calibration.closedValue);
    uint16_t maxVal = std::max(calibration.openValue, calibration.closedValue);
    
    if (filteredValue <= minVal) {
        normalizedValue = 0.0f;
    } else if (filteredValue >= maxVal) {
        normalizedValue = 1.0f;
    } else {
        normalizedValue = static_cast<float>(filteredValue - minVal) / static_cast<float>(maxVal - minVal);
    }
    
    if (calibration.isInverted) {
        normalizedValue = 1.0f - normalizedValue;
    }
}

// ============================================================================
// HallSensorArray Implementation
// ============================================================================

HallSensorArray::HallSensorArray()
    : leftSensor(GPIO::HALL_LEFT, SensorPosition::LEFT),
      rightSensor(GPIO::HALL_RIGHT, SensorPosition::RIGHT) {}

void HallSensorArray::begin() {
    leftSensor.begin();
    rightSensor.begin();
}

void HallSensorArray::update() {
    leftSensor.update();
    rightSensor.update();
}

HallSensor& HallSensorArray::getLeftSensor() {
    return leftSensor;
}

HallSensor& HallSensorArray::getRightSensor() {
    return rightSensor;
}

HallSensor& HallSensorArray::getSensor(SensorPosition position) {
    if (position == SensorPosition::LEFT) {
        return leftSensor;
    } else {
        return rightSensor;
    }
}

float HallSensorArray::getLeftNormalized() const {
    return leftSensor.getNormalizedValue();
}

float HallSensorArray::getRightNormalized() const {
    return rightSensor.getNormalizedValue();
}

bool HallSensorArray::isFullyCalibrated() const {
    return leftSensor.getCalibration().isCalibrated &&
           rightSensor.getCalibration().isCalibrated;
}

bool HallSensorArray::areSensorsConnected() const {
    return leftSensor.isConnected() && rightSensor.isConnected();
}

bool HallSensorArray::getSystemHealth() const {
    return isFullyCalibrated() && areSensorsConnected();
}
