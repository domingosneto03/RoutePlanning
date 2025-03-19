#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include "menu.h"
#include "reader.h"
#include "data_structures/Graph.h"

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
int readAnyInteger(const string &prompt) {
    while (true) {
        cout << prompt;
        string input;
        cin >> input;

        try {
            int val = stoi(input);
            return val;
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
vector<int> readCommaSeparatedInts(const string &prompt) {
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
vector<pair<int,int>> readSegments(const string &prompt) {
    while (true) {
        cout << prompt;
        // clear leftover newline if present
        if (cin.peek() == '\n') {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

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
    cout << "1. Find Best Route\n";
    cout << "2. Find Restricted Route\n";
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
                // TODO: implement best driving route logic
                break;
            }
            case 2: {
                cout << "\n--- Restricted Driving Route ---\n";
                int source       = readAnyInteger("Enter Source ID: ");
                int destination  = readAnyInteger("Enter Destination ID: ");
                auto avoidNodes  = readCommaSeparatedInts("Enter Nodes to Avoid (comma-separated, leave blank if none): ");
                auto avoidSegs   = readSegments("Enter Segments to Avoid (format: (id1,id2), space-separated, blank if none): ");
                int includeNode  = readAnyInteger("Enter a Node to Include in Route (or -1 if none): ");

                cout << "Finding Restricted Driving Route...\n";
                // TODO: call restricted route logic
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
        int subVal = readIntChoice("Enter your choice: ", {1, 2, 3});

        switch (subVal) {
            case 1:
                cout << "Finding Best Driving-Walking Route...\n";
                // TODO: implement best driving-walking route logic
                break;
            case 2:
                cout << "Finding Restricted Driving-Walking Route...\n";
                // TODO: implement restricted driving-walking route logic
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
