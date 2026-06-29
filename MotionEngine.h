/**
 * @file MotionEngine.h
 * @brief Motion control engine with easing, filtering, and spring-damper physics
 * @details Converts normalized Hall sensor values to servo angles with smooth motion
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_MOTION_ENGINE_H
#define SPIDEREYES_MOTION_ENGINE_H

#include "Config.h"
#include "Utilities.h"
#include "HallSensors.h"
#include "ServoControl.h"
#include <cstdint>

/**
 * @class MotionEngine
 * @brief Main motion processing engine
 * @details Processes Hall sensor input and converts to servo output with advanced motion modeling
 */
class MotionEngine {
public:
    /// Initialize motion engine
    MotionEngine();
    
    /// Process motion update
    void update(uint32_t deltaTimeMs, const HallSensorArray& sensors, ServoArray& servos);
    
    /// Set motion sensitivity (0.1 - 2.0)
    void setSensitivity(float sensitivity);
    
    /// Get current sensitivity
    float getSensitivity() const;
    
    /// Set deadband threshold (0.0 - 1.0)
    void setDeadband(float deadband);
    
    /// Get current deadband
    float getDeadband() const;
    
    /// Set easing curve type
    void setEasingCurve(Motion::EasingCurve curve);
    
    /// Get current easing curve
    Motion::EasingCurve getEasingCurve() const;
    
    /// Set servo speed (degrees per second)
    void setServoSpeed(float degreesPerSecond);
    
    /// Get servo speed
    float getServoSpeed() const;
    
    /// Set servo acceleration (degrees per second squared)
    void setServoAcceleration(float degreesPerSecond2);
    
    /// Get servo acceleration
    float getServoAcceleration() const;
    
    /// Set spring-damper stiffness (0.0 - 1.0)
    void setSpringStiffness(float stiffness);
    
    /// Get spring-damper stiffness
    float getSpringStiffness() const;
    
    /// Set spring-damper damping (0.0 - 1.0)
    void setSpringDamping(float damping);
    
    /// Get spring-damper damping
    float getSpringDamping() const;
    
    /// Enable/disable spring-damper physics
    void setSpringDamperEnabled(bool enabled);
    
    /// Check if spring-damper is enabled
    bool isSpringDamperEnabled() const;
    
    /// Get processed left value (after all filtering)
    float getProcessedLeftValue() const;
    
    /// Get processed right value (after all filtering)
    float getProcessedRightValue() const;
    
    /// Reset all filters and accumulators
    void reset();
    
    /// Get motion update frequency
    float getUpdateFrequencyHz() const;
    
private:
    // Configuration
    float sensitivity;                          ///< Motion sensitivity multiplier
    float deadband;                             ///< Deadband threshold
    Motion::EasingCurve easingCurve;            ///< Easing curve type
    float servoSpeed;                           ///< Servo maximum speed
    float servoAcceleration;                    ///< Servo maximum acceleration
    bool springDamperEnabled;                   ///< Spring-damper physics enabled
    
    // Filters for each eye
    ExponentialMovingAverage leftFilter;        ///< Left eye EMA filter
    ExponentialMovingAverage rightFilter;       ///< Right eye EMA filter
    Deadband leftDeadband;                      ///< Left eye deadband
    Deadband rightDeadband;                     ///< Right eye deadband
    SpringDamper leftSpringDamper;              ///< Left eye spring-damper
    SpringDamper rightSpringDamper;             ///< Right eye spring-damper
    
    // Processing state
    float processedLeftValue;                   ///< Processed left normalized value
    float processedRightValue;                  ///< Processed right normalized value
    uint32_t lastUpdateTimeMs;                  ///< Last update timestamp
    uint32_t updateCount;                       ///< Total update count
    uint32_t lastSecondUpdateCount;             ///< Update count in last second
    uint32_t lastFrequencyCheckTime;            ///< Last frequency check time
    float measureUpdateFrequencyHz;             ///< Measured update frequency
    
    /// Apply easing curve to value
    float applyEasingCurve(float value);
    
    /// Process single normalized value
    float processSingleValue(float rawValue, ExponentialMovingAverage& filter,
                            Deadband& deadband, SpringDamper& springDamper,
                            float deltaTimeS);
};

#endif // SPIDEREYES_MOTION_ENGINE_H
