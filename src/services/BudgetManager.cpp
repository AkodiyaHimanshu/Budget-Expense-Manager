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
    // Check if a budget for this category and month already exists
    if (hasBudget(budget->getCategory(), budget->getYearMonth())) {
        // Update the existing budget instead of adding a new one
        updateBudget(budget->getCategory(), budget->getYearMonth(), budget->getLimitAmount());
        return;
    }

    // Add the new budget
    budgets.push_back(budget);
    saveBudgets();
}

void BudgetManager::updateBudget(const std::string& category, const std::string& yearMonth, double newLimit) {
    bool changed = false;
    bool budgetFound = false;

    // Try to find and update existing budget
    for (auto& budget : budgets) {
        if (budget->getCategory() == category && budget->getYearMonth() == yearMonth) {
            // Only update if the value is actually changing
            if (budget->getLimitAmount() != newLimit) {
                budget->setLimitAmount(newLimit);
                changed = true;
            }
            budgetFound = true;
            break;  // Exit loop once we find the matching budget
        }
    }

    // If budget doesn't exist, create a new one
    if (!budgetFound) {
        auto newBudget = std::make_shared<Budget>(category, yearMonth, newLimit);
        budgets.push_back(newBudget);
        changed = true;
    }

    // Only save if something actually changed
    if (changed) {
        saveBudgets();
    }
}

bool BudgetManager::removeBudget(const std::string& category, const std::string& yearMonth) {
    auto it = std::remove_if(budgets.begin(), budgets.end(),
        [&](const std::shared_ptr<Budget>& budget) {
            return budget->getCategory() == category && budget->getYearMonth() == yearMonth;
        });

    if (it != budgets.end()) {
        budgets.erase(it, budgets.end());
        saveBudgets();
        return true;
    }

    return false;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getAllBudgets() const {
    return budgets;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getBudgetsByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Budget>> result;

    for (const auto& budget : budgets) {
        if (budget->getCategory() == category) {
            result.push_back(budget);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Budget>> BudgetManager::getBudgetsByYearMonth(const std::string& yearMonth) const {
    std::vector<std::shared_ptr<Budget>> result;

    for (const auto& budget : budgets) {
        if (budget->getYearMonth() == yearMonth) {
            result.push_back(budget);
        }
    }

    return result;
}

std::shared_ptr<Budget> BudgetManager::getBudget(const std::string& category, const std::string& yearMonth) const {
    for (const auto& budget : budgets) {
        if (budget->getCategory() == category && budget->getYearMonth() == yearMonth) {
            return budget;
        }
    }

    return nullptr; // Return nullptr if no matching budget is found
}

bool BudgetManager::hasBudget(const std::string& category, const std::string& yearMonth) const {
    return getBudget(category, yearMonth) != nullptr;
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
    for (const auto& budget : budgets) {
        file << budget->getCategory() << ","
            << budget->getYearMonth() << ","
            << budget->getLimitAmount() << "\n";
    }

    file.close();
}

void BudgetManager::loadBudgets() {
    if (!FileUtils::fileExists(dataFilePath)) {
        // No file to load, start with empty budget list
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
                budgets.push_back(budget);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing budget data: " << line << std::endl;
            }
        }
    }

    file.close();
}