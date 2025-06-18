#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include "../models/Transaction.h"
#include "../utils/DateUtils.h"

class TransactionManager {
private:
    std::vector<std::shared_ptr<Transaction>> transactions;

    // Helper method to save transactions to file
    bool saveTransactionsToFile(const std::string& filename) const;

    // Helper method to load transactions from file
    bool loadTransactionsFromFile(const std::string& filename);

public:
    TransactionManager();
    ~TransactionManager();

    // Add a new transaction
    void addTransaction(const std::shared_ptr<Transaction>& transaction);

    // Get all transactions
    std::vector<std::shared_ptr<Transaction>> getAllTransactions() const;

    // Filter transactions by category
    std::vector<std::shared_ptr<Transaction>> getTransactionsByCategory(const std::string& category) const;

    // Filter transactions by type
    std::vector<std::shared_ptr<Transaction>> getTransactionsByType(TransactionType type) const;

    // Filter transactions within a specific date range (inclusive)
    std::vector<std::shared_ptr<Transaction>> getTransactionsByDateRange(time_t startDate, time_t endDate) const;

    // Get monthly breakdown of transactions, returns a map of month key (YYYY-MM) to transaction list
    std::map<std::string, std::vector<std::shared_ptr<Transaction>>> getTransactionsByMonth() const;

    // Calculate monthly summary (total income, expenses, net)
    std::map<std::string, std::tuple<double, double, double>> calculateMonthlySummary() const;

    // Save all transactions on destruction
    void saveTransactions();

    // Explicitly load transactions from storage
    void loadTransactions();

    // Get total income
    double getTotalIncome() const;

    // Get total expenses
    double getTotalExpenses() const;

    // Get net amount (income - expenses)
    double getNetAmount() const;
};

#endif // TRANSACTION_MANAGER_H