/**
 * @file Calibration.h
 * @brief Calibration wizard and manager
 * @details Manages Hall sensor calibration with multi-step wizard
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_CALIBRATION_H
#define SPIDEREYES_CALIBRATION_H

#include "Config.h"
#include "HallSensors.h"
#include <cstdint>

/**
 * @class CalibrationWizard
 * @brief Multi-step calibration wizard for Hall sensors
 */
class CalibrationWizard {
public:
    /// Initialize calibration wizard
    CalibrationWizard();
    
    /// Start calibration wizard
    void startCalibration();
    
    /// Get current calibration state
    Calibration::State getCurrentState() const;
    
    /// Advance to next calibration step
    void nextStep();
    
    /// Cancel calibration
    void cancel();
    
    /// Check if calibration is in progress
    bool isCalibrating() const;
    
    /// Get calibration progress (0-100)
    uint8_t getProgress() const;
    
    /// Get progress message for current step
    const char* getStateMessage() const;
    
    /// Update calibration (call regularly)
    void update(HallSensorArray& sensors);
    
    /// Get captured left calibration values
    struct CalibratedValues {
        uint16_t neutral;
        uint16_t open;
        uint16_t closed;
    };
    
    /// Get left sensor calibrated values
    CalibratedValues getLeftCalibration() const;
    
    /// Get right sensor calibrated values
    CalibratedValues getRightCalibration() const;
    
    /// Apply captured calibration to sensors
    void applyCalibration(HallSensorArray& sensors);
    
    /// Save calibration to persistent storage
    void saveCalibration(HallSensorArray& sensors);
    
    /// Load calibration from persistent storage
    void loadCalibration(HallSensorArray& sensors);
    
    /// Reset calibration to defaults
    void resetToDefaults(HallSensorArray& sensors);
    
    /// Get time remaining for current step (milliseconds)
    uint32_t getTimeRemaining() const;
    
private:
    Calibration::State currentState;                        ///< Current calibration state
    uint32_t stepStartTimeMs;                               ///< When current step started
    
    // Captured calibration values
    HallCalibration leftCalibration;                        ///< Captured left sensor calibration
    HallCalibration rightCalibration;                       ///< Captured right sensor calibration
    
    /// Capture calibration values for current step
    void captureCurrentStep(HallSensorArray& sensors);
};

#endif // SPIDEREYES_CALIBRATION_H
