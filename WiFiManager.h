/**
 * @file WiFiManager.h
 * @brief WiFi and network management
 * @details Handles WiFi AP setup and basic connectivity
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_WIFI_MANAGER_H
#define SPIDEREYES_WIFI_MANAGER_H

#include "Config.h"
#include <WiFi.h>
#include <cstdint>

/**
 * @enum WiFiStatus
 * @brief WiFi connection status
 */
enum class WiFiStatus : uint8_t {
    DISCONNECTED = 0,                          ///< Not connected
    CONNECTING = 1,                            ///< Connecting to network
    CONNECTED = 2,                             ///< Connected to network
    AP_ACTIVE = 3,                             ///< Access point active
    ERROR = 4,                                 ///< Connection error
};

/**
 * @class WiFiManager
 * @brief Manages WiFi connectivity and AP
 */
class WiFiManager {
public:
    /// Initialize WiFi manager
    WiFiManager();
    
    /// Start WiFi in AP mode
    void startAccessPoint();
    
    /// Stop WiFi
    void stop();
    
    /// Get current WiFi status
    WiFiStatus getStatus() const;
    
    /// Get signal strength in dBm
    int8_t getSignalStrength() const;
    
    /// Get IP address
    IPAddress getIPAddress() const;
    
    /// Get MAC address
    String getMACAddress() const;
    
    /// Get number of connected clients
    uint8_t getConnectedClients() const;
    
    /// Check if AP is active
    bool isAPActive() const;
    
    /// Update WiFi state (call regularly)
    void update();
    
    /// Set AP SSID
    void setAPSSID(const char* ssid);
    
    /// Set AP password
    void setAPPassword(const char* password);
    
private:
    WiFiStatus status;                          ///< Current WiFi status
    bool apActive;                              ///< Whether AP is active
    String apSSID;                              ///< AP SSID
    String apPassword;                          ///< AP password
    uint32_t lastStatusUpdateMs;                ///< Last status update time
};

#endif // SPIDEREYES_WIFI_MANAGER_H
