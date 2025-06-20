#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "../models/Budget.h"

class BudgetManager {
private:
    std::vector<std::shared_ptr<Budget>> budgets;
    const std::string dataFilePath = "data/budgets.csv";

public:
    BudgetManager();
    ~BudgetManager();

    // Core budget operations
    void addBudget(const std::shared_ptr<Budget>& budget);
    void updateBudget(const std::string& category, const std::string& yearMonth, double newLimit);
    bool removeBudget(const std::string& category, const std::string& yearMonth);

    // Retrieval methods
    std::vector<std::shared_ptr<Budget>> getAllBudgets() const;
    std::vector<std::shared_ptr<Budget>> getBudgetsByCategory(const std::string& category) const;
    std::vector<std::shared_ptr<Budget>> getBudgetsByYearMonth(const std::string& yearMonth) const;
    std::shared_ptr<Budget> getBudget(const std::string& category, const std::string& yearMonth) const;

    // Check if a budget exists
    bool hasBudget(const std::string& category, const std::string& yearMonth) const;

    // Data persistence
    void saveBudgets();
    void loadBudgets();
};