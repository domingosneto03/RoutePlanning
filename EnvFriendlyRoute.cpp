#include "EnvFriendlyRoute.h"
#include "data_structures/Graph.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <limits>

using namespace std;

bool isParking(Vertex<int>* v) {
    return v && v->getParking() == 1;
}

// Modified Dijkstra to find multiple driving routes to parking spots
vector<EnvFriendlyRoute> findMultipleDrivingRoutes(Graph<int>& g, int source, int destination, int k,
                                                   const vector<int>& avoidNodes, const vector<pair<int, int>>& avoidSegments) {

    struct State {
        int node;
        vector<int> path;
        double time;

        bool operator>(const State& other) const {
            return time > other.time;
        }
    };

    priority_queue<State, vector<State>, greater<>> pq;
    pq.push({source, {source}, 0});

    unordered_map<int, int> pathsFound;
    vector<EnvFriendlyRoute> results;

    while (!pq.empty()) {
        auto [node, path, time] = pq.top(); pq.pop();

        Vertex<int>* v = g.findVertex(node);
        if (!v || find(avoidNodes.begin(), avoidNodes.end(), node) != avoidNodes.end()) continue;

        if (isParking(v) && node != destination) {
            pathsFound[node]++;
            if (pathsFound[node] <= k) {
                results.push_back({node, time, 0, 0, path, {}});
            }
        }

        for (auto e : v->getAdj()) {
            int next = e->getDest()->getInfo();
            if (e->getDrivingWeight() == INF) continue;
            if (find(path.begin(), path.end(), next) != path.end()) continue;
            if (find(avoidNodes.begin(), avoidNodes.end(), next) != avoidNodes.end()) continue;

            if (find(avoidSegments.begin(), avoidSegments.end(), make_pair(node, next)) != avoidSegments.end() ||
                find(avoidSegments.begin(), avoidSegments.end(), make_pair(next, node)) != avoidSegments.end()) continue;

            vector<int> newPath = path;
            newPath.push_back(next);
            pq.push({next, newPath, time + e->getDrivingWeight()});
        }
    }

    return results;
}

pair<vector<int>, double> findWalkingPath(Graph<int>& g, int source, int destination, double maxWalk,
                                          const vector<int>& avoidNodes, const vector<pair<int, int>>& avoidSegments) {

    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    for (auto v : g.getVertexSet()) {
        dist[v->getInfo()] = INF;
    }
    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (d > maxWalk) break;
        if (u == destination) break;
        if (find(avoidNodes.begin(), avoidNodes.end(), u) != avoidNodes.end()) continue;

        Vertex<int>* v = g.findVertex(u);
        if (!v) continue;

        for (auto e : v->getAdj()) {
            int next = e->getDest()->getInfo();
            double w = e->getWalkingWeight();

            if (find(avoidNodes.begin(), avoidNodes.end(), next) != avoidNodes.end()) continue;
            if (find(avoidSegments.begin(), avoidSegments.end(), make_pair(u, next)) != avoidSegments.end() ||
                find(avoidSegments.begin(), avoidSegments.end(), make_pair(next, u)) != avoidSegments.end()) continue;

            if (dist[next] > dist[u] + w) {
                dist[next] = dist[u] + w;
                prev[next] = u;
                pq.push({dist[next], next});
            }
        }
    }

    if (dist[destination] == INF || dist[destination] > maxWalk) {
        return {{}, INF};
    }

    vector<int> path;
    for (int at = destination; at != source; at = prev[at]) {
        path.push_back(at);
    }
    path.push_back(source);
    reverse(path.begin(), path.end());

    return {path, dist[destination]};
}

EnvFriendlyRoute findEnvFriendlyRoute(Graph<int>& g, int source, int destination, double maxWalk,
                                      const vector<int>& avoidNodes, const vector<pair<int, int>>& avoidSegments) {

    vector<EnvFriendlyRoute> candidates = findMultipleDrivingRoutes(g, source, destination, 3, avoidNodes, avoidSegments);
    vector<EnvFriendlyRoute> valid;

    for (auto& c : candidates) {
        auto [walkPath, walkTime] = findWalkingPath(g, c.parkingNode, destination, maxWalk, avoidNodes, avoidSegments);
        if (!walkPath.empty()) {
            double total = c.drivingTime + walkTime;
            valid.push_back({c.parkingNode, c.drivingTime, walkTime, total, c.drivingPath, walkPath});
        }
    }

    if (valid.empty()) {
        return EnvFriendlyRoute{-1, 0, 0, 0, {}, {}, "No valid routes within max walking time or due to avoids."};
    }

    sort(valid.begin(), valid.end(), [](const auto& a, const auto& b) {
        if (a.totalTime != b.totalTime) return a.totalTime < b.totalTime;
        return a.walkingTime > b.walkingTime;
    });

    EnvFriendlyRoute best = valid[0];
    for (size_t i = 1; i < valid.size(); ++i) {
        best.alternatives.push_back(valid[i]);
    }

    return best;
}
