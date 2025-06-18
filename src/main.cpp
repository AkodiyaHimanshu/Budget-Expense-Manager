#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <filesystem>
#include <chrono>
#include <iomanip>

// Project includes
#include "../include/models/Transaction.h"
#include "../include/services/TransactionManager.h"
#include "../include/services/CategoryManager.h"
#include "../include/ui/TransactionInput.h"
#include "../include/ui/CategoryManagementUI.h"
#include "../include/ui/TransactionUI.h"  // Added for TransactionUI
#include "../include/utils/FileUtils.h"
#include "../include/utils/DateUtils.h"

// Namespace aliases
namespace fs = std::filesystem;

// Function prototypes
void displayMenu();
void displayTransactionViewMenu();
void displayMonthlyAnalysisMenu();

int main() {
    std::cout << "================================================\n";
    std::cout << "    BUDGET & EXPENSE MANAGER - VERSION 1.0\n";
    std::cout << "================================================\n";
    std::cout << "Welcome to your personal finance management tool!\n\n";

    // Initialize managers
    TransactionManager transactionManager;
    CategoryManager categoryManager;

    // Create the UI handlers
    TransactionInput inputHandler(transactionManager, categoryManager);
    CategoryManagementUI categoryUI(categoryManager);

    // Define the path to the transactions CSV file
    const std::string transactionsFilePath = "data/transactions/transactions.csv";

    // Load transactions from file on startup if the file exists
    try {
        // Check if the file exists before attempting to load
        if (fs::exists(transactionsFilePath)) {
            std::cout << "Loading transactions from " << transactionsFilePath << "...\n";
            std::cout << "-----------------------------------------------------\n";

            auto startTime = std::chrono::high_resolution_clock::now();

            // Load and validate transactions (errors are logged directly to console)
            auto loadResult = FileUtils::loadTransactionsFromCSV(transactionsFilePath);

            // Add each successfully parsed transaction to the transaction manager
            // Using reserve to optimize this loop as well
            transactionManager.reserveCapacity(loadResult.transactions.size());
            for (const auto& transaction : loadResult.transactions) {
                transactionManager.addTransaction(transaction);
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

            std::cout << "-----------------------------------------------------\n";

            // Display loading summary with timing information
            std::cout << "Loading complete in " << duration << "ms. " << loadResult.getSummary() << "\n";

            // Display appropriate message based on loading results
            if (loadResult.hasErrors()) {
                std::cout << "Note: " << loadResult.getErrorCount()
                    << " lines were skipped due to validation errors.\n";
                std::cout << "These errors have been logged above.\n";
            }
            else {
                std::cout << "All transactions loaded successfully with no validation errors.\n";
            }
        }
        else {
            std::cout << "No transactions file found at " << transactionsFilePath << ". Starting with empty transaction list.\n";

            // Ensure the directory exists for future saves
            fs::path path(transactionsFilePath);
            fs::create_directories(path.parent_path());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error loading transactions: " << e.what() << std::endl;
        std::cerr << "Starting with empty transaction list.\n";
    }

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

            // Validate choice range
            if (choice < 0 || choice > 7) {
                std::cout << "Error: Please enter a choice between 0 and 7.\n";
                continue;
            }
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
            // Save transactions to file before exiting
            try {
                // Measure save performance
                auto startTime = std::chrono::high_resolution_clock::now();

                // Get all transactions and reserve space in a vector to avoid reallocations
                auto transactions = transactionManager.getAllTransactions();
                std::cout << "Saving " << transactions.size() << " transactions to "
                    << transactionsFilePath << "...\n";

                int savedCount = FileUtils::saveTransactionsToCSV(transactions, transactionsFilePath);

                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

                std::cout << "Successfully saved " << savedCount << " transactions in "
                    << duration << "ms.\n";

                if (savedCount != static_cast<int>(transactions.size())) {
                    std::cout << "Warning: Only " << savedCount << " of "
                        << transactions.size() << " transactions were saved.\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error saving transactions: " << e.what() << std::endl;
                std::cerr << "Your transactions may not have been saved properly.\n";
            }

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
            // Transaction view submenu
            int viewChoice = -1;
            while (viewChoice != 0) {
                displayTransactionViewMenu();

                // Get user input with exception handling
                std::string input;
                std::getline(std::cin, input);

                // Handle empty input
                if (input.empty()) {
                    std::cout << "Please select an option from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    viewChoice = std::stoi(input);

                    switch (viewChoice) {
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
                        std::cout << "Sorry, that's not a valid option. Please choose between 0 and 3.\n";
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Sorry, '" << input << "' isn't a valid option. Please choose a number from the menu.\n";
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
            // Monthly analysis submenu
            int monthlyChoice = -1;
            while (monthlyChoice != 0) {
                displayMonthlyAnalysisMenu();

                // Get user input with exception handling
                std::string input;
                std::getline(std::cin, input);

                // Handle empty input
                if (input.empty()) {
                    std::cout << "Please select an option from the menu.\n";
                    continue;
                }

                try {
                    // Try to convert input to integer
                    monthlyChoice = std::stoi(input);

                    switch (monthlyChoice) {
                    case 0:
                        std::cout << "Returning to main menu...\n";
                        break;
                    case 1:
                        inputHandler.displayMonthlyTransactions();
                        break;
                    case 2:
                        inputHandler.displayMonthlySummary();
                        break;
                    case 3:
                        inputHandler.displayAllMonthlySummaries();
                        break;
                    default:
                        std::cout << "Sorry, that's not a valid option. Please choose between 0 and 3.\n";
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout << "Sorry, '" << input << "' isn't a valid option. Please choose a number from the menu.\n";
                }
                catch (const std::out_of_range&) {
                    std::cout << "That number is too large. Please enter a number between 0 and 3.\n";
                }
            }
            break;
        }
        case 6:
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
        case 7: // Transactions (new date range filtering)
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
                    transactionUI.showTransactionsByMonth();
                    break;
                case 6:
                    transactionUI.showMonthlySummary();
                    break;
                case 7:
                    transactionUI.addNewTransaction();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-7).\n";
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