#include "AlternativeRoute.h"
#include "EnvFriendlyRoute.h"
#include "data_structures/Graph.h"
#include <unordered_set>
#include <iostream>

std::vector<EnvFriendlyRoute> AlternativeRoute::findTwoSolutions(
    Graph<int>& graph,
    int source,
    int destination,
    double maxWalkTime,
    const std::vector<int>& avoidNodes,
    const std::vector<std::pair<int, int>>& avoidSegments) {

    std::vector<EnvFriendlyRoute> twosolutions;
    std::unordered_set<int> usedParkingNodes;  // Track used parking nodes

    // Find the best route
    EnvFriendlyRoute bestRoute = findEnvFriendlyRoute(graph, source, destination, maxWalkTime, avoidNodes, avoidSegments);

    // If the best route is valid, add it
    if (bestRoute.parkingNode != -1) {
        twosolutions.push_back(bestRoute);
        usedParkingNodes.insert(bestRoute.parkingNode);
    }

    // Check if the best route has alternatives and add them
    for (const auto& alt : bestRoute.alternatives) {
        if (twosolutions.size() < 2 && usedParkingNodes.find(alt.parkingNode) == usedParkingNodes.end()) {
            twosolutions.push_back(alt);
            usedParkingNodes.insert(alt.parkingNode);
        }
    }

    // If not enough solutions, continue searching with increased walking time
    double currentMaxWalkTime = maxWalkTime;
    int attemptCount = 0;
    const int maxAttempts = 20;

    while (twosolutions.size() < 2 && attemptCount < maxAttempts) {
        currentMaxWalkTime += 5;  // Increment walking time
        attemptCount++;

        EnvFriendlyRoute altRoute = findEnvFriendlyRoute(graph, source, destination, currentMaxWalkTime, avoidNodes, avoidSegments);

        if (altRoute.parkingNode != -1) {
            // Check if the main route is distinct
            if (usedParkingNodes.find(altRoute.parkingNode) == usedParkingNodes.end()) {
                twosolutions.push_back(altRoute);
                usedParkingNodes.insert(altRoute.parkingNode);
            }

            // Now, check inside its alternatives
            for (const auto& alt : altRoute.alternatives) {
                if (twosolutions.size() < 2 && usedParkingNodes.find(alt.parkingNode) == usedParkingNodes.end()) {
                    twosolutions.push_back(alt);
                    usedParkingNodes.insert(alt.parkingNode);
                }
            }
        }

        // Nested check: If we still need more solutions, check inside alternatives
        if (twosolutions.size() < 2 && !twosolutions.empty()) {
            for (const auto& alt : twosolutions.front().alternatives) {
                if (twosolutions.size() < 2 && usedParkingNodes.find(alt.parkingNode) == usedParkingNodes.end()) {
                    twosolutions.push_back(alt);
                    usedParkingNodes.insert(alt.parkingNode);
                }
            }
        }
    }

    // **Sorting to prioritize best solutions**
    std::sort(twosolutions.begin(), twosolutions.end(), [](const EnvFriendlyRoute& a, const EnvFriendlyRoute& b) {
        if (a.totalTime != b.totalTime)
            return a.totalTime < b.totalTime;  // Prefer lower total time
        return a.walkingTime > b.walkingTime; // If equal total time, prefer higher walking time
    });

    return twosolutions;
}
