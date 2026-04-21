#include <iostream>
#include <fstream>
#include <windows.h>   // for Sleep()
#include "Simulation.h"
#include "json.hpp"

using json = nlohmann::json;

int main() {
    std::cout << "Starting Real-Time Emergency Dispatch System..." << std::endl;

    Simulation system;

    // Create a 10x10 city grid
    system.initializeMap(10, 10);

    // Deploy 3 ambulances
    system.initializeFleet(3);

    while (true) {
        system.step();   // run ONE step of simulation

        // get current state
        json state = system.getState();

        // save to file
        std::ofstream file("state.json");
        file << state.dump(4);
        file.close();

        Sleep(1000); // wait 1 second
    }

    return 0;
}
