#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include "menu.h"
#include "reader.h"
//#include "cmake-build-debug/batch/batch.h"
#include "RestrictedRoute.h"
#include "BestRoute.h"
#include "data_structures/Graph.h"
#include "EnvFriendlyRoute.h"

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
            if (isValidNode(graph, val) || (val==-1 && prompt.starts_with("Enter a Node to Include"))) {
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
                int source = readAnyInteger("Enter Source ID: ", graph);
                int destination;
                while (true) {
                    destination = readAnyInteger("Enter Destination ID: ", graph);
                    if (destination != source) break;
                    cout << "Destination must be different from source. Try again.\n";
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
                    cout << "DrivingRoute:\n";
                    cout << "ParkingNode:\n";
                    cout << "WalkingRoute:\n";
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

            case 2:
                cout << "Finding Aproximate Solution...\n";
                // TODO: implement aproximate solution logic
                break;
            case 3:
                cout << "Returning to Main Menu...\n";
                return; // exit sub-menu
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
    reader.loadLocations(graph, "../csv_data/Locations.csv");
    reader.loadDistances(graph, "../csv_data/Distances.csv");

    // TODO: implement batch mode at the end of the project
    /*
    // Check for batch mode input
    ifstream test("batch/input.txt");
    if (test.is_open()) {
        string line;
        getline(test, line);
        if (line.starts_with("Mode:driving")) {
            runBatchMode_T2_2(graph);
            return; // skip interactive menu
        }
    }
     */

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
