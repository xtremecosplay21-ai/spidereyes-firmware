/**
 * @file Utilities.h
 * @brief Utility functions and helper classes
 * @details Mathematical operations, conversions, and debugging utilities
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_UTILITIES_H
#define SPIDEREYES_UTILITIES_H

#include "Config.h"
#include <cmath>
#include <algorithm>
#include <chrono>

/**
 * @class PerformanceMonitor
 * @brief Tracks loop timing and CPU performance metrics
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    
    /// Start timing a section
    void startSection();
    
    /// End timing a section and update metrics
    void endSection();
    
    /// Get average loop time in milliseconds
    float getAverageLoopTimeMs() const;
    
    /// Get current loop frequency in Hz
    float getLoopFrequencyHz() const;
    
    /// Get peak CPU load percentage
    float getPeakCpuLoad() const;
    
    /// Get average CPU load percentage
    float getAverageCpuLoad() const;
    
    /// Get maximum recorded section time
    uint32_t getMaxSectionTimeUs() const;
    
    /// Reset all metrics
    void reset();
    
private:
    uint32_t sectionStartTimeUs;
    uint32_t totalLoopTimeUs;
    uint32_t loopCount;
    float averageLoopTimeMs;
    float peakCpuLoad;
    float averageCpuLoad;
    uint32_t maxSectionTimeUs;
    uint32_t lastLoopStartTimeUs;
};

/**
 * @class ExponentialMovingAverage
 * @brief Implements exponential moving average filtering for noise reduction
 */
class ExponentialMovingAverage {
public:
    /**
     * @brief Construct EMA filter
     * @param alpha Filter coefficient (0.0-1.0), higher = more responsive
     * @param initialValue Initial filtered value
     */
    explicit ExponentialMovingAverage(float alpha = ADC::FILTER_ALPHA, float initialValue = 0.0f);
    
    /// Apply filter to new sample
    float filter(float newSample);
    
    /// Get current filtered value
    float getValue() const;
    
    /// Reset filter to initial value
    void reset(float initialValue = 0.0f);
    
    /// Set filter coefficient
    void setAlpha(float alpha);
    
private:
    float alpha;
    float filteredValue;
};

/**
 * @class Deadband
 * @brief Implements deadband filtering to ignore small changes
 */
class Deadband {
public:
    /**
     * @brief Construct deadband filter
     * @param threshold Deadband threshold value
     */
    explicit Deadband(float threshold = 0.05f);
    
    /// Apply deadband filter
    float filter(float value);
    
    /// Check if value is within deadband
    bool isInDeadband(float value) const;
    
    /// Set deadband threshold
    void setThreshold(float threshold);
    
private:
    float threshold;
    float lastValue;
};

/**
 * @class RateLimiter
 * @brief Limits the rate of change of a value
 */
class RateLimiter {
public:
    /**
     * @brief Construct rate limiter
     * @param maxRate Maximum rate of change per second
     */
    explicit RateLimiter(float maxRate);
    
    /// Apply rate limiting
    float limit(float targetValue, uint32_t deltaTimeMs);
    
    /// Get current rate-limited value
    float getValue() const;
    
    /// Set maximum rate of change
    void setMaxRate(float maxRate);
    
    /// Reset to a specific value
    void reset(float value = 0.0f);
    
private:
    float maxRate;
    float currentValue;
    float lastUpdateTimeMs;
};

/**
 * @class SpringDamper
 * @brief Implements spring-damper physics for smooth motion
 */
class SpringDamper {
public:
    /**
     * @brief Construct spring-damper system
     * @param stiffness Spring stiffness coefficient (0.0-1.0)
     * @param damping Damping coefficient (0.0-1.0)
     */
    SpringDamper(float stiffness = Servo::SPRING_STIFFNESS, float damping = Servo::SPRING_DAMPING);
    
    /// Update spring-damper system
    float update(float targetValue, float deltaTimeS);
    
    /// Get current position
    float getPosition() const;
    
    /// Get current velocity
    float getVelocity() const;
    
    /// Set spring parameters
    void setParameters(float stiffness, float damping);
    
    /// Reset to initial state
    void reset(float initialPosition = 0.0f);
    
private:
    float stiffness;
    float damping;
    float position;
    float velocity;
};

/**
 * @namespace MathUtils
 * @brief Mathematical utility functions
 */
namespace MathUtils {
    /// Clamp value between min and max
    template<typename T>
    T clamp(T value, T minVal, T maxVal) {
        return std::max(minVal, std::min(maxVal, value));
    }
    
    /// Linear interpolation
    template<typename T>
    T lerp(T a, T b, float t) {
        return a + (b - a) * t;
    }
    
    /// Map value from one range to another
    template<typename T>
    T map(T value, T inMin, T inMax, T outMin, T outMax) {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    
    /// Cubic ease-in-out function
    float cubicEaseInOut(float t);
    
    /// Smooth step function
    float smoothStep(float t);
    
    /// Convert ADC value to normalized 0.0-1.0 range
    float normalizeAdcValue(uint16_t adcValue);
    
    /// Convert normalized value to servo angle
    int16_t normalizedToAngle(float normalized, int16_t minAngle, int16_t maxAngle);
    
    /// Convert servo angle to pulse width
    uint16_t angleToPulseWidth(int16_t angle);
    
    /// Convert pulse width to angle
    int16_t pulseWidthToAngle(uint16_t pulseWidth);
}

#endif // SPIDEREYES_UTILITIES_H
