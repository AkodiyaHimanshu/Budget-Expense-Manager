#ifndef CATEGORY_MANAGER_H
#define CATEGORY_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "../models/Transaction.h"

class CategoryManager {
private:
    // Default categories for income and expense
    std::map<TransactionType, std::set<std::string>> defaultCategories;

    // Custom categories added by user
    std::map<TransactionType, std::set<std::string>> customCategories;

    // Initialize default categories
    void initializeDefaultCategories();

public:
    // Constructor
    CategoryManager();

    // Get all categories for a specific transaction type
    std::vector<std::string> getAllCategories(TransactionType type) const;

    // Get only default categories for a specific transaction type
    std::vector<std::string> getDefaultCategories(TransactionType type) const;

    // Get only custom categories for a specific transaction type
    std::vector<std::string> getCustomCategories(TransactionType type) const;

    // Add a new custom category
    bool addCategory(const std::string& category, TransactionType type);

    // Remove a custom category (default categories cannot be removed)
    bool removeCategory(const std::string& category, TransactionType type);

    // Check if a category exists
    bool categoryExists(const std::string& category, TransactionType type) const;

    // Check if a category is a default category
    bool isDefaultCategory(const std::string& category, TransactionType type) const;
};

#endif // CATEGORY_MANAGER_H