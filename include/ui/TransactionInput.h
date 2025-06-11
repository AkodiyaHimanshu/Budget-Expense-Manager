#ifndef TRANSACTION_INPUT_H
#define TRANSACTION_INPUT_H

#include <string>
#include <memory>
#include <iostream>
#include <limits>
#include "../models/Transaction.h"
#include "../services/TransactionManager.h"

class TransactionInput {
private:
    TransactionManager& transactionManager;

    // Helper methods for input validation
    double getValidAmount();
    std::string getValidCategory();
    time_t getValidDate(); // For simplicity, we'll use current date in implementation

public:
    // Constructor
    TransactionInput(TransactionManager& manager);

    // Method to handle income transaction input
    void addIncomeTransaction();

    // Method to handle expense transaction input
    void addExpenseTransaction();

    // Method to display all transactions
    void displayAllTransactions() const;

    // Method to display summary
    void displaySummary() const;
};

#endif // TRANSACTION_INPUT_H