//
// Created by domin on 27/02/2025.
//

#include <iostream>
#include "menu.h"
using namespace std;

void displayMainMenu() {
    cout << "\n===== Route Planning Tool =====\n";
    cout << "1. Driving Only\n";
    cout << "2. Driving and Walking\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

void displayDrivingMenu() {
    cout << "\n===== Driving Only Functions =====\n";
    cout << "1. Find Best Route\n";
    cout << "2. Find Restricted Route \n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

void displayDrivingWalkingMenu() {
    cout << "\n===== Driving and Walking Functions =====\n";
    cout << "1. Find Best Route\n";
    cout << "2. Find Restricted Route \n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

void menu() {
    string mainChoice, subChoice;
    do {
        displayMainMenu();
        cin >> mainChoice;

        if (mainChoice == "1") {
            do {
                displayDrivingMenu();
                cin >> subChoice;
                if (subChoice == "1") {
                    cout << "Finding Best Driving Route...\n";
                } else if (subChoice == "2") {
                    cout << "Finding Restricted Driving Route\n";
                } else if (subChoice == "3") {
                    break;
                } else {
                    cout << "Invalid choice, please try again.\n";
                }
            } while (subChoice != "3");
        } else if (mainChoice == "2") {
            do {
                displayDrivingWalkingMenu();
                cin >> subChoice;
                if (subChoice == "1") {
                    cout << "Finding Best Driving-Walking Route...\n";
                } else if (subChoice == "2") {
                    cout << "Finding Restricted Driving-Walking Route\n";
                } else if (subChoice == "3") {
                    break;
                } else {
                    cout << "Invalid choice, please try again.\n";
                }
            } while (subChoice != "3");
        } else if (mainChoice == "3") {
            cout << "Exiting Program...\n";
            break;
        } else {
            cout << "Invalid choice, please try again.\n";
        }
    } while (mainChoice != "3");
}
