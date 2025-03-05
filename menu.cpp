//
// Created by domin on 27/02/2025.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "menu.h"
#include "data_structures/Graph.h"

using namespace std;


Graph<int> graph; // Global graph instance

void displayMainMenu() {
    std::cout << "\n===== Route Planning Tool =====\n";
    std::cout << "1. Display Graph Info\n";
    std::cout << "2. Driving Only\n";
    std::cout << "3. Driving and Walking\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

void displayDrivingMenu() {
    std::cout << "\n===== Driving Only Functions =====\n";
    std::cout << "1. Find Best Route\n";
    std::cout << "2. Find Restricted Route\n";
    std::cout << "3. Back to Main Menu\n";
    std::cout << "Enter your choice: ";
}

void displayDrivingWalkingMenu() {
    std::cout << "\n===== Driving and Walking Functions =====\n";
    std::cout << "1. Find Best Route\n";
    std::cout << "2. Find Restricted Route\n";
    std::cout << "3. Back to Main Menu\n";
    std::cout << "Enter your choice: ";
}

void handleBestAndAlternativeRoute(Graph<int> &graph) {
    int source, destination;
    std::cout << "Enter Source ID: ";
    std::cin >> source;
    std::cout << "Enter Destination ID: ";
    std::cin >> destination;

    if (!graph.findVertex(source) || !graph.findVertex(destination)) {
        std::cout << "Error: One or both nodes do not exist in the graph.\n";
        return;
    }

    std::vector<int> bestPath, altPath;
    double bestTime, altTime;
    bool successBest = graph.findBestRoute(source, destination, bestPath, bestTime);
    bool successAlt = successBest && graph.findAlternativeRoute(source, destination, bestPath, altPath, altTime);

    std::ofstream output("output.txt");
    std::cout << "Source:" << source << "\n";
    std::cout << "Destination:" << destination << "\n";
    output << "Source:" << source << "\n";
    output << "Destination:" << destination << "\n";

    if (!successBest) {
        std::cout << "BestDrivingRoute:none\n";
        std::cout << "AlternativeDrivingRoute:none\n";
        output << "BestDrivingRoute:none\n";
        output << "AlternativeDrivingRoute:none\n";
    } else {
        std::cout << "BestDrivingRoute:";
        output << "BestDrivingRoute:";
        for (size_t i = 0; i < bestPath.size(); ++i) {
            std::cout << bestPath[i];
            output << bestPath[i];
            if (i != bestPath.size() - 1) {
                std::cout << ",";
                output << ",";
            }
        }
        std::cout << "(" << bestTime << ")\n";
        output << "(" << bestTime << ")\n";

        if (!successAlt) {
            std::cout << "AlternativeDrivingRoute:none\n";
            output << "AlternativeDrivingRoute:none\n";
        } else {
            std::cout << "AlternativeDrivingRoute:";
            output << "AlternativeDrivingRoute:";
            for (size_t i = 0; i < altPath.size(); ++i) {
                std::cout << altPath[i];
                output << altPath[i];
                if (i != altPath.size() - 1) {
                    std::cout << ",";
                    output << ",";
                }
            }
            std::cout << "(" << altTime << ")\n";
            output << "(" << altTime << ")\n";
        }
    }
    output.close();
    std::cout << "Press Enter to return to the menu...";
    std::cin.ignore();
    std::cin.get();
}


// Function to read input file in batch mode
void processBatchMode(Graph<int> &graph) {
    std::ifstream input("input.txt");
    if (!input.is_open()) {
        std::cerr << "Error opening input.txt" << std::endl;
        return;
    }

    std::string line, mode;
    int source, destination;
    bool alternative;
    std::vector<int> bestPath, altPath;
    double bestTime, altTime;

    // Open output file to write results
    std::ofstream output("output.txt");

    while (getline(input, line)) {
        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "Mode:") {
            ss >> mode;
        } else if (key == "Source:") {
            ss >> source;
        } else if (key == "Destination:") {
            ss >> destination;
        } else if (key == "Alternative:") {
            ss >> std::boolalpha >> alternative; // Read 'true'/'false'
        } else if (key == "Exclude:") {
            // Logic for exclusions (if any)
            // Skipping exclusion parsing for now, assuming it's implemented elsewhere.
        }

        if (!mode.empty() && source > 0 && destination > 0) {
            // Compute the best and alternative routes
            bool successBest = graph.findBestRoute(source, destination, bestPath, bestTime);
            bool successAlt = false;
            if (alternative && successBest) {
                successAlt = graph.findAlternativeRoute(source, destination, bestPath, altPath, altTime);
            }

            // Write the results to output.txt
            output << "Source: " << source << std::endl;
            output << "Destination: " << destination << std::endl;

            if (successBest) {
                output << "BestRoute: ";
                for (size_t i = 0; i < bestPath.size(); ++i) {
                    output << bestPath[i];
                    if (i != bestPath.size() - 1) output << ",";
                }
                output << " (Time: " << bestTime << ")" << std::endl;
            } else {
                output << "BestRoute: none" << std::endl;
            }

            if (successAlt) {
                output << "AlternativeRoute: ";
                for (size_t i = 0; i < altPath.size(); ++i) {
                    output << altPath[i];
                    if (i != altPath.size() - 1) output << ",";
                }
                output << " (Time: " << altTime << ")" << std::endl;
            } else {
                output << "AlternativeRoute: none" << std::endl;
            }

            output << std::endl; // Add a newline between different entries
        }
    }

    input.close();
    output.close();
}

void menu() {
    // Auto-load locations and distances at startup
    bool locationsLoaded = graph.loadLocations("../csv_data/Locations.csv");
    bool distancesLoaded = graph.loadDistances("../csv_data/Distances.csv");

    if (!locationsLoaded || !distancesLoaded) {
        std::cout << "Error: Locations or Distances file not loaded properly.\n";
        return;
    }

    std::cout << "Graph data loaded successfully.\n";

    int mainChoice, subChoice;
    do {
        displayMainMenu();
        std::cin >> mainChoice;

        switch (mainChoice) {
            case 1:
                graph.displayGraphInfo();
                break;
            case 2:
                do {
                    displayDrivingMenu();
                    std::cin >> subChoice;
                    switch (subChoice) {
                        case 1:
                            handleBestAndAlternativeRoute(graph); // Call route planning function
                            break;
                        case 2:
                            std::cout << "Finding Restricted Driving Route...\n";
                            break;
                        case 3:
                            break;
                        default:
                            std::cout << "Invalid choice, please try again.\n";
                    }
                } while (subChoice != 3);
                break;
            case 3:
                do {
                    displayDrivingWalkingMenu();
                    std::cin >> subChoice;
                    switch (subChoice) {
                        case 1:
                            std::cout << "Finding Best Driving-Walking Route...\n";
                            break;
                        case 2:
                            std::cout << "Finding Restricted Driving-Walking Route...\n";
                            break;
                        case 3:
                            break;
                        default:
                            std::cout << "Invalid choice, please try again.\n";
                    }
                } while (subChoice != 3);
                break;
            case 4:
                std::cout << "Exiting program...\n";
                break;
            case 5:
                // Call batch mode functionality
                processBatchMode(graph);
                break;
            default:
                std::cout << "Invalid choice, please try again.\n";
        }
    } while (mainChoice != 4);
}