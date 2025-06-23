#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <unordered_map>
#include "../models/Budget.h"
#include "../models/UserProfile.h"

class BudgetManager {
private:
    // Use unordered_map for O(1) lookups instead of O(n) vector scans
    std::unordered_map<std::string, std::shared_ptr<Budget>> budgets;
    std::string filePath; // Will be set based on the user profile
    std::shared_ptr<UserProfile> userProfile; // Add user profile reference

    const std::string dataFilePath = "data/budgets.csv";

    // Helper method to create a unique key for the map
    std::string createBudgetKey(const std::string& category, const std::string& yearMonth) const {
        return category + "|" + yearMonth;
    }

public:
    BudgetManager();
    BudgetManager(std::shared_ptr<UserProfile> profile);

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

    // Set/change the user profile
    void setUserProfile(std::shared_ptr<UserProfile> profile);
};