#include "BestRoute.h"
#include "data_structures/MutablePriorityQueue.h"
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

bool relax(Edge<int> *edge) {
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

void dijkstra(Graph<int> &graph, int source) {
    for (auto v : graph.getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }
    auto s = graph.findVertex(source);
    if (!s) return;

    s->setDist(0);
    MutablePriorityQueue<Vertex<int>> q;
    q.insert(s);

    while (!q.empty()) {
        auto v = q.extractMin();
        for (auto e : v->getAdj()) {
            auto oldDist = e->getDest()->getDist();
            if (relax(e)) {
                if (oldDist == INF) q.insert(e->getDest());
                else q.decreaseKey(e->getDest());
            }
        }
    }
}

vector<int> findBestRoute(Graph<int> &graph, int source, int destination, double &totalTime) {

    vector<int> path;
    dijkstra(graph, source);
    auto v = graph.findVertex(destination);
    if (!v || v->getDist() == INF) return {}; // Return empty vector if no path exists

    while (v) {
        path.push_back(v->getInfo());
        if (!v->getPath()) break;
        v = v->getPath()->getOrig();
    }
    std::reverse(path.begin(), path.end());
    totalTime = graph.findVertex(destination)->getDist();
    return path;
}

vector <int> findAlternativeRoute(Graph<int> &graph, int source, int destination, const std::vector<int> &bestPath, double &altTime) {
    std::unordered_map<int, bool> bestPathNodes;
    vector<int> altPath;
    for (int node : bestPath) {
        bestPathNodes[node] = true;
    }

    for (auto v : graph.getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }

    auto s = graph.findVertex(source);
    if (!s) return {};
    s->setDist(0);

    MutablePriorityQueue<Vertex<int>> q;
    q.insert(s);

    while (!q.empty()) {
        auto v = q.extractMin();
        for (auto e : v->getAdj()) {
            int vInfo = e->getDest()->getInfo();
            if (bestPathNodes[vInfo] && vInfo != source && vInfo != destination) continue;

            auto oldDist = e->getDest()->getDist();
            if (relax(e)) {
                if (oldDist == INF) q.insert(e->getDest());
                else q.decreaseKey(e->getDest());
            }
        }
    }

    auto v = graph.findVertex(destination);
    if (!v || v->getDist() == INF) return {};

    while (v) {
        altPath.push_back(v->getInfo());
        if (!v->getPath()) break;
        v = v->getPath()->getOrig();
    }
    std::reverse(altPath.begin(), altPath.end());
    altTime = graph.findVertex(destination)->getDist();
    return altPath;
}