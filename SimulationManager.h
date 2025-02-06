// SimulationManager.h
#pragma once
#include <string>
#include <vector>
#include <queue>
#include "SimulationManager.h"
#include "Customer.h"
#include "BankUtilities.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <windows.h>
#include <conio.h>

using namespace std;

class Customer;

class SimulationManager {
public:
    SimulationManager();
    void startSimulation();

private:
    int getCustomerInput();
    void runSimulation();
    void manageTellers(int& tellersAvailable, queue<Customer>& customerQueue);
    void printTellerStatus(int tellersAvailable, const vector<int>& tellerTimers, const vector<string>& tellerCurrentCustomer);
    void printCustomerQueue(const queue<Customer>& customerQueue);
    void addNewCustomer(int currentHour, int currentMinute);
    void updateTellerTimers(int tellersAvailable, vector<int>& tellerTimers, vector<string>& tellerCurrentCustomer, int currentHour, int currentMinute, vector<int>& tellerStartTime);
    void printStatistics(int totalQueueSize, int timeSteps);
    void printCustomerReport(const string& title, const vector<Customer>& customers);


    int numCustomers;
    vector<Customer> customers;
    queue<Customer> customerQueue;
};

//********************
// Guts of program
//********************

SimulationManager::SimulationManager() : numCustomers(0) {}

// Simulation start and loop cycle
void SimulationManager::startSimulation() {
    char exitInput;
    do {
        system("cls");
        numCustomers = getCustomerInput();
        system("cls");
        runSimulation();
        cout << "Would you like to do another simulation (Y/N): ";
        cin >> exitInput;
    } while (toupper(exitInput) != 'N');
}

//
int SimulationManager::getCustomerInput() {
    return BankUtilities::randomInt(1, 5);
}

void SimulationManager::runSimulation() {
    customers.clear();
    int currentHour = 10;
    int currentMinute = 0;
    int closingHour = 13;

    int tellersAvailable = 1;
    vector<int> tellerTimers(3, 0);
    vector<string> tellerCurrentCustomer(3, "");
    vector<int> tellerStartTime(3, 0);

    // Add customers to the queue initially
    for (int i = 0; i < numCustomers; i++) {
        customers.emplace_back(i, currentHour, currentMinute);  // Pass current time to the constructor
        customerQueue.push(customers.back());
    }

    cout << "Simulation started. Press SPACE to advance time by 1 minute.\n\n";

    int totalQueueSize = 0;  // To accumulate total queue size
    int timeSteps = 0;       // To track the total number of time steps

    while (currentHour < closingHour || !customerQueue.empty() || any_of(tellerTimers.begin(), tellerTimers.end(), [](int t) { return t > 0; })) {
        system("cls");
        cout << "Current Time: " << BankUtilities::formatTime(currentHour, currentMinute) << "\n\n";

        // Increment waiting time for all customers still in the queue
        queue<Customer> tempQueue = customerQueue;
        while (!tempQueue.empty()) {
            tempQueue.front().waitingTime++;  // Increment the waiting time for each customer in the queue
            tempQueue.pop();
        }

        // Track queue size at each minute
        totalQueueSize += customerQueue.size();  // Add current queue size to total
        timeSteps++;  // Increment the number of time steps

        // Manage teller availability based on the queue size
        manageTellers(tellersAvailable, customerQueue);

        // Process the customers at the tellers
        for (int i = 0; i < tellersAvailable; i++) {
            if (tellerTimers[i] == 0 && !customerQueue.empty()) {
                Customer& currentCustomer = customerQueue.front();
                customerQueue.pop();
                tellerTimers[i] = currentCustomer.serviceTime;
                tellerCurrentCustomer[i] = currentCustomer.lastName;
                tellerStartTime[i] = currentHour * 60 + currentMinute;
                currentCustomer.startTime = tellerStartTime[i];
            }
        }

        // Print the teller status and customer queue
        printTellerStatus(tellersAvailable, tellerTimers, tellerCurrentCustomer);
        printCustomerQueue(customerQueue);

        cout << "\nPress SPACE to continue...";
        while (_getch() != ' ') {}

        // Add new customers to the queue with a 5% chance
        addNewCustomer(currentHour, currentMinute);

        // Update teller timers and customer waiting times
        updateTellerTimers(tellersAvailable, tellerTimers, tellerCurrentCustomer, currentHour, currentMinute, tellerStartTime);

        // Advance simulation time by 1 minute
        currentMinute++;
        if (currentMinute == 60) {
            currentMinute = 0;
            currentHour++;

            if (currentHour >= closingHour) {
                break;
            }
        }
    }

    system("cls");
    cout << "Simulation ended at 1:00 PM.\nAll remaining customers will be served the next day.\n";
    printStatistics(totalQueueSize, timeSteps);  // Print statistics after simulation ends
}

void SimulationManager::manageTellers(int& tellersAvailable, queue<Customer>& customerQueue) {
    if (customerQueue.size() > 4 && tellersAvailable < 2) {
        tellersAvailable = 2;
    }
    if (customerQueue.size() > 8 && tellersAvailable < 3) {
        tellersAvailable = 3;
    }
    if (customerQueue.size() <= 4 && tellersAvailable > 1) {
        tellersAvailable = 1;
    }
    else if (customerQueue.size() <= 8 && tellersAvailable > 2) {
        tellersAvailable = 2;
    }
}

// Function to determine if teller is busy with customer or open for another
void SimulationManager::printTellerStatus(int tellersAvailable, const vector<int>& tellerTimers, const vector<string>& tellerCurrentCustomer) {
    cout << "Teller Status:\n";
    for (int i = 0; i < tellersAvailable; i++) {
        cout << "Teller " << i + 1 << ": ";
        if (tellerTimers[i] > 0) {
            cout << "Serving " << tellerCurrentCustomer[i] << " (" << tellerTimers[i] << " min left)";
        }
        else {
            cout << "Available";
        }
        cout << endl;
    }
}

// Print customer queue to console
void SimulationManager::printCustomerQueue(const queue<Customer>& customerQueue) {
    cout << "\nCustomers in Queue:\n";
    if (customerQueue.empty()) {
        cout << "No customers waiting.\n";
    }
    else {
        queue<Customer> tempQueue = customerQueue;
        while (!tempQueue.empty()) {
            tempQueue.front().waitingTime++;
            cout << "- " << tempQueue.front().lastName << " (" << tempQueue.front().transactionType << ")\n";
            tempQueue.pop();
        }
    }
}

// % chance to add customer for x amount of customers to queue
void SimulationManager::addNewCustomer(int currentHour, int currentMinute) {
    if (rand() % 100 < 20 && currentHour < 13) {
        Customer newCustomer(customers.size(), currentHour, currentMinute);

        customers.push_back(newCustomer);
        customerQueue.push(newCustomer); 
        cout << "\nNew customer arrived: " << newCustomer.lastName << " (" << newCustomer.transactionType << ")\n";
        Sleep(500);
    }
}

// Function that updates customer service time every minute
void SimulationManager::updateTellerTimers(int tellersAvailable, vector<int>& tellerTimers, vector<string>& tellerCurrentCustomer, int currentHour, int currentMinute, vector<int>& tellerStartTime) {
    for (int i = 0; i < tellersAvailable; i++) {
        if (tellerTimers[i] > 0) {
            tellerTimers[i]--;
            if (tellerTimers[i] == 0) {
                tellerCurrentCustomer[i] = "";
            }
        }
    }
}

// Prints statistics by different order processes, and prints statistics of day
void SimulationManager::printStatistics(int totalQueueSize, int timeSteps) {
    if (customers.empty()) {
        cout << "\nNo customers to analyze.\n";
        return;
    }
    // Debug: Print out waiting times and arrival times
    for (const auto& customer : customers) {
        cout << "Customer " << customer.lastName
            << " arrived at " << customer.arrivalTime
            << " and has waited for " << customer.waitingTime << " minutes.\n";
    }
    // === Sort and Display Reports ===

    // Sort by last name (alphabetical order)
    sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
        return a.lastName < b.lastName;
        });
    printCustomerReport("Sorted by Last Name", customers);

    // Sort by transaction amount (smallest to largest)
    sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
        return a.transactionAmount < b.transactionAmount;
        });
    printCustomerReport("Sorted by Transaction Amount", customers);

    // Sort by arrival time (earliest to latest)
    sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
        return a.arrivalTime < b.arrivalTime;
        });
    printCustomerReport("Sorted by Arrival Time", customers);


    // Calculate the average waiting time per customer
    double totalWaitingTime = accumulate(customers.begin(), customers.end(), 0.0, [](double sum, const Customer& c) {
        return sum + c.waitingTime;  // Sum all customers' waiting times
        });
    double averageWaitingTime = (customers.size() > 0) ? totalWaitingTime / customers.size() : 0;

    // Calculate average queue size over time (totalQueueSize / timeSteps)
    double averageQueueSize = (timeSteps > 0) ? static_cast<double>(totalQueueSize) / timeSteps : 0;

    // === Print the Results ===
    cout << "\n";
    cout << "--------------------------------------\n";
    cout << "            Simulation Statistics     \n";
    cout << "--------------------------------------\n";

    cout << fixed << setprecision(2);  // Ensure two decimal places for float values
    cout << setw(30) << left << "Average waiting time:" << averageWaitingTime << " min\n";
    cout << setw(30) << left << "Average queue size:" << averageQueueSize << "\n";
    cout << "--------------------------------------\n";  // Footer
}


//
void SimulationManager::printCustomerReport(const string& title, const vector<Customer>& customers) {
    cout << "\n"; // Add extra newline for spacing
    cout << "======================================\n"; // Report Header
    cout << "Customer Report (" << title << ")\n";
    cout << "======================================\n";

    cout << setw(15) << left << "Last Name"
        << setw(20) << "Transaction"
        << setw(10) << "Amount"
        << setw(10) << "Arrival" << endl;
    cout << setfill('-') << setw(50) << "-" << setfill(' ') << endl;

    // Print customer details
    for (const auto& customer : customers) {
        // Convert arrival time to hours and minutes (using formatTime)
        int arrivalHour = customer.arrivalTime / 60;  // Calculate hour
        int arrivalMinute = customer.arrivalTime % 60;  // Calculate minute
        cout << setw(15) << left << customer.lastName
            << setw(20) << customer.transactionType
            << "$" << fixed << setw(10) << customer.transactionAmount
            << setw(10) << BankUtilities::formatTime(arrivalHour, arrivalMinute) << endl;  // Format time
    }
    cout << "======================================\n"; // Report Footer
}
