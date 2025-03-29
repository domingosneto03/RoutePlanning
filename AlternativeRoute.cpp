#include "AlternativeRoute.h"
#include "EnvFriendlyRoute.h"
#include <vector>
#include <unordered_set>

std::vector<EnvFriendlyRoute> AlternativeRoute::findAlternatives(
    Graph<int>& graph,
    int source,
    int destination,
    double maxWalkTime,
    const std::vector<int>& avoidNodes,
    const std::vector<std::pair<int, int>>& avoidSegments
) {
    std::vector<EnvFriendlyRoute> alternatives;
    std::unordered_set<int> usedParkingNodes; // Track unique parking locations
    double increasedWalkTime = maxWalkTime;

    while (alternatives.size() < 2) { // Keep searching until we have 2 distinct alternatives
        increasedWalkTime += 5; // Increase max walk time gradually

        EnvFriendlyRoute altRoute = findEnvFriendlyRoute(
            graph, source, destination, increasedWalkTime, avoidNodes, avoidSegments);

        // Ensure it's a valid and unique alternative
        if (altRoute.parkingNode != -1 && usedParkingNodes.find(altRoute.parkingNode) == usedParkingNodes.end()) {
            alternatives.push_back(altRoute);
            usedParkingNodes.insert(altRoute.parkingNode);
        }
    }

    return alternatives;
}