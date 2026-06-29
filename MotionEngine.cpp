/**
 * @file MotionEngine.cpp
 * @brief Implementation of motion control engine
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "MotionEngine.h"
#include <Arduino.h>
#include <cmath>

// ============================================================================
// MotionEngine Implementation
// ============================================================================

MotionEngine::MotionEngine()
    : sensitivity(Motion::DEFAULT_SENSITIVITY),
      deadband(0.05f),
      easingCurve(Motion::DEFAULT_CURVE),
      servoSpeed(Servo::MAX_SPEED_DPS),
      servoAcceleration(Servo::MAX_ACCELERATION_DPS2),
      springDamperEnabled(true),
      leftFilter(ADC::FILTER_ALPHA),
      rightFilter(ADC::FILTER_ALPHA),
      leftDeadband(0.05f),
      rightDeadband(0.05f),
      leftSpringDamper(Servo::SPRING_STIFFNESS, Servo::SPRING_DAMPING),
      rightSpringDamper(Servo::SPRING_STIFFNESS, Servo::SPRING_DAMPING),
      processedLeftValue(0.0f),
      processedRightValue(0.0f),
      lastUpdateTimeMs(millis()),
      updateCount(0),
      lastSecondUpdateCount(0),
      lastFrequencyCheckTime(millis()),
      measureUpdateFrequencyHz(0.0f) {}

void MotionEngine::update(uint32_t deltaTimeMs, const HallSensorArray& sensors, ServoArray& servos) {
    // Calculate delta time in seconds
    float deltaTimeS = deltaTimeMs / 1000.0f;
    if (deltaTimeS < 0.001f) deltaTimeS = 0.001f;  // Minimum 1ms
    
    // Get raw normalized values from sensors
    float leftRawValue = sensors.getLeftNormalized();
    float rightRawValue = sensors.getRightNormalized();
    
    // Apply sensitivity multiplier
    leftRawValue = MathUtils::clamp(leftRawValue * sensitivity, 0.0f, 1.0f);
    rightRawValue = MathUtils::clamp(rightRawValue * sensitivity, 0.0f, 1.0f);
    
    // Process values through filters
    processedLeftValue = processSingleValue(leftRawValue, leftFilter, leftDeadband,
                                           leftSpringDamper, deltaTimeS);
    processedRightValue = processSingleValue(rightRawValue, rightFilter, rightDeadband,
                                            rightSpringDamper, deltaTimeS);
    
    // Apply easing curves
    float easedLeftValue = applyEasingCurve(processedLeftValue);
    float easedRightValue = applyEasingCurve(processedRightValue);
    
    // Set servo angles with mirroring
    servos.setMirroredAngle(easedLeftValue);
    
    // Update frequency tracking
    updateCount++;
    uint32_t currentTime = millis();
    if (currentTime - lastFrequencyCheckTime >= 1000) {
        measureUpdateFrequencyHz = static_cast<float>(updateCount - lastSecondUpdateCount);
        lastSecondUpdateCount = updateCount;
        lastFrequencyCheckTime = currentTime;
    }
}

void MotionEngine::setSensitivity(float newSensitivity) {
    sensitivity = MathUtils::clamp(newSensitivity, Motion::SENSITIVITY_MIN, Motion::SENSITIVITY_MAX);
}

float MotionEngine::getSensitivity() const {
    return sensitivity;
}

void MotionEngine::setDeadband(float newDeadband) {
    deadband = MathUtils::clamp(newDeadband, 0.0f, 1.0f);
    leftDeadband.setThreshold(deadband);
    rightDeadband.setThreshold(deadband);
}

float MotionEngine::getDeadband() const {
    return deadband;
}

void MotionEngine::setEasingCurve(Motion::EasingCurve curve) {
    easingCurve = curve;
}

Motion::EasingCurve MotionEngine::getEasingCurve() const {
    return easingCurve;
}

void MotionEngine::setServoSpeed(float degreesPerSecond) {
    servoSpeed = MathUtils::clamp(degreesPerSecond, 1.0f, 360.0f);
}

float MotionEngine::getServoSpeed() const {
    return servoSpeed;
}

void MotionEngine::setServoAcceleration(float degreesPerSecond2) {
    servoAcceleration = MathUtils::clamp(degreesPerSecond2, 1.0f, 720.0f);
}

float MotionEngine::getServoAcceleration() const {
    return servoAcceleration;
}

void MotionEngine::setSpringStiffness(float stiffness) {
    stiffness = MathUtils::clamp(stiffness, 0.0f, 1.0f);
    leftSpringDamper.setParameters(stiffness, leftSpringDamper.getVelocity());
    rightSpringDamper.setParameters(stiffness, rightSpringDamper.getVelocity());
}

float MotionEngine::getSpringStiffness() const {
    return Servo::SPRING_STIFFNESS;  // Return default; could track actual if needed
}

void MotionEngine::setSpringDamping(float damping) {
    damping = MathUtils::clamp(damping, 0.0f, 1.0f);
    leftSpringDamper.setParameters(leftSpringDamper.getPosition(), damping);
    rightSpringDamper.setParameters(rightSpringDamper.getPosition(), damping);
}

float MotionEngine::getSpringDamping() const {
    return Servo::SPRING_DAMPING;  // Return default; could track actual if needed
}

void MotionEngine::setSpringDamperEnabled(bool enabled) {
    springDamperEnabled = enabled;
}

bool MotionEngine::isSpringDamperEnabled() const {
    return springDamperEnabled;
}

float MotionEngine::getProcessedLeftValue() const {
    return processedLeftValue;
}

float MotionEngine::getProcessedRightValue() const {
    return processedRightValue;
}

void MotionEngine::reset() {
    leftFilter.reset(0.0f);
    rightFilter.reset(0.0f);
    leftSpringDamper.reset(0.0f);
    rightSpringDamper.reset(0.0f);
    processedLeftValue = 0.0f;
    processedRightValue = 0.0f;
    updateCount = 0;
    lastSecondUpdateCount = 0;
}

float MotionEngine::getUpdateFrequencyHz() const {
    return measureUpdateFrequencyHz;
}

float MotionEngine::applyEasingCurve(float value) {
    value = MathUtils::clamp(value, 0.0f, 1.0f);
    
    switch (easingCurve) {
        case Motion::EasingCurve::LINEAR:
            return value;
            
        case Motion::EasingCurve::CUBIC_IN_OUT:
            return MathUtils::cubicEaseInOut(value);
            
        case Motion::EasingCurve::SMOOTH_STEP:
            return MathUtils::smoothStep(value);
            
        case Motion::EasingCurve::EXPONENTIAL: {
            // Simple exponential: e^(2*x) / e^2, mapped to [0, 1]
            return (std::exp(2.0f * value) - 1.0f) / (std::exp(2.0f) - 1.0f);
        }
            
        default:
            return value;
    }
}

float MotionEngine::processSingleValue(float rawValue, ExponentialMovingAverage& filter,
                                       Deadband& deadband, SpringDamper& springDamper,
                                       float deltaTimeS) {
    // Apply EMA filter first
    float filtered = filter.filter(rawValue);
    
    // Apply deadband
    float deadzoned = deadband.filter(filtered);
    
    // Apply spring-damper if enabled
    float result = deadzoned;
    if (springDamperEnabled) {
        result = springDamper.update(deadzoned, deltaTimeS);
    }
    
    // Ensure result is in valid range
    return MathUtils::clamp(result, 0.0f, 1.0f);
}
