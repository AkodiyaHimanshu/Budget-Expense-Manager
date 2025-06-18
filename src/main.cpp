#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <limits>
#include <stdexcept>
#include <ctime>

// Include needed headers
#include "../include/models/Transaction.h"
#include "../include/services/TransactionManager.h"
#include "../include/utils/DateUtils.h"
#include "../include/utils/FileUtils.h"
#include "../include/ui/TransactionUI.h"

// Forward declarations for menu functions
void displayMenu();
void displayTransactionViewMenu();
void displayMonthlyAnalysisMenu();

/**
 * Main function - entry point of the Budget & Expense Manager application
 */
int main() {
    std::cout << "===== Welcome to Budget & Expense Manager =====\n";

    int choice = -1;

    while (choice != 0) {
        displayMenu();

        // Improved input handling with error detection
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
            std::cout << "Thank you for using Budget & Expense Manager. Goodbye!\n";
            break;
        case 1: // Add Income
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);
            transactionUI.addIncomeTransaction();
            break;
        }
        case 2: // Add Expense
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);
            transactionUI.addExpenseTransaction();
            break;
        }
        case 3: // View Transactions
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);

            int viewChoice = -1;
            while (viewChoice != 0) {
                displayTransactionViewMenu();

                // Input handling with improved error detection
                std::string viewInput;
                std::getline(std::cin, viewInput);

                // Handle empty input
                if (viewInput.empty()) {
                    std::cout << "Error: Please enter a choice from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    viewChoice = std::stoi(viewInput);
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Error: '" << viewInput << "' is not a valid number. Please try again.\n";
                    continue;
                }
                catch (const std::out_of_range&) {
                    std::cout << "Error: The number you entered is too large. Please try again.\n";
                    continue;
                }

                switch (viewChoice) {
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
                default:
                    std::cout << "Invalid choice. Please try again (0-3).\n";
                }
            }
            break;
        }
        case 4: // Financial Summary
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);

            // Display financial summary
            transactionUI.displayFinancialSummary();
            break;
        }
        case 5: // Monthly Analysis
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);

            int analysisChoice = -1;
            while (analysisChoice != 0) {
                displayMonthlyAnalysisMenu();

                // Input handling with improved error detection
                std::string analysisInput;
                std::getline(std::cin, analysisInput);

                // Handle empty input
                if (analysisInput.empty()) {
                    std::cout << "Error: Please enter a choice from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    analysisChoice = std::stoi(analysisInput);
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Error: '" << analysisInput << "' is not a valid number. Please try again.\n";
                    continue;
                }
                catch (const std::out_of_range&) {
                    std::cout << "Error: The number you entered is too large. Please try again.\n";
                    continue;
                }

                switch (analysisChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    transactionUI.showTransactionsByMonth();
                    break;
                case 2:
                    transactionUI.showMonthlySummary();
                    break;
                case 3:
                    // View all monthly summaries
                    transactionUI.showMonthlySummary();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-3).\n";
                }
            }
            break;
        }
        case 6: // Category Management
        {
            // Add category management implementation
            std::cout << "Category management is not implemented in this version.\n";
            break;
        }
        case 7: // Advanced Transactions (with filtering options)
        {
            auto transactionManager = std::make_shared<TransactionManager>();
            TransactionUI transactionUI(transactionManager);

            int transactionChoice = -1;
            while (transactionChoice != 0) {
                transactionUI.displayTransactionsMenu();

                // Input handling with improved error detection
                std::string input;
                std::getline(std::cin, input);

                // Handle empty input
                if (input.empty()) {
                    std::cout << "Error: Please enter a choice from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    transactionChoice = std::stoi(input);
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Error: '" << input << "' is not a valid number. Please try again.\n";
                    continue;
                }
                catch (const std::out_of_range&) {
                    std::cout << "Error: The number you entered is too large. Please try again.\n";
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
                    transactionUI.showTransactionsByAmountRange(); // New case for amount range filtering
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
            }
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again (0-7).\n";
            // Clear input buffer
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}

void displayMenu() {
    std::cout << "\n===== Budget & Expense Manager Menu =====\n";
    std::cout << "1. Add Income Transaction\n";
    std::cout << "2. Add Expense Transaction\n";
    std::cout << "3. View Transactions\n";
    std::cout << "4. View Financial Summary\n";
    std::cout << "5. Monthly Analysis\n";
    std::cout << "6. Manage Categories\n";
    std::cout << "7. Advanced Transactions (with Date Range Filtering)\n";  // Added option
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice (0-7): ";  // Updated range
}

void displayTransactionViewMenu() {
    std::cout << "\n===== View Transactions =====\n";
    std::cout << "1. All Transactions\n";
    std::cout << "2. Transactions by Category\n";
    std::cout << "3. Transactions by Type\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-3): ";
}

void displayMonthlyAnalysisMenu() {
    std::cout << "\n===== Monthly Analysis =====\n";
    std::cout << "1. View Transactions by Month\n";
    std::cout << "2. View Monthly Summary\n";
    std::cout << "3. View All Monthly Summaries\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-3): ";
}