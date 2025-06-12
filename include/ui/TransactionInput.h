#ifndef TRANSACTION_INPUT_H
#define TRANSACTION_INPUT_H

#include <string>
#include <memory>
#include <iostream>
#include <limits>
#include "../models/Transaction.h"
#include "../services/TransactionManager.h"
#include "../services/CategoryManager.h"

class TransactionInput {
private:
    TransactionManager& transactionManager;
    CategoryManager& categoryManager;

    // Helper methods for input validation
    double getValidAmount();
    std::string getValidCategory(TransactionType type);
    time_t getValidDate(); // For simplicity, we'll use current date in implementation

public:
    // Constructor
    TransactionInput(TransactionManager& tManager, CategoryManager& cManager);

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