#include "../../include/services/CategoryManager.h"
#include <algorithm>

CategoryManager::CategoryManager() {
    initializeDefaultCategories();
}

void CategoryManager::initializeDefaultCategories() {
    // Initialize default income categories
    defaultCategories[TransactionType::INCOME] = {
        "Salary",
        "Freelance",
        "Investments",
        "Gifts",
        "Refunds",
        "Other Income"
    };

    // Initialize default expense categories
    defaultCategories[TransactionType::EXPENSE] = {
        "Food & Dining",
        "Housing",
        "Transportation",
        "Utilities",
        "Healthcare",
        "Entertainment",
        "Shopping",
        "Personal Care",
        "Education",
        "Travel",
        "Debt Payments",
        "Savings",
        "Gifts & Donations",
        "Other Expenses"
    };

    // Initialize empty custom categories maps
    customCategories[TransactionType::INCOME] = {};
    customCategories[TransactionType::EXPENSE] = {};
}

std::vector<std::string> CategoryManager::getAllCategories(TransactionType type) const {
    std::vector<std::string> result;

    // Add default categories
    for (const auto& category : defaultCategories.at(type)) {
        result.push_back(category);
    }

    // Add custom categories
    for (const auto& category : customCategories.at(type)) {
        result.push_back(category);
    }

    // Sort alphabetically for better user experience
    std::sort(result.begin(), result.end());

    return result;
}

std::vector<std::string> CategoryManager::getDefaultCategories(TransactionType type) const {
    std::vector<std::string> result;
    for (const auto& category : defaultCategories.at(type)) {
        result.push_back(category);
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> CategoryManager::getCustomCategories(TransactionType type) const {
    std::vector<std::string> result;
    for (const auto& category : customCategories.at(type)) {
        result.push_back(category);
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool CategoryManager::addCategory(const std::string& category, TransactionType type) {
    // Check if category already exists (either as default or custom)
    if (categoryExists(category, type)) {
        return false;
    }

    // Add to custom categories
    customCategories[type].insert(category);
    return true;
}

bool CategoryManager::removeCategory(const std::string& category, TransactionType type) {
    // Cannot remove default categories
    if (isDefaultCategory(category, type)) {
        return false;
    }

    // Check if the custom category exists
    if (customCategories[type].find(category) == customCategories[type].end()) {
        return false;
    }

    // Remove the category
    customCategories[type].erase(category);
    return true;
}

bool CategoryManager::categoryExists(const std::string& category, TransactionType type) const {
    // Check in default categories
    if (defaultCategories.at(type).find(category) != defaultCategories.at(type).end()) {
        return true;
    }

    // Check in custom categories
    if (customCategories.at(type).find(category) != customCategories.at(type).end()) {
        return true;
    }

    return false;
}

bool CategoryManager::isDefaultCategory(const std::string& category, TransactionType type) const {
    return defaultCategories.at(type).find(category) != defaultCategories.at(type).end();
}