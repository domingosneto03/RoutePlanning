#ifndef ENVFRIENDLYROUTE_H
#define ENVFRIENDLYROUTE_H

#include <vector>
#include <string>
#include "data_structures/Graph.h"

struct EnvFriendlyRoute {
    int parkingNode = -1;
    double drivingTime = 0;
    double walkingTime = 0;
    double totalTime = 0;
    std::vector<int> drivingPath;
    std::vector<int> walkingPath;
    std::string message;

    // Multiple alternative routes (if any)
    std::vector<EnvFriendlyRoute> alternatives;
};

EnvFriendlyRoute findEnvFriendlyRoute(
        Graph<int>& graph,
        int source,
        int destination,
        double maxWalkTime,
        const std::vector<int>& avoidNodes,
        const std::vector<std::pair<int, int>>& avoidSegments
);

#endif // ENVFRIENDLYROUTE_H
