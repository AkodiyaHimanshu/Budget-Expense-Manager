#include "../../include/ui/CategoryManagementUI.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <iomanip>
#include <limits>

namespace {
    void clearInputStream() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool readIntInRange(int& out, int min, int max, const char* prompt = nullptr) {
        while (true) {
            if (prompt) std::cout << prompt;

            if (std::cin >> out && out >= min && out <= max) {
                clearInputStream();
                return true;
            }

            clearInputStream();
            std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        }
    }
}

CategoryManagementUI::CategoryManagementUI(CategoryManager& manager) : categoryManager(manager) {}

void CategoryManagementUI::displayCategoryMenu() const {
    std::cout << "\n=== Category Management ===\n";
    std::cout << "1. View All Categories\n";
    std::cout << "2. Add New Category\n";
    std::cout << "3. Remove Custom Category\n";
    std::cout << "0. Return to Main Menu\n";
    std::cout << "Choice: ";
}

void CategoryManagementUI::displayCategories(const std::vector<std::string>& categories, const std::string& type) const {
    const int nameWidth = 30;
    const int typeWidth = 10;

    std::cout << "\n--- " << type << " Categories ---\n\n";

    std::cout << std::left
        << std::setw(5) << "No." << " | "
        << std::setw(nameWidth) << "Category Name" << " | "
        << std::setw(typeWidth) << "Type"
        << std::endl;

    std::string separator(5 + nameWidth + typeWidth + 6, '-');
    std::cout << separator << std::endl;

    for (size_t i = 0; i < categories.size(); ++i) {
        std::string categoryType;
        if (categoryManager.isDefaultCategory(categories[i],
            type == "Income" ? TransactionType::INCOME : TransactionType::EXPENSE)) {
            categoryType = "Default";
        }
        else {
            categoryType = "Custom";
        }

        std::cout << std::left
            << std::setw(5) << (i + 1) << " | "
            << std::setw(nameWidth) << categories[i] << " | "
            << std::setw(typeWidth) << categoryType
            << std::endl;
    }

    std::cout << separator << std::endl;
    std::cout << "\nTotal Categories: " << categories.size() << std::endl << std::endl;
}

void CategoryManagementUI::showAllCategories() const {
    TransactionType type = getTransactionTypeChoice();

    std::vector<std::string> categories = categoryManager.getAllCategories(type);

    displayCategories(categories, type == TransactionType::INCOME ? "Income" : "Expense");
}

TransactionType CategoryManagementUI::getTransactionTypeChoice() const {
    int choice;
    readIntInRange(choice, 1, 2, "\nSelect transaction type:\n1. Income\n2. Expense\nChoice: ");
    return choice == 1 ? TransactionType::INCOME : TransactionType::EXPENSE;
}

bool CategoryManagementUI::isValidCategoryName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }

    if (name.length() < 3 || name.length() > 25) {
        return false;
    }

    static const std::regex pattern("^[a-zA-Z0-9 &\\-_\\.\\(\\)]+$");
    return std::regex_match(name, pattern);
}

void CategoryManagementUI::addNewCategory() {
    TransactionType type = getTransactionTypeChoice();
    std::string typeName = (type == TransactionType::INCOME) ? "Income" : "Expense";
    std::string categoryName;

    std::cout << "\nEnter new " << typeName << " category name (3-25 chars, letters, numbers, spaces, &-_.()): ";
    std::getline(std::cin, categoryName);

    if (!isValidCategoryName(categoryName)) {
        std::cout << "Invalid category name. Category names must be 3-25 characters and contain only "
            << "letters, numbers, spaces, and some special characters (&-_.()).\n";
        return;
    }

    if (categoryManager.categoryExists(categoryName, type)) {
        std::cout << "Category '" << categoryName << "' already exists for " << typeName << " transactions.\n";
        return;
    }

    if (categoryManager.addCategory(categoryName, type)) {
        std::cout << "Successfully added '" << categoryName << "' to " << typeName << " categories.\n";
    }
    else {
        std::cout << "Failed to add category.\n";
    }
}

void CategoryManagementUI::removeCustomCategory() {
    TransactionType type = getTransactionTypeChoice();
    std::string typeName = (type == TransactionType::INCOME) ? "Income" : "Expense";

    std::vector<std::string> customCategories = categoryManager.getCustomCategories(type);

    if (customCategories.empty()) {
        std::cout << "\nNo custom " << typeName << " categories to remove.\n";
        return;
    }

    std::cout << "\n--- Custom " << typeName << " Categories ---\n\n";
    for (size_t i = 0; i < customCategories.size(); ++i) {
        std::cout << (i + 1) << ". " << customCategories[i] << "\n";
    }

    int choice;
    if (!readIntInRange(choice, 0, static_cast<int>(customCategories.size()),
        "\nEnter number of category to remove (0 to cancel): ")) {
        std::cout << "Invalid selection.\n";
        return;
    }

    if (choice == 0) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    std::string selectedCategory = customCategories[choice - 1];

    char confirm;
    std::cout << "Are you sure you want to remove '" << selectedCategory << "'? (y/n): ";
    std::cin >> confirm;
    clearInputStream();

    if (confirm == 'y' || confirm == 'Y') {
        if (categoryManager.removeCategory(selectedCategory, type)) {
            std::cout << "Successfully removed '" << selectedCategory << "' from " << typeName << " categories.\n";
        }
        else {
            std::cout << "Failed to remove category. Default categories cannot be removed.\n";
        }
    }
    else {
        std::cout << "Operation cancelled.\n";
    }
}
