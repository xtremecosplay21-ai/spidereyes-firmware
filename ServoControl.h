/**
 * @file ServoControl.h
 * @brief Servo motor control using ESP32 LEDC PWM
 * @details Native PWM control with angle tracking, soft startup, and mirroring
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_SERVO_CONTROL_H
#define SPIDEREYES_SERVO_CONTROL_H

#include "Config.h"
#include "Utilities.h"
#include <cstdint>
#include <array>
#include <driver/ledc.h>

/**
 * @enum ServoId
 * @brief Identifies which servo (left or right)
 */
enum class ServoId : uint8_t {
    LEFT = 0,      ///< Left eye servo
    RIGHT = 1,     ///< Right eye servo
};

/**
 * @struct ServoLimits
 * @brief Physical limits for a servo
 */
struct ServoLimits {
    int16_t minAngle;           ///< Minimum angle limit (degrees)
    int16_t maxAngle;           ///< Maximum angle limit (degrees)
    int16_t closedAngle;        ///< Fully closed position (degrees)
    int16_t openAngle;          ///< Fully open position (degrees)
    bool reversed;              ///< Whether servo direction is reversed
};

/**
 * @class ServoMotor
 * @brief Single servo motor controller
 */
class ServoMotor {
public:
    /**
     * @brief Construct servo motor
     * @param gpioPin GPIO pin for PWM output
     * @param servoId Servo identifier (left/right)
     * @param channel LEDC channel to use
     * @param timer LEDC timer to use
     */
    ServoMotor(uint8_t gpioPin, ServoId servoId, ledc_channel_t channel, ledc_timer_t timer);
    
    /// Initialize servo and LEDC PWM
    void begin();
    
    /// Update servo position (must be called regularly)
    void update(uint32_t deltaTimeMs);
    
    /// Set target angle
    void setTargetAngle(int16_t angle);
    
    /// Get current angle
    int16_t getCurrentAngle() const;
    
    /// Get target angle
    int16_t getTargetAngle() const;
    
    /// Get servo ID
    ServoId getServoId() const;
    
    /// Set servo limits
    void setLimits(const ServoLimits& limits);
    
    /// Get servo limits
    const ServoLimits& getLimits() const;
    
    /// Set maximum speed (degrees per second)
    void setMaxSpeed(float degreesPerSecond);
    
    /// Get maximum speed
    float getMaxSpeed() const;
    
    /// Set maximum acceleration (degrees per second squared)
    void setMaxAcceleration(float degreesPerSecond2);
    
    /// Get maximum acceleration
    float getMaxAcceleration() const;
    
    /// Move to neutral position
    void moveToNeutral();
    
    /// Move to fully closed position
    void moveToClosed();
    
    /// Move to fully open position
    void moveToOpen();
    
    /// Stop and hold current position
    void stop();
    
    /// Enable servo (start PWM)
    void enable();
    
    /// Disable servo (stop PWM)
    void disable();
    
    /// Check if servo is enabled
    bool isEnabled() const;
    
    /// Get current velocity (degrees per second)
    float getCurrentVelocity() const;
    
    /// Set servo reversal
    void setReversed(bool reversed);
    
    /// Check if servo is reversed
    bool isReversed() const;
    
private:
    uint8_t gpioPin;                           ///< GPIO pin for PWM
    ServoId servoId;                           ///< Servo identifier
    ledc_channel_t channel;                    ///< LEDC channel
    ledc_timer_t timer;                        ///< LEDC timer
    ServoLimits limits;                        ///< Servo limits
    int16_t currentAngle;                      ///< Current servo angle
    int16_t targetAngle;                       ///< Target servo angle
    float maxSpeed;                            ///< Maximum speed (deg/sec)
    float maxAcceleration;                     ///< Maximum acceleration (deg/sec^2)
    float currentVelocity;                     ///< Current velocity (deg/sec)
    bool enabled;                              ///< Whether servo is enabled
    uint32_t lastUpdateTimeMs;                 ///< Last update timestamp
    RateLimiter speedLimiter;                  ///< Speed limiter
    
    /// Update PWM duty based on angle
    void updatePWM();
    
    /// Apply acceleration limiting
    void applyAccelerationLimit(uint32_t deltaTimeMs);
};

/**
 * @class ServoArray
 * @brief Manages both servo motors with mirroring
 */
class ServoArray {
public:
    /// Initialize servo array
    ServoArray();
    
    /// Initialize both servos
    void begin();
    
    /// Update both servos
    void update(uint32_t deltaTimeMs);
    
    /// Get left servo
    ServoMotor& getLeftServo();
    
    /// Get right servo
    ServoMotor& getRightServo();
    
    /// Get servo by ID
    ServoMotor& getServo(ServoId id);
    
    /// Set target angle for left servo
    void setLeftAngle(int16_t angle);
    
    /// Set target angle for right servo
    void setRightAngle(int16_t angle);
    
    /// Set both servos with automatic mirroring
    void setMirroredAngle(float normalizedValue);
    
    /// Get left servo current angle
    int16_t getLeftAngle() const;
    
    /// Get right servo current angle
    int16_t getRightAngle() const;
    
    /// Move all servos to neutral
    void moveToNeutral();
    
    /// Move all servos to closed
    void moveToClosed();
    
    /// Move all servos to open
    void moveToOpen();
    
    /// Stop all servos
    void stopAll();
    
    /// Enable all servos
    void enableAll();
    
    /// Disable all servos
    void disableAll();
    
    /// Set synchronized speed for all servos
    void setSpeed(float degreesPerSecond);
    
    /// Set synchronized acceleration for all servos
    void setAcceleration(float degreesPerSecond2);
    
private:
    ServoMotor leftServo;                      ///< Left eye servo
    ServoMotor rightServo;                     ///< Right eye servo
};

#endif // SPIDEREYES_SERVO_CONTROL_H
