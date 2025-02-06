// BankUtilities.h
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>

using namespace std;

class BankUtilities {
public:
    static int randomInt(int min, int max);
    static string randomLastName();
    static string randomTransactionType();
    static string formatTime(int hour, int minute);
};


// Random Gen Function
int BankUtilities::randomInt(int min, int max) {
    return min + (rand() % (max - min + 1));
}

// Function to read last0names.txt and import all names into vector for selection
string BankUtilities::randomLastName() {
    static vector<string> lastNames;
    if (lastNames.empty()) {
        ifstream file("last-names.txt");
        if (!file) {
            cerr << "Error: Couldn't open file" << endl;
            exit(1);
        }
        string name;
        while (getline(file, name)) {
            lastNames.push_back(name);
        }
        file.close();
    }
    return lastNames[randomInt(0, lastNames.size() - 1)];
}

// Function to select random transaction type
string BankUtilities::randomTransactionType() {
    int randomTransaction = randomInt(1, 4);
    switch (randomTransaction) {
    case 1: return "Opened Account";
    case 2: return "Closed Account";
    case 3: return "Deposited Money";
    case 4: return "Withdrew Money";
    default: return "Unknown";
    }
}

// Time format function
string BankUtilities::formatTime(int hour, int minute) {
    // Convert hour to 12-hour format
    string period = (hour >= 12) ? "PM" : "AM";  // Determine if it's AM or PM
    hour = (hour % 12 == 0) ? 12 : hour % 12;  // Convert hour to 12-hour format (0 becomes 12)

    // Format time as HH:MM AM/PM
    ostringstream oss;
    oss << setw(2) << setfill('0') << hour << ":"  // Two-digit hour
        << setw(2) << setfill('0') << minute << " "  // Two-digit minute
        << period;  // AM/PM period

    return oss.str();
}