/**
 * @file WebUI.h
 * @brief Web user interface server and routing
 * @details HTTP server with JSON API endpoints
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_WEBUI_H
#define SPIDEREYES_WEBUI_H

#include "Config.h"
#include "HallSensors.h"
#include "ServoControl.h"
#include "MotionEngine.h"
#include "Calibration.h"
#include "Storage.h"
#include <WebServer.h>
#include <cstdint>

/**
 * @class WebUIServer
 * @brief HTTP web server for SpiderEyes control
 */
class WebUIServer {
public:
    /**
     * @brief Construct web server
     * @param port HTTP port (default 80)
     */
    explicit WebUIServer(uint16_t port = WebUI::SERVER_PORT);
    
    /// Initialize web server
    void begin(HallSensorArray* sensors, ServoArray* servos, MotionEngine* motionEngine,
              CalibrationWizard* calibration, PersistentStorage* storage);
    
    /// Handle client connections (call regularly)
    void handleClient();
    
    /// Stop web server
    void stop();
    
    /// Check if server is running
    bool isRunning() const;
    
private:
    WebServer server;                           ///< HTTP server instance
    bool running;                               ///< Server running flag
    
    // Pointers to system components
    HallSensorArray* sensorArray;               ///< Hall sensor array
    ServoArray* servoArray;                     ///< Servo array
    MotionEngine* motionEngine;                 ///< Motion engine
    CalibrationWizard* calibrationWizard;       ///< Calibration wizard
    PersistentStorage* storage;                 ///< Persistent storage
    
    // Request handlers
    void handleRoot();                          ///< GET /
    void handleDashboard();                     ///< GET /api/dashboard
    void handleCalibration();                   ///< GET/POST /api/calibration
    void handleServoSettings();                 ///< GET/POST /api/servo
    void handleSensorSettings();                ///< GET/POST /api/sensors
    void handleMotionSettings();                ///< GET/POST /api/motion
    void handleProfiles();                      ///< GET/POST /api/profiles
    void handleDiagnostics();                   ///< GET /api/diagnostics
    void handleNotFound();                      ///< 404 handler
    
    /// Send JSON response
    void sendJSON(const String& json);
    
    /// Build dashboard JSON
    String buildDashboardJSON();
    
    /// Build diagnostics JSON
    String buildDiagnosticsJSON();
};

#endif // SPIDEREYES_WEBUI_H
