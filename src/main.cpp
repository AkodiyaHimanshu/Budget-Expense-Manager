#include <iostream>
#include <memory>
#include <limits>
#include <string>

#include "models/Transaction.h"
#include "services/TransactionManager.h"
#include "ui/TransactionUI.h"

#include "models/Budget.h"
#include "services/BudgetManager.h"
#include "ui/BudgetUI.h"

void displayMainMenu() {
    std::cout << "\n===== Budget & Expense Manager =====\n";
    std::cout << "1. Transaction Management\n";
    std::cout << "2. Budget Management\n";  // New option for budget management
    std::cout << "3. Financial Reports\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice (0-3): ";
}

int main() {
    // Initialize managers with shared_ptr for better memory management
    auto transactionManager = std::make_shared<TransactionManager>();
    auto budgetManager = std::make_shared<BudgetManager>();

    // Initialize UI components
    TransactionUI transactionUI(transactionManager);
    BudgetUI budgetUI(budgetManager, transactionManager);  // Pass both managers to BudgetUI

    // Main application loop
    int choice = -1;
    bool exitRequested = false;

    std::cout << "Welcome to Budget & Expense Manager!\n";

    while (!exitRequested) {
        displayMainMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 0: // Exit
            std::cout << "Thank you for using Budget & Expense Manager. Goodbye!\n";
            exitRequested = true;
            break;

        case 1: { // Transaction Management
            int transactionChoice = -1;

            do {
                transactionUI.displayTransactionsMenu();

                if (!(std::cin >> transactionChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (transactionChoice < 0 || transactionChoice > 8) {
                    std::cout << "Invalid choice. Please try again (0-8).\n";
                    continue;
                }

                switch (transactionChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    transactionUI.showAllTransactions();
                    break;
                case 2:
                    transactionUI.showTransactionsByCategory();
                    break;
                case 3:
                    transactionUI.showTransactionsByType();
                    break;
                case 4:
                    transactionUI.showTransactionsByDateRange();
                    break;
                case 5:
                    transactionUI.showTransactionsByAmountRange();
                    break;
                case 6:
                    transactionUI.showTransactionsByMonth();
                    break;
                case 7:
                    transactionUI.showMonthlySummary();
                    break;
                case 8:
                    transactionUI.addNewTransaction();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-8).\n";
                }
            } while (transactionChoice != 0);

            break;
        }

        case 2: { // Budget Management (NEW)
            int budgetChoice = -1;

            do {
                budgetUI.displayBudgetMenu();

                if (!(std::cin >> budgetChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (budgetChoice < 0 || budgetChoice > 7) {
                    std::cout << "Invalid choice. Please try again (0-7).\n";
                    continue;
                }

                switch (budgetChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    budgetUI.showAllBudgets();
                    break;
                case 2:
                    budgetUI.showBudgetsByCategory();
                    break;
                case 3:
                    budgetUI.showBudgetsByMonth();
                    break;
                case 4:
                    budgetUI.setBudget();
                    break;
                case 5:
                    budgetUI.updateBudget();
                    break;
                case 6:
                    budgetUI.removeBudget();
                    break;
                case 7:
                    budgetUI.showBudgetUsageReport();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-7).\n";
                }
            } while (budgetChoice != 0);

            break;
        }

        case 3: { // Financial Reports
            std::cout << "\n===== Financial Reports =====\n";
            std::cout << "1. Monthly Summary\n";
            std::cout << "2. Budget Utilization Report\n";
            std::cout << "0. Back to Main Menu\n";
            std::cout << "Enter your choice (0-2): ";

            int reportChoice;
            if (!(std::cin >> reportChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (reportChoice) {
            case 0:
                std::cout << "Returning to main menu...\n";
                break;
            case 1:
                transactionUI.showMonthlySummary();
                break;
            case 2:
                budgetUI.showBudgetUsageReport();
                break;
            default:
                std::cout << "Invalid choice. Please try again (0-2).\n";
            }

            break;
        }

        default:
            std::cout << "Invalid choice. Please try again (0-3).\n";
        }
    }

    return 0;
}