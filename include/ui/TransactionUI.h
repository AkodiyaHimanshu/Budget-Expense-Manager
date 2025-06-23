#ifndef TRANSACTION_UI_H
#define TRANSACTION_UI_H

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
#include "../services/TransactionManager.h"
#include "../models/Transaction.h"
#include "../utils/DateUtils.h"

class TransactionUI {
private:
    std::shared_ptr<TransactionManager> transactionManager;
    std::shared_ptr<BudgetManager> budgetManager;

    // Helper methods for displaying transaction data
    void displayTransactionHeader() const;
    void displayTransactions(const std::vector<std::shared_ptr<Transaction>>& transactions) const;
    void displayMonthlySummaryHeader() const;

    // Input validation helpers - make them const
    bool validateDoubleInput(double& value, const std::string& prompt) const;
    bool validateDateInput(std::string& dateStr, const std::string& prompt) const;
    bool validateCategoryInput(std::string& category, const std::string& prompt) const;

    // Transaction creation helpers - these might need to remain non-const
    std::shared_ptr<Transaction> createTransaction(TransactionType type);

public:
    // Constructor
    TransactionUI(std::shared_ptr<TransactionManager> tm, std::shared_ptr<BudgetManager> bm);

    // Menu display methods
    void displayTransactionsMenu() const;

    // Financial summary methods
    void displayFinancialSummary() const;

    // Transaction display methods
    void showAllTransactions() const;
    void showTransactionsByCategory() const;
    void showTransactionsByType() const;
    void showTransactionsByDateRange() const;
    void showTransactionsByMonth() const;
    void showTransactionsByAmountRange();
    void showMonthlySummary() const;

    // Transaction management methods
    void addNewTransaction();
    void addIncomeTransaction();
    void addExpenseTransaction();
};

#endif // TRANSACTION_UI_H