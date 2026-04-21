#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

// Represents an intersection in the city grid
struct Node {
    int id;
    int x;
    int y;
};

// Represents an incoming 911 call
struct EmergencyCall {
    int id;
    int patientNodeId;
    int severity; // 1-10, 10 being most severe
    int timestamp; // When the call came in
};

enum AmbulanceStatus {
    AVAILABLE,
    DISPATCHED,
    RETURNING
};

// Represents an ambulance in the fleet
struct Ambulance {
    int id;
    int currentNodeId;
    int targetNodeId;
    AmbulanceStatus status;
    std::vector<int> currentPath;
};

#endif
