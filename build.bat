@echo off
echo Building Real-Time Emergency Dispatch System...
g++ main.cpp Simulation.cpp CityGraph.cpp -o dispatch_sim.exe -std=c++14
if "%ERRORLEVEL%" == "0" (
    echo Build successful. You can start the simulation by running: dispatch_sim.exe
) else (
    echo Build failed. Please check for errors.
)
