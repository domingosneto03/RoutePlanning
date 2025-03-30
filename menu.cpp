#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <fstream>
#include <filesystem>
#include "menu.h"
#include "reader.h"
#include "cmake-build-debug/batch/batch.h"
#include "RestrictedRoute.h"
#include "BestRoute.h"
#include "data_structures/Graph.h"
#include "EnvFriendlyRoute.h"
#include "AlternativeRoute.h"

using namespace std;

// ----------------------------------------------------------
// HELPER FUNCTIONS FOR USER INPUT
// ----------------------------------------------------------

/**
 * Reads an integer from user, ensuring it's one of the valid choices passed in validChoices.
 * E.g., validChoices = {1, 2, 3}.
 * Loops until a valid input is entered, returning that integer.
 */
int readIntChoice(const string &prompt, const vector<int> &validChoices) {
    while (true) {
        cout << prompt;
        string input;
        cin >> input; // read raw string

        try {
            int val = stoi(input);
            // Check if val is in validChoices
            if (find(validChoices.begin(), validChoices.end(), val) != validChoices.end()) {
                return val;
            } else {
                cout << "Invalid choice. Expected one of: ";
                for (auto c : validChoices) {
                    cout << c << " ";
                }
                cout << "\n";
            }
        } catch (...) {
            // stoi failed or something else
            cout << "Please enter a valid integer.\n";
        }
    }
}

/**
 * Reads any integer from user input.
 * Loops until a valid integer is provided (no set of valid choices).
 */
int readAnyInteger(const string &prompt, const Graph<int>& graph) {
    while (true) {
        cout << prompt;
        string input;
        cin >> input;

        try {
            int val = stoi(input);
            if (isValidNode(graph, val) || (val==-1 && prompt.starts_with("Enter a Node to Include")) || prompt.starts_with("Enter Max Walking Time")) {
                return val;
            } else {
                cout << "Node id " << val << " does not exist in the graph. Try again.\n";
            }
        } catch (...) {
            cout << "Invalid integer. Please try again.\n";
        }
    }
}

/**
 * Reads a line of comma-separated integers from the user.
 * Returns an empty vector if the line is blank.
 * If any token is invalid, the user is prompted to retry.
 */
vector<int> readCommaSeparatedInts(const string &prompt, const Graph<int>& graph) {
    while (true) {
        cout << prompt;
        // clear leftover newline if present
        if (cin.peek() == '\n') {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        string line;
        getline(cin, line);

        if (line.empty()) {
            // user left it blank -> no avoids
            return {};
        }

        vector<int> results;
        bool valid = true;
        stringstream ss(line);
        string token;
        while (getline(ss, token, ',')) {
            try {
                int v = stoi(token);
                if (!isValidNode(graph, v)) {
                    cout << "Node id " << v << " does not exist in the graph.\n";
                    valid = false;
                    break;
                }
                results.push_back(v);
            } catch (...) {
                cout << "Invalid integer in list: \"" << token << "\". Please try again.\n";
                valid = false;
                break;
            }
        }

        if (valid) {
            return results;
        }
        // If invalid, loop again
    }
}

/**
 * Reads space-separated segments in the format (id1,id2).
 * Returns an empty vector if the user inputs a blank line.
 * If any token is invalid, prompt is repeated.
 */
vector<pair<int,int>> readSegments(const string &prompt, const Graph<int>& graph) {
    while (true) {
        cout << prompt;

        string line;
        getline(cin, line);

        if (line.empty()) {
            // no segments
            return {};
        }

        vector<pair<int,int>> segments;
        bool valid = true;
        stringstream ss(line);
        string segment;

        // e.g.: (1,2) (3,4)
        while (ss >> segment) {
            if (segment.front() != '(' || segment.back() != ')') {
                cout << "Segment \"" << segment << "\" missing parentheses. Use (id1,id2)\n";
                valid = false;
                break;
            }

            // remove ( and )
            segment = segment.substr(1, segment.size() - 2);

            // parse inside
            stringstream pairStream(segment);
            string id1, id2;
            if (!getline(pairStream, id1, ',') || !getline(pairStream, id2, ',')) {
                cout << "Segment \"" << segment << "\" not in format id1,id2.\n";
                valid = false;
                break;
            }

            try {
                int i1 = stoi(id1);
                int i2 = stoi(id2);
                if (!isValidNode(graph, i1) || !isValidNode(graph, i2)) {
                    cout << "Segment contains node not in graph: (" << i1 << "," << i2 << ")\n";
                    valid = false;
                    break;
                }
                segments.emplace_back(i1, i2);
            } catch (...) {
                cout << "Invalid integer in segment: (" << id1 << "," << id2 << ").\n";
                valid = false;
                break;
            }
        }

        if (valid) {
            return segments;
        }
        // otherwise, prompt again
    }
}

bool isValidNode(const Graph<int>& graph, int id) {
    return graph.findVertex(id) != nullptr;
}

bool isValidEnv(const Graph<int>& graph, int source, int destination) {
    if (source == destination) {
        cout << "Destination must be different from source. Try again.\n";
        return false;
    }

    Vertex<int>* src = graph.findVertex(source);
    Vertex<int>* dst = graph.findVertex(destination);

    if (!src || !dst) return false;

    if (src->getParking() == 1 || dst->getParking() == 1) {
        cout << "Source and Destination cannot be parking nodes. Try again.\n";
        return false;
    }

    for (auto e : src->getAdj()) {
        if (e->getDest()->getInfo() == destination) {
            cout << "Source and Destination cannot be directly connected. Try again.\n";
            return false;
        }
    }

    return true;
}



// ----------------------------------------------------------
// DISPLAY FUNCTIONS
// ----------------------------------------------------------

void displayMainMenu() {
    cout << "\n===== Route Planning Tool =====\n";
    cout << "1. Driving Only\n";
    cout << "2. Driving and Walking\n";
    cout << "3. Exit\n";
    // no immediate prompt here because we handle input with readIntChoice
}

void displayDrivingMenu() {
    cout << "\n===== Driving Only Functions =====\n";
    cout << "1. Find Best Route\n";
    cout << "2. Find Restricted Route\n";
    cout << "3. Back to Main Menu\n";
}

void displayDrivingWalkingMenu() {
    cout << "\n===== Driving and Walking Functions =====\n";
    cout << "1. Find Best Driving and Walking Route\n";
    cout << "2. Find aproximate solution\n";
    cout << "3. Back to Main Menu\n";
}

// ----------------------------------------------------------
// SUB-MENU HANDLERS
// ----------------------------------------------------------

/**
 * Handles the Driving-Only sub-menu logic.
 * Loops until user chooses option 3 (Back to Main Menu).
 */
void handleDrivingSubMenu(Graph<int>& graph) {
    while (true) {
        displayDrivingMenu();
        // only accept 1, 2, or 3
        int subVal = readIntChoice("Enter your choice: ", {1, 2, 3});

        switch (subVal) {
            case 1: {
                cout << "Finding Best Driving Route...\n";
                int source = readAnyInteger("Enter Source ID: ", graph);
                int destination;
                while (true) {
                    destination = readAnyInteger("Enter Destination ID: ", graph);
                    if (destination != source) break;
                    cout << "Destination must be different from source. Try again.\n";
                }
                double bestTime, altTime;
                auto bestPath = findBestRoute(graph, source, destination, bestTime);
                auto altPath = bestPath.empty() ? std::vector<int>() : findAlternativeRoute(graph, source, destination, bestPath, altTime);

                cout << "Source:" << source << "\n";
                cout << "Destination:" << destination << "\n";

                if (bestPath.empty()) {
                    cout << "BestDrivingRoute:none\n";
                    cout << "AlternativeDrivingRoute:none\n";
                } else {
                    cout << "BestDrivingRoute:";
                    for (size_t i = 0; i < bestPath.size(); ++i) {
                        cout << bestPath[i];
                        if (i != bestPath.size() - 1) cout << ",";
                    }
                    cout << "(" << bestTime << ")\n";

                    if (altPath.empty()) {
                        cout << "AlternativeDrivingRoute:none\n";
                    } else {
                        cout << "AlternativeDrivingRoute:";
                        for (size_t i = 0; i < altPath.size(); ++i) {
                            cout << altPath[i];
                            if (i != altPath.size() - 1) cout << ",";
                        }
                        cout << "(" << altTime << ")\n";
                    }
                }
                break;
            }
            case 2: {
                cout << "\n--- Restricted Driving Route ---\n";
                int source       = readAnyInteger("Enter Source ID: ", graph);
                int destination;
                while (true) {
                    destination = readAnyInteger("Enter Destination ID: ", graph);
                    if (destination != source) break;
                    cout << "Destination must be different from source. Try again.\n";
                }
                vector<int> avoidNodes;
                while (true) {
                    avoidNodes = readCommaSeparatedInts("Enter Nodes to Avoid (comma-separated, leave blank if none): ", graph);
                    if (find(avoidNodes.begin(), avoidNodes.end(), source) != avoidNodes.end() || find(avoidNodes.begin(), avoidNodes.end(), destination) != avoidNodes.end()) {
                        cout << "Source and Destination cannot be in the set of nodes to avoid. Please try again.\n";
                        continue;
                    }
                    break;
                }
                auto avoidSegs   = readSegments("Enter Segments to Avoid (format: (id1,id2) space-separated, blank if none): ", graph);
                int includeNode;
                while (true) {
                    includeNode = readAnyInteger("Enter a Node to Include in Route (or -1 if none): ", graph);
                    if (includeNode == -1 || find(avoidNodes.begin(), avoidNodes.end(), includeNode) == avoidNodes.end()) break;
                    cout << "Include node cannot be in the set of nodes to avoid. Please try again.\n";
                }

                cout << "Finding Restricted Driving Route...\n";
                auto path = restrictedDrivingRoute(graph, source, destination, avoidNodes, avoidSegs, includeNode);
                if (path.empty()) {
                    cout << "RestrictedDrivingRoute:none\n";
                } else {
                    cout << "RestrictedDrivingRoute:";
                    int totalTime = 0;
                    for (size_t i = 0; i < path.size(); ++i) {
                        cout << path[i];
                        if (i + 1 < path.size()) {
                            cout << ",";
                            auto u = graph.findVertex(path[i]);
                            for (auto e : u->getAdj()) {
                                if (e->getDest()->getInfo() == path[i+1]) {
                                    totalTime += e->getDrivingWeight();
                                    break;
                                }
                            }
                        }
                    }
                    cout << "(" << totalTime << ")\n";
                }
                break;
            }
            case 3:
                cout << "Returning to Main Menu...\n";
                return; // exit this sub-menu
        }
    }
}

/**
 * Handles the Driving+Walking sub-menu logic.
 * Loops until user chooses option 3 (Back to Main Menu).
 */
void handleDrivingWalkingSubMenu(Graph<int>& graph) {
    while (true) {
        displayDrivingWalkingMenu();
        int subVal = readIntChoice("Enter your choice: ", {1, 2, 3, 4});

        switch (subVal) {
            case 1: {
                cout << "\n--- Environmentally-Friendly Route ---\n";
                int source; int destination;
                while (true) {
                    source = readAnyInteger("Enter Source ID: ", graph);
                    destination = readAnyInteger("Enter Destination ID: ", graph);
                    if (isValidEnv(graph, source, destination)) break;
                }
                double maxWalk = readAnyInteger("Enter Max Walking Time (minutes): ", graph);
                vector<int> avoidNodes;
                while (true) {
                    avoidNodes = readCommaSeparatedInts("Enter Nodes to Avoid (comma-separated, leave blank if none): ", graph);
                    if (find(avoidNodes.begin(), avoidNodes.end(), source) != avoidNodes.end() || find(avoidNodes.begin(), avoidNodes.end(), destination) != avoidNodes.end()) {
                        cout << "Source and Destination cannot be in the set of nodes to avoid. Please try again.\n";
                        continue;
                    }
                    break;
                }
                auto avoidSegs  = readSegments("Enter Segments to Avoid (format: (id1,id2) space-separated, blank if none): ", graph);

                EnvFriendlyRoute route = findEnvFriendlyRoute(graph, source, destination, maxWalk, avoidNodes, avoidSegs);


                cout << "Source:" << source << "\n";
                cout << "Destination:" << destination << "\n";

                if (route.parkingNode == -1) {
                    cout << "DrivingRoute:none\n";
                    cout << "ParkingNode:none\n";
                    cout << "WalkingRoute:none\n";
                    cout << "TotalTime:\n";
                    cout << "Message:" << route.message << "\n";
                } else {
                    cout << "DrivingRoute:";
                    for (size_t i = 0; i < route.drivingPath.size(); ++i) {
                        cout << route.drivingPath[i];
                        if (i + 1 < route.drivingPath.size()) cout << ",";
                    }
                    cout << "(" << route.drivingTime << ")\n";

                    cout << "ParkingNode:" << route.parkingNode << "\n";

                    cout << "WalkingRoute:";
                    for (size_t i = 0; i < route.walkingPath.size(); ++i) {
                        cout << route.walkingPath[i];
                        if (i + 1 < route.walkingPath.size()) cout << ",";
                    }
                    cout << "(" << route.walkingTime << ")\n";

                    cout << "TotalTime:" << route.totalTime << "\n";
                }
                break;
            }

            case 2: {
                cout << "Finding Approximate Solution...\n";

                // User input for alternative route options
                int source; int destination;
                while (true) {
                    source = readAnyInteger("Enter Source ID: ", graph);
                    destination = readAnyInteger("Enter Destination ID: ", graph);
                    if (isValidEnv(graph, source, destination)) break;
                }
                double maxWalk = readAnyInteger("Enter Max Walking Time (minutes): ", graph);
                vector<int> avoidNodes;
                while (true) {
                    avoidNodes = readCommaSeparatedInts("Enter Nodes to Avoid (comma-separated, leave blank if none): ", graph);
                    if (find(avoidNodes.begin(), avoidNodes.end(), source) != avoidNodes.end() || find(avoidNodes.begin(), avoidNodes.end(), destination) != avoidNodes.end()) {
                        cout << "Source and Destination cannot be in the set of nodes to avoid. Please try again.\n";
                        continue;
                    }
                    break;
                }
                auto avoidSegs  = readSegments("Enter Segments to Avoid (format: (id1,id2), space-separated, blank if none): ", graph);

                // Fetch alternative routes
                auto alternatives = AlternativeRoute::findTwoSolutions(graph, source, destination, maxWalk, avoidNodes, avoidSegs);

                if (!alternatives.empty()) {
                    cout << "Source:" << source << "\n";
                    cout << "Destination:" << destination << "\n";
                    for (size_t i = 0; i < alternatives.size(); ++i) {
                        cout << "DrivingRoute" << (i + 1) << ": ";
                        for (size_t j = 0; j < alternatives[i].drivingPath.size(); ++j) {
                            cout << alternatives[i].drivingPath[j];
                            if (j + 1 < alternatives[i].drivingPath.size()) cout << ",";
                        }
                        cout << "(" << alternatives[i].drivingTime << ")\n";

                        cout << "ParkingNode" << (i + 1) << ": " << alternatives[i].parkingNode << "\n";

                        cout << "WalkingRoute" << (i + 1) << ": ";
                        for (size_t j = 0; j < alternatives[i].walkingPath.size(); ++j) {
                            cout << alternatives[i].walkingPath[j];
                            if (j + 1 < alternatives[i].walkingPath.size()) cout << ",";
                        }
                        cout << "(" << alternatives[i].walkingTime << ")\n";

                        cout << "TotalTime" << (i + 1) << ": " << alternatives[i].totalTime << "\n";
                    }
                } else {
                    cout << "No alternative routes found.\n";
                }
                break;
            }

            case 3:
                cout << "Returning to Main Menu...\n";
                return; // exit sub-menu
        }
    }
}

// ----------------------------------------------------------
// BATCH MODE
// ----------------------------------------------------------

void runBatchMode(Graph<int>& graph) {
    ifstream inFile("batch/input.txt");
    ofstream outFile("batch/output.txt");
    if (!inFile.is_open() || !outFile.is_open()) return;

    string line;
    while (getline(inFile, line)) {
        if (line.starts_with("Mode:")) {
            string mode = line.substr(5);

            // Shared variables
            int source = -1, destination = -1, includeNode = -1;
            double maxWalk = 0;
            vector<int> avoidNodes;
            vector<pair<int, int>> avoidSegs;

            // Read block
            while (getline(inFile, line) && !line.starts_with("---")) {
                if (line.starts_with("Source:")) {
                    source = stoi(line.substr(7));
                } else if (line.starts_with("Destination:")) {
                    destination = stoi(line.substr(12));
                } else if (line.starts_with("AvoidNodes:")) {
                    avoidNodes.clear();
                    string nodes = line.substr(11);
                    stringstream ss(nodes);
                    string tok;
                    while (getline(ss, tok, ',')) {
                        if (!tok.empty()) avoidNodes.push_back(stoi(tok));
                    }
                } else if (line.starts_with("AvoidSegments:")) {
                    avoidSegs.clear();
                    string segments = line.substr(14);
                    stringstream ss(segments);
                    string seg;
                    while (ss >> seg) {
                        if (seg.front() == '(' && seg.back() == ')') {
                            seg = seg.substr(1, seg.size() - 2);
                            size_t comma = seg.find(',');
                            if (comma != string::npos) {
                                int a = stoi(seg.substr(0, comma));
                                int b = stoi(seg.substr(comma + 1));
                                avoidSegs.emplace_back(a, b);
                            }
                        }
                    }
                } else if (line.starts_with("IncludeNode:")) {
                    includeNode = stoi(line.substr(12));
                } else if (line.starts_with("MaxWalkTime:")) {
                    maxWalk = stod(line.substr(13));
                }
            }

            // Dispatch to correct function
            if (mode == "driving") {
                double bestTime, altTime;
                auto bestPath = findBestRoute(graph, source, destination, bestTime);
                auto altPath = bestPath.empty() ? vector<int>() : findAlternativeRoute(graph, source, destination, bestPath, altTime);
                outFile << "Source:" << source << "\nDestination:" << destination << "\n";
                if (bestPath.empty()) {
                    outFile << "BestDrivingRoute:none\nAlternativeDrivingRoute:none\n";
                } else {
                    outFile << "BestDrivingRoute:";
                    for (size_t i = 0; i < bestPath.size(); ++i) {
                        outFile << bestPath[i] << (i + 1 < bestPath.size() ? "," : "");
                    }
                    outFile << "(" << bestTime << ")\n";

                    if (altPath.empty()) {
                        outFile << "AlternativeDrivingRoute:none\n";
                    } else {
                        outFile << "AlternativeDrivingRoute:";
                        for (size_t i = 0; i < altPath.size(); ++i) {
                            outFile << altPath[i] << (i + 1 < altPath.size() ? "," : "");
                        }
                        outFile << "(" << altTime << ")\n";
                    }
                }
            } else if (mode == "restricted") {
                auto path = restrictedDrivingRoute(graph, source, destination, avoidNodes, avoidSegs, includeNode);
                outFile << "Source:" << source << "\nDestination:" << destination << "\n";
                if (path.empty()) {
                    outFile << "RestrictedDrivingRoute:none\n";
                } else {
                    outFile << "RestrictedDrivingRoute:";
                    int totalTime = 0;
                    for (size_t i = 0; i < path.size(); ++i) {
                        outFile << path[i];
                        if (i + 1 < path.size()) outFile << ",";
                        if (i + 1 < path.size()) {
                            auto u = graph.findVertex(path[i]);
                            for (auto e : u->getAdj()) {
                                if (e->getDest()->getInfo() == path[i + 1]) {
                                    totalTime += e->getDrivingWeight();
                                    break;
                                }
                            }
                        }
                    }
                    outFile << "(" << totalTime << ")\n";
                }
            } else if (mode == "env") {
                auto route = findEnvFriendlyRoute(graph, source, destination, maxWalk, avoidNodes, avoidSegs);
                outFile << "Source:" << source << "\nDestination:" << destination << "\n";
                if (route.parkingNode == -1) {
                    outFile << "DrivingRoute:none\nParkingNode:none\nWalkingRoute:none\nTotalTime:\nMessage:" << route.message << "\n";
                } else {
                    outFile << "DrivingRoute:";
                    for (size_t i = 0; i < route.drivingPath.size(); ++i) {
                        outFile << route.drivingPath[i] << (i + 1 < route.drivingPath.size() ? "," : "");
                    }
                    outFile << "(" << route.drivingTime << ")\n";

                    outFile << "ParkingNode:" << route.parkingNode << "\n";

                    outFile << "WalkingRoute:";
                    for (size_t i = 0; i < route.walkingPath.size(); ++i) {
                        outFile << route.walkingPath[i] << (i + 1 < route.walkingPath.size() ? "," : "");
                    }
                    outFile << "(" << route.walkingTime << ")\n";

                    outFile << "TotalTime:" << route.totalTime << "\n";
                }
            } else if (mode == "env_alt") {
                auto results = AlternativeRoute::findTwoSolutions(graph, source, destination, maxWalk, avoidNodes, avoidSegs);
                outFile << "Source:" << source << "\nDestination:" << destination << "\n";
                if (results.empty()) {
                    outFile << "Message:No alternative routes found.\n";
                } else {
                    for (size_t i = 0; i < results.size(); ++i) {
                        const auto& r = results[i];
                        outFile << "DrivingRoute" << (i+1) << ":";
                        for (size_t j = 0; j < r.drivingPath.size(); ++j) {
                            outFile << r.drivingPath[j] << (j + 1 < r.drivingPath.size() ? "," : "");
                        }
                        outFile << "(" << r.drivingTime << ")\n";
                        outFile << "ParkingNode" << (i+1) << ":" << r.parkingNode << "\n";
                        outFile << "WalkingRoute" << (i+1) << ":";
                        for (size_t j = 0; j < r.walkingPath.size(); ++j) {
                            outFile << r.walkingPath[j] << (j + 1 < r.walkingPath.size() ? "," : "");
                        }
                        outFile << "(" << r.walkingTime << ")\n";
                        outFile << "TotalTime" << (i+1) << ":" << r.totalTime << "\n";
                    }
                }
            }

            outFile << "\n---\n";
        }
    }
}


// ----------------------------------------------------------
// MAIN MENU FUNCTION
// ----------------------------------------------------------

void menu() {
    // Load Data
    Reader<int> reader;
    Graph<int> graph;
    reader.loadLocations(graph, "../mock_csv_data/Locations.csv");
    reader.loadDistances(graph, "../mock_csv_data/Distances.csv");

    ifstream test("batch/input.txt");
    if (test.is_open()) {
        string line;
        getline(test, line);
        if (line.starts_with("Mode:")) {
            runBatchMode(graph);
            return;
        }
    }


    while (true) {
        displayMainMenu();
        // only accept 1, 2, or 3
        int mainChoice = readIntChoice("Enter your choice: ", {1, 2, 3});

        switch(mainChoice) {
            case 1:
                handleDrivingSubMenu(graph);
                break;
            case 2:
                handleDrivingWalkingSubMenu(graph);
                break;
            case 3:
                cout << "Exiting Program...\n";
                return;
        }
    }
}
