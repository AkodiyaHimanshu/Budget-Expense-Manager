#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <tuple>
#include <ctime>
#include "../models/Transaction.h"
#include "../services/BudgetManager.h"
#include "../models/UserProfile.h" // Add this include


class TransactionManager {
private:
    std::vector<std::shared_ptr<Transaction>> transactions;
    const std::string dataFilePath = "data/transactions.csv";
    std::string filePath; // Will be set based on the user profile
    std::shared_ptr<UserProfile> userProfile; // Add user profile reference

public:
    TransactionManager();
    ~TransactionManager();

    // Core transaction operations
    void addTransaction(const std::shared_ptr<Transaction>& transaction);
    std::vector<std::shared_ptr<Transaction>> getAllTransactions() const;

    // Filtering methods
    std::vector<std::shared_ptr<Transaction>> getTransactionsByCategory(const std::string& category) const;
    std::vector<std::shared_ptr<Transaction>> getTransactionsByType(TransactionType type) const;
    std::vector<std::shared_ptr<Transaction>> getTransactionsByDateRange(time_t startDate, time_t endDate) const;
    std::vector<std::shared_ptr<Transaction>> getTransactionsByAmountRange(double minAmount, double maxAmount) const;

    bool checkBudgetExceeded(const std::shared_ptr<Transaction>& transaction, const std::shared_ptr<BudgetManager>& budgetManager, std::string& warningMessage) const;

    // Grouping and analysis
    std::map<std::string, std::vector<std::shared_ptr<Transaction>>> getTransactionsByMonth() const;
    std::map<std::string, std::tuple<double, double, double>> calculateMonthlySummary() const;

    // Data persistence
    void saveTransactions();
    void loadTransactions();

    // Financial calculations
    double getTotalIncome() const;
    double getTotalExpenses() const;
    double getNetAmount() const;


    // Updated constructor to accept a user profile
    TransactionManager(std::shared_ptr<UserProfile> profile);

    // Set/change the user profile
    void setUserProfile(std::shared_ptr<UserProfile> profile);

};

#endif // TRANSACTION_MANAGER_H