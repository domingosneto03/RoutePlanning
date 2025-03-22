#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <utility>
#include "data_structures/Graph.h"
using namespace std;

/**
 * Reads an integer from user, ensuring it's among validChoices.
 * Loops until a valid integer choice is entered.
 */
int readIntChoice(const string &prompt, vector<int> &validChoices);

/**
 * Reads any integer from user input.
 * Loops until a valid integer is provided.
 */
int readAnyInteger(const string &prompt);

/**
 * Reads a line of comma-separated integers from the user.
 * Returns an empty vector if line is blank.
 * Loops until all tokens are valid integers.
 */
vector<int> readCommaSeparatedInts(const string &prompt);

/**
 * Reads space-separated segments in the format (id1,id2).
 * Returns an empty vector if the user inputs a blank line.
 * Loops until all tokens are valid (id1,id2).
 */
vector<pair<int,int>> readSegments(const string &prompt);

/**
 * Displays the main menu (Driving Only / Driving & Walking / Exit).
 */
void displayMainMenu();

/**
 * Displays the Driving Only Functions menu (Best Route, Restricted Route, Back).
 */
void displayDrivingMenu();

/**
 * Displays the Driving+Walking Functions menu (Best Route, Restricted Route, Back).
 */
void displayDrivingWalkingMenu();

/**
 * Handles the Driving-Only sub-menu logic:
 * - Repeatedly shows the Driving menu
 * - Reads user input for sub-choices
 * - Calls appropriate route functions (Best Route, Restricted Route) or returns to Main.
 */
void handleDrivingSubMenu(Graph<int>& graph);

/**
 * Handles the Driving+Walking sub-menu logic:
 * - Repeatedly shows the Driving+Walking menu
 * - Reads user input for sub-choices
 * - Calls appropriate route functions (Best Route, Restricted Route) or returns to Main.
 */
void handleDrivingWalkingSubMenu(Graph<int>& graph);

/**
 * The main menu function:
 * - Loads the Locations/Distances data into the Graph.
 * - Repeatedly shows the Main menu.
 * - Invokes the sub-menu handlers or exits.
 */
void menu();

#endif // MENU_H
