#include <iostream>
#include <memory>
#include "../include/models/Transaction.h"
#include "../include/services/TransactionManager.h"
#include "../include/ui/TransactionInput.h"

void displayMenu() {
    std::cout << "===== Budget & Expense Manager =====\n";
    std::cout << "1. Add Income Transaction\n";
    std::cout << "2. Add Expense Transaction\n";
    std::cout << "3. View All Transactions\n";
    std::cout << "4. View Financial Summary\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int main() {
    // Create a transaction manager to hold our transactions
    TransactionManager transactionManager;

    // Create the transaction input handler
    TransactionInput inputHandler(transactionManager);

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
        default:
            std::cout << "Invalid choice. Please try again.\n";
            // Clear input buffer
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}