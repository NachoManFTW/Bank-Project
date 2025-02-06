// Customer.h
#pragma once
#include "BankUtilities.h"
#include <string>

using namespace std;

class Customer {
public:
    string lastName;
    string transactionType;
    int transactionAmount;
    int arrivalTime;
    int serviceTime;
    int waitingTime = 0;
    int startTime;

    Customer(int currentCustomerIndex, int currentHour, int currentMinute);
};


// Customer constructor function
Customer::Customer(int currentCustomerIndex, int currentHour, int currentMinute) : waitingTime(0), startTime(0) {
    lastName = BankUtilities::randomLastName();
    transactionType = BankUtilities::randomTransactionType();
    transactionAmount = BankUtilities::randomInt(1, 1000);
    serviceTime = BankUtilities::randomInt(5, 15);
    arrivalTime = currentHour * 60 + currentMinute;
}