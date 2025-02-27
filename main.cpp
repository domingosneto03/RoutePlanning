#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "data_structures/Graph.h"
#include "menu.h"

using namespace std;

template <class T>
void loadLocations(Graph<T>& graph, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string location, id_str, code, parking_str;

        getline(ss, location, ',');
        getline(ss, id_str, ',');
        getline(ss, code, ',');
        getline(ss, parking_str, ',');

        int id = stoi(id_str);
        int parking = stoi(parking_str); // 1 if parking exists, 0 otherwise

        // Add vertex (node) to the graph
        graph.addVertex(id);
        auto v = graph.findVertex(id);
        v->setLocation(location); v->setCode(code); v->setParking(parking);
        graph.storeCode(code, id);
    }
    file.close();
}


template <class T>
void loadDistances(Graph<T>& graph, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string loc1_code, loc2_code, driving_str, walking_str;

        getline(ss, loc1_code, ',');
        getline(ss, loc2_code, ',');
        getline(ss, driving_str, ',');
        getline(ss, walking_str, ',');

        // Convert codes to IDs

        int loc1 = graph.getIdFromCode(loc1_code);
        int loc2 = graph.getIdFromCode(loc2_code);
        double driving = (driving_str == "X") ? INF : stod(driving_str);
        double walking = stod(walking_str);

        // Add bidirectional edges for driving and walking
        graph.addBidirectionalEdge(loc1, loc2, driving, walking);
    }
    file.close();
}


int main() {
    Graph<int> graph;

    loadLocations(graph, "../csv_data/Locations.csv");
    loadDistances(graph, "../csv_data/Distances.csv");
    menu();

    /*
     * Insert a breakpoint here and explore the graph
     */

    return 0;
}

