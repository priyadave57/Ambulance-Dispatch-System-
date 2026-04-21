#include "Simulation.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

Simulation::Simulation() : currentTime(0) {
    std::srand(1234); // Fixed seed for reproducible output
}

void Simulation::initializeMap(int sizeX, int sizeY) {
    int id = 0;
    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX; ++x) {
            map.addNode(id++, x, y);
        }
    }

    // Add edges for a grid
    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX; ++x) {
            int currentId = y * sizeX + x;
            if (x < sizeX - 1) map.addEdge(currentId, currentId + 1, 1.0);
            if (x > 0) map.addEdge(currentId, currentId - 1, 1.0);
            if (y < sizeY - 1) map.addEdge(currentId, currentId + sizeX, 1.0);
            if (y > 0) map.addEdge(currentId, currentId - sizeX, 1.0);
        }
    }
}

void Simulation::initializeFleet(int numAmbulances) {
    for (int i = 0; i < numAmbulances; ++i) {
        Ambulance a;
        a.id = i + 1;
        a.currentNodeId = std::rand() % map.numNodes();
        a.targetNodeId = a.currentNodeId;
        a.status = AVAILABLE;
        fleet.push_back(a);
    }
}

void Simulation::generateRandomCall() {
   if (std::rand() % 10 < 7 && standbyQueue.size() < 8) {
        EmergencyCall call;
        call.id = currentTime;
        call.patientNodeId = std::rand() % map.numNodes();
        call.severity = (std::rand() % 10) + 1;
        call.timestamp = currentTime;
        standbyQueue.push(call);
    }
}

void Simulation::updateTrafficConditions() {
    // You can enhance this later
}

void Simulation::processDispatch() {
    if (!standbyQueue.empty()) {
        for (auto& amb : fleet) {
            if (amb.status == AVAILABLE) {
                EmergencyCall call = standbyQueue.front();
                standbyQueue.pop();
                amb.targetNodeId = call.patientNodeId;
                amb.status = DISPATCHED;
                amb.currentPath = map.calculateShortestPath(amb.currentNodeId, amb.targetNodeId);
                break;
            }
        }
    }
}

void Simulation::moveAmbulances() {
    for (auto& amb : fleet) {
        if (amb.status == DISPATCHED || amb.status == RETURNING) {
            if (amb.currentNodeId != amb.targetNodeId) {
                if (!amb.currentPath.empty()) {
                    amb.currentNodeId = amb.currentPath.front();
                    amb.currentPath.erase(amb.currentPath.begin());
                }

                if (amb.currentNodeId == amb.targetNodeId) {
                    amb.status = AVAILABLE;
                    amb.currentPath.clear();
                }
            }
        }
    }
}

json Simulation::getState() {
    json j;
    j["time"] = currentTime;

    // Ambulances
    json ambArray = json::array();
    for (const auto& amb : fleet) {
        Node n = map.getNode(amb.currentNodeId);
        json a;
        a["id"] = amb.id;
        a["x"] = n.x;
        a["y"] = n.y;

        json pArray = json::array();
        for (int pId : amb.currentPath) {
            Node pn = map.getNode(pId);
            pArray.push_back({{"x", pn.x}, {"y", pn.y}});
        }
        a["route"] = pArray;

        if (amb.status == AVAILABLE) {
            a["status"] = "AVAILABLE";
        } else if (amb.status == DISPATCHED) {
            a["status"] = "DISPATCHED";
            Node target = map.getNode(amb.targetNodeId);
            a["targetX"] = target.x;
            a["targetY"] = target.y;
        } else {
            a["status"] = "RETURNING";
            // Optional: target for returning
            Node target = map.getNode(amb.targetNodeId);
            a["targetX"] = target.x;
            a["targetY"] = target.y;
        }

        ambArray.push_back(a);
    }
    j["ambulances"] = ambArray;

    // Emergency Calls
    json urgArray = json::array();
    std::queue<EmergencyCall> tempQ = standbyQueue;

    while (!tempQ.empty()) {
        EmergencyCall c = tempQ.front();
        tempQ.pop();

        Node n = map.getNode(c.patientNodeId);

        json em;
        em["id"] = c.id;
        em["x"] = n.x;
        em["y"] = n.y;
        em["severity"] = c.severity;

        urgArray.push_back(em);
    }

    j["emergencies"] = urgArray;
    j["activeDispatches"] = json::array();

for (const auto& amb : fleet) {
    if (amb.status == DISPATCHED) {
        Node n = map.getNode(amb.targetNodeId);

        j["activeDispatches"].push_back({
            {"x", n.x},
            {"y", n.y}
        });
    }
}
    return j;
}

void Simulation::step() {
    currentTime++;
    generateRandomCall();
    updateTrafficConditions();
    processDispatch();
    moveAmbulances();
}

void Simulation::renderMap() {
    // Left for backwards compatibility if needed
    json j = getState();
    std::cout << j.dump() << std::endl;
}

void Simulation::run() {
    while (true) {
        step();
        renderMap();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}