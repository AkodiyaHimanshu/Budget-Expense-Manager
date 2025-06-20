#include "../../include/services/BudgetManager.h"
#include "../../include/utils/FileUtils.h"
#include "../../include/models/Budget.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

BudgetManager::BudgetManager() {
    // Load existing budgets from file when manager is created
    loadBudgets();
}

BudgetManager::~BudgetManager() {
    // Save budgets when manager is destroyed
    saveBudgets();
}

void BudgetManager::addBudget(const std::shared_ptr<Budget>& budget) {
    // Get key for lookup
    std::string key = createBudgetKey(budget->getCategory(), budget->getYearMonth());

    // Check if a budget for this category and month already exists
    auto it = budgets.find(key);
    if (it != budgets.end()) {
        // Update the existing budget if the amount is different
        if (it->second->getLimitAmount() != budget->getLimitAmount()) {
            it->second->setLimitAmount(budget->getLimitAmount());
            saveBudgets();
        }
        return;
    }

    // Add the new budget - O(1) operation with unordered_map
    budgets[key] = budget;
    saveBudgets();
}

void BudgetManager::updateBudget(const std::string& category, const std::string& yearMonth, double newLimit) {
    std::string key = createBudgetKey(category, yearMonth);
    bool changed = false;

    // Try to find and update existing budget - O(1) lookup
    auto it = budgets.find(key);
    if (it != budgets.end()) {
        // Only update if the value is actually changing
        if (it->second->getLimitAmount() != newLimit) {
            it->second->setLimitAmount(newLimit);
            changed = true;
        }
    }
    else {
        // If budget doesn't exist, create a new one
        auto newBudget = std::make_shared<Budget>(category, yearMonth, newLimit);
        budgets[key] = newBudget;
        changed = true;
    }

    // Only save if something actually changed
    if (changed) {
        saveBudgets();
    }
}

bool BudgetManager::removeBudget(const std::string& category, const std::string& yearMonth) {
    std::string key = createBudgetKey(category, yearMonth);

    // Look up the budget - O(1) operation
    auto it = budgets.find(key);
    if (it != budgets.end()) {
        budgets.erase(it);
        saveBudgets();
        return true;
    }

    return false;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getAllBudgets() const {
    // Convert map values to vector for API compatibility
    std::vector<std::shared_ptr<Budget>> result;
    result.reserve(budgets.size()); // Preallocate for efficiency

    for (const auto& pair : budgets) {
        result.push_back(pair.second);
    }

    return result;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getBudgetsByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Budget>> result;

    // Still need to scan, but only scanning the values is more cache-friendly
    for (const auto& pair : budgets) {
        if (pair.second->getCategory() == category) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getBudgetsByYearMonth(const std::string& yearMonth) const {
    std::vector<std::shared_ptr<Budget>> result;

    // Still need to scan, but only scanning the values is more cache-friendly
    for (const auto& pair : budgets) {
        if (pair.second->getYearMonth() == yearMonth) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::shared_ptr<Budget> BudgetManager::getBudget(const std::string& category, const std::string& yearMonth) const {
    std::string key = createBudgetKey(category, yearMonth);

    // Direct lookup - O(1) operation
    auto it = budgets.find(key);
    return (it != budgets.end()) ? it->second : nullptr;
}

bool BudgetManager::hasBudget(const std::string& category, const std::string& yearMonth) const {
    std::string key = createBudgetKey(category, yearMonth);

    // Direct lookup without creating a temporary shared_ptr - O(1) operation
    return budgets.find(key) != budgets.end();
}

void BudgetManager::saveBudgets() {
    std::ofstream file(dataFilePath);

    if (!file) {
        // If the directory doesn't exist, create it
        if (!FileUtils::createDirectoryIfNotExists("data")) {
            std::cerr << "Error creating data directory for budget storage" << std::endl;
            return;
        }

        file.open(dataFilePath);
        if (!file) {
            std::cerr << "Error opening budget file for writing: " << dataFilePath << std::endl;
            return;
        }
    }

    // Write header
    file << "Category,YearMonth,LimitAmount\n";

    // Write budget data
    for (const auto& [key, budget] : budgets) {
        file << budget->getCategory() << ","
            << budget->getYearMonth() << ","
            << budget->getLimitAmount() << "\n";
    }

    file.close();
}


void BudgetManager::loadBudgets() {
    if (!FileUtils::fileExists(dataFilePath)) {
        // No file to load, start with empty budget map
        return;
    }

    std::ifstream file(dataFilePath);
    if (!file) {
        std::cerr << "Error opening budget file for reading: " << dataFilePath << std::endl;
        return;
    }

    budgets.clear();

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue; // Skip the header line
        }

        std::stringstream ss(line);
        std::string category, yearMonth, limitStr;

        // Parse CSV line
        if (std::getline(ss, category, ',') &&
            std::getline(ss, yearMonth, ',') &&
            std::getline(ss, limitStr)) {

            try {
                double limitAmount = std::stod(limitStr);
                auto budget = std::make_shared<Budget>(category, yearMonth, limitAmount);

                // Use the map with our composite key for O(1) insertion
                std::string key = createBudgetKey(category, yearMonth);
                budgets[key] = budget;
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing budget data: " << line << std::endl;
            }
        }
    }

    file.close();
}