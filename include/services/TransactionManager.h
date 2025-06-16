#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "../models/Transaction.h"

// Structure to hold monthly financial summary
struct MonthlySummary {
    double totalIncome;
    double totalExpenses;
    double netAmount;

    // Method to calculate and update net amount
    void updateNetAmount() {
        netAmount = totalIncome - totalExpenses;
    }
};

class TransactionManager {
private:
    std::vector<std::shared_ptr<Transaction>> transactions;

    // Cache for monthly summaries to avoid redundant calculations
    mutable std::map<std::string, MonthlySummary> monthlySummaryCache;

    // Cache for monthly transactions
    mutable std::map<std::string, std::vector<std::shared_ptr<Transaction>>> monthlyTransactionsCache;

    // Flag to track if the cache needs to be invalidated
    mutable bool cacheValid = true;

    // Utility method to calculate net amount (income - expenses)
    static double calculateNetAmount(double income, double expenses) {
        return income - expenses;
    }

    // Utility method to clear all caches
    void clearCaches() const {
        monthlySummaryCache.clear();
        monthlyTransactionsCache.clear();
        cacheValid = true;
    }

public:
    // Add a transaction to the in-memory list
    void addTransaction(std::shared_ptr<Transaction> transaction);

    // Get all transactions
    const std::vector<std::shared_ptr<Transaction>>& getAllTransactions() const;

    // Get transactions by type
    std::vector<std::shared_ptr<Transaction>> getTransactionsByType(TransactionType type) const;

    // Get transactions by category
    std::vector<std::shared_ptr<Transaction>> getTransactionsByCategory(const std::string& category) const;

    // Calculate total for a specific transaction type
    double calculateTotal(TransactionType type) const;

    // Calculate net amount (income - expenses) across all transactions
    double calculateNetTotal() const;

    // Get transactions for a specific month (format: YYYY-MM)
    std::vector<std::shared_ptr<Transaction>> getTransactionsByMonth(const std::string& yearMonth) const;

    // Calculate monthly summary for a specific month (format: YYYY-MM)
    MonthlySummary calculateMonthlySummary(const std::string& yearMonth) const;

    // Group transactions by month and return monthly summaries
    std::map<std::string, MonthlySummary> getMonthlyTransactionSummaries() const;
};

#endif // TRANSACTION_MANAGER_H