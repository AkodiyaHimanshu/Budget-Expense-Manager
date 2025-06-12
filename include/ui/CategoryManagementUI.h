#ifndef CATEGORY_MANAGEMENT_UI_H
#define CATEGORY_MANAGEMENT_UI_H

#include <iostream>
#include <iomanip>
#include <limits>
#include "../services/CategoryManager.h"

class CategoryManagementUI {
private:
    CategoryManager& categoryManager;

    // Helper method to display categories
    void displayCategories(const std::vector<std::string>& categories, const std::string& type) const;

    // Helper method to get a valid transaction type from user
    TransactionType getTransactionTypeChoice() const;

    // Helper method to validate category name
    bool isValidCategoryName(const std::string& name) const;

public:
    // Constructor
    CategoryManagementUI(CategoryManager& manager);

    // Display main category management menu
    void displayCategoryMenu() const;

    // Display all categories of a specific type
    void showAllCategories() const;

    // Add a new category
    void addNewCategory();

    // Remove an existing custom category
    void removeCustomCategory();
};

#endif // CATEGORY_MANAGEMENT_UI_H