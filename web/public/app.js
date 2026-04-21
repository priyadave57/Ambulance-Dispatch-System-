const socket = io();

// Canvas context
const canvas = document.getElementById('city-map');
const ctx = canvas.getContext('2d');
const CANVAS_SIZE = 800; // Fixed size for retina
const GRID_SIZE = 10; // 10x10 city graph 
const CELL_SIZE = CANVAS_SIZE / GRID_SIZE;

// UI Elements
const tickCounter = document.getElementById('tick-counter');
const emergenciesList = document.getElementById('emergencies-list');
const fleetList = document.getElementById('fleet-list');

// Data
let currentState = null;

// Listen for JSON from C++
socket.on('simulationState', (data) => {
    currentState = data;
    updateUI(data);
    drawMap(data);
});

function updateUI(data) {
    // 1. Tick Counter
    tickCounter.textContent = data.time.toString().padStart(4, '0');

    // 2. Emergencies List
    if (data.emergencies.length === 0) {
        emergenciesList.innerHTML = '<div class="empty-state">No Active Emergencies</div>';
    } else {
        emergenciesList.innerHTML = '';
        data.emergencies.forEach(em => {
            const el = document.createElement('div');
            el.className = 'card emergency';
            el.innerHTML = `
                <div class="card-title">🚨 Call ID: #${em.id}</div>
                <div class="card-meta">Location: [X:${em.x}, Y:${em.y}] | Severity: ${em.severity}/10</div>
            `;
            emergenciesList.appendChild(el);
        });
    }

    // 3. Fleet List
    fleetList.innerHTML = '';
    data.ambulances.forEach(amb => {
        const el = document.createElement('div');
        // Class color based on status
        let statusClass = amb.status === 'AVAILABLE' ? 'ambulance' : 
                          amb.status === 'DISPATCHED' ? 'dispatched' : 'ambulance';
        
        el.className = `card ${statusClass}`;
        el.innerHTML = `
            <div class="card-title">🚑 Unit #${amb.id} - ${amb.status}</div>
            <div class="card-meta">Current Node: [X:${amb.x}, Y:${amb.y}]</div>
        `;
        fleetList.appendChild(el);
    });
}

function drawMap(data) {
    // Clear
    ctx.clearRect(0, 0, CANVAS_SIZE, CANVAS_SIZE);

    // Draw Grid (Graph Edges)
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
    ctx.lineWidth = 2;
    
    // Draw edges
    for (let i = 0; i < GRID_SIZE; i++) {
        // Vertical lines
        ctx.beginPath();
        ctx.moveTo(i * CELL_SIZE + CELL_SIZE/2, CELL_SIZE/2);
        ctx.lineTo(i * CELL_SIZE + CELL_SIZE/2, CANVAS_SIZE - CELL_SIZE/2);
        ctx.stroke();

        // Horizontal lines
        ctx.beginPath();
        ctx.moveTo(CELL_SIZE/2, i * CELL_SIZE + CELL_SIZE/2);
        ctx.lineTo(CANVAS_SIZE - CELL_SIZE/2, i * CELL_SIZE + CELL_SIZE/2);
        ctx.stroke();
    }

    // Draw Nodes (Intersections)
    ctx.fillStyle = 'rgba(59, 130, 246, 0.2)';
    for (let y = 0; y < GRID_SIZE; y++) {
        for (let x = 0; x < GRID_SIZE; x++) {
            ctx.beginPath();
            ctx.arc(x * CELL_SIZE + CELL_SIZE/2, y * CELL_SIZE + CELL_SIZE/2, 5, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    // Draw Emergencies
    data.emergencies.forEach(em => {
        const x = em.x * CELL_SIZE + CELL_SIZE/2;
        const y = em.y * CELL_SIZE + CELL_SIZE/2;
        
        // Pulsuing glow for emergency
        const time = Date.now() / 200;
        const radius = 15 + Math.sin(time) * 5;

        // Glow
        const gradient = ctx.createRadialGradient(x, y, 0, x, y, radius);
        gradient.addColorStop(0, 'rgba(239, 68, 68, 0.8)');
        gradient.addColorStop(1, 'rgba(239, 68, 68, 0)');
        ctx.fillStyle = gradient;
        ctx.beginPath();
        ctx.arc(x, y, radius, 0, Math.PI * 2);
        ctx.fill();

        // Center dot
        ctx.fillStyle = '#EF4444';
        ctx.beginPath();
        ctx.arc(x, y, 6, 0, Math.PI*2);
        ctx.fill();
    });

    // Draw Routes
    data.ambulances.forEach(amb => {
        if ((amb.status === 'DISPATCHED' || amb.status === 'RETURNING') && amb.route && amb.route.length > 0) {
            ctx.beginPath();
            ctx.setLineDash([8, 8]);
            ctx.lineDashOffset = -(Date.now() / 40); // animate dashes
            
            // Start from ambulance current pos
            ctx.moveTo(amb.x * CELL_SIZE + CELL_SIZE/2, amb.y * CELL_SIZE + CELL_SIZE/2);
            
            // Draw along the route array
            amb.route.forEach(point => {
                ctx.lineTo(point.x * CELL_SIZE + CELL_SIZE/2, point.y * CELL_SIZE + CELL_SIZE/2);
            });
            
            ctx.strokeStyle = amb.status === 'DISPATCHED' ? 'rgba(16, 185, 129, 0.9)' : 'rgba(59, 130, 246, 0.9)';
            ctx.lineWidth = 3;
            ctx.stroke();
            ctx.setLineDash([]); // reset
        }
    });

    // Draw Ambulances
    data.ambulances.forEach(amb => {
        const x = amb.x * CELL_SIZE + CELL_SIZE/2;
        const y = amb.y * CELL_SIZE + CELL_SIZE/2;
        
        // Color based on status
        let color = '#3B82F6'; // Blue
        if (amb.status === 'DISPATCHED') color = '#10B981'; // Green
        
        // Glow
        ctx.shadowBlur = 15;
        ctx.shadowColor = color;
        
        ctx.fillStyle = color;
        ctx.beginPath();
        ctx.roundRect(x - 12, y - 12, 24, 24, 6); // Appears as square with rounded corners
        ctx.fill();
        
        // Reset shadow
        ctx.shadowBlur = 0;
        
        // Icon (text) inside
        ctx.fillStyle = 'white';
        ctx.font = '12px Outfit';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText('🚑', x, y);
    });
}

// Map Animation loop for continuous glowing effects between ticks
function renderLoop() {
    if (currentState) {
        drawMap(currentState);
    }
    requestAnimationFrame(renderLoop);
}

renderLoop();
