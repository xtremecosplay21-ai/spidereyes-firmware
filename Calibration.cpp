/**
 * @file Calibration.cpp
 * @brief Implementation of calibration wizard
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "Calibration.h"
#include "Storage.h"
#include <Arduino.h>

// ============================================================================
// CalibrationWizard Implementation
// ============================================================================

CalibrationWizard::CalibrationWizard()
    : currentState(Calibration::State::IDLE),
      stepStartTimeMs(0),
      leftCalibration{0, 0, 0, false, false},
      rightCalibration{0, 0, 0, false, false} {}

void CalibrationWizard::startCalibration() {
    currentState = Calibration::State::CAPTURING_NEUTRAL;
    stepStartTimeMs = millis();
}

Calibration::State CalibrationWizard::getCurrentState() const {
    return currentState;
}

void CalibrationWizard::nextStep() {
    if (!isCalibrating()) return;
    
    switch (currentState) {
        case Calibration::State::CAPTURING_NEUTRAL:
            currentState = Calibration::State::CAPTURING_OPEN;
            break;
        case Calibration::State::CAPTURING_OPEN:
            currentState = Calibration::State::CAPTURING_CLOSED;
            break;
        case Calibration::State::CAPTURING_CLOSED:
            currentState = Calibration::State::COMPLETE;
            break;
        default:
            break;
    }
    
    stepStartTimeMs = millis();
}

void CalibrationWizard::cancel() {
    currentState = Calibration::State::IDLE;
}

bool CalibrationWizard::isCalibrating() const {
    return currentState != Calibration::State::IDLE && currentState != Calibration::State::COMPLETE;
}

uint8_t CalibrationWizard::getProgress() const {
    switch (currentState) {
        case Calibration::State::IDLE:
            return 0;
        case Calibration::State::CAPTURING_NEUTRAL:
            return 25;
        case Calibration::State::CAPTURING_OPEN:
            return 50;
        case Calibration::State::CAPTURING_CLOSED:
            return 75;
        case Calibration::State::COMPLETE:
            return 100;
        default:
            return 0;
    }
}

const char* CalibrationWizard::getStateMessage() const {
    switch (currentState) {
        case Calibration::State::IDLE:
            return "Idle";
        case Calibration::State::CAPTURING_NEUTRAL:
            return "Position eyes at neutral and hold steady";
        case Calibration::State::CAPTURING_OPEN:
            return "Open eyes fully and hold steady";
        case Calibration::State::CAPTURING_CLOSED:
            return "Close eyes fully and hold steady";
        case Calibration::State::COMPLETE:
            return "Calibration complete";
        default:
            return "Unknown state";
    }
}

void CalibrationWizard::update(HallSensorArray& sensors) {
    if (!isCalibrating()) return;
    
    uint32_t elapsedMs = millis() - stepStartTimeMs;
    
    // Start sensor capture
    if (elapsedMs == 0) {
        sensors.getLeftSensor().startCalibrationCapture();
        sensors.getRightSensor().startCalibrationCapture();
    }
    
    // Check if capture is complete
    if (elapsedMs >= Calibration::CAPTURE_DURATION_MS) {
        if (sensors.getLeftSensor().isCalibrationComplete() &&
            sensors.getRightSensor().isCalibrationComplete()) {
            captureCurrentStep(sensors);
        }
    }
}

CalibrationWizard::CalibratedValues CalibrationWizard::getLeftCalibration() const {
    return {
        leftCalibration.neutralValue,
        leftCalibration.openValue,
        leftCalibration.closedValue
    };
}

CalibrationWizard::CalibratedValues CalibrationWizard::getRightCalibration() const {
    return {
        rightCalibration.neutralValue,
        rightCalibration.openValue,
        rightCalibration.closedValue
    };
}

void CalibrationWizard::applyCalibration(HallSensorArray& sensors) {
    leftCalibration.isCalibrated = true;
    rightCalibration.isCalibrated = true;
    
    sensors.getLeftSensor().setCalibration(leftCalibration);
    sensors.getRightSensor().setCalibration(rightCalibration);
}

void CalibrationWizard::saveCalibration(HallSensorArray& sensors) {
    PersistentStorage storage;
    
    HallCalibration leftCal = sensors.getLeftSensor().getCalibration();
    HallCalibration rightCal = sensors.getRightSensor().getCalibration();
    
    storage.saveHallCalibration(SensorPosition::LEFT, leftCal);
    storage.saveHallCalibration(SensorPosition::RIGHT, rightCal);
}

void CalibrationWizard::loadCalibration(HallSensorArray& sensors) {
    PersistentStorage storage;
    
    HallCalibration leftCal = storage.loadHallCalibration(SensorPosition::LEFT);
    HallCalibration rightCal = storage.loadHallCalibration(SensorPosition::RIGHT);
    
    sensors.getLeftSensor().setCalibration(leftCal);
    sensors.getRightSensor().setCalibration(rightCal);
}

void CalibrationWizard::resetToDefaults(HallSensorArray& sensors) {
    HallCalibration defaultCal;
    defaultCal.neutralValue = ADC::MAX_VALUE / 2;
    defaultCal.openValue = ADC::MAX_VALUE;
    defaultCal.closedValue = 0;
    defaultCal.isCalibrated = false;
    defaultCal.isInverted = false;
    
    sensors.getLeftSensor().setCalibration(defaultCal);
    sensors.getRightSensor().setCalibration(defaultCal);
    
    currentState = Calibration::State::IDLE;
}

uint32_t CalibrationWizard::getTimeRemaining() const {
    if (!isCalibrating()) return 0;
    
    uint32_t elapsedMs = millis() - stepStartTimeMs;
    if (elapsedMs >= Calibration::CAPTURE_DURATION_MS) {
        return 0;
    }
    
    return Calibration::CAPTURE_DURATION_MS - elapsedMs;
}

void CalibrationWizard::captureCurrentStep(HallSensorArray& sensors) {
    switch (currentState) {
        case Calibration::State::CAPTURING_NEUTRAL:
            leftCalibration.neutralValue = sensors.getLeftSensor().getCapturedCalibrationValue();
            rightCalibration.neutralValue = sensors.getRightSensor().getCapturedCalibrationValue();
            break;
            
        case Calibration::State::CAPTURING_OPEN:
            leftCalibration.openValue = sensors.getLeftSensor().getCapturedCalibrationValue();
            rightCalibration.openValue = sensors.getRightSensor().getCapturedCalibrationValue();
            break;
            
        case Calibration::State::CAPTURING_CLOSED:
            leftCalibration.closedValue = sensors.getLeftSensor().getCapturedCalibrationValue();
            rightCalibration.closedValue = sensors.getRightSensor().getCapturedCalibrationValue();
            break;
            
        default:
            break;
    }
}
