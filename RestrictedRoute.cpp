#include "RestrictedRoute.h"
#include "data_structures/MutablePriorityQueue.h"
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <iostream>

using namespace std;

struct pair_hash {
    size_t operator()(const pair<int, int> &p) const {
        auto h1 = hash<int>{}(p.first);
        auto h2 = hash<int>{}(p.second);
        return h1 ^ (h2 << 1); // combine hashes
    }
};

template <class T>
bool relaxDrivingOnly(Edge<T> *edge) {
    auto u = edge->getOrig();
    auto v = edge->getDest();
    double weight = edge->getDrivingWeight();
    if (u->getDist() + weight < v->getDist()) {
        v->setDist(u->getDist() + weight);
        v->setPath(edge);
        return true;
    }
    return false;
}

template <class T>
void dijkstraDriving(Graph<T> *g, int origin, const unordered_set<int> &avoidNodes, const unordered_set<pair<int, int>, pair_hash> &avoidSegments) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }

    auto s = g->findVertex(origin);
    if (!s) return;

    s->setDist(0);
    MutablePriorityQueue<Vertex<T>> q;
    q.insert(s);

    while (!q.empty()) {
        auto v = q.extractMin();
        for (auto e : v->getAdj()) {
            auto u = e->getOrig()->getInfo();
            auto v_ = e->getDest()->getInfo();

            if (avoidNodes.count(v_)) continue;
            if (avoidSegments.count({u, v_}) || avoidSegments.count({v_, u})) continue;

            auto oldDist = e->getDest()->getDist();
            if (relaxDrivingOnly(e)) {
                if (oldDist == INF) q.insert(e->getDest());
                else q.decreaseKey(e->getDest());
            }
        }
    }
}

template <class T>
vector<T> getDrivingPath(Graph<T> *g, int origin, int dest) {
    vector<T> path;
    auto v = g->findVertex(dest);
    if (!v || v->getDist() == INF) return {};
    while (v) {
        path.push_back(v->getInfo());
        if (!v->getPath()) break;
        v = v->getPath()->getOrig();
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<int> restrictedDrivingRoute(Graph<int> &graph, int source, int destination, const vector<int> &avoidNodes, const vector<pair<int, int>> &avoidSegments, int includeNode) {
    unordered_set<int> avoidNodeSet(avoidNodes.begin(), avoidNodes.end());
    unordered_set<pair<int, int>, pair_hash> avoidSegmentSet(avoidSegments.begin(), avoidSegments.end());

    // If there's a node that MUST be visited, split into two Dijkstra runs
    if (includeNode != -1 && includeNode != source && includeNode != destination) {
        auto toIncludePath = restrictedDrivingRoute(graph, source, includeNode, avoidNodes, avoidSegments, -1);
        auto fromIncludePath = restrictedDrivingRoute(graph, includeNode, destination, avoidNodes, avoidSegments, -1);

        if (toIncludePath.empty() || fromIncludePath.empty()) return {};

        toIncludePath.pop_back(); // Avoid duplication
        toIncludePath.insert(toIncludePath.end(), fromIncludePath.begin(), fromIncludePath.end());
        return toIncludePath;
    }

    dijkstraDriving(&graph, source, avoidNodeSet, avoidSegmentSet);
    return getDrivingPath(&graph, source, destination);
}
