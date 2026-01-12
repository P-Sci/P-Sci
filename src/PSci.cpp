#include "PSci.h"

PSci psci;

// ===== SerialCapture Implementation =====
size_t SerialCapture::write(uint8_t c) {
    buffer.push(c);
    return Serial.write(c);
}

String SerialCapture::getBuffer() {
    String output;
    for (size_t i = 0; i < buffer.size(); i++) {
        output += buffer[i];
    }
    return output;
}

void SerialCapture::clearBuffer() {
    while (buffer.size() > 0) buffer.pop();
}

// ===== PSci Constructor =====
PSci::PSci() : server(80) {
    bootTime = millis();
}

// ===== Public Setup Function =====
void PSci::setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n🚀 P-Sci Library Initializing...");
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("❌ SPIFFS initialization failed!");
    }
    
    // Load WiFi credentials
    prefs.begin("psci", false);
    ssid = prefs.getString("ssid", "");
    password = prefs.getString("password", "");
    apiKey = prefs.getString("apiKey", "");
    deviceName = prefs.getString("deviceName", "ESP32-Device");
    
    if (ssid.length() == 0) {
        Serial.println("📡 No WiFi credentials found. Starting AP mode...");
        startAPMode();
    } else {
        connectToWiFi();
    }
    
    // Setup mDNS
    if (!MDNS.begin("p-sci")) {
        Serial.println("❌ mDNS initialization failed!");
    } else {
        Serial.println("✅ mDNS started: http://p-sci.local");
        MDNS.addService("http", "tcp", 80);
    }
    
    // Setup web dashboard
    setupDashboard();
    
    Serial.println("✅ P-Sci Library initialized!");
}

// ===== Public Run Function =====
void PSci::run() {
    server.handleClient();
    dnsServer.processNextRequest();
    
    // Check WiFi connection periodically
    if (millis() - lastConnectAttempt > connectInterval) {
        checkWiFiConnection();
        lastConnectAttempt = millis();
    }
    
    // Send heartbeat to app
    if (wifiConnected && connectedToApp && (millis() - lastHeartbeat > 30000)) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    // Check for task instructions
    if (wifiConnected && connectedToApp && (millis() - lastTaskCheck > 20000)) {
        checkForTaskInstructions();
        lastTaskCheck = millis();
    }
}

// ===== WiFi Setup =====
void PSci::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.printf("📡 Connecting to WiFi: %s\n", ssid.c_str());
}

void PSci::connectToWiFi() {
    setupWiFi();
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.printf("\n✅ Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("\n❌ WiFi connection failed!");
        wifiConnected = false;
    }
}

void PSci::startAPMode() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP("P-Sci_Config", "12345678");
    
    Serial.println("📡 AP Mode started!");
    Serial.println("SSID: P-Sci_Config");
    Serial.println("Password: 12345678");
    Serial.println("IP: 192.168.4.1");
    
    dnsServer.start(53, "*", WiFi.softAPIP());
    
    // Setup AP routes
    server.on("/", [this]() { handleRoot(); });
    server.on("/save", HTTP_GET, [this]() { handleSaveWiFi(); });
    server.onNotFound([this]() { handleNotFound(); });
    
    server.begin();
    addToMonitorBuffer("🌐 AP Mode Active: http://192.168.4.1");
}

void PSci::handleSaveWiFi() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
        ssid = server.arg("ssid");
        password = server.arg("password");
        
        prefs.putString("ssid", ssid);
        prefs.putString("password", password);
        
        String response = "<html><body style='font-family:Arial;text-align:center;padding:50px;'>";
        response += "<h1>✅ Saved!</h1>";
        response += "<p>Connecting to WiFi...</p>";
        response += "<script>setTimeout(() => location.href = 'http://p-sci.local', 3000);</script>";
        response += "</body></html>";
        
        server.send(200, "text/html", response);
        delay(3000);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Missing SSID or Password");
    }
}

void PSci::handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>P-Sci Setup</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
            max-width: 400px;
            width: 100%;
        }
        h1 {
            text-align: center;
            color: #fff;
            margin-bottom: 30px;
            font-size: 28px;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            color: #e0e0e0;
            margin-bottom: 8px;
            font-weight: 500;
        }
        input {
            width: 100%;
            padding: 12px;
            border: 1px solid rgba(255, 255, 255, 0.3);
            background: rgba(255, 255, 255, 0.05);
            border-radius: 10px;
            color: #fff;
            font-size: 16px;
            transition: all 0.3s ease;
        }
        input:focus {
            outline: none;
            background: rgba(255, 255, 255, 0.1);
            border-color: rgba(255, 255, 255, 0.5);
            box-shadow: 0 0 10px rgba(255, 255, 255, 0.2);
        }
        input::placeholder {
            color: rgba(255, 255, 255, 0.5);
        }
        button {
            width: 100%;
            padding: 12px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-top: 20px;
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 20px rgba(102, 126, 234, 0.4);
        }
        button:active {
            transform: translateY(0);
        }
        .status {
            text-align: center;
            color: #b0b0b0;
            font-size: 14px;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔧 P-Sci Setup</h1>
        <form method="GET" action="/save">
            <div class="form-group">
                <label for="ssid">WiFi Network</label>
                <input type="text" id="ssid" name="ssid" placeholder="Enter WiFi SSID" required>
            </div>
            <div class="form-group">
                <label for="password">WiFi Password</label>
                <input type="password" id="password" name="password" placeholder="Enter WiFi Password" required>
            </div>
            <button type="submit">Save & Connect</button>
        </form>
        <div class="status">
            <p>After saving, the device will reboot and connect to your WiFi.</p>
        </div>
    </div>
</body>
</html>
    )rawliteral";
    
    server.send(200, "text/html", html);
}

void PSci::handleNotFound() {
    String url = server.uri();
    if (url != "/favicon.ico") {
        server.sendHeader("Location", "http://192.168.4.1/");
        server.send(302, "text/plain", "Redirecting to setup page...");
    }
}

// ===== Placeholder Methods =====
void PSci::checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED && ssid.length() > 0) {
        connectToWiFi();
    }
}

void PSci::sendHeartbeat() {
    // Implementation for heartbeat
    Serial.println("💓 Heartbeat sent");
}

void PSci::checkForTaskInstructions() {
    // Implementation for task checking
}

void PSci::addToMonitorBuffer(const String &message) {
    if (monitorIndex < 100) {
        monitorBuffer[monitorIndex++] = message;
    }
}

void PSci::validateProject(JsonObject validation) {
    // Placeholder
}

bool PSci::validateDigitalToggle(int pin, int expectedPeriod, int tolerance, int samples) {
    return false; // Placeholder
}

void PSci::reportTaskResult(const String &taskId, const String &moduleId, bool success, JsonObject validationData) {
    // Placeholder
}

void PSci::updateSystemStats() {
    // Placeholder
}

void PSci::setupDashboard() {
    // Setup all web routes for the dashboard
    server.on("/", [this]() { handleRoot(); });
    server.onNotFound([this]() { handleNotFound(); });
    server.begin();
}
