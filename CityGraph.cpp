#include "CityGraph.h"
#include <queue>
#include <limits>
#include <algorithm>

void CityGraph::addNode(int id, int x, int y) {
    nodes[id] = {id, x, y};
}

void CityGraph::addEdge(int sourceId, int destId, double distance) {
    adjList[sourceId].push_back({destId, distance, 1.0});
}

void CityGraph::updateTraffic(int sourceId, int destId, double newTraffic) {
    for (auto& edge : adjList[sourceId]) {
        if (edge.destinationNodeId == destId) {
            edge.trafficMultiplier = newTraffic;
            break;
        }
    }
}

std::vector<int> CityGraph::calculateShortestPath(int startNode, int endNode) {
    if (startNode == endNode) return {};

    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;
    
    using pdi = std::pair<double, int>;
    std::priority_queue<pdi, std::vector<pdi>, std::greater<pdi>> pq;

    for (const auto& pair : nodes) {
        dist[pair.first] = std::numeric_limits<double>::infinity();
        prev[pair.first] = -1;
    }

    dist[startNode] = 0.0;
    pq.push({0.0, startNode});

    while (!pq.empty()) {
        double currentDist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (u == endNode) break;
        if (currentDist > dist[u]) continue;

        for (const auto& edge : adjList[u]) {
            int v = edge.destinationNodeId;
            double weight = edge.baseDistance * edge.trafficMultiplier;
            double alt = dist[u] + weight;
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
                pq.push({alt, v});
            }
        }
    }

    std::vector<int> path;
    int curr = endNode;
    if (prev.find(curr) != prev.end() && (prev[curr] != -1 || curr == startNode)) {
        while (curr != -1) {
            path.push_back(curr);
            curr = prev[curr];
        }
        std::reverse(path.begin(), path.end());
    }

    if (!path.empty() && path.front() == startNode) {
        path.erase(path.begin());
    }

    return path;
}

Node CityGraph::getNode(int id) const {
    if (nodes.find(id) != nodes.end()) {
        return nodes.at(id);
    }
    return {-1, -1, -1}; // safe fallback
}

int CityGraph::numNodes() const {
    return nodes.size();
}
