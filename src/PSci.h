#ifndef PSCI_H
#define PSCI_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <CircularBuffer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

struct PinConfig {
    int pin;
    String name;
    String type; // "digital_out", "pwm", "digital_in"
    int value;
    int pwmChannel;
};

class SerialCapture : public Print {
private:
    CircularBuffer<char, 512> buffer;
public:
    size_t write(uint8_t c) override;
    String getBuffer();
    void clearBuffer();
};

class PSci {
private:
    // System variables
    Preferences prefs;
    WebServer server;
    DNSServer dnsServer;
    SerialCapture serialCapture;
    
    // WiFi and configuration
    String ssid, password, apiKey, deviceName;
    bool wifiConnected = false;
    bool connectedToApp = false;
    
    // Timing
    unsigned long lastSystemCheck = 0;
    unsigned long connectInterval = 15000;
    unsigned long lastConnectAttempt = 0;
    unsigned long lastTaskCheck = 0;
    unsigned long lastHeartbeat = 0;
    unsigned long bootTime = 0;
    
    // Pin management
    PinConfig pins[10];
    int pinCount = 0;
    
    // Monitoring
    String monitorBuffer[100];
    int monitorIndex = 0;
    String sensorData = "N/A";
    bool clientRunning = false;
    
    // URLs
    const String CONNECT_URL = "https://p-sci.vercel.app/api/electronics/boards/connect";
    const String HEARTBEAT_URL = "https://p-sci.vercel.app/api/electronics/boards/heartbeat";
    const String TASK_URL = "https://p-sci.vercel.app/api/esp/task-instructions";
    const String TASK_COMPLETE_URL = "https://p-sci.vercel.app/api/electronics/boards/task-complete";
    
    // Private methods
    void setupWiFi();
    void startAPMode();
    void connectToWiFi();
    void handleRoot();
    void handleNotFound();
    void handleAddPin();
    void handleControlPin();
    void handleRemovePin();
    void handleSerial();
    void handleSystemInfo();
    void handleTaskValidation();
    void handleOTACheck();
    void handleClientToggle();
    void handleSaveWiFi();
    
    void addToMonitorBuffer(const String &message);
    void checkWiFiConnection();
    void sendHeartbeat();
    void checkForTaskInstructions();
    void validateProject(JsonObject validation);
    bool validateDigitalToggle(int pin, int expectedPeriod, int tolerance, int samples);
    void reportTaskResult(const String &taskId, const String &moduleId, bool success, JsonObject validationData);
    void updateSystemStats();
    void setupDashboard();
    
public:
    // Constructor
    PSci();
    
    // Public interface
    void setup();  // Call in Arduino setup()
    void run();    // Call in Arduino loop()
    
    // Accessors
    String getSSID() const { return ssid; }
    String getAPIKey() const { return apiKey; }
    String getDeviceName() const { return deviceName; }
    bool isWiFiConnected() const { return wifiConnected; }
    bool isConnectedToApp() const { return connectedToApp; }
};

extern PSci psci; // Global instance

#endif // PSCI_H
