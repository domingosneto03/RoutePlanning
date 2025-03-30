#include "EnvFriendlyRoute.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>

using namespace std;

bool isAvoided(int node, const vector<int>& avoidNodes) {
    return find(avoidNodes.begin(), avoidNodes.end(), node) != avoidNodes.end();
}

bool isBlocked(int u, int v, const vector<pair<int, int>>& avoidSegments) {
    return find(avoidSegments.begin(), avoidSegments.end(), make_pair(u, v)) != avoidSegments.end() ||
           find(avoidSegments.begin(), avoidSegments.end(), make_pair(v, u)) != avoidSegments.end();
}

vector<int> reconstructPath(int src, int tgt, unordered_map<int, int>& prev) {
    vector<int> path;
    for (int at = tgt; at != src; at = prev[at]) {
        path.push_back(at);
    }
    path.push_back(src);
    reverse(path.begin(), path.end());
    return path;
}

unordered_map<int, pair<double, vector<int>>> runDijkstra(
        Graph<int>& g, int start,
        bool useDriving,
        const vector<int>& avoidNodes,
        const vector<pair<int, int>>& avoidSegments) {

    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    for (auto v : g.getVertexSet()) {
        dist[v->getInfo()] = INF;
    }

    dist[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty()) {
        auto [cost, u] = pq.top(); pq.pop();

        if (isAvoided(u, avoidNodes)) continue;
        Vertex<int>* v = g.findVertex(u);
        if (!v) continue;

        for (auto e : v->getAdj()) {
            int next = e->getDest()->getInfo();
            if (isAvoided(next, avoidNodes)) continue;
            if (isBlocked(u, next, avoidSegments)) continue;

            double weight = useDriving ? e->getDrivingWeight() : e->getWalkingWeight();
            if (weight >= INF) continue;

            if (dist[next] > dist[u] + weight) {
                dist[next] = dist[u] + weight;
                prev[next] = u;
                pq.emplace(dist[next], next);
            }
        }
    }

    unordered_map<int, pair<double, vector<int>>> result;
    for (auto& [node, d] : dist) {
        if (d < INF && node != start && prev.count(node)) {
            result[node] = {d, reconstructPath(start, node, prev)};
        }
    }

    return result;
}

EnvFriendlyRoute findEnvFriendlyRoute(
        Graph<int>& g,
        int source,
        int destination,
        double maxWalk,
        const vector<int>& avoidNodes,
        const vector<pair<int, int>>& avoidSegments) {

    auto driveMap = runDijkstra(g, source, true, avoidNodes, avoidSegments);
    auto walkMap = runDijkstra(g, destination, false, avoidNodes, avoidSegments); // reversed Dijkstra

    vector<EnvFriendlyRoute> valid;

    for (auto& [parkingNode, driveInfo] : driveMap) {
        Vertex<int>* pv = g.findVertex(parkingNode);
        if (!pv || pv->getParking() != 1) continue;

        if (!walkMap.count(parkingNode)) continue;
        auto [walkTime, walkPath] = walkMap[parkingNode];
        if (walkTime > maxWalk) continue;
        reverse(walkPath.begin(), walkPath.end());
        double totalTime = driveInfo.first + walkTime;
        valid.push_back({
                                parkingNode,
                                driveInfo.first,
                                walkTime,
                                totalTime,
                                driveInfo.second,
                                walkPath
                        });
    }

    if (valid.empty()) {
        bool anyParkingCandidate = false;
        bool anyWalkableCandidate = false;

        for (auto& [parkingNode, driveInfo] : driveMap) {
            Vertex<int>* pv = g.findVertex(parkingNode);
            if (!pv || pv->getParking() != 1) continue;

            anyParkingCandidate = true;

            if (walkMap.count(parkingNode)) {
                auto [walkTime, _] = walkMap[parkingNode];
                if (walkTime <= maxWalk) {
                    anyWalkableCandidate = true;
                    break;
                }
            }
        }

        string reason;
        if (!anyParkingCandidate && !anyWalkableCandidate) {
            reason = "No parking nodes reachable from source and walking routes exceed max walking time.";
        } else if (!anyParkingCandidate) {
            reason = "No parking nodes reachable from source.";
        } else {
            reason = "All walking routes from parking exceed max walking time.";
        }
        return EnvFriendlyRoute{-1, 0, 0, 0, {}, {}, reason};
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
