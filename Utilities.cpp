/**
 * @file Utilities.cpp
 * @brief Implementation of utility functions and helper classes
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "Utilities.h"
#include <Arduino.h>

// ============================================================================
// PerformanceMonitor Implementation
// ============================================================================

PerformanceMonitor::PerformanceMonitor()
    : sectionStartTimeUs(0),
      totalLoopTimeUs(0),
      loopCount(0),
      averageLoopTimeMs(0.0f),
      peakCpuLoad(0.0f),
      averageCpuLoad(0.0f),
      maxSectionTimeUs(0),
      lastLoopStartTimeUs(micros()) {}

void PerformanceMonitor::startSection() {
    sectionStartTimeUs = micros();
}

void PerformanceMonitor::endSection() {
    uint32_t sectionTimeUs = micros() - sectionStartTimeUs;
    if (sectionTimeUs > maxSectionTimeUs) {
        maxSectionTimeUs = sectionTimeUs;
    }
    totalLoopTimeUs += sectionTimeUs;
}

float PerformanceMonitor::getAverageLoopTimeMs() const {
    return averageLoopTimeMs;
}

float PerformanceMonitor::getLoopFrequencyHz() const {
    if (averageLoopTimeMs == 0.0f) return 0.0f;
    return 1000.0f / averageLoopTimeMs;
}

float PerformanceMonitor::getPeakCpuLoad() const {
    return peakCpuLoad;
}

float PerformanceMonitor::getAverageCpuLoad() const {
    return averageCpuLoad;
}

uint32_t PerformanceMonitor::getMaxSectionTimeUs() const {
    return maxSectionTimeUs;
}

void PerformanceMonitor::reset() {
    totalLoopTimeUs = 0;
    loopCount = 0;
    averageLoopTimeMs = 0.0f;
    peakCpuLoad = 0.0f;
    maxSectionTimeUs = 0;
}

// ============================================================================
// ExponentialMovingAverage Implementation
// ============================================================================

ExponentialMovingAverage::ExponentialMovingAverage(float alpha, float initialValue)
    : alpha(MathUtils::clamp(alpha, 0.0f, 1.0f)),
      filteredValue(initialValue) {}

float ExponentialMovingAverage::filter(float newSample) {
    filteredValue = alpha * newSample + (1.0f - alpha) * filteredValue;
    return filteredValue;
}

float ExponentialMovingAverage::getValue() const {
    return filteredValue;
}

void ExponentialMovingAverage::reset(float initialValue) {
    filteredValue = initialValue;
}

void ExponentialMovingAverage::setAlpha(float newAlpha) {
    alpha = MathUtils::clamp(newAlpha, 0.0f, 1.0f);
}

// ============================================================================
// Deadband Implementation
// ============================================================================

Deadband::Deadband(float threshold)
    : threshold(threshold),
      lastValue(0.0f) {}

float Deadband::filter(float value) {
    if (isInDeadband(value)) {
        return lastValue;
    }
    lastValue = value;
    return value;
}

bool Deadband::isInDeadband(float value) const {
    return std::abs(value - lastValue) < threshold;
}

void Deadband::setThreshold(float newThreshold) {
    threshold = newThreshold;
}

// ============================================================================
// RateLimiter Implementation
// ============================================================================

RateLimiter::RateLimiter(float maxRate)
    : maxRate(maxRate),
      currentValue(0.0f),
      lastUpdateTimeMs(millis()) {}

float RateLimiter::limit(float targetValue, uint32_t deltaTimeMs) {
    float maxChange = maxRate * (deltaTimeMs / 1000.0f);
    float change = targetValue - currentValue;
    change = MathUtils::clamp(change, -maxChange, maxChange);
    currentValue += change;
    return currentValue;
}

float RateLimiter::getValue() const {
    return currentValue;
}

void RateLimiter::setMaxRate(float newMaxRate) {
    maxRate = newMaxRate;
}

void RateLimiter::reset(float value) {
    currentValue = value;
}

// ============================================================================
// SpringDamper Implementation
// ============================================================================

SpringDamper::SpringDamper(float stiffness, float damping)
    : stiffness(MathUtils::clamp(stiffness, 0.0f, 1.0f)),
      damping(MathUtils::clamp(damping, 0.0f, 1.0f)),
      position(0.0f),
      velocity(0.0f) {}

float SpringDamper::update(float targetValue, float deltaTimeS) {
    // Spring force: F = -k * (x - target)
    float springForce = -stiffness * (position - targetValue);
    
    // Damping force: F = -d * v
    float dampingForce = -damping * velocity;
    
    // Total acceleration
    float acceleration = springForce + dampingForce;
    
    // Update velocity and position
    velocity += acceleration * deltaTimeS;
    position += velocity * deltaTimeS;
    
    return position;
}

float SpringDamper::getPosition() const {
    return position;
}

float SpringDamper::getVelocity() const {
    return velocity;
}

void SpringDamper::setParameters(float newStiffness, float newDamping) {
    stiffness = MathUtils::clamp(newStiffness, 0.0f, 1.0f);
    damping = MathUtils::clamp(newDamping, 0.0f, 1.0f);
}

void SpringDamper::reset(float initialPosition) {
    position = initialPosition;
    velocity = 0.0f;
}

// ============================================================================
// MathUtils Implementation
// ============================================================================

float MathUtils::cubicEaseInOut(float t) {
    // Clamp t to [0, 1]
    t = clamp(t, 0.0f, 1.0f);
    
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float f = 2.0f * t - 2.0f;
        return 0.5f * f * f * f + 1.0f;
    }
}

float MathUtils::smoothStep(float t) {
    // Clamp t to [0, 1]
    t = clamp(t, 0.0f, 1.0f);
    
    // Smoothstep: 3t² - 2t³
    return t * t * (3.0f - 2.0f * t);
}

float MathUtils::normalizeAdcValue(uint16_t adcValue) {
    return static_cast<float>(adcValue) / static_cast<float>(ADC::MAX_VALUE);
}

int16_t MathUtils::normalizedToAngle(float normalized, int16_t minAngle, int16_t maxAngle) {
    normalized = clamp(normalized, 0.0f, 1.0f);
    return static_cast<int16_t>(lerp(minAngle, maxAngle, normalized));
}

uint16_t MathUtils::angleToPulseWidth(int16_t angle) {
    // Clamp angle to valid range
    angle = clamp(angle, Servo::ANGLE_MIN, Servo::ANGLE_MAX);
    
    // Map angle to pulse width
    // -90° -> 500µs, 0° -> 1500µs, +90° -> 2500µs
    float normalizedAngle = (angle - Servo::ANGLE_MIN) / static_cast<float>(Servo::ANGLE_MAX - Servo::ANGLE_MIN);
    return static_cast<uint16_t>(lerp(Servo::PULSE_MIN_US, Servo::PULSE_MAX_US, normalizedAngle));
}

int16_t MathUtils::pulseWidthToAngle(uint16_t pulseWidth) {
    // Clamp pulse width to valid range
    pulseWidth = clamp(pulseWidth, Servo::PULSE_MIN_US, Servo::PULSE_MAX_US);
    
    // Map pulse width to angle
    float normalized = (pulseWidth - Servo::PULSE_MIN_US) / static_cast<float>(Servo::PULSE_MAX_US - Servo::PULSE_MIN_US);
    return static_cast<int16_t>(lerp(Servo::ANGLE_MIN, Servo::ANGLE_MAX, normalized));
}
