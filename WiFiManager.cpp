/**
 * @file WiFiManager.cpp
 * @brief Implementation of WiFi manager
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#include "WiFiManager.h"
#include <Arduino.h>
#include <esp_wifi.h>

// ============================================================================
// WiFiManager Implementation
// ============================================================================

WiFiManager::WiFiManager()
    : status(WiFiStatus::DISCONNECTED),
      apActive(false),
      apSSID(WiFi::AP_SSID),
      apPassword(WiFi::AP_PASSWORD),
      lastStatusUpdateMs(millis()) {}

void WiFiManager::startAccessPoint() {
    // Stop any existing WiFi
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    // Configure and start AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    
    // Set static IP
    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    
    apActive = true;
    status = WiFiStatus::AP_ACTIVE;
}

void WiFiManager::stop() {
    WiFi.mode(WIFI_OFF);
    apActive = false;
    status = WiFiStatus::DISCONNECTED;
}

WiFiStatus WiFiManager::getStatus() const {
    return status;
}

int8_t WiFiManager::getSignalStrength() const {
    if (status == WiFiStatus::CONNECTED) {
        return WiFi.RSSI();
    }
    return 0;
}

IPAddress WiFiManager::getIPAddress() const {
    if (apActive) {
        return WiFi.softAPIP();
    }
    return WiFi.localIP();
}

String WiFiManager::getMACAddress() const {
    return WiFi.macAddress();
}

uint8_t WiFiManager::getConnectedClients() const {
    if (apActive) {
        return WiFi.softAPgetStationNum();
    }
    return 0;
}

bool WiFiManager::isAPActive() const {
    return apActive;
}

void WiFiManager::update() {
    // Update status periodically
    uint32_t currentTime = millis();
    if (currentTime - lastStatusUpdateMs < 1000) {
        return;
    }
    lastStatusUpdateMs = currentTime;
    
    if (apActive) {
        status = WiFiStatus::AP_ACTIVE;
    } else {
        wl_status_t wifiStatus = WiFi.status();
        if (wifiStatus == WL_CONNECTED) {
            status = WiFiStatus::CONNECTED;
        } else if (wifiStatus == WL_CONNECTING) {
            status = WiFiStatus::CONNECTING;
        } else if (wifiStatus == WL_NO_SSID_AVAIL || wifiStatus == WL_CONNECT_FAILED) {
            status = WiFiStatus::ERROR;
        } else {
            status = WiFiStatus::DISCONNECTED;
        }
    }
}

void WiFiManager::setAPSSID(const char* ssid) {
    if (ssid != nullptr) {
        apSSID = ssid;
    }
}

void WiFiManager::setAPPassword(const char* password) {
    if (password != nullptr) {
        apPassword = password;
    }
}
