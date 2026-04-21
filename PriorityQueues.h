#ifndef PRIORITY_QUEUES_H
#define PRIORITY_QUEUES_H

#include <vector>
#include <iostream>
#include "Models.h"

// MAX-HEAP for Emergency Calls (Triage)
// Prioritizes by Severity first, then falls back to Timestamp
class TriageQueue {
private:
    std::vector<EmergencyCall> heap;
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }
    
    void heapifyUp(int i);
    void heapifyDown(int i);

public:
    void push(EmergencyCall call);
    EmergencyCall pop();
    bool isEmpty() const;
    EmergencyCall peek() const;
};

// MIN-HEAP for Dijkstra (Shortest Path)
// Prioritizes the lowest totalTime
struct DijkstraNode {
    int nodeId;
    double totalTime;
};

class RoutingQueue {
private:
    std::vector<DijkstraNode> heap;
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

    void heapifyUp(int i);
    void heapifyDown(int i);
    
public:
    void push(DijkstraNode node);
    DijkstraNode pop();
    bool isEmpty() const;
};

#endif
