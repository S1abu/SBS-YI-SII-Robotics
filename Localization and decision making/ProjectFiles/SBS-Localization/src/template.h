const char* webpageTemplate = R"=====(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robot Control & Mapping</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; text-align: center; }
        #mapCanvas { border: 1px solid #333; margin: 20px auto; background: #f0f0f0; }
        #controls { margin: 20px 0; }
        button { padding: 10px 20px; margin: 0 10px; font-size: 16px; cursor: pointer; }
        .status { margin: 10px 0; font-weight: bold; }
    </style>
</head>
<body>
    <h1>Robot Control & Mapping</h1>
    <div id="controls">
        <button id="startBtn" onclick="startRobot()">Start Autonomous Mode</button>
        <button id="stopBtn" onclick="stopRobot()">Stop Robot</button>
    </div>
    <div class="status">Status: <span id="statusText">Idle</span></div>
    <canvas id="mapCanvas" width="500" height="500"></canvas>

    <script>
        const canvas = document.getElementById('mapCanvas');
        const ctx = canvas.getContext('2d');
        const centerX = canvas.width / 2;
        const centerY = canvas.height / 2;
        const scaleFactor = 2; // (pixels per cm)
        const updateSpeed = 100; // (ms)
        let mapData = {};
        drawMap();
        function startRobot() {
            fetch('/control?cmd=START')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('statusText').innerText = 'Running';
                    document.getElementById('startBtn').disabled = true;
                    document.getElementById('stopBtn').disabled = false;
                });
        }
        
        function stopRobot() {
            fetch('/control?cmd=STOP')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('statusText').innerText = 'Stopped';
                    document.getElementById('startBtn').disabled = false;
                    document.getElementById('stopBtn').disabled = true;
                });
        }
        
        function drawMap() {
            ctx.fillStyle = '#f0f0f0';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            ctx.strokeStyle = '#ddd';
            ctx.lineWidth = 1;
            
            for (let i = 0; i < canvas.width; i += 50) {
                ctx.beginPath();
                ctx.moveTo(i, 0);
                ctx.lineTo(i, canvas.height);
                ctx.stroke();
                
                ctx.beginPath();
                ctx.moveTo(0, i);
                ctx.lineTo(canvas.width, i);
                ctx.stroke();
            }
            
            for (let r = 50; r < 250; r += 50) {
                ctx.beginPath();
                ctx.arc(centerX, centerY, r, 0, 2 * Math.PI);
                ctx.stroke();
            }
            
            ctx.fillStyle = 'blue';
            ctx.beginPath();
            ctx.arc(centerX, centerY, 10, 0, 2 * Math.PI);
            ctx.fill();
            
            ctx.fillStyle = 'red';
            ctx.lineWidth = 2;
            ctx.strokeStyle = 'rgba(255,0,0,0.5)';
            for (const [angle, distance] of Object.entries(mapData)) {
                const rad = (parseInt(angle) - 90) * Math.PI / 180;
                const dist = Math.min(distance, 200) * scaleFactor;
                const x = centerX + Math.cos(rad) * dist;
                const y = centerY + Math.sin(rad) * dist;
                ctx.beginPath();
                ctx.moveTo(centerX, centerY);
                ctx.lineTo(x, y);
                ctx.stroke();
                ctx.beginPath();
                ctx.arc(x, y, 4, 0, 2 * Math.PI);
                ctx.fill();
            }
        }
        function fetchData() {
            fetch('/data')
                .then(response => response.text())
                .then(data => {
                    const entries = data.trim().split(';');
                    entries.forEach(entry => {
                        const [angle, distance] = entry.split(',');
                        if (angle && distance) {
                            mapData[angle] = parseInt(distance);
                        }
                    });
                    drawMap();
                })
                .catch(error => console.error('Error fetching data:', error));
        }
        setInterval(fetchData, updateSpeed);
    </script>
</body>
</html>
    )=====";