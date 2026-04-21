#ifndef CITY_GRAPH_H
#define CITY_GRAPH_H

#include <vector>
#include <unordered_map>
#include <list>
#include "Models.h"
#include "PriorityQueues.h"

struct Edge {
    int destinationNodeId;
    double baseDistance;
    double trafficMultiplier; 
};

class CityGraph {
private:
    // Adjacency List: NodeID -> list of Edges
    std::unordered_map<int, std::list<Edge>> adjList;
    std::unordered_map<int, Node> nodes;

public:
    void addNode(int id, int x, int y);
    void addEdge(int sourceId, int destId, double distance);
    void updateTraffic(int sourceId, int destId, double newTraffic);
    
    // Dijkstra's Algorithm from startNode to endNode
    // Returns an ordered dynamic array of Node IDs representing the route
    std::vector<int> calculateShortestPath(int startNode, int endNode);
    
    Node getNode(int id) const;
    int numNodes() const;
};

#endif
