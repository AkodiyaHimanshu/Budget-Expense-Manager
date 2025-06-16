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
};

class TransactionManager {
private:
    std::vector<std::shared_ptr<Transaction>> transactions;

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

    // Get transactions for a specific month (format: YYYY-MM)
    std::vector<std::shared_ptr<Transaction>> getTransactionsByMonth(const std::string& yearMonth) const;

    // Calculate monthly summary for a specific month (format: YYYY-MM)
    MonthlySummary calculateMonthlySummary(const std::string& yearMonth) const;

    // Group transactions by month and return monthly summaries
    std::map<std::string, MonthlySummary> getMonthlyTransactionSummaries() const;
};

#endif // TRANSACTION_MANAGER_H