/**
 * @file WebPages.h
 * @brief HTML web page content
 * @details Contains embedded HTML/CSS/JavaScript for web UI
 * @author SpiderEyes Development Team
 * @version 1.0.0
 */

#ifndef SPIDEREYES_WEBPAGES_H
#define SPIDEREYES_WEBPAGES_H

const char WEB_PAGE_INDEX[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SpiderEyes Control</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .tabs {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
            flex-wrap: wrap;
        }
        .tab-button {
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            background: white;
            color: #667eea;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.3s;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .tab-button.active {
            background: #667eea;
            color: white;
        }
        .tab-button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 12px rgba(0,0,0,0.15);
        }
        .tab-content {
            display: none;
            background: white;
            border-radius: 12px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        .tab-content.active {
            display: block;
        }
        .stat-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-bottom: 20px;
        }
        .stat-card {
            background: #f5f7fa;
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #667eea;
        }
        .stat-label {
            font-size: 0.85em;
            color: #666;
            margin-bottom: 8px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        .stat-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #333;
        }
        .slider-group {
            margin-bottom: 25px;
        }
        .slider-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 10px;
            font-weight: 600;
            color: #333;
        }
        .slider-label span:last-child {
            color: #667eea;
            font-size: 1.1em;
        }
        input[type="range"] {
            width: 100%;
            height: 6px;
            border-radius: 3px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        }
        input[type="range"]::-moz-range-thumb {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            border: none;
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        }
        .button-group {
            display: flex;
            gap: 10px;
            flex-wrap: wrap;
            margin: 20px 0;
        }
        button {
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s;
            font-size: 1em;
        }
        .btn-primary {
            background: #667eea;
            color: white;
        }
        .btn-primary:hover {
            background: #5568d3;
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
        }
        .btn-secondary {
            background: #e0e7ff;
            color: #667eea;
        }
        .btn-secondary:hover {
            background: #c7d2f8;
        }
        .btn-success {
            background: #10b981;
            color: white;
        }
        .btn-success:hover {
            background: #059669;
        }
        .btn-danger {
            background: #ef4444;
            color: white;
        }
        .btn-danger:hover {
            background: #dc2626;
        }
        .status-badge {
            display: inline-block;
            padding: 6px 12px;
            border-radius: 20px;
            font-size: 0.85em;
            font-weight: 600;
        }
        .status-connected {
            background: #d1fae5;
            color: #065f46;
        }
        .status-disconnected {
            background: #fee2e2;
            color: #7f1d1d;
        }
        .progress-bar {
            width: 100%;
            height: 8px;
            background: #e5e7eb;
            border-radius: 4px;
            overflow: hidden;
            margin: 10px 0;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea, #764ba2);
            width: 0%;
            transition: width 0.3s ease;
        }
        .info-box {
            background: #f0f9ff;
            border-left: 4px solid #3b82f6;
            padding: 15px;
            border-radius: 6px;
            margin-bottom: 20px;
            color: #1e40af;
        }
        .error-box {
            background: #fef2f2;
            border-left: 4px solid #ef4444;
            padding: 15px;
            border-radius: 6px;
            margin-bottom: 20px;
            color: #7f1d1d;
        }
        .success-box {
            background: #f0fdf4;
            border-left: 4px solid #10b981;
            padding: 15px;
            border-radius: 6px;
            margin-bottom: 20px;
            color: #065f46;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🕷️ SpiderEyes</h1>
            <p>Smart Eye Shutter Control System</p>
        </div>
        
        <div class="tabs">
            <button class="tab-button active" data-tab="dashboard">Dashboard</button>
            <button class="tab-button" data-tab="calibration">Calibration</button>
            <button class="tab-button" data-tab="servo">Servo Control</button>
            <button class="tab-button" data-tab="motion">Motion Settings</button>
            <button class="tab-button" data-tab="diagnostics">Diagnostics</button>
        </div>
        
        <!-- Dashboard Tab -->
        <div id="dashboard" class="tab-content active">
            <h2>System Dashboard</h2>
            <div class="stat-grid">
                <div class="stat-card">
                    <div class="stat-label">Left Hall Value</div>
                    <div class="stat-value" id="hall-left">0.00</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Right Hall Value</div>
                    <div class="stat-value" id="hall-right">0.00</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Left Servo Angle</div>
                    <div class="stat-value" id="servo-left">0°</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Right Servo Angle</div>
                    <div class="stat-value" id="servo-right">0°</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Free Heap Memory</div>
                    <div class="stat-value" id="heap-free">0 KB</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Update Frequency</div>
                    <div class="stat-value" id="update-freq">0 Hz</div>
                </div>
            </div>
        </div>
        
        <!-- Calibration Tab -->
        <div id="calibration" class="tab-content">
            <h2>Sensor Calibration</h2>
            <div id="calibration-status"></div>
            <div class="progress-bar">
                <div class="progress-fill" id="calibration-progress" style="width: 0%"></div>
            </div>
            <div class="button-group">
                <button class="btn-primary" onclick="startCalibration()">Start Calibration</button>
                <button class="btn-secondary" onclick="nextCalibrationStep()">Next Step</button>
                <button class="btn-danger" onclick="cancelCalibration()">Cancel</button>
                <button class="btn-success" onclick="saveCalibration()">Save Calibration</button>
                <button class="btn-secondary" onclick="resetCalibration()">Reset to Defaults</button>
            </div>
        </div>
        
        <!-- Servo Control Tab -->
        <div id="servo" class="tab-content">
            <h2>Servo Control</h2>
            <div class="slider-group">
                <div class="slider-label">
                    <span>Left Eye Angle</span>
                    <span id="left-angle-value">0°</span>
                </div>
                <input type="range" id="left-servo-slider" min="-90" max="70" value="0" step="1" 
                       oninput="setLeftServo()">
            </div>
            <div class="slider-group">
                <div class="slider-label">
                    <span>Right Eye Angle</span>
                    <span id="right-angle-value">0°</span>
                </div>
                <input type="range" id="right-servo-slider" min="-70" max="90" value="0" step="1" 
                       oninput="setRightServo()">
            </div>
            <div class="slider-group">
                <div class="slider-label">
                    <span>Speed (deg/sec)</span>
                    <span id="speed-value">180</span>
                </div>
                <input type="range" id="speed-slider" min="10" max="360" value="180" step="10" 
                       oninput="setServoSpeed()">
            </div>
            <div class="button-group">
                <button class="btn-primary" onclick="moveServoNeutral()">Neutral</button>
                <button class="btn-success" onclick="moveServoOpen()">Open</button>
                <button class="btn-danger" onclick="moveServoClosed()">Closed</button>
            </div>
        </div>
        
        <!-- Motion Settings Tab -->
        <div id="motion" class="tab-content">
            <h2>Motion Settings</h2>
            <div class="slider-group">
                <div class="slider-label">
                    <span>Sensitivity</span>
                    <span id="sensitivity-value">1.0</span>
                </div>
                <input type="range" id="sensitivity-slider" min="0.1" max="2.0" value="1.0" step="0.1" 
                       oninput="setMotionSensitivity()">
            </div>
            <div class="slider-group">
                <div class="slider-label">
                    <span>Deadband</span>
                    <span id="deadband-value">0.05</span>
                </div>
                <input type="range" id="deadband-slider" min="0" max="0.5" value="0.05" step="0.01" 
                       oninput="setMotionDeadband()">
            </div>
            <div class="button-group">
                <button class="btn-primary" onclick="saveMotionSettings()">Save Settings</button>
            </div>
        </div>
        
        <!-- Diagnostics Tab -->
        <div id="diagnostics" class="tab-content">
            <h2>System Diagnostics</h2>
            <div class="stat-grid">
                <div class="stat-card">
                    <div class="stat-label">Total Heap</div>
                    <div class="stat-value" id="diag-heap-total">0 KB</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Free Heap</div>
                    <div class="stat-value" id="diag-heap-free">0 KB</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Used Heap</div>
                    <div class="stat-value" id="diag-heap-used">0 KB</div>
                </div>
                <div class="stat-card">
                    <div class="stat-label">Motion Frequency</div>
                    <div class="stat-value" id="diag-motion-freq">0 Hz</div>
                </div>
            </div>
            <div class="info-box" id="firmware-info"></div>
        </div>
    </div>
    
    <script>
        // Tab switching
        document.querySelectorAll('.tab-button').forEach(button => {
            button.addEventListener('click', function() {
                const tabName = this.getAttribute('data-tab');
                switchTab(tabName);
            });
        });
        
        function switchTab(tabName) {
            document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
            document.querySelectorAll('.tab-button').forEach(el => el.classList.remove('active'));
            document.getElementById(tabName).classList.add('active');
            event.target.classList.add('active');
        }
        
        // Dashboard updates
        function updateDashboard() {
            fetch('/api/dashboard')
                .then(r => r.json())
                .then(d => {
                    document.getElementById('hall-left').textContent = d.hall_left.toFixed(2);
                    document.getElementById('hall-right').textContent = d.hall_right.toFixed(2);
                    document.getElementById('servo-left').textContent = d.servo_left + '°';
                    document.getElementById('servo-right').textContent = d.servo_right + '°';
                    document.getElementById('heap-free').textContent = (d.heap_free / 1024).toFixed(0) + ' KB';
                    document.getElementById('update-freq').textContent = d.motion_update_freq.toFixed(1) + ' Hz';
                });
        }
        
        // Servo control
        function setLeftServo() {
            const angle = document.getElementById('left-servo-slider').value;
            document.getElementById('left-angle-value').textContent = angle + '°';
            fetch('/api/servo?left_angle=' + angle, {method: 'POST'});
        }
        
        function setRightServo() {
            const angle = document.getElementById('right-servo-slider').value;
            document.getElementById('right-angle-value').textContent = angle + '°';
            fetch('/api/servo?right_angle=' + angle, {method: 'POST'});
        }
        
        function setServoSpeed() {
            const speed = document.getElementById('speed-slider').value;
            document.getElementById('speed-value').textContent = speed;
            fetch('/api/servo?speed=' + speed, {method: 'POST'});
        }
        
        function moveServoNeutral() {
            fetch('/api/servo?action=neutral', {method: 'POST'});
        }
        
        function moveServoOpen() {
            fetch('/api/servo?action=open', {method: 'POST'});
        }
        
        function moveServoClosed() {
            fetch('/api/servo?action=closed', {method: 'POST'});
        }
        
        // Calibration
        function startCalibration() {
            fetch('/api/calibration?action=start', {method: 'POST'})
                .then(() => updateCalibration());
        }
        
        function nextCalibrationStep() {
            fetch('/api/calibration?action=next', {method: 'POST'})
                .then(() => updateCalibration());
        }
        
        function cancelCalibration() {
            fetch('/api/calibration?action=cancel', {method: 'POST'})
                .then(() => updateCalibration());
        }
        
        function saveCalibration() {
            fetch('/api/calibration?action=save', {method: 'POST'})
                .then(() => alert('Calibration saved!'));
        }
        
        function resetCalibration() {
            if(confirm('Reset calibration to defaults?')) {
                fetch('/api/calibration?action=reset', {method: 'POST'})
                    .then(() => updateCalibration());
            }
        }
        
        function updateCalibration() {
            fetch('/api/calibration')
                .then(r => r.json())
                .then(d => {
                    document.getElementById('calibration-progress').style.width = d.progress + '%';
                    document.getElementById('calibration-status').innerHTML = '<div class="info-box">' + d.message + '</div>';
                });
        }
        
        // Motion settings
        function setMotionSensitivity() {
            const value = document.getElementById('sensitivity-slider').value;
            document.getElementById('sensitivity-value').textContent = parseFloat(value).toFixed(1);
        }
        
        function setMotionDeadband() {
            const value = document.getElementById('deadband-slider').value;
            document.getElementById('deadband-value').textContent = parseFloat(value).toFixed(2);
        }
        
        function saveMotionSettings() {
            const sens = document.getElementById('sensitivity-slider').value;
            const dead = document.getElementById('deadband-slider').value;
            fetch(`/api/motion?sensitivity=${sens}&deadband=${dead}`, {method: 'POST'})
                .then(() => alert('Motion settings saved!'));
        }
        
        // Diagnostics
        function updateDiagnostics() {
            fetch('/api/diagnostics')
                .then(r => r.json())
                .then(d => {
                    document.getElementById('diag-heap-total').textContent = (d.heap_total / 1024).toFixed(0) + ' KB';
                    document.getElementById('diag-heap-free').textContent = (d.heap_free / 1024).toFixed(0) + ' KB';
                    document.getElementById('diag-heap-used').textContent = (d.heap_used / 1024).toFixed(0) + ' KB';
                    document.getElementById('diag-motion-freq').textContent = d.motion_freq.toFixed(1) + ' Hz';
                    document.getElementById('firmware-info').innerHTML = 'Firmware: ' + d.firmware + '<br>Build: ' + d.build_date;
                });
        }
        
        // Start periodic updates
        setInterval(updateDashboard, 500);
        setInterval(updateDiagnostics, 2000);
        updateDashboard();
        updateDiagnostics();
        updateCalibration();
    </script>
</body>
</html>
)";

#endif // SPIDEREYES_WEBPAGES_H
