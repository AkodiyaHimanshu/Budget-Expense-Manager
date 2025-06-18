#ifndef TRANSACTION_UI_H
#define TRANSACTION_UI_H

#include <memory>
#include "../services/TransactionManager.h"

class TransactionUI {
private:
    std::shared_ptr<TransactionManager> transactionManager;

public:
    explicit TransactionUI(std::shared_ptr<TransactionManager> manager);

    // Display transactions menu
    void displayTransactionsMenu();

    // Show all transactions
    void showAllTransactions();

    // Show transactions filtered by category
    void showTransactionsByCategory();

    // Show transactions filtered by type (income/expense)
    void showTransactionsByType();

    // Show transactions filtered by date range
    void showTransactionsByDateRange();

    // Show transactions grouped by month
    void showTransactionsByMonth();

    // Show monthly summary
    void showMonthlySummary();

    // Add a new transaction
    void addNewTransaction();

    // Display a list of transactions formatted for the UI
    void displayTransactions(const std::vector<std::shared_ptr<Transaction>>& transactions);
};

#endif // TRANSACTION_UI_H