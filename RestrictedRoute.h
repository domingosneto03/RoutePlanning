#ifndef RESTRICTED_ROUTE_H
#define RESTRICTED_ROUTE_H

#include "data_structures/Graph.h"

/**
 * Computes the fastest driving route from source to destination,
 * avoiding specified nodes and segments and optionally passing through a given includeNode.
 * Returns the sequence of node IDs that represent the path.
 */
std::vector<int> restrictedDrivingRoute(
        Graph<int> &graph,
        int source,
        int destination,
        const std::vector<int> &avoidNodes,
        const std::vector<std::pair<int, int>> &avoidSegments,
        int includeNode);

#endif // RESTRICTED_ROUTE_H
