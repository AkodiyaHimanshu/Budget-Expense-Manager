#pragma once

#include <memory>
#include "../services/BudgetManager.h"
#include "../services/TransactionManager.h"

class BudgetUI {
private:
    std::shared_ptr<BudgetManager> budgetManager;
    std::shared_ptr<TransactionManager> transactionManager;

    // Helper methods
    void displayBudget(const std::shared_ptr<Budget>& budget);
    void displayBudgets(const std::vector<std::shared_ptr<Budget>>& budgets);
    void displayBudgetUsage(const std::shared_ptr<Budget>& budget);

public:
    BudgetUI(const std::shared_ptr<BudgetManager>& budgetManager,
        const std::shared_ptr<TransactionManager>& transactionManager);

    // Menu display
    void displayBudgetMenu();

    // Budget operations
    void showAllBudgets();
    void showBudgetsByCategory();
    void showBudgetsByMonth();
    void setBudget();
    void updateBudget();
    void removeBudget();
    void showBudgetUsageReport();
};