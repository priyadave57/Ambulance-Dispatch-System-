#include "json.hpp"   // add this at top

using json = nlohmann::json;
#ifndef SIMULATION_H
#define SIMULATION_H

#include "CityGraph.h"
#include "PriorityQueues.h"
#include <queue> // Provides a standard FIFO queue
#include <vector>

class Simulation {
private:
    CityGraph map;
    TriageQueue triageQueue; // Custom Max-Heap
    std::queue<EmergencyCall> standbyQueue; // Standard FIFO Queue
    std::vector<Ambulance> fleet; // Replaces linked list for contiguous memory fleet tracking
    int currentTime;

    // Core private loop functions
    void generateRandomCall();
    void updateTrafficConditions();
    void processDispatch();
    void moveAmbulances();
    void renderMap();

public:
   public:
    Simulation();
    void initializeMap(int sizeX, int sizeY);
    void initializeFleet(int numAmbulances);

    void run();  

    // ✅ ADD THESE TWO
    void step();
    json getState();
};

#endif
