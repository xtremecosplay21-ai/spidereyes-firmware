/**
 * @file ServoControl.cpp
 * @brief Implementation of servo motor control
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "ServoControl.h"
#include <Arduino.h>
#include <driver/gpio.h>
#include <cmath>

// ============================================================================
// ServoMotor Implementation
// ============================================================================

ServoMotor::ServoMotor(uint8_t gpioPin, ServoId servoId, ledc_channel_t channel, ledc_timer_t timer)
    : gpioPin(gpioPin),
      servoId(servoId),
      channel(channel),
      timer(timer),
      currentAngle(Servo::ANGLE_NEUTRAL),
      targetAngle(Servo::ANGLE_NEUTRAL),
      maxSpeed(Servo::MAX_SPEED_DPS),
      maxAcceleration(Servo::MAX_ACCELERATION_DPS2),
      currentVelocity(0.0f),
      enabled(false),
      lastUpdateTimeMs(millis()),
      speedLimiter(Servo::MAX_SPEED_DPS) {
    
    // Set default limits based on servo ID
    if (servoId == ServoId::LEFT) {
        limits.minAngle = Servo::ANGLE_MIN;
        limits.maxAngle = Servo::ANGLE_MAX;
        limits.closedAngle = Servo::LEFT_CLOSED;
        limits.openAngle = Servo::LEFT_OPEN;
        limits.reversed = false;
    } else {
        limits.minAngle = Servo::ANGLE_MIN;
        limits.maxAngle = Servo::ANGLE_MAX;
        limits.closedAngle = Servo::RIGHT_CLOSED;
        limits.openAngle = Servo::RIGHT_OPEN;
        limits.reversed = true;
    }
}

void ServoMotor::begin() {
    // Configure LEDC timer
    ledc_timer_config_t timerConfig = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_RES_16_BIT,
        .timer_num = timer,
        .freq_hz = Servo::PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timerConfig);
    
    // Configure LEDC channel
    ledc_channel_config_t channelConfig = {
        .gpio_num = gpioPin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 0,
        .hpoint = 0,
        .flags = {0},
    };
    ledc_channel_config(&channelConfig);
    
    // Initial PWM update
    updatePWM();
    enabled = true;
    lastUpdateTimeMs = millis();
}

void ServoMotor::update(uint32_t deltaTimeMs) {
    if (!enabled) return;
    
    // Calculate acceleration limit
    applyAccelerationLimit(deltaTimeMs);
    
    // Apply speed limiting
    int16_t limitedTarget = speedLimiter.limit(targetAngle, deltaTimeMs);
    
    // Update current angle towards target
    int16_t diff = limitedTarget - currentAngle;
    if (std::abs(diff) > 0) {
        currentVelocity = static_cast<float>(diff) / (deltaTimeMs / 1000.0f);
        currentAngle = limitedTarget;
    } else {
        currentVelocity = 0.0f;
    }
    
    // Clamp to limits
    currentAngle = MathUtils::clamp(currentAngle, limits.minAngle, limits.maxAngle);
    
    // Update PWM
    updatePWM();
}

void ServoMotor::setTargetAngle(int16_t angle) {
    targetAngle = MathUtils::clamp(angle, limits.minAngle, limits.maxAngle);
}

int16_t ServoMotor::getCurrentAngle() const {
    return currentAngle;
}

int16_t ServoMotor::getTargetAngle() const {
    return targetAngle;
}

ServoId ServoMotor::getServoId() const {
    return servoId;
}

void ServoMotor::setLimits(const ServoLimits& newLimits) {
    limits = newLimits;
    currentAngle = MathUtils::clamp(currentAngle, limits.minAngle, limits.maxAngle);
    targetAngle = MathUtils::clamp(targetAngle, limits.minAngle, limits.maxAngle);
    updatePWM();
}

const ServoLimits& ServoMotor::getLimits() const {
    return limits;
}

void ServoMotor::setMaxSpeed(float degreesPerSecond) {
    maxSpeed = MathUtils::clamp(degreesPerSecond, 1.0f, 360.0f);
    speedLimiter.setMaxRate(maxSpeed);
}

float ServoMotor::getMaxSpeed() const {
    return maxSpeed;
}

void ServoMotor::setMaxAcceleration(float degreesPerSecond2) {
    maxAcceleration = MathUtils::clamp(degreesPerSecond2, 1.0f, 720.0f);
}

float ServoMotor::getMaxAcceleration() const {
    return maxAcceleration;
}

void ServoMotor::moveToNeutral() {
    setTargetAngle(Servo::ANGLE_NEUTRAL);
}

void ServoMotor::moveToClosed() {
    setTargetAngle(limits.closedAngle);
}

void ServoMotor::moveToOpen() {
    setTargetAngle(limits.openAngle);
}

void ServoMotor::stop() {
    targetAngle = currentAngle;
}

void ServoMotor::enable() {
    enabled = true;
    updatePWM();
}

void ServoMotor::disable() {
    enabled = false;
    // Set duty to 0 to stop PWM
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

bool ServoMotor::isEnabled() const {
    return enabled;
}

float ServoMotor::getCurrentVelocity() const {
    return currentVelocity;
}

void ServoMotor::setReversed(bool reversed) {
    limits.reversed = reversed;
    updatePWM();
}

bool ServoMotor::isReversed() const {
    return limits.reversed;
}

void ServoMotor::updatePWM() {
    if (!enabled) return;
    
    // Convert angle to pulse width
    int16_t angleToUse = currentAngle;
    if (limits.reversed) {
        angleToUse = -angleToUse;
    }
    
    uint16_t pulseWidth = MathUtils::angleToPulseWidth(angleToUse);
    
    // Convert pulse width to duty cycle
    // Duty = (pulseWidth / period) * max_duty
    // For 16-bit resolution, max_duty = 65535
    // Period = 20ms = 20000us
    uint32_t duty = (pulseWidth * 65535UL) / Servo::PWM_PERIOD_US;
    duty = MathUtils::clamp(duty, 0UL, 65535UL);
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

void ServoMotor::applyAccelerationLimit(uint32_t deltaTimeMs) {
    // Calculate maximum change based on acceleration limit
    float maxChange = maxAcceleration * (deltaTimeMs / 1000.0f);
    
    // Calculate desired change
    int16_t desiredChange = targetAngle - currentAngle;
    
    // Limit the change
    if (std::abs(desiredChange) > maxChange) {
        if (desiredChange > 0) {
            targetAngle = currentAngle + static_cast<int16_t>(maxChange);
        } else {
            targetAngle = currentAngle - static_cast<int16_t>(maxChange);
        }
    }
}

// ============================================================================
// ServoArray Implementation
// ============================================================================

ServoArray::ServoArray()
    : leftServo(GPIO::SERVO_LEFT, ServoId::LEFT, LEDC_CHANNEL_0, LEDC_TIMER_0),
      rightServo(GPIO::SERVO_RIGHT, ServoId::RIGHT, LEDC_CHANNEL_1, LEDC_TIMER_0) {}

void ServoArray::begin() {
    leftServo.begin();
    rightServo.begin();
}

void ServoArray::update(uint32_t deltaTimeMs) {
    leftServo.update(deltaTimeMs);
    rightServo.update(deltaTimeMs);
}

ServoMotor& ServoArray::getLeftServo() {
    return leftServo;
}

ServoMotor& ServoArray::getRightServo() {
    return rightServo;
}

ServoMotor& ServoArray::getServo(ServoId id) {
    if (id == ServoId::LEFT) {
        return leftServo;
    } else {
        return rightServo;
    }
}

void ServoArray::setLeftAngle(int16_t angle) {
    leftServo.setTargetAngle(angle);
}

void ServoArray::setRightAngle(int16_t angle) {
    rightServo.setTargetAngle(angle);
}

void ServoArray::setMirroredAngle(float normalizedValue) {
    // Normalize input to [0, 1]
    normalizedValue = MathUtils::clamp(normalizedValue, 0.0f, 1.0f);
    
    // Map normalized value to left servo angle
    int16_t leftAngle = MathUtils::normalizedToAngle(normalizedValue, Servo::LEFT_CLOSED, Servo::LEFT_OPEN);
    
    // Mirror for right servo (opposite direction)
    int16_t rightAngle = MathUtils::normalizedToAngle(normalizedValue, Servo::RIGHT_CLOSED, Servo::RIGHT_OPEN);
    
    leftServo.setTargetAngle(leftAngle);
    rightServo.setTargetAngle(rightAngle);
}

int16_t ServoArray::getLeftAngle() const {
    return leftServo.getCurrentAngle();
}

int16_t ServoArray::getRightAngle() const {
    return rightServo.getCurrentAngle();
}

void ServoArray::moveToNeutral() {
    leftServo.moveToNeutral();
    rightServo.moveToNeutral();
}

void ServoArray::moveToClosed() {
    leftServo.moveToClosed();
    rightServo.moveToClosed();
}

void ServoArray::moveToOpen() {
    leftServo.moveToOpen();
    rightServo.moveToOpen();
}

void ServoArray::stopAll() {
    leftServo.stop();
    rightServo.stop();
}

void ServoArray::enableAll() {
    leftServo.enable();
    rightServo.enable();
}

void ServoArray::disableAll() {
    leftServo.disable();
    rightServo.disable();
}

void ServoArray::setSpeed(float degreesPerSecond) {
    leftServo.setMaxSpeed(degreesPerSecond);
    rightServo.setMaxSpeed(degreesPerSecond);
}

void ServoArray::setAcceleration(float degreesPerSecond2) {
    leftServo.setMaxAcceleration(degreesPerSecond2);
    rightServo.setMaxAcceleration(degreesPerSecond2);
}
