#include <iostream>
#include <memory>
#include "../include/models/Transaction.h"
#include "../include/services/TransactionManager.h"
#include "../include/services/CategoryManager.h"
#include "../include/ui/TransactionInput.h"
#include "../include/ui/CategoryManagementUI.h"

void displayMenu() {
    std::cout << "\n===== Budget & Expense Manager =====\n";
    std::cout << "What would you like to do?\n\n";
    std::cout << "1. Record New Income\n";
    std::cout << "2. Record New Expense\n";
    std::cout << "3. View Transactions\n";
    std::cout << "4. View Financial Summary\n";
    std::cout << "5. Manage Categories\n";
    std::cout << "0. Exit Application\n\n";
    std::cout << "Please enter your choice (0-5): ";
}

void displayTransactionsMenu() {
    std::cout << "\n----- View Transactions -----\n";
    std::cout << "How would you like to view your transactions?\n\n";
    std::cout << "1. View All Transactions\n";
    std::cout << "2. Filter by Category\n";
    std::cout << "3. Filter by Type (Income/Expense)\n";
    std::cout << "0. Return to Main Menu\n\n";
    std::cout << "Please enter your choice (0-3): ";
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

        // Improved input handling for main menu using getline and exception handling
        std::string input;
        std::getline(std::cin, input);

        // Handle empty input
        if (input.empty()) {
            std::cout << "Error: Please enter a choice from the menu.\n";
            continue;
        }

        try {
            // Try to convert input to integer
            choice = std::stoi(input);
        }
        catch (const std::invalid_argument&) {
            std::cout << "Error: '" << input << "' is not a valid number. Please try again.\n";
            continue;
        }
        catch (const std::out_of_range&) {
            std::cout << "Error: The number you entered is too large. Please try again.\n";
            continue;
        }

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
        {
            // Transactions submenu
            int transactionChoice = -1;
            while (transactionChoice != 0) {
                displayTransactionsMenu();

                // Improved input handling for transactions menu
                std::string input;
                std::getline(std::cin, input);

                // Handle empty input
                if (input.empty()) {
                    std::cout << "Please select how you'd like to view your transactions.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    transactionChoice = std::stoi(input);

                    // Process user's choice
                    switch (transactionChoice) {
                    case 0:
                        std::cout << "Returning to main menu...\n";
                        break;
                    case 1:
                        inputHandler.displayAllTransactions();
                        break;
                    case 2:
                        inputHandler.displayTransactionsByCategory();
                        break;
                    case 3:
                        inputHandler.displayTransactionsByType();
                        break;
                    default:
                        std::cout << "Sorry, that's not a valid option. Please enter a number between 0 and 3.\n";
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Sorry, '" << input << "' isn't a valid option. Please choose from the available options.\n";
                }
                catch (const std::out_of_range&) {
                    std::cout << "That number is too large. Please enter a number between 0 and 3.\n";
                }
            }
            break;
        }
        case 4:
            inputHandler.displaySummary();
            break;
        case 5:
        {
            // Category management submenu
            int categoryChoice = -1;
            while (categoryChoice != 0) {
                categoryUI.displayCategoryMenu();

                // Improved input handling for category menu using getline and exception handling
                std::string input;
                std::getline(std::cin, input);

                // Handle empty input
                if (input.empty()) {
                    std::cout << "Error: Please enter a choice from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    categoryChoice = std::stoi(input);
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Error: '" << input << "' is not a valid number. Please try again.\n";
                    continue;
                }
                catch (const std::out_of_range&) {
                    std::cout << "Error: The number you entered is too large. Please try again.\n";
                    continue;
                }

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