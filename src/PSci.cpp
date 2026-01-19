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

// new 
// ----------------------------------- FUTURISTIC DASHBOARD -----------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Control Panel</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Helvetica Neue', sans-serif;
            background: linear-gradient(135deg, #0f0f1e 0%, #1a0f2e 100%);
            color: #e0e0e0;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .container {
            width: 100%;
            max-width: 900px;
            background: rgba(20, 15, 35, 0.85);
            backdrop-filter: blur(20px);
            border: 1px solid rgba(200, 100, 200, 0.2);
            border-radius: 24px;
            padding: 30px;
            box-shadow: 0 8px 32px rgba(200, 100, 200, 0.1);
        }

        .header {
            text-align: center;
            margin-bottom: 30px;
        }

        .header h1 {
            font-size: 28px;
            font-weight: 700;
            margin-bottom: 8px;
            background: linear-gradient(135deg, #d4689f 0%, #c75a9a 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }

        .header p {
            font-size: 14px;
            color: #999;
        }

        .tab-navigation {
            display: flex;
            gap: 12px;
            background: rgba(255, 255, 255, 0.05);
            padding: 12px;
            border-radius: 50px;
            margin-bottom: 30px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(200, 100, 200, 0.15);
        }

        .tab-btn {
            flex: 1;
            padding: 14px 24px;
            border: none;
            background: transparent;
            color: #999;
            font-size: 14px;
            font-weight: 600;
            cursor: pointer;
            border-radius: 40px;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }

        .tab-btn:hover {
            color: #d4689f;
            background: rgba(212, 104, 159, 0.1);
        }

        .tab-btn.active {
            background: linear-gradient(135deg, rgba(212, 104, 159, 0.3) 0%, rgba(199, 90, 154, 0.3) 100%);
            color: #d4689f;
            border: 1px solid rgba(212, 104, 159, 0.5);
            box-shadow: 0 4px 12px rgba(212, 104, 159, 0.2);
        }

        .tab-content {
            display: none;
            animation: fadeIn 0.3s ease;
        }

        .tab-content.active {
            display: block;
        }

        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 16px;
            margin-bottom: 24px;
        }

        .stat-card {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(200, 100, 200, 0.15);
            border-radius: 16px;
            padding: 16px;
            backdrop-filter: blur(10px);
        }

        .stat-label {
            font-size: 12px;
            color: #999;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            margin-bottom: 8px;
        }

        .stat-value {
            font-size: 20px;
            font-weight: 700;
            color: #d4689f;
        }

        .monitor-section {
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(200, 100, 200, 0.15);
            border-radius: 16px;
            padding: 16px;
            margin-bottom: 20px;
        }

        .monitor-title {
            font-size: 14px;
            font-weight: 600;
            color: #d4689f;
            margin-bottom: 12px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        .monitor-output {
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid rgba(200, 100, 200, 0.1);
            border-radius: 12px;
            padding: 12px;
            height: 200px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 12px;
            color: #4ade80;
            line-height: 1.5;
        }

        .button-group {
            display: flex;
            gap: 12px;
            margin-top: 20px;
        }

        .btn {
            flex: 1;
            padding: 12px 20px;
            border: none;
            border-radius: 12px;
            font-weight: 600;
            font-size: 14px;
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .btn-primary {
            background: linear-gradient(135deg, #d4689f 0%, #c75a9a 100%);
            color: white;
            border: 1px solid rgba(212, 104, 159, 0.8);
        }

        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 16px rgba(212, 104, 159, 0.3);
        }

        .btn-primary:active {
            transform: translateY(0);
        }

        .btn-secondary {
            background: rgba(212, 104, 159, 0.15);
            color: #d4689f;
            border: 1px solid rgba(212, 104, 159, 0.4);
        }

        .btn-secondary:hover {
            background: rgba(212, 104, 159, 0.25);
        }

        .controls-list {
            display: flex;
            flex-direction: column;
            gap: 16px;
        }

        .control-item {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(200, 100, 200, 0.15);
            border-radius: 16px;
            padding: 16px;
            display: flex;
            align-items: center;
            gap: 16px;
            backdrop-filter: blur(10px);
        }

        .control-info {
            flex: 1;
        }

        .control-label {
            font-weight: 600;
            margin-bottom: 4px;
        }

        .control-type {
            font-size: 12px;
            color: #999;
        }

        .control-input {
            width: 120px;
            padding: 8px 12px;
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(200, 100, 200, 0.2);
            border-radius: 8px;
            color: #d4689f;
            font-weight: 600;
        }

        .control-input:focus {
            outline: none;
            border-color: #d4689f;
            background: rgba(0, 0, 0, 0.5);
        }

        .slider {
            width: 150px;
            height: 6px;
            border-radius: 3px;
            background: rgba(200, 100, 200, 0.2);
            outline: none;
            -webkit-appearance: none;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: linear-gradient(135deg, #d4689f 0%, #c75a9a 100%);
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .slider::-webkit-slider-thumb:hover {
            transform: scale(1.2);
            box-shadow: 0 0 12px rgba(212, 104, 159, 0.4);
        }

        .slider::-moz-range-thumb {
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: linear-gradient(135deg, #d4689f 0%, #c75a9a 100%);
            cursor: pointer;
            border: none;
            transition: all 0.2s ease;
        }

        .control-actions {
            display: flex;
            gap: 8px;
        }

        .btn-small {
            padding: 8px 16px;
            font-size: 12px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .btn-toggle {
            background: rgba(212, 104, 159, 0.2);
            color: #d4689f;
            border: 1px solid rgba(212, 104, 159, 0.4);
        }

        .btn-toggle.on {
            background: rgba(74, 222, 128, 0.2);
            color: #4ade80;
            border-color: rgba(74, 222, 128, 0.4);
        }

        .btn-delete {
            background: rgba(239, 68, 68, 0.15);
            color: #ef4444;
            border: 1px solid rgba(239, 68, 68, 0.3);
        }

        .btn-delete:hover {
            background: rgba(239, 68, 68, 0.25);
        }

        .pin-selector {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(80px, 1fr));
            gap: 8px;
            margin-bottom: 16px;
        }

        .pin-option {
            padding: 10px;
            text-align: center;
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(200, 100, 200, 0.2);
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.2s ease;
            font-size: 12px;
        }

        .pin-option:hover {
            border-color: #d4689f;
        }

        .pin-option.selected {
            background: rgba(212, 104, 159, 0.2);
            border-color: #d4689f;
            color: #d4689f;
        }

        .type-selector {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(100px, 1fr));
            gap: 8px;
            margin-bottom: 16px;
        }

        .type-option {
            padding: 10px;
            text-align: center;
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(200, 100, 200, 0.2);
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.2s ease;
            font-size: 12px;
        }

        .type-option:hover {
            border-color: #d4689f;
        }

        .type-option.selected {
            background: rgba(212, 104, 159, 0.2);
            border-color: #d4689f;
            color: #d4689f;
        }

        .add-control-section {
            background: rgba(255, 255, 255, 0.05);
            border: 2px dashed rgba(200, 100, 200, 0.3);
            border-radius: 16px;
            padding: 20px;
            margin-bottom: 20px;
        }

        .form-group {
            margin-bottom: 16px;
        }

        .form-label {
            display: block;
            font-size: 12px;
            font-weight: 600;
            color: #d4689f;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            margin-bottom: 8px;
        }

        .form-input {
            width: 100%;
            padding: 10px 12px;
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(200, 100, 200, 0.2);
            border-radius: 8px;
            color: #e0e0e0;
            font-size: 14px;
        }

        .form-input:focus {
            outline: none;
            border-color: #d4689f;
            background: rgba(0, 0, 0, 0.5);
        }

        .status-badge {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 20px;
            font-size: 12px;
            font-weight: 600;
        }

        .status-online {
            background: rgba(74, 222, 128, 0.2);
            color: #4ade80;
        }

        .status-offline {
            background: rgba(239, 68, 68, 0.2);
            color: #ef4444;
        }

        @media (max-width: 640px) {
            .container {
                padding: 20px;
            }

            .header h1 {
                font-size: 22px;
            }

            .tab-btn {
                padding: 10px 16px;
                font-size: 12px;
            }

            .stats-grid {
                grid-template-columns: 1fr;
            }

            .button-group {
                flex-direction: column;
            }

            .control-item {
                flex-direction: column;
                align-items: flex-start;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>P-Sci Dashboard</h1>
            <p><span class="status-badge" id="statusBadge">Connecting...</span></p>
        </div>

        <div class="tab-navigation">
            <button class="tab-btn active" onclick="switchTab('monitor')">
                <span>📊 Monitor</span>
            </button>
            <button class="tab-btn" onclick="switchTab('control')">
                <span>🎮 Control</span>
            </button>
        </div>

        <!-- Monitor Tab -->
        <div id="monitor" class="tab-content active">
            <div class="stats-grid">
                <div class="stat-card">
                    <div class="stat-label">Device Name</div>
                    <div class="stat-value" id="deviceName">Loading...</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">IP Address</div>
                    <div class="stat-value" id="ipAddress">--</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">WiFi Status</div>
                    <div class="stat-value" id="wifiStatus">--</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">App Connection</div>
                    <div class="stat-value" id="appConnection">--</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Last Heartbeat</div>
                    <div class="stat-value" id="heartbeat">--</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Sensor Data</div>
                    <div class="stat-value" id="sensorData">--</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Uptime</div>
                    <div class="stat-value" id="uptime">0s</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Free Memory</div>
                    <div class="stat-value" id="freeMemory">0 KB</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">CPU Temp</div>
                    <div class="stat-value" id="cpuTemp">--°C</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">WiFi Signal</div>
                    <div class="stat-value" id="wifiSignal">--%</div>
                </div>
            </div>

            <div class="monitor-section">
                <div class="monitor-title">Serial Monitor Output</div>
                <div class="monitor-output" id="serialMonitor">
Waiting for connection...
                </div>
            </div>

            <div class="button-group">
                <button class="btn btn-primary" onclick="updateFirmware()">
                    <span>Check for Updates</span>
                </button>
                <button class="btn btn-secondary" onclick="clearMonitor()">
                    Clear Log
                </button>
            </div>
        </div>

        <!-- Control Tab -->
        <div id="control" class="tab-content">
            <div class="add-control-section">
                <h3 style="color: #d4689f; margin-bottom: 16px; font-size: 16px;">Add New Pin Control</h3>

                <div class="form-group">
                    <label class="form-label">Control Name</label>
                    <input type="text" id="controlName" class="form-input" placeholder="e.g., LED 1, Motor Speed">
                </div>

                <div class="form-group">
                    <label class="form-label">Select Pin</label>
                    <div class="pin-selector" id="pinSelector"></div>
                </div>

                <div class="form-group">
                    <label class="form-label">Control Type</label>
                    <div class="type-selector" id="typeSelector"></div>
                </div>

                <button class="btn btn-primary" onclick="addControl()" style="width: 100%;">Add Control</button>
            </div>

            <div class="controls-list" id="controlsList"></div>
        </div>
    </div>

    <script>
        const AVAILABLE_PINS = [2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33];
        const CONTROL_TYPES = [
            { id: 'digital', label: 'Digital Output', icon: '⚡' },
            { id: 'pwm', label: 'PWM Output', icon: '💡' },
            { id: 'input', label: 'Digital Input', icon: '📥' }
        ];

        let controls = [];
        let selectedPin = null;
        let selectedType = null;

        document.addEventListener('DOMContentLoaded', function() {
            initializePinSelector();
            initializeTypeSelector();
            loadControls();
            updateSystemStats();
            refreshSystemInfo();
            setInterval(updateSystemStats, 5000);
            setInterval(refreshSystemInfo, 5000);
            startMonitoringSerial();
        });

        function refreshSystemInfo() {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('deviceName').textContent = data.deviceName || 'Unknown';
                    document.getElementById('ipAddress').textContent = data.ip || '--';
                    document.getElementById('wifiStatus').textContent = data.wifi || '--';
                    document.getElementById('appConnection').textContent = data.app || '--';
                    document.getElementById('heartbeat').textContent = data.heartbeat || '--';
                    document.getElementById('sensorData').textContent = data.sensor || '--';
                    
                    const badge = document.getElementById('statusBadge');
                    if (data.wifi && data.wifi.includes('Connected')) {
                        badge.className = 'status-badge status-online';
                        badge.textContent = 'Online';
                    } else {
                        badge.className = 'status-badge status-offline';
                        badge.textContent = 'Offline';
                    }
                })
                .catch(err => console.log('[v0] Info fetch error:', err));
        }

        function initializePinSelector() {
            const selector = document.getElementById('pinSelector');
            AVAILABLE_PINS.forEach(pin => {
                const btn = document.createElement('button');
                btn.className = 'pin-option';
                btn.textContent = `GPIO ${pin}`;
                btn.onclick = () => selectPin(pin, btn);
                selector.appendChild(btn);
            });
        }

        function initializeTypeSelector() {
            const selector = document.getElementById('typeSelector');
            CONTROL_TYPES.forEach(type => {
                const btn = document.createElement('button');
                btn.className = 'type-option';
                btn.textContent = `${type.icon} ${type.label}`;
                btn.onclick = () => selectType(type.id, btn);
                selector.appendChild(btn);
            });
        }

        function selectPin(pin, element) {
            document.querySelectorAll('.pin-option').forEach(el => el.classList.remove('selected'));
            element.classList.add('selected');
            selectedPin = pin;
        }

        function selectType(type, element) {
            document.querySelectorAll('.type-option').forEach(el => el.classList.remove('selected'));
            element.classList.add('selected');
            selectedType = type;
        }

        function addControl() {
            const name = document.getElementById('controlName').value;

            if (!name || !selectedPin || !selectedType) {
                alert('Please fill in all fields');
                return;
            }

            const control = {
                id: Date.now(),
                name: name,
                pin: selectedPin,
                type: selectedType,
                value: selectedType === 'pwm' ? 128 : 0
            };

            controls.push(control);
            saveControls();
            renderControls();

            document.getElementById('controlName').value = '';
            document.querySelectorAll('.pin-option').forEach(el => el.classList.remove('selected'));
            document.querySelectorAll('.type-option').forEach(el => el.classList.remove('selected'));
            selectedPin = null;
            selectedType = null;
        }

        function renderControls() {
            const list = document.getElementById('controlsList');
            list.innerHTML = '';

            if (controls.length === 0) {
                list.innerHTML = '<p style="text-align: center; color: #999;">No controls added yet. Add one above!</p>';
                return;
            }

            controls.forEach(control => {
                const item = document.createElement('div');
                item.className = 'control-item';

                let controlHTML = `
                    <div class="control-info">
                        <div class="control-label">${control.name}</div>
                        <div class="control-type">GPIO ${control.pin} • ${CONTROL_TYPES.find(t => t.id === control.type).label}</div>
                    </div>
                `;

                if (control.type === 'digital') {
                    const isOn = control.value === 1;
                    controlHTML += `
                        <button class="btn-small btn-toggle ${isOn ? 'on' : ''}" onclick="toggleDigital(${control.id})">
                            ${isOn ? 'ON' : 'OFF'}
                        </button>
                    `;
                } else if (control.type === 'pwm') {
                    controlHTML += `
                        <input type="range" min="0" max="255" value="${control.value}" class="slider" 
                            oninput="updatePWM(${control.id}, this.value)">
                        <span style="color: #999; font-size: 12px; min-width: 50px; text-align: right;">${control.value}</span>
                    `;
                } else if (control.type === 'input') {
                    controlHTML += `
                        <div style="color: #999; font-size: 12px;">State: <span id="input-${control.id}">--</span></div>
                    `;
                }

                controlHTML += `
                    <button class="btn-small btn-delete" onclick="deleteControl(${control.id})">Delete</button>
                `;

                item.innerHTML = controlHTML;
                list.appendChild(item);
            });
        }

        function toggleDigital(id) {
            const control = controls.find(c => c.id === id);
            if (control) {
                control.value = control.value === 0 ? 1 : 0;
                sendPinCommand(control.pin, control.type, control.value);
                renderControls();
            }
        }

        function updatePWM(id, value) {
            const control = controls.find(c => c.id === id);
            if (control) {
                control.value = parseInt(value);
                sendPinCommand(control.pin, control.type, control.value);
            }
        }

        function deleteControl(id) {
            controls = controls.filter(c => c.id !== id);
            saveControls();
            renderControls();
        }

        function sendPinCommand(pin, type, value) {
            fetch(`/api/pin?pin=${pin}&type=${type}&value=${value}`)
                .catch(err => console.log('[v0] Pin command sent:', pin, type, value));
        }

        function switchTab(tabName) {
            document.querySelectorAll('.tab-content').forEach(tab => tab.classList.remove('active'));
            document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));

            document.getElementById(tabName).classList.add('active');
            event.target.classList.add('active');
        }

        function updateSystemStats() {
            fetch('/api/stats')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('uptime').textContent = formatUptime(data.uptime || 0);
                    document.getElementById('freeMemory').textContent = (data.freeMemory || 0) + ' KB';
                    document.getElementById('cpuTemp').textContent = (data.cpuTemp || 0).toFixed(1) + '°C';
                    document.getElementById('wifiSignal').textContent = (data.wifiSignal || 0) + '%';
                })
                .catch(err => console.log('[v0] Stats fetch error:', err));
        }

        function formatUptime(ms) {
            const seconds = Math.floor(ms / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);

            if (days > 0) return days + 'd ' + (hours % 24) + 'h';
            if (hours > 0) return hours + 'h ' + (minutes % 60) + 'm';
            if (minutes > 0) return minutes + 'm ' + (seconds % 60) + 's';
            return seconds + 's';
        }

        function updateFirmware() {
            alert('Firmware update initiated. Check the Serial Monitor for progress.');
            fetch('/api/update').catch(err => console.log('[v0] Update request sent'));
        }

        function clearMonitor() {
            document.getElementById('serialMonitor').textContent = '';
            fetch('/api/serial/clear').catch(err => console.log('[v0] Clear serial buffer request failed'));
        }

        function startMonitoringSerial() {
            const monitor = document.getElementById('serialMonitor');
            let lastContent = '';
            
            setInterval(() => {
                fetch('/api/serial')
                    .then(r => r.text())
                    .then(data => {
                        if (data && data !== lastContent) {
                            monitor.textContent = data;
                            lastContent = data;
                            monitor.scrollTop = monitor.scrollHeight;
                        } else if (data === "" && monitor.textContent === "Waiting for serial output...\n") {
                            // If the server sends an empty string and the monitor is showing the initial message, keep it.
                        } else if (data !== lastContent) {
                           // If data changed but it's empty, clear the monitor.
                           // This condition might be redundant if the server always sends a message like "Waiting..." when empty.
                           monitor.textContent = data;
                           lastContent = data;
                           monitor.scrollTop = monitor.scrollHeight;
                        }
                    })
                    .catch(err => {});
            }, 500);  // Increased frequency from 1000ms to 500ms for better real-time
        }

        function saveControls() {
            localStorage.setItem('espControls', JSON.stringify(controls));
        }

        function loadControls() {
            const saved = localStorage.getItem('espControls');
            if (saved) {
                controls = JSON.parse(saved);
                renderControls();
            }
        }
    </script>
</body>
</html>
)rawliteral";



// ------------------------------------ START AP MODE WITH CAPTIVE PORTAL -----------
void PSci::startAPMode() {
  Serial.println("⚙️ Starting Access Point Mode...");
  
  // Stop any existing server
  server.stop();
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("P-Sci_Config", "12345678");
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  Serial.print("📶 AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("🌐 Access setup at: http://192.168.4.1");
  Serial.println("📱 Connect to WiFi 'P-Sci_Config', password: '12345678'");

  dnsServer.start(53, "*", apIP);

  String setupHTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="UTF-8">
  <title>P-Sci Setup</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #0f0f1e 0%, #1a0f2e 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .container {
      background: rgba(20, 15, 35, 0.95);
      backdrop-filter: blur(10px);
      border: 1px solid rgba(212, 104, 159, 0.3);
      border-radius: 16px;
      padding: 40px;
      width: 100%;
      max-width: 400px;
      box-shadow: 0 8px 32px rgba(212, 104, 159, 0.2);
    }
    h1 {
      color: #d4689f;
      margin-bottom: 10px;
      font-size: 24px;
      text-align: center;
    }
    .subtitle {
      color: #999;
      text-align: center;
      margin-bottom: 30px;
      font-size: 13px;
    }
    form {
      display: flex;
      flex-direction: column;
      gap: 15px;
    }
    label {
      color: #b0b0b0;
      font-size: 12px;
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    input {
      padding: 12px 15px;
      border: 1px solid rgba(212, 104, 159, 0.3);
      border-radius: 8px;
      background: rgba(0, 0, 0, 0.3);
      color: #e0e0e0;
      font-size: 14px;
      transition: all 0.3s ease;
    }
    input:focus {
      outline: none;
      border-color: #d4689f;
      background: rgba(0, 0, 0, 0.5);
      box-shadow: 0 0 10px rgba(212, 104, 159, 0.2);
    }
    button {
      padding: 12px;
      background: linear-gradient(135deg, #d4689f 0%, #c75a9a 100%);
      color: white;
      border: none;
      border-radius: 8px;
      font-weight: 600;
      font-size: 14px;
      cursor: pointer;
      transition: all 0.3s ease;
      margin-top: 10px;
    }
    button:hover {
      transform: translateY(-2px);
      box-shadow: 0 8px 16px rgba(212, 104, 159, 0.4);
    }
    button:active {
      transform: translateY(0);
    }
    .info-box {
      background: rgba(212, 104, 159, 0.1);
      border-left: 3px solid #d4689f;
      padding: 12px;
      margin-top: 20px;
      border-radius: 4px;
      font-size: 12px;
      color: #b0b0b0;
    }
    .info-box strong {
      color: #d4689f;
    }
    .footer {
      text-align: center;
      margin-top: 20px;
      font-size: 11px;
      color: #666;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>⚡ P-Sci Setup</h1>
    <div class="subtitle">Configure your ESP32 device</div>
    
    <form action="/save" method="POST">
      <div>
        <label for="deviceName">Device Name</label>
        <input type="text" id="deviceName" name="deviceName" placeholder="e.g., ESP32-Lab" required>
      </div>
      
      <div>
        <label for="ssid">WiFi SSID</label>
        <input type="text" id="ssid" name="ssid" placeholder="WiFi network name" required>
      </div>
      
      <div>
        <label for="password">WiFi Password</label>
        <input type="password" id="password" name="password" placeholder="WiFi password" required>
      </div>
      
      <div>
        <label for="apiKey">API Key</label>
        <input type="text" id="apiKey" name="apiKey" placeholder="Your API key" required>
      </div>
      
      <button type="submit">Save & Connect</button>
    </form>
    
    <div class="info-box">
      Currently connected to: <strong>P-Sci_Config</strong><br>
      Password: <strong>12345678</strong>
    </div>
    
    <div class="footer">
      Device will reboot after configuration
    </div>
  </div>
</body>
</html>
  )rawliteral";

  server.on("/", HTTP_GET, [this, setupHTML]() {
    server.send(200, "text/html; charset=UTF-8", setupHTML);
  });

  server.on("/save", HTTP_POST, [this, setupHTML]() {
    if (server.hasArg("deviceName") && server.hasArg("ssid") && 
        server.hasArg("password") && server.hasArg("apiKey")) {
      
      deviceName = server.arg("deviceName");
      ssid = server.arg("ssid");
      password = server.arg("password");
      apiKey = server.arg("apiKey");
      
      prefs.putString("deviceName", deviceName);
      prefs.putString("ssid", ssid);
      prefs.putString("password", password);
      prefs.putString("apiKey", apiKey);
      
      Serial.println("✅ Credentials saved!");
      Serial.printf("   Device: %s\n", deviceName.c_str());
      Serial.printf("   WiFi: %s\n", ssid.c_str());
      Serial.println("🔄 Rebooting in 2 seconds...");
      
      String successHTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Configuration Saved</title>
  <style>
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #0f0f1e 0%, #1a0f2e 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .container {
      background: rgba(20, 15, 35, 0.95);
      border: 1px solid rgba(74, 222, 128, 0.3);
      border-radius: 16px;
      padding: 40px;
      text-align: center;
      max-width: 400px;
    }
    h2 {
      color: #4ade80;
      margin-bottom: 10px;
    }
    p {
      color: #b0b0b0;
      margin: 10px 0;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>✅ Configuration Saved!</h2>
    <p>Connecting to WiFi...</p>
    <p style="font-size: 12px; color: #666;">This page will close shortly</p>
  </div>
</body>
</html>
      )rawliteral";
      
      server.send(200, "text/html; charset=UTF-8", successHTML);
      
      delay(2000);
      dnsServer.stop();
      server.stop();
      ESP.restart();
    } else {
      server.send(400, "text/plain", "Missing credentials");
    }
  });

  server.onNotFound([this, setupHTML]() {
    String uri = server.uri();
    // Common captive portal detection requests
    if (uri == "/generate_204" || uri == "/redirect" || uri == "/fwlink" || 
        uri == "/connecttest.txt" || uri == "/success.txt") {
      server.send(204, "text/plain", "");
      return;
    }
    // Redirect everything else to setup page
    server.sendHeader("Location", String("http://192.168.4.1/"), true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  Serial.println("🌐 Web server started on AP mode");
}

// ------------------------------------ CONNECT TO WIFI + APP -----------
void PSci::connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("📡 Connecting to Wi-Fi (%s)...", ssid.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    bootTime = millis(); // Initialize bootTime on successful WiFi connection
    Serial.println("✅ Wi-Fi Connected!");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    
    if (!MDNS.begin("p-sci")) {
      Serial.println("⚠️ mDNS initialization failed, retrying...");
      delay(500);
      if (!MDNS.begin("p-sci")) {
        Serial.println("❌ mDNS failed after retry");
      } else {
        MDNS.addService("http", "tcp", 80);
        Serial.println("✅ mDNS ready! Access at http://p-sci.local");
      }
    } else {
      MDNS.addService("http", "tcp", 80);
      Serial.println("✅ mDNS ready! Access at http://p-sci.local");
      Serial.println("📱 Mobile phone can connect via: http://p-sci.local or http://" + WiFi.localIP().toString());
    }
    
    addMonitorLine("WiFi connected to: " + ssid); // Add to monitor buffer
    connectToApp();
    // Removed the call to setupDashboard() here as it's now handled by setupWebRoutes()
  } else {
    Serial.println("❌ Wi-Fi connection failed. Switching to AP mode.");
    addMonitorLine("WiFi connection failed");
    startAPMode(); // startAPMode will now handle DNS and server setup for the portal

  }
}



//-------------------------------------Connect to app----------
void PSci::connectToApp() {

    HTTPClient http;
    http.begin(CONNECT_URL);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(256);
    doc["apiKey"] = apiKey;
    doc["deviceName"] = deviceName;
    String body;
    serializeJson(doc, body);
    int code = http.POST(body);
    String res = http.getString();
    http.end();

    if (code == 200) {
      connectedToApp = true;
      lastHeartbeat = millis(); // Update lastHeartbeat on successful connection
      Serial.println("🟢 Connected to P-Sci App!");
      Serial.println(res);
      addMonitorLine("App connection established"); // Add to monitor buffer
    } else {
      connectedToApp = false;
      Serial.printf("❌ App connection failed! Code: %d\n", code);
    }
}
// ================================== HEARTBEAT ========
void PSci::sendHeartbeat() {

      HTTPClient http;
    String url = HEARTBEAT_URL + "?apiKey=" + apiKey;
    http.begin(url);
    int code = http.GET();
    String res = http.getString();
    http.end();
    Serial.printf("💓 Heartbeat [%d]: %s\n", code, res.c_str());
    lastHeartbeat = millis(); // Update last heartbeat time
}

// -------------------------- FETCH TASK INSTRUCTIONS -----------
void PSci::fetchTaskInstructions() {
  if (!wifiConnected || !connectedToApp) {
    Serial.println("⚠️ Skipping task fetch — not connected to app.");
    return;
  }

  HTTPClient http;
  http.begin(TASK_URL);
  http.addHeader("X-API-Key", apiKey);   // authenticate this ESP device
  http.addHeader("Content-Type", "application/json");

  int code = http.GET();
  String res = http.getString();

  Serial.printf("📡 Task Fetch [%d]: %s\n", code, res.c_str());

  if (code == 200) {
    // Try to parse JSON response
    StaticJsonDocument<1024> doc;
    DeserializationError err = deserializeJson(doc, res);
    if (err) {
      Serial.print("❌ JSON parse failed: ");
      Serial.println(err.c_str());
    } else {
      bool success = doc["success"] | false;
      bool hasTask = doc["has_task"] | false;

if (success && hasTask) {
  Serial.println("✅ New Task Received!");
  Serial.printf("   Task ID: %s\n", doc["task"]["task_id"] | "none");
  Serial.printf("   Module ID: %s\n", doc["task"]["module_id"] | "none");
  Serial.printf("   Started At: %s\n", doc["task"]["started_at"] | "none");

  // Handle nested "instructions" object properly
if (doc["task"]["instructions"].is<JsonObject>()) {
  JsonObject instr = doc["task"]["instructions"].as<JsonObject>();
  Serial.println("   Instructions received ✅");
  serializeJsonPretty(instr, Serial);
  Serial.println("\n🔎 Passing to validator...");
  validateProject(instr);
}
 else if (doc["task"]["instructions"].is<const char*>()) {
    // In case some tasks still return instructions as plain text
    Serial.printf("   Instructions: %s\n", doc["task"]["instructions"] | "none");
  } else {
    Serial.println("   Instructions: (unknown format)");
  }
}
 else {
        Serial.println("ℹ️ No pending tasks available.");
      }
    }
  } else {
    Serial.printf("❌ Failed to fetch tasks (HTTP %d)\n", code);
  }

  http.end();
}




// ==========================================================================================================================================
//                                                                           🧠 VALIDATION SYSTEM
// ==================================================================================================================================================
void PSci::validateProject(JsonObject validation) {
  String type = validation["type"] | "unknown";
  Serial.printf("\n🔍 Validation type: %s\n", type.c_str());

if (type == "digital_toggle") {
    int pin = validation["pin"] | -1;
    int expectedPeriod = validation["expected_period_ms"] | 1000;
    int tolerance = validation["tolerance"] | 200;
    int samples = validation["samples"] | 5;

    if (pin < 0) {
      Serial.println("❌ Invalid pin number!");
      return;
    }

    bool result = validateDigitalToggle(pin, expectedPeriod, tolerance, samples);
    Serial.printf("✅ digital_toggle validation result: %s\n", result ? "PASS" : "FAIL") ;

    // ------------------------ Report to App
    // taskId & moduleId are expected to come from the task JSON
    String taskId = validation["taskId"] | "";
    String moduleId = validation["moduleId"] | "";

    if (taskId.length() > 0 && moduleId.length() > 0) {
      reportTaskResult(taskId, moduleId, result, validation);
    } else {
      Serial.println("⚠️ taskId/moduleId missing — cannot report to app.");
    }
}
 else if (type == "digital_state") {
  int pin = validation["pin"] | -1;
  int expected = validation["expected"] | 1; // 1 = HIGH, 0 = LOW

  if (pin < 0) {
    Serial.println("❌ Invalid pin number for digital_state!");
    return;
  }

  pinMode(pin, INPUT);
  delay(50); // small stabilization delay

  int state = digitalRead(pin);
  bool result = (state == expected);

  Serial.printf("🧩 digital_state check — pin: %d, read: %d, expected: %d → %s\n",
                pin, state, expected, result ? "PASS ✅" : "FAIL ❌");

  // ------------------------------------- Report to App-----------------------------------------------
  String taskId = validation["taskId"] | "";
  String moduleId = validation["moduleId"] | "";

  if (taskId.length() > 0 && moduleId.length() > 0) {
    // add extra result details
    validation["measured_state"] = state;
    validation["expected_state"] = expected;
    validation["pass"] = result;

    reportTaskResult(taskId, moduleId, result, validation);
  } else {
    Serial.println("⚠️ taskId/moduleId missing — cannot report to app.");
  }
}
 else if (type == "analog_range") {
    Serial.println("🕓 Placeholder for analog_range validation...");
  } else if (type == "sequence") {
    Serial.println("🕓 Placeholder for sequence validation...");
  } else {
    Serial.println("⚠️ Unknown validation type received!");
  }
}

// =========================================
// 🔁 digital_toggle VALIDATION LOGIC
// =========================================
bool PSci::validateDigitalToggle(int pin, int expectedPeriod, int tolerance, int samples) {
  pinMode(pin, INPUT);
  Serial.printf("🧩 Checking pin %d toggling around %d ms ± %d\n", pin, expectedPeriod, tolerance);

  unsigned long lastChange = millis();
  int lastState = digitalRead(pin);
  unsigned long intervals[samples];
  int count = 0;

  while (count < samples) {
    int state = digitalRead(pin);
    if (state != lastState) {
      unsigned long now = millis();
      unsigned long delta = now - lastChange;
      lastChange = now;
      if (delta > 10) { // filter noise
        intervals[count++] = delta;
        Serial.printf("⚡ Toggle %d: %lu ms\n", count, delta);
      }
      lastState = state;
    }
    delay(10);
  }

  unsigned long total = 0;
  for (int i = 0; i < samples; i++) total += intervals[i];
  float avg = total / (float)samples;

  Serial.printf("📏 Average toggle period: %.2f ms\n", avg);
  bool pass = abs((int)avg - expectedPeriod) <= tolerance;
  Serial.printf(pass ? "✅ Within tolerance!\n" : "❌ Out of tolerance!\n");

  return pass;
}
void PSci::reportTaskResult(const String &taskId, const String &moduleId, bool success, JsonObject validationData) {
  if (!wifiConnected || !connectedToApp) {
    Serial.println("⚠️ Cannot report task — not connected to app.");
    return;
  }

  HTTPClient http;
  http.begin(TASK_COMPLETE_URL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey); // Authenticate ESP device

  DynamicJsonDocument doc(512);
  doc["apiKey"] = apiKey;
  doc["taskId"] = taskId;
  doc["moduleId"] = moduleId;
  doc["success"] = success;

  // Embed validation data
  JsonObject vData = doc.createNestedObject("validationData");
  for (JsonPair kv : validationData) {
    vData[kv.key()] = kv.value();
  }

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  String res = http.getString();
  http.end();

  if (code == 200) {
    Serial.println("🟢 Task result reported successfully!");
    Serial.println(res);
  } else {
    Serial.printf("❌ Failed to report task result [%d]: %s\n", code, res.c_str());
  }
}






// -------------------------------------------------------------- MAIN SETUP --------------------------------------------------------------------------
void PSci::setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.setDebugOutput(false); // Disable ESP32's internal debug output to Serial
  
  Serial.println("🚀 Booting P-Sci ESP32...");

  prefs.begin("psciconfig", false);
  deviceName = prefs.getString("deviceName", "");
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("password", "");
  apiKey = prefs.getString("apiKey", "");

  if (ssid.isEmpty() || password.isEmpty() || apiKey.isEmpty() || deviceName.isEmpty()) {
    Serial.println("⚠️ No saved credentials. Starting AP Mode...");
    startAPMode();
  } else {
    Serial.println("✅ Credentials found. Connecting to WiFi...");
    connectToWiFi();
    setupWebRoutes();
  }
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

// ===== Public Run Function =====
void PSci::run() {
  server.handleClient();
    ArduinoOTA.handle();
  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.processNextRequest();
  }

  unsigned long now = millis();
  // ✅ Only send heartbeat if connected to app
 if (wifiConnected && connectedToApp && now - lastConnectAttempt > connectInterval) { // Modified condition to ensure connectedToApp
    connectToApp(); // Re-establish connection if lost
    sendHeartbeat();
    lastConnectAttempt = now;
  }

  if (wifiConnected && connectedToApp && now - lastTaskCheck > connectInterval) {
    fetchTaskInstructions();
    lastTaskCheck = now;
  }

  yield();
}

void PSci::setupWebRoutes() {
    server.on("/", HTTP_GET, [this](){
        server.send(200, "text/html", index_html);
    });
    
    server.on("/status", HTTP_GET, [this](){
        DynamicJsonDocument doc(512);
        doc["deviceName"] = deviceName;
        doc["wifi"] = wifiConnected ? "Connected ✅" : "Disconnected ❌";
        doc["app"] = connectedToApp ? "Linked ✅" : "Not Linked ❌";
        doc["ip"] = WiFi.localIP().toString();
        doc["heartbeat"] = String((millis() - lastHeartbeat) / 1000) + " sec ago";
        doc["sensor"] = sensorData;
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    });
    
    server.on("/api/stats", HTTP_GET, [this](){ handleGetStats(); });
    
    // Added handler for clearing the serial buffer
    server.on("/api/serial/clear", HTTP_GET, [this]() {
        serialCapture.clearBuffer();
        server.send(200, "text/plain", "Serial buffer cleared");
    });
    
    server.on("/api/serial", HTTP_GET, [this](){ handleGetSerial(); });
    server.on("/api/pin", HTTP_GET, [this](){ handlePinControl(); });
    server.on("/api/update", HTTP_GET, [this](){ handleCheckUpdate(); });
    
    server.onNotFound([this](){ handleNotFound(); });
    
    server.begin();
    Serial.println("🌐 Web server started!");
}

void PSci::handleGetStats() {
    DynamicJsonDocument doc(256);
    doc["uptime"] = millis() - bootTime; // Use bootTime for accurate uptime
    doc["freeMemory"] = ESP.getFreeHeap() / 1024;
    doc["cpuTemp"] = readCoreTemperature();
    doc["wifiSignal"] = constrain(map(WiFi.RSSI(), -100, -30, 0, 100), 0, 100);
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
} 

void PSci::handleGetSerial() {
    String output = serialCapture.getBuffer();
    if (output.isEmpty()) {
        output = "Waiting for serial output...\n";
    }
    server.send(200, "text/plain", output);
}

void PSci::handlePinControl() {
    if (server.hasArg("pin") && server.hasArg("type") && server.hasArg("value")) {
        int pin = server.arg("pin").toInt();
        String type = server.arg("type");
        int value = server.arg("value").toInt();
        
        controlPin(pin, type, value);
        addMonitorLine("Pin " + String(pin) + " (" + type + ") = " + String(value));
        
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Missing parameters");
    }
}
void PSci::handleCheckUpdate() {
    server.send(200, "application/json", "{\"status\":\"checking\"}");
    addMonitorLine("Update check requested");
}
void PSci::handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}

void PSci::controlPin(int pinNum, String type, int value) {
    if (type == "digital") {
        pinMode(pinNum, OUTPUT);
        digitalWrite(pinNum, value ? HIGH : LOW);
    } else if (type == "pwm") {
        pinMode(pinNum, OUTPUT);
        analogWrite(pinNum, constrain(value, 0, 255));
    } else if (type == "input") {
        pinMode(pinNum, INPUT);
    }
}

float PSci::readCoreTemperature() {

    return temperatureRead();
}

void PSci::addMonitorLine(String text) {

    
    Serial.println(text); // Log to the Arduino Serial Monitor
}
