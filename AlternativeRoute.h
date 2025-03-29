#ifndef ALTERNATIVEROUTE_H
#define ALTERNATIVEROUTE_H

#include <vector>
#include <utility> // For std::pair
#include "EnvFriendlyRoute.h"
#include "data_structures/Graph.h"

class AlternativeRoute {
public:
    // Declare the function
    static std::vector<EnvFriendlyRoute> findAlternatives(
        Graph<int>& graph,
        int source,
        int destination,
        double maxWalkTime,
        const std::vector<int>& avoidNodes,
        const std::vector<std::pair<int, int>>& avoidSegments
    );
};

#endif // ALTERNATIVEROUTE_H