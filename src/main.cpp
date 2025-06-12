#include <iostream>
#include <memory>
#include "../include/models/Transaction.h"
#include "../include/services/TransactionManager.h"
#include "../include/services/CategoryManager.h"
#include "../include/ui/TransactionInput.h"
#include "../include/ui/CategoryManagementUI.h"

void displayMenu() {
    std::cout << "===== Budget & Expense Manager =====\n";
    std::cout << "1. Add Income Transaction\n";
    std::cout << "2. Add Expense Transaction\n";
    std::cout << "3. View All Transactions\n";
    std::cout << "4. View Financial Summary\n";
    std::cout << "5. Manage Categories\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int main() {
    // Create our service managers
    TransactionManager transactionManager;
    CategoryManager categoryManager;

    // Create the UI handlers
    TransactionInput inputHandler(transactionManager, categoryManager);
    CategoryManagementUI categoryUI(categoryManager);

    int choice = -1;

    while (choice != 0) {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
        case 0:
            std::cout << "Exiting application. Goodbye!\n";
            break;
        case 1:
            inputHandler.addIncomeTransaction();
            break;
        case 2:
            inputHandler.addExpenseTransaction();
            break;
        case 3:
            inputHandler.displayAllTransactions();
            break;
        case 4:
            inputHandler.displaySummary();
            break;
        case 5:
        {
            // Category management submenu
            int categoryChoice = -1;
            while (categoryChoice != 0) {
                categoryUI.displayCategoryMenu();
                std::cin >> categoryChoice;

                switch (categoryChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    categoryUI.showAllCategories();
                    break;
                case 2:
                    categoryUI.addNewCategory();
                    break;
                case 3:
                    categoryUI.removeCustomCategory();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
                    // Clear input buffer
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
            // Clear input buffer
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}