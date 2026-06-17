#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sistema de Seguridad Táctica ESP32</title>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&display=swap" rel="stylesheet">
    <style>
        :root {
            --neon-green: #39ff14;
            --neon-red: #ff073a;
            --neon-blue: #00f0ff;
            --neon-yellow: #ffa500;
            --dark-bg: #070907;
            --card-bg: rgba(15, 23, 15, 0.85);
        }
        body {
            font-family: 'Orbitron', monospace;
            background-color: var(--dark-bg);
            color: var(--neon-green);
            display: flex;
            flex-direction: column;
            align-items: center;
            margin: 0;
            padding: 20px;
            overflow-x: hidden;
            background-image: 
                radial-gradient(circle at 50% 50%, rgba(57, 255, 20, 0.05) 0%, transparent 80%),
                linear-gradient(rgba(18, 30, 18, 0.3) 1px, transparent 1px),
                linear-gradient(90deg, rgba(18, 30, 18, 0.3) 1px, transparent 1px);
            background-size: 100% 100%, 20px 20px, 20px 20px;
        }
        h1 {
            text-shadow: 0 0 10px rgba(57, 255, 20, 0.6);
            font-weight: 900;
            letter-spacing: 2px;
            margin-bottom: 5px;
            text-align: center;
            font-size: 2.2em;
        }
        .subtitle {
            color: #88ff88;
            font-size: 0.9em;
            margin-bottom: 30px;
            text-shadow: 0 0 5px rgba(136, 255, 136, 0.3);
            text-align: center;
            text-transform: uppercase;
        }
        .dashboard {
            display: flex;
            flex-wrap: wrap;
            gap: 25px;
            justify-content: center;
            width: 100%;
            max-width: 1000px;
        }
        .card {
            background: var(--card-bg);
            padding: 25px;
            border-radius: 12px;
            box-shadow: 0 0 20px rgba(57, 255, 20, 0.15);
            border: 1px solid rgba(57, 255, 20, 0.3);
            backdrop-filter: blur(8px);
            text-align: center;
            flex: 1;
            min-width: 320px;
            transition: all 0.3s ease;
        }
        .card:hover {
            box-shadow: 0 0 25px rgba(57, 255, 20, 0.3);
            border-color: rgba(57, 255, 20, 0.5);
        }
        .radar-container {
            position: relative;
            width: 300px;
            height: 160px;
            margin: 15px auto;
            overflow: hidden;
            background: #000c00;
            border-top-left-radius: 150px;
            border-top-right-radius: 150px;
            border: 2px solid var(--neon-green);
            box-shadow: 0 0 20px rgba(57, 255, 20, 0.4) inset, 0 0 15px rgba(57, 255, 20, 0.2);
        }
        canvas {
            position: absolute;
            bottom: 0;
            left: 0;
        }
        .status-box {
            font-size: 1.1em;
            margin: 15px 0;
            padding: 12px;
            border-radius: 6px;
            background: rgba(0, 0, 0, 0.5);
            border-left: 4px solid var(--neon-green);
            display: flex;
            justify-content: space-around;
            align-items: center;
        }
        .status-item {
            display: flex;
            flex-direction: column;
            align-items: center;
            min-width: 120px;
        }
        .status-label {
            font-size: 0.7em;
            color: #88ff88;
            margin-bottom: 5px;
            letter-spacing: 1px;
            text-transform: uppercase;
        }
        .status-number {
            font-size: 1.5em;
            font-weight: bold;
            color: #fff;
            text-shadow: 0 0 5px rgba(255, 255, 255, 0.2);
        }
        .sys-state {
            font-size: 1.2em;
            font-weight: bold;
            padding: 10px 20px;
            border-radius: 6px;
            margin: 10px 0;
            text-transform: uppercase;
            letter-spacing: 1px;
            display: inline-block;
        }
        .state-secure {
            background: rgba(57, 255, 20, 0.15);
            color: var(--neon-green);
            border: 1px solid var(--neon-green);
            box-shadow: 0 0 10px rgba(57, 255, 20, 0.2);
        }
        .state-alert {
            background: rgba(255, 7, 58, 0.15);
            color: var(--neon-red);
            border: 1px solid var(--neon-red);
            box-shadow: 0 0 10px rgba(255, 7, 58, 0.3);
            animation: blinker 1s linear infinite;
        }
        .state-waiting {
            background: rgba(255, 165, 0, 0.15);
            color: var(--neon-yellow);
            border: 1px solid var(--neon-yellow);
            box-shadow: 0 0 10px rgba(255, 165, 0, 0.3);
            animation: blinker 1.5s linear infinite;
        }
        .state-granted {
            background: rgba(0, 240, 255, 0.15);
            color: var(--neon-blue);
            border: 1px solid var(--neon-blue);
            box-shadow: 0 0 10px rgba(0, 240, 255, 0.3);
        }
        .state-denied {
            background: rgba(255, 7, 58, 0.25);
            color: var(--neon-red);
            border: 1px solid var(--neon-red);
            box-shadow: 0 0 10px rgba(255, 7, 58, 0.4);
        }
        @keyframes blinker {
            50% { opacity: 0.5; }
        }
        .controls button {
            background: transparent;
            border: 1px solid var(--neon-green);
            color: var(--neon-green);
            padding: 12px 24px;
            text-transform: uppercase;
            font-family: 'Orbitron', monospace;
            font-weight: bold;
            font-size: 13px;
            letter-spacing: 1px;
            margin: 8px 4px;
            border-radius: 6px;
            cursor: pointer;
            transition: all 0.2s ease;
            box-shadow: 0 0 10px rgba(57, 255, 20, 0.1);
        }
        .controls button:hover {
            background: var(--neon-green);
            color: #000;
            box-shadow: 0 0 15px var(--neon-green);
        }
        .controls button.danger {
            border-color: var(--neon-red);
            color: var(--neon-red);
            box-shadow: 0 0 10px rgba(255, 7, 58, 0.1);
        }
        .controls button.danger:hover {
            background: var(--neon-red);
            color: #000;
            box-shadow: 0 0 15px var(--neon-red);
        }
        .slider-container {
            margin: 25px 0;
            text-align: left;
        }
        .slider-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 8px;
            font-size: 0.9em;
        }
        input[type=range] {
            -webkit-appearance: none;
            width: 100%;
            background: #152515;
            height: 8px;
            border-radius: 4px;
            outline: none;
            border: 1px solid rgba(57, 255, 20, 0.3);
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: var(--neon-green);
            cursor: pointer;
            box-shadow: 0 0 8px var(--neon-green);
        }
        
        /* Ventana Modal */
        .modal {
            display: none;
            position: fixed;
            z-index: 100;
            left: 0;
            top: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(0, 0, 0, 0.85);
            backdrop-filter: blur(10px);
            align-items: center;
            justify-content: center;
        }
        .modal-content {
            background-color: #0d120d;
            border: 2px solid var(--neon-yellow);
            box-shadow: 0 0 30px rgba(255, 165, 0, 0.4);
            color: var(--neon-yellow);
            padding: 30px;
            border-radius: 12px;
            text-align: center;
            max-width: 450px;
            width: 90%;
            font-family: 'Orbitron', monospace;
            animation: pop-up 0.3s ease;
        }
        @keyframes pop-up {
            from { transform: scale(0.8); opacity: 0; }
            to { transform: scale(1); opacity: 1; }
        }
        .modal-title {
            font-size: 1.8em;
            font-weight: bold;
            margin-bottom: 15px;
            text-shadow: 0 0 10px rgba(255, 165, 0, 0.5);
        }
        .modal-desc {
            color: #ccc;
            margin-bottom: 25px;
            line-height: 1.4;
        }
        .modal-buttons {
            display: flex;
            justify-content: space-around;
            margin-top: 25px;
        }
        .modal-btn {
            padding: 12px 24px;
            font-family: 'Orbitron', monospace;
            font-weight: bold;
            border-radius: 6px;
            cursor: pointer;
            text-transform: uppercase;
            transition: all 0.2s;
        }
        .btn-grant {
            background: transparent;
            color: var(--neon-green);
            border: 1px solid var(--neon-green);
            box-shadow: 0 0 10px rgba(57, 255, 20, 0.2);
        }
        .btn-grant:hover {
            background: var(--neon-green);
            color: #000;
            box-shadow: 0 0 15px var(--neon-green);
        }
        .btn-deny {
            background: transparent;
            color: var(--neon-red);
            border: 1px solid var(--neon-red);
            box-shadow: 0 0 10px rgba(255, 7, 58, 0.2);
        }
        .btn-deny:hover {
            background: var(--neon-red);
            color: #000;
            box-shadow: 0 0 15px var(--neon-red);
        }
    </style>
</head>
<body>
    <h1>SISTEMA TÁCTICO DE SEGURIDAD ESP32</h1>
    <div class="subtitle">Monitoreo de Maqueta e Interfaz de Control de Acceso</div>
    
    <div class="dashboard">
        <div class="card">
            <h2>Radar Táctico (180&deg;)</h2>
            <div class="radar-container">
                <canvas id="radarCanvas" width="300" height="150"></canvas>
            </div>
            <div class="status-box">
                <div class="status-item">
                    <span class="status-label">Ángulo</span>
                    <span class="status-number"><span id="angleVal">--</span>&deg;</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Distancia</span>
                    <span class="status-number"><span id="distVal">--</span> cm</span>
                </div>
            </div>
            <div id="sysStateBox" class="sys-state state-secure">Sistema Seguro</div>
        </div>
        
        <div class="card controls">
            <h2>Consola de Control</h2>
            <p style="margin: 10px 0;">Estado Puerta: <strong id="doorStatus" style="color:#fff">Cerrada</strong></p>
            
            <div class="slider-container">
                <div class="slider-label">
                    <span>Inclinación Radar (Tilt):</span>
                    <span><span id="tiltVal">90</span>&deg;</span>
                </div>
                <input type="range" id="tiltSlider" min="0" max="180" value="90" oninput="sendTilt(this.value)">
            </div>
            
            <div style="margin-top: 15px;">
                <button onclick="sendCommand('OPEN_DOOR')">Abrir Entrada</button>
                <button class="danger" onclick="sendCommand('CLOSE_DOOR')">Cerrar Entrada</button>
            </div>
            <button class="danger" style="margin-top: 15px; width: 90%;" onclick="sendCommand('MUTE_ALARM')">Silenciar Alerta Sonora</button>
        </div>
    </div>

    <!-- Modal de Visitante -->
    <div id="visitorModal" class="modal">
        <div class="modal-content">
            <div class="modal-title">🚨 VISITANTE DETECTADO</div>
            <div class="modal-desc">Una persona está esperando frente a la entrada. ¿Desea concederle acceso?</div>
            <div class="modal-buttons">
                <button class="modal-btn btn-grant" onclick="authVisitor('GRANT')">Conceder</button>
                <button class="modal-btn btn-deny" onclick="authVisitor('DENY')">Rechazar</button>
            </div>
        </div>
    </div>
    
    <script>
        const canvas = document.getElementById('radarCanvas');
        const ctx = canvas.getContext('2d');
        const W = canvas.width;
        const H = canvas.height;
        let radarData = []; // Guardará {angle, dist, age}
        let targets = [];   // Guardará {angle, dist, age, maxAge, isVisitor}
        
        function clearRadar() {
            ctx.fillStyle = '#000c00';
            ctx.fillRect(0, 0, W, H);
            
            // Rejilla de Radar Verde militar con etiquetas de distancia
            ctx.strokeStyle = 'rgba(57, 255, 20, 0.15)';
            ctx.fillStyle = 'rgba(57, 255, 20, 0.4)';
            ctx.font = '9px Orbitron';
            ctx.textAlign = 'center';
            ctx.lineWidth = 1;
            for(let r = 20; r <= 60; r += 20) {
                ctx.beginPath();
                ctx.arc(W/2, H, (r/60)*W/2, Math.PI, 0);
                ctx.stroke();
                
                // Dibujar etiqueta de distancia justo abajo de la línea de cada arco
                ctx.fillText(r + 'cm', W/2, H - (r/60)*(W/2) + 10);
            }
            
            // Líneas de ángulo
            for(let a = 30; a <= 150; a += 30) {
                const rad = a * Math.PI / 180;
                const x = W/2 - Math.cos(rad) * (W/2);
                const y = H - Math.sin(rad) * (W/2);
                ctx.beginPath();
                ctx.moveTo(W/2, H);
                ctx.lineTo(x, y);
                ctx.stroke();
            }
        }
        
        function drawSweepLine(angle, dist, age) {
            const rad = angle * Math.PI / 180;
            const rMax = W/2;
            const x = W/2 - Math.cos(rad) * rMax;
            const y = H - Math.sin(rad) * rMax;
            
            // Opacidad que se desvanece con la edad (efecto fósforo)
            const maxAge = 40;
            const opacity = Math.max(0, 0.8 * (1.0 - (age / maxAge)));
            
            if (opacity > 0) {
                ctx.beginPath();
                ctx.moveTo(W/2, H);
                ctx.lineTo(x, y);
                ctx.strokeStyle = `rgba(57, 255, 20, ${opacity})`;
                ctx.lineWidth = age === 0 ? 3 : 1.5;
                ctx.stroke();
            }
        }
        
        function drawTarget(t) {
            const rad = t.angle * Math.PI / 180;
            const rMax = W/2;
            const rDist = Math.min((t.dist/60)*rMax, rMax);
            
            const ox = W/2 - Math.cos(rad) * rDist;
            const oy = H - Math.sin(rad) * rDist;
            
            const opacity = Math.max(0, 1.0 - (t.age / t.maxAge));
            if (opacity <= 0) return;
            
            // 1. Efecto de brillo táctico (gradiente radial)
            let grad = ctx.createRadialGradient(ox, oy, 1, ox, oy, 14);
            grad.addColorStop(0, t.isVisitor ? `rgba(255, 165, 0, ${opacity})` : `rgba(255, 7, 58, ${opacity})`);
            grad.addColorStop(0.3, t.isVisitor ? `rgba(255, 165, 0, ${opacity * 0.4})` : `rgba(255, 7, 58, ${opacity * 0.4})`);
            grad.addColorStop(1, 'rgba(0,0,0,0)');
            
            ctx.fillStyle = grad;
            ctx.beginPath();
            ctx.arc(ox, oy, 14, 0, 2 * Math.PI);
            ctx.fill();
            
            // 2. Retícula Táctica (Círculo de objetivo + Cruz de mira)
            ctx.lineWidth = 1.5;
            if (t.isVisitor) {
                // Diamante para visitante
                ctx.strokeStyle = `rgba(255, 165, 0, ${opacity * 0.85})`;
                ctx.beginPath();
                ctx.moveTo(ox, oy - 7);
                ctx.lineTo(ox + 7, oy);
                ctx.lineTo(ox, oy + 7);
                ctx.lineTo(ox - 7, oy);
                ctx.closePath();
                ctx.stroke();
            } else {
                // Círculo + Cruz de mira para intruso
                ctx.strokeStyle = `rgba(255, 7, 58, ${opacity * 0.85})`;
                
                // Círculo
                ctx.beginPath();
                ctx.arc(ox, oy, 6, 0, 2 * Math.PI);
                ctx.stroke();
                
                // Líneas de la cruz
                ctx.beginPath();
                ctx.moveTo(ox - 10, oy); ctx.lineTo(ox - 3, oy);
                ctx.moveTo(ox + 3, oy); ctx.lineTo(ox + 10, oy);
                ctx.moveTo(ox, oy - 10); ctx.lineTo(ox, oy - 3);
                ctx.moveTo(ox, oy + 3); ctx.lineTo(ox, oy + 10);
                ctx.stroke();
            }
        }
        
        function updateRadarView(currentAngle, currentDist) {
            clearRadar();
            
            // 1. Actualizar historial de barrido (radarData)
            let found = false;
            for (let i = 0; i < radarData.length; i++) {
                if (radarData[i].angle === currentAngle) {
                    radarData[i].dist = currentDist;
                    radarData[i].age = 0;
                    found = true;
                } else {
                    radarData[i].age += 1;
                }
            }
            if (!found) radarData.push({angle: currentAngle, dist: currentDist, age: 0});
            
            // Limpieza de datos viejos para conservar memoria
            radarData = radarData.filter(d => d.age < 50);
            
            // 2. Actualizar objetivos detectados (targets)
            if (currentDist >= 3 && currentDist < 60) {
                let targetFound = false;
                for (let i = 0; i < targets.length; i++) {
                    // Agrupar ecos cercanos dentro de un margen de 4 grados
                    if (Math.abs(targets[i].angle - currentAngle) <= 4) {
                        targets[i].angle = currentAngle;
                        targets[i].dist = currentDist;
                        targets[i].age = 0;
                        targets[i].isVisitor = currentDist <= 20;
                        targetFound = true;
                        break;
                    }
                }
                if (!targetFound) {
                    targets.push({
                        angle: currentAngle,
                        dist: currentDist,
                        age: 0,
                        maxAge: 35,
                        isVisitor: currentDist <= 20
                    });
                }
            }
            
            // Incrementar edad de objetivos
            targets.forEach(t => {
                t.age += 0.25;
            });
            
            // Filtrar objetivos desvanecidos por completo
            targets = targets.filter(t => t.age < t.maxAge);
            
            // 3. Dibujar estela de barrido
            radarData.forEach(d => {
                drawSweepLine(d.angle, d.dist, d.age);
            });
            
            // 4. Dibujar objetivos activos sobre la estela
            targets.forEach(t => {
                drawTarget(t);
            });
        }

        clearRadar();

        // Audio Web Context (WOW effect)
        let audioCtx = null;
        function initAudio() {
            if (!audioCtx) {
                audioCtx = new (window.AudioContext || window.webkitAudioContext)();
            }
        }

        function playBeep(freq, duration, type='sine') {
            try {
                initAudio();
                if (!audioCtx) return;
                let osc = audioCtx.createOscillator();
                let gain = audioCtx.createGain();
                osc.connect(gain);
                gain.connect(audioCtx.destination);
                osc.type = type;
                osc.frequency.value = freq;
                gain.gain.setValueAtTime(0.12, audioCtx.currentTime);
                gain.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + duration);
                osc.start();
                osc.stop(audioCtx.currentTime + duration);
            } catch(e) {}
        }

        function playDingDong() {
            playBeep(880, 0.4, 'triangle'); // Ding
            setTimeout(() => {
                playBeep(659, 0.5, 'triangle'); // Dong
            }, 200);
        }

        // Websockets
        let gateway = `ws://${window.location.hostname}/ws`;
        let websocket;
        let lastState = -1;
        let alarmIntervalId = null;

        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onmessage = onMessage;
            websocket.onclose = () => setTimeout(initWebSocket, 2000); // Auto-reconnect
        }

        function onMessage(event) {
            try {
                let data = JSON.parse(event.data);
                if (data.type === 'radar') {
                    document.getElementById('distVal').innerText = data.dist;
                    document.getElementById('angleVal').innerText = data.angle;
                    document.getElementById('tiltVal').innerText = data.tilt;
                    document.getElementById('tiltSlider').value = data.tilt;
                    document.getElementById('doorStatus').innerText = data.doorOpen ? 'Abierta' : 'Cerrada';
                    
                    updateRadarView(data.angle, data.dist);
                    
                    let state = data.state;
                    if (state !== lastState) {
                        updateSystemStateUI(state);
                        lastState = state;
                    }
                }
            } catch (e) {
                console.error("WS parse error", e);
            }
        }
        
        function updateSystemStateUI(state) {
            const box = document.getElementById('sysStateBox');
            const modal = document.getElementById('visitorModal');
            
            // Limpiar clases
            box.className = 'sys-state';
            
            // Detener sonidos de alarma previos
            if (alarmIntervalId) {
                clearInterval(alarmIntervalId);
                alarmIntervalId = null;
            }

            // Mapeo: 0=SECURE, 1=ALERT, 2=WAITING, 3=GRANTED, 4=DENIED
            if (state === 0) {
                box.innerText = "Sistema Seguro";
                box.classList.add('state-secure');
                modal.style.display = 'none';
            } 
            else if (state === 1) {
                box.innerText = "🚨 Alerta de Intruso";
                box.classList.add('state-alert');
                modal.style.display = 'none';
                
                // Beep de alarma en el navegador
                alarmIntervalId = setInterval(() => {
                    playBeep(987, 0.15, 'sawtooth');
                }, 350);
            } 
            else if (state === 2) {
                box.innerText = "¿Visitante esperando?";
                box.classList.add('state-waiting');
                modal.style.display = 'flex';
                
                // Ring Ding-Dong
                playDingDong();
            } 
            else if (state === 3) {
                box.innerText = "Acceso Concedido";
                box.classList.add('state-granted');
                modal.style.display = 'none';
                playBeep(1200, 0.4, 'sine');
            } 
            else if (state === 4) {
                box.innerText = "Acceso Denegado";
                box.classList.add('state-denied');
                modal.style.display = 'none';
                playBeep(300, 0.6, 'sawtooth');
            }
        }
        
        function sendCommand(cmd) {
            initAudio(); // User interaction allows audio to start
            if (websocket && websocket.readyState === WebSocket.OPEN) {
                websocket.send(cmd);
            }
        }

        function sendTilt(val) {
            document.getElementById('tiltVal').innerText = val;
            if (websocket && websocket.readyState === WebSocket.OPEN) {
                websocket.send("TILT:" + val);
            }
        }

        function authVisitor(action) {
            if (action === 'GRANT') {
                sendCommand('GRANT_ACCESS');
            } else {
                sendCommand('DENY_ACCESS');
            }
        }

        window.addEventListener('load', initWebSocket);
        // Desbloquear audio en el primer clic del usuario
        document.body.addEventListener('click', initAudio, { once: true });
    </script>
</body>
</html>
)rawliteral";

#endif // WEB_CONTENT_H