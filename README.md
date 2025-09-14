# Route Planning Tool
This project consists of a route planning tool developed for the Design of Algorithms course. The goal was to implement several shortest-path algorithms in the context of an urban mobility system.
The tool processes a dataset representing a city's road network and allows the user to compute optimized routes based on different constraints and mobility modes.

## Main Features
- Fastest route between two locations (driving only)
- Alternative route avoiding shared segments with the main route
- Restricted route that avoids specific nodes or road segments
- Environmentally-friendly route combining driving and walking, with parking constraints

## Algorithm and Data Structures
- Greedy-based routing using Dijkstra's algorithm
- Modified graph traversal with node/edge exclusions
- Extended support for dual-mode routing (driving + walking)
- Approximation heuristics for fallback scenarios
