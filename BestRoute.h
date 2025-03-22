#ifndef BEST_ROUTE_H
#define BEST_ROUTE_H

#include "data_structures/Graph.h"
#include <vector>

std::vector<int> findBestRoute(Graph<int> &graph, int source, int destination, double &totalTime);
std::vector<int> findAlternativeRoute(Graph<int> &graph, int source, int destination, const std::vector<int> &bestPath, double &altTime);

#endif // BEST_ROUTE_H