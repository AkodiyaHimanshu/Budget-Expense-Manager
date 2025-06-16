#ifndef TRANSACTION_INPUT_H
#define TRANSACTION_INPUT_H

#include <string>
#include <memory>
#include <iostream>
#include <limits>
#include <regex>
#include <algorithm>
#include <sstream>
#include <map>
#include <iomanip>
#include <stdexcept>  // For std::invalid_argument and std::out_of_range exceptions
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
    std::string getValidYearMonth(); // Get a valid YYYY-MM format string

    // Helper method to display transactions in a tabular format
    void displayTransactionsTabular(const std::vector<std::shared_ptr<Transaction>>& transactions, const std::string& title) const;

public:
    TransactionInput(TransactionManager& manager, CategoryManager& catManager);

    // Add methods for transaction entry
    void addIncomeTransaction();
    void addExpenseTransaction();

    // Display methods
    void displayAllTransactions() const;
    void displayTransactionsByCategory() const;
    void displayTransactionsByType() const;
    void displaySummary() const; // Show basic summary statistics
    void displayMonthlyTransactions(); // Display transactions for a specific month
    void displayMonthlySummary(); // Display summary for a specific month
    void displayAllMonthlySummaries() const; // Display summaries for all months
};

#endif // TRANSACTION_INPUT_H