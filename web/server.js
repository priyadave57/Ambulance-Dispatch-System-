const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.static(path.join(__dirname, 'public')));

let cppProcess = null;

io.on('connection', (socket) => {
    console.log('Frontend connected to dashboard.');
    
    // Start C++ process if not already running
    if (!cppProcess) {
        console.log('Starting C++ Simulation Process...');
        
        // Spawn the executable from the parent directory
        cppProcess = spawn(path.join(__dirname, '../dispatch_sim.exe'), [], {
            cwd: path.join(__dirname, '..')
        });

        const fs = require('fs');
        const statePath = path.join(__dirname, '../state.json');

        // Watch the JSON file written by the C++ executable.
        fs.watchFile(statePath, { interval: 200 }, (curr, prev) => {
            if (curr.mtimeMs !== prev.mtimeMs) {
                try {
                    const fileData = fs.readFileSync(statePath, 'utf-8');
                    if (fileData) {
                        const jsonState = JSON.parse(fileData);
                        io.emit('simulationState', jsonState); // Broadcast to frontend
                    }
                } catch (e) {
                    // Ignorable: File might be in the middle of being written by C++
                }
            }
        });

        cppProcess.on('close', (code) => {
            console.log(`C++ process exited with code ${code}`);
            fs.unwatchFile(statePath);
            cppProcess = null;
        });
    }
});

const PORT = 3005;
server.listen(PORT, () => {
    console.log(`Live Dispatch Dashboard running at http://localhost:${PORT}`);
});
