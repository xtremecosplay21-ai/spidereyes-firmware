/**
 * @file SpiderEyes.ino
 * @brief Main firmware sketch for SpiderEyes
 * @details ESP32-C3 based smart eye shutter control system
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "Config.h"
#include "Utilities.h"
#include "HallSensors.h"
#include "ServoControl.h"
#include "MotionEngine.h"
#include "Calibration.h"
#include "Storage.h"
#include "WiFiManager.h"
#include "WebUI.h"
#include <Arduino.h>

// ============================================================================
// Global System Objects
// ============================================================================

HallSensorArray hallSensors;                    ///< Hall sensor management
ServoArray servos;                              ///< Servo motor management
MotionEngine motionEngine;                      ///< Motion control engine
CalibrationWizard calibration;                  ///< Calibration wizard
PersistentStorage storage;                      ///< Persistent storage
WiFiManager wifiManager;                        ///< WiFi management
WebUIServer webServer(WebUI::SERVER_PORT);      ///< Web UI server

PerformanceMonitor perfMonitor;                 ///< Performance monitoring

// ============================================================================
// Timing and State Management
// ============================================================================

uint32_t lastMainLoopTime = 0;
uint32_t lastSensorUpdateTime = 0;
uint32_t lastServoUpdateTime = 0;
uint32_t lastDiagnosticsTime = 0;

bool systemInitialized = false;
bool calibrationMode = false;

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
    // Initialize serial for debugging
    Serial.begin(Debug::SERIAL_BAUD_RATE);
    delay(1000);
    
    Serial.println("\n\n================================");
    Serial.println("SpiderEyes Firmware v" + String(System::FIRMWARE_VERSION));
    Serial.println("Build: " + String(System::BUILD_DATE) + " " + String(System::BUILD_TIME));
    Serial.println("================================\n");
    
    // Initialize storage
    Serial.print("Initializing storage... ");
    if (storage.begin()) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
        // Continue anyway, storage will use defaults
    }
    
    // Initialize Hall sensors
    Serial.print("Initializing Hall sensors... ");
    hallSensors.begin();
    Serial.println("OK");
    
    // Load calibration from storage
    Serial.print("Loading calibration... ");
    calibration.loadCalibration(hallSensors);
    if (hallSensors.isFullyCalibrated()) {
        Serial.println("OK (calibration found)");
    } else {
        Serial.println("No calibration found, entering calibration mode");
        calibrationMode = true;
    }
    
    // Initialize servos
    Serial.print("Initializing servos... ");
    servos.begin();
    servos.moveToNeutral();
    Serial.println("OK");
    
    // Initialize motion engine
    Serial.print("Initializing motion engine... ");
    float sensitivity, deadband, speed, acceleration;
    uint8_t curve;
    storage.loadMotionSettings(sensitivity, deadband, speed, acceleration, curve);
    motionEngine.setSensitivity(sensitivity);
    motionEngine.setDeadband(deadband);
    motionEngine.setServoSpeed(speed);
    motionEngine.setServoAcceleration(acceleration);
    Serial.println("OK");
    
    // Initialize WiFi in AP mode
    Serial.print("Starting WiFi access point... ");
    wifiManager.startAccessPoint();
    Serial.println("OK");
    Serial.println("AP SSID: " + String(WiFi::AP_SSID));
    Serial.println("AP IP: " + wifiManager.getIPAddress().toString());
    
    // Initialize web server
    Serial.print("Starting web server... ");
    webServer.begin(&hallSensors, &servos, &motionEngine, &calibration, &storage);
    Serial.println("OK");
    Serial.println("Web UI: http://192.168.4.1");
    
    // Timing initialization
    lastMainLoopTime = millis();
    lastSensorUpdateTime = millis();
    lastServoUpdateTime = millis();
    lastDiagnosticsTime = millis();
    
    systemInitialized = true;
    Serial.println("\nSystem initialized successfully!");
    Serial.println("Ready for operation.\n");
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    uint32_t currentTime = millis();
    
    // ========== Sensor Updates (100 Hz) ==========
    uint32_t sensorDeltaMs = currentTime - lastSensorUpdateTime;
    if (sensorDeltaMs >= (1000 / Timing::SENSOR_UPDATE_FREQUENCY_HZ)) {
        hallSensors.update();
        
        // Update calibration if in calibration mode
        if (calibrationMode || calibration.isCalibrating()) {
            calibration.update(hallSensors);
        }
        
        lastSensorUpdateTime = currentTime;
    }
    
    // ========== Motion Engine Update (100 Hz) ==========
    uint32_t motionDeltaMs = currentTime - lastMainLoopTime;
    if (motionDeltaMs >= Timing::MAIN_LOOP_PERIOD_MS) {
        // Process motion from Hall sensors to servo commands
        motionEngine.update(motionDeltaMs, hallSensors, servos);
        lastMainLoopTime = currentTime;
    }
    
    // ========== Servo Updates (50 Hz) ==========
    uint32_t servoDeltaMs = currentTime - lastServoUpdateTime;
    if (servoDeltaMs >= (1000 / Timing::SERVO_UPDATE_FREQUENCY_HZ)) {
        servos.update(servoDeltaMs);
        lastServoUpdateTime = currentTime;
    }
    
    // ========== WiFi Updates ==========
    wifiManager.update();
    
    // ========== Web Server Client Handling ==========
    webServer.handleClient();
    
    // ========== Diagnostics (1 Hz) ==========
    uint32_t diagDeltaMs = currentTime - lastDiagnosticsTime;
    if (diagDeltaMs >= (1000 / Timing::DIAGNOSTICS_UPDATE_HZ)) {
        if (Debug::ENABLE_PERFORMANCE_METRICS) {
            // Print performance metrics
            if (diagDeltaMs % 5000 < 100) {  // Every 5 seconds
                Serial.print("Motion Freq: ");
                Serial.print(motionEngine.getUpdateFrequencyHz(), 1);
                Serial.print(" Hz | Heap: ");
                Serial.print(esp_get_free_heap_size() / 1024);
                Serial.println(" KB");
            }
        }
        lastDiagnosticsTime = currentTime;
    }
    
    // Small delay to prevent watchdog issues
    delay(1);
}

// ============================================================================
// Optional: Serial Command Interface (for debugging)
// ============================================================================

void serialEvent() {
    if (!Serial.available()) return;
    
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "help") {
        Serial.println("\nAvailable commands:");
        Serial.println("  help - Show this message");
        Serial.println("  status - Show system status");
        Serial.println("  calibrate - Start calibration wizard");
        Serial.println("  neutral - Move servos to neutral");
        Serial.println("  open - Open eyes");
        Serial.println("  close - Close eyes");
        Serial.println("  sensors - Show sensor values");
        Serial.println("  reset - Reset to defaults\n");
    }
    else if (command == "status") {
        Serial.println("\n=== System Status ===");
        Serial.print("Initialized: ");
        Serial.println(systemInitialized ? "YES" : "NO");
        Serial.print("Calibrated: ");
        Serial.println(hallSensors.isFullyCalibrated() ? "YES" : "NO");
        Serial.print("Sensors Connected: ");
        Serial.println(hallSensors.areSensorsConnected() ? "YES" : "NO");
        Serial.print("WiFi Status: ");
        Serial.println((int)wifiManager.getStatus());
        Serial.print("Free Heap: ");
        Serial.print(esp_get_free_heap_size() / 1024);
        Serial.println(" KB");
        Serial.print("Motion Frequency: ");
        Serial.print(motionEngine.getUpdateFrequencyHz(), 1);
        Serial.println(" Hz\n");
    }
    else if (command == "calibrate") {
        Serial.println("Starting calibration wizard...");
        calibration.startCalibration();
        calibrationMode = true;
    }
    else if (command == "neutral") {
        servos.moveToNeutral();
        Serial.println("Moving to neutral position...");
    }
    else if (command == "open") {
        servos.moveToOpen();
        Serial.println("Opening eyes...");
    }
    else if (command == "close") {
        servos.moveToClosed();
        Serial.println("Closing eyes...");
    }
    else if (command == "sensors") {
        Serial.println("\n=== Sensor Values ===");
        Serial.print("Left Hall (raw/filtered/norm): ");
        Serial.print(hallSensors.getLeftSensor().getRawValue());
        Serial.print(" / ");
        Serial.print(hallSensors.getLeftSensor().getFilteredValue());
        Serial.print(" / ");
        Serial.println(hallSensors.getLeftNormalized(), 3);
        
        Serial.print("Right Hall (raw/filtered/norm): ");
        Serial.print(hallSensors.getRightSensor().getRawValue());
        Serial.print(" / ");
        Serial.print(hallSensors.getRightSensor().getFilteredValue());
        Serial.print(" / ");
        Serial.println(hallSensors.getRightNormalized(), 3);
        Serial.println();
    }
    else if (command == "reset") {
        Serial.println("Resetting to defaults...");
        calibration.resetToDefaults(hallSensors);
        storage.eraseAll();
        Serial.println("Reset complete. Please restart.\n");
    }
    else if (command.length() > 0) {
        Serial.println("Unknown command. Type 'help' for available commands.");
    }
}
