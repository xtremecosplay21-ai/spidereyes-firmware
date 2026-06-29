/**
 * @file WebUI.cpp
 * @brief Implementation of web UI server
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "WebUI.h"
#include "WebPages.h"
#include <Arduino.h>
#include <esp_heap_caps.h>

// ============================================================================
// WebUIServer Implementation
// ============================================================================

WebUIServer::WebUIServer(uint16_t port)
    : server(port),
      running(false),
      sensorArray(nullptr),
      servoArray(nullptr),
      motionEngine(nullptr),
      calibrationWizard(nullptr),
      storage(nullptr) {}

void WebUIServer::begin(HallSensorArray* sensors, ServoArray* servos, MotionEngine* motion,
                       CalibrationWizard* calibration, PersistentStorage* persistentStorage) {
    sensorArray = sensors;
    servoArray = servos;
    motionEngine = motion;
    calibrationWizard = calibration;
    storage = persistentStorage;
    
    // Register request handlers
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/api/dashboard", HTTP_GET, [this]() { handleDashboard(); });
    server.on("/api/calibration", HTTP_ANY, [this]() { handleCalibration(); });
    server.on("/api/servo", HTTP_ANY, [this]() { handleServoSettings(); });
    server.on("/api/sensors", HTTP_ANY, [this]() { handleSensorSettings(); });
    server.on("/api/motion", HTTP_ANY, [this]() { handleMotionSettings(); });
    server.on("/api/profiles", HTTP_ANY, [this]() { handleProfiles(); });
    server.on("/api/diagnostics", HTTP_GET, [this]() { handleDiagnostics(); });
    server.onNotFound([this]() { handleNotFound(); });
    
    server.begin();
    running = true;
}

void WebUIServer::handleClient() {
    if (running) {
        server.handleClient();
    }
}

void WebUIServer::stop() {
    server.stop();
    running = false;
}

bool WebUIServer::isRunning() const {
    return running;
}

void WebUIServer::handleRoot() {
    server.send(200, "text/html", WEB_PAGE_INDEX);
}

void WebUIServer::handleDashboard() {
    if (!sensorArray || !servoArray || !motionEngine) {
        server.send(500, "application/json", "{\"error\":\"System not initialized\"}");
        return;
    }
    
    String json = buildDashboardJSON();
    sendJSON(json);
}

void WebUIServer::handleCalibration() {
    if (!calibrationWizard || !sensorArray) {
        server.send(500, "application/json", "{\"error\":\"Calibration unavailable\"}");
        return;
    }
    
    if (server.method() == HTTP_POST) {
        String action = server.arg("action");
        
        if (action == "start") {
            calibrationWizard->startCalibration();
            server.send(200, "application/json", "{\"status\":\"calibration started\"}");
        } else if (action == "next") {
            calibrationWizard->nextStep();
            server.send(200, "application/json", "{\"status\":\"next step\"}");
        } else if (action == "cancel") {
            calibrationWizard->cancel();
            server.send(200, "application/json", "{\"status\":\"cancelled\"}");
        } else if (action == "save") {
            calibrationWizard->saveCalibration(*sensorArray);
            server.send(200, "application/json", "{\"status\":\"saved\"}");
        } else if (action == "reset") {
            calibrationWizard->resetToDefaults(*sensorArray);
            server.send(200, "application/json", "{\"status\":\"reset\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Unknown action\"}");
        }
    } else {
        // GET - return calibration status
        String json = "{\"state\":";
        json += (int)calibrationWizard->getCurrentState();
        json += ",\"progress\":";
        json += calibrationWizard->getProgress();
        json += ",\"message\":\"";
        json += calibrationWizard->getStateMessage();
        json += "\"}";
        sendJSON(json);
    }
}

void WebUIServer::handleServoSettings() {
    if (!servoArray) {
        server.send(500, "application/json", "{\"error\":\"Servos unavailable\"}");
        return;
    }
    
    if (server.method() == HTTP_POST) {
        if (server.hasArg("left_angle")) {
            int16_t angle = server.arg("left_angle").toInt();
            servoArray->setLeftAngle(angle);
        }
        if (server.hasArg("right_angle")) {
            int16_t angle = server.arg("right_angle").toInt();
            servoArray->setRightAngle(angle);
        }
        if (server.hasArg("speed")) {
            float speed = server.arg("speed").toFloat();
            servoArray->setSpeed(speed);
        }
        if (server.hasArg("acceleration")) {
            float accel = server.arg("acceleration").toFloat();
            servoArray->setAcceleration(accel);
        }
        if (server.hasArg("action")) {
            String action = server.arg("action");
            if (action == "neutral") servoArray->moveToNeutral();
            else if (action == "open") servoArray->moveToOpen();
            else if (action == "closed") servoArray->moveToClosed();
        }
        server.send(200, "application/json", "{\"status\":\"updated\"}");
    } else {
        String json = "{\"left_angle\":";
        json += servoArray->getLeftAngle();
        json += ",\"right_angle\":";
        json += servoArray->getRightAngle();
        json += ",\"left_speed\":";
        json += servoArray->getLeftServo().getMaxSpeed();
        json += ",\"right_speed\":";
        json += servoArray->getRightServo().getMaxSpeed();
        json += "}";
        sendJSON(json);
    }
}

void WebUIServer::handleSensorSettings() {
    if (!sensorArray) {
        server.send(500, "application/json", "{\"error\":\"Sensors unavailable\"}");
        return;
    }
    
    String json = "{\"left_raw\":";
    json += sensorArray->getLeftSensor().getRawValue();
    json += ",\"left_filtered\":";
    json += sensorArray->getLeftSensor().getFilteredValue();
    json += ",\"left_normalized\":";
    json += sensorArray->getLeftNormalized();
    json += ",\"right_raw\":";
    json += sensorArray->getRightSensor().getRawValue();
    json += ",\"right_filtered\":";
    json += sensorArray->getRightSensor().getFilteredValue();
    json += ",\"right_normalized\":";
    json += sensorArray->getRightNormalized();
    json += "}";
    sendJSON(json);
}

void WebUIServer::handleMotionSettings() {
    if (!motionEngine) {
        server.send(500, "application/json", "{\"error\":\"Motion engine unavailable\"}");
        return;
    }
    
    if (server.method() == HTTP_POST) {
        if (server.hasArg("sensitivity")) {
            motionEngine->setSensitivity(server.arg("sensitivity").toFloat());
        }
        if (server.hasArg("deadband")) {
            motionEngine->setDeadband(server.arg("deadband").toFloat());
        }
        if (server.hasArg("speed")) {
            motionEngine->setServoSpeed(server.arg("speed").toFloat());
        }
        if (server.hasArg("acceleration")) {
            motionEngine->setServoAcceleration(server.arg("acceleration").toFloat());
        }
        server.send(200, "application/json", "{\"status\":\"updated\"}");
    } else {
        String json = "{\"sensitivity\":";
        json += motionEngine->getSensitivity();
        json += ",\"deadband\":";
        json += motionEngine->getDeadband();
        json += ",\"speed\":";
        json += motionEngine->getServoSpeed();
        json += ",\"acceleration\":";
        json += motionEngine->getServoAcceleration();
        json += "}";
        sendJSON(json);
    }
}

void WebUIServer::handleProfiles() {
    if (!storage) {
        server.send(500, "application/json", "{\"error\":\"Storage unavailable\"}");
        return;
    }
    
    String json = "[";
    auto profiles = storage->listProfiles();
    for (size_t i = 0; i < profiles.size(); i++) {
        if (i > 0) json += ",";
        json += "{\"id\":" + String((int)i);
        json += ",\"name\":\"" + String(profiles[i].name);
        json += "\",\"used\":" + String(profiles[i].used ? "true" : "false");
        json += "}";
    }
    json += "]";
    sendJSON(json);
}

void WebUIServer::handleDiagnostics() {
    String json = buildDiagnosticsJSON();
    sendJSON(json);
}

void WebUIServer::handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}

void WebUIServer::sendJSON(const String& json) {
    server.send(200, "application/json", json);
}

String WebUIServer::buildDashboardJSON() {
    String json = "{";
    
    // Hall sensor data
    json += "\"hall_left\":" + String(sensorArray->getLeftNormalized());
    json += ",\"hall_right\":" + String(sensorArray->getRightNormalized());
    
    // Servo data
    json += ",\"servo_left\":" + String(servoArray->getLeftAngle());
    json += ",\"servo_right\":" + String(servoArray->getRightAngle());
    
    // Motion data
    json += ",\"motion_update_freq\":" + String(motionEngine->getUpdateFrequencyHz());
    
    // System data
    json += ",\"heap_free\":" + String(esp_get_free_heap_size());
    
    json += "}";
    return json;
}

String WebUIServer::buildDiagnosticsJSON() {
    String json = "{";
    
    json += "\"heap_total\":" + String(esp_get_heap_size());
    json += ",\"heap_free\":" + String(esp_get_free_heap_size());
    json += ",\"heap_used\":" + String(esp_get_heap_size() - esp_get_free_heap_size());
    json += ",\"motion_freq\":" + String(motionEngine->getUpdateFrequencyHz());
    json += ",\"firmware\":\"" + String(System::FIRMWARE_VERSION);
    json += "\",\"build_date\":\"" + String(System::BUILD_DATE);
    json += "\"";
    
    json += "}";
    return json;
}
