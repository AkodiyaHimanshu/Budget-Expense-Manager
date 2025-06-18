#include "../../include/ui/TransactionUI.h"
#include "../../include/utils/DateUtils.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>

TransactionUI::TransactionUI(std::shared_ptr<TransactionManager> manager)
    : transactionManager(manager) {
}

void TransactionUI::displayTransactionsMenu() {
    std::cout << "\n===== Transactions Menu =====\n";
    std::cout << "1. View All Transactions\n";
    std::cout << "2. Filter Transactions by Category\n";
    std::cout << "3. Filter Transactions by Type (Income/Expense)\n";
    std::cout << "4. Filter Transactions by Date Range\n";
    std::cout << "5. View Transactions by Month\n";
    std::cout << "6. View Monthly Summary\n";
    std::cout << "7. Add New Transaction\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-7): ";
}

void TransactionUI::showAllTransactions() {
    std::cout << "\n===== All Transactions =====\n";

    auto transactions = transactionManager->getAllTransactions();

    if (transactions.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    displayTransactions(transactions);
}

void TransactionUI::showTransactionsByCategory() {
    std::cout << "\n===== Filter Transactions by Category =====\n";
    std::cout << "Enter category name: ";

    std::string category;
    std::getline(std::cin, category);

    auto transactions = transactionManager->getTransactionsByCategory(category);

    std::cout << "\n===== Transactions in Category: " << category << " =====\n";

    if (transactions.empty()) {
        std::cout << "No transactions found in this category.\n";
        return;
    }

    displayTransactions(transactions);
}

void TransactionUI::showTransactionsByType() {
    std::cout << "\n===== Filter Transactions by Type =====\n";
    std::cout << "1. Income\n";
    std::cout << "2. Expense\n";
    std::cout << "Enter your choice (1-2): ";

    std::string input;
    std::getline(std::cin, input);

    int choice;
    try {
        choice = std::stoi(input);
    }
    catch (const std::exception& e) {
        std::cout << "Invalid choice. Returning to transactions menu.\n";
        return;
    }

    TransactionType type;
    std::string typeStr;

    if (choice == 1) {
        type = TransactionType::INCOME;
        typeStr = "Income";
    }
    else if (choice == 2) {
        type = TransactionType::EXPENSE;
        typeStr = "Expense";
    }
    else {
        std::cout << "Invalid choice. Returning to transactions menu.\n";
        return;
    }

    auto transactions = transactionManager->getTransactionsByType(type);

    std::cout << "\n===== " << typeStr << " Transactions =====\n";

    if (transactions.empty()) {
        std::cout << "No " << typeStr << " transactions found.\n";
        return;
    }

    displayTransactions(transactions);
}

void TransactionUI::showTransactionsByDateRange() {
    std::cout << "\n===== Filter Transactions by Date Range =====\n";

    // Get start date
    std::string startDateStr;
    time_t startDate;
    bool validStartDate = false;

    while (!validStartDate) {
        std::cout << "Enter start date (YYYY-MM-DD): ";
        std::getline(std::cin, startDateStr);

        if (startDateStr.empty()) {
            std::cout << "Operation cancelled. Returning to transactions menu.\n";
            return;
        }

        try {
            // Validate and convert start date
            DateUtils::validateDateString(startDateStr);
            startDate = DateUtils::stringToTime(startDateStr);
            validStartDate = true;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << ". Please try again.\n";
        }
    }

    // Get end date
    std::string endDateStr;
    time_t endDate;
    bool validEndDate = false;

    while (!validEndDate) {
        std::cout << "Enter end date (YYYY-MM-DD): ";
        std::getline(std::cin, endDateStr);

        if (endDateStr.empty()) {
            std::cout << "Operation cancelled. Returning to transactions menu.\n";
            return;
        }

        try {
            // Validate and convert end date
            DateUtils::validateDateString(endDateStr);
            endDate = DateUtils::stringToTime(endDateStr);

            // Make sure end date is not before start date
            if (difftime(endDate, startDate) < 0) {
                throw std::invalid_argument("End date cannot be before start date");
            }

            validEndDate = true;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << ". Please try again.\n";
        }
    }

    // Get and display filtered transactions
    auto transactions = transactionManager->getTransactionsByDateRange(startDate, endDate);

    std::cout << "\n===== Transactions from " << startDateStr << " to " << endDateStr << " =====\n";

    if (transactions.empty()) {
        std::cout << "No transactions found in this date range.\n";
        return;
    }

    displayTransactions(transactions);
}

void TransactionUI::showTransactionsByMonth() {
    std::cout << "\n===== View Transactions by Month =====\n";

    // Get month in YYYY-MM format
    std::string monthStr;
    bool validMonth = false;

    while (!validMonth) {
        std::cout << "Enter month (YYYY-MM): ";
        std::getline(std::cin, monthStr);

        if (monthStr.empty()) {
            std::cout << "Operation cancelled. Returning to transactions menu.\n";
            return;
        }

        try {
            // Validate month format
            DateUtils::validateYearMonth(monthStr);
            validMonth = true;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << ". Please try again.\n";
        }
    }

    // Get transactions grouped by month
    auto monthlyTransactions = transactionManager->getTransactionsByMonth();

    std::cout << "\n===== Transactions for " << monthStr << " =====\n";

    // Check if the month exists in our transactions
    if (monthlyTransactions.find(monthStr) == monthlyTransactions.end()) {
        std::cout << "No transactions found for this month.\n";
        return;
    }

    // Display transactions for the selected month
    displayTransactions(monthlyTransactions[monthStr]);
}

void TransactionUI::showMonthlySummary() {
    std::cout << "\n===== Monthly Financial Summary =====\n";

    // Calculate monthly summary
    auto summary = transactionManager->calculateMonthlySummary();

    if (summary.empty()) {
        std::cout << "No transactions available for summary.\n";
        return;
    }

    // Display summary header
    std::cout << std::left << std::setw(10) << "Month"
        << std::right << std::setw(15) << "Income"
        << std::setw(15) << "Expenses"
        << std::setw(15) << "Net" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    // Display summary for each month
    for (const auto& [month, values] : summary) {
        double income = std::get<0>(values);
        double expenses = std::get<1>(values);
        double net = std::get<2>(values);

        std::cout << std::left << std::setw(10) << month
            << std::fixed << std::setprecision(2)
            << std::right << std::setw(15) << income
            << std::setw(15) << expenses
            << std::setw(15) << net << std::endl;
    }

    // Display overall total
    double totalIncome = transactionManager->getTotalIncome();
    double totalExpenses = transactionManager->getTotalExpenses();
    double totalNet = transactionManager->getNetAmount();

    std::cout << std::string(55, '-') << std::endl;
    std::cout << std::left << std::setw(10) << "TOTAL"
        << std::fixed << std::setprecision(2)
        << std::right << std::setw(15) << totalIncome
        << std::setw(15) << totalExpenses
        << std::setw(15) << totalNet << std::endl;
}

void TransactionUI::addNewTransaction() {
    std::cout << "\n===== Add New Transaction =====\n";

    // Get transaction type
    std::cout << "Select transaction type:\n";
    std::cout << "1. Income\n";
    std::cout << "2. Expense\n";
    std::cout << "Enter your choice (1-2): ";

    std::string typeInput;
    std::getline(std::cin, typeInput);

    int typeChoice;
    try {
        typeChoice = std::stoi(typeInput);
        if (typeChoice < 1 || typeChoice > 2) {
            std::cout << "Invalid choice. Transaction cancelled.\n";
            return;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Invalid input. Transaction cancelled.\n";
        return;
    }

    TransactionType type = (typeChoice == 1) ? TransactionType::INCOME : TransactionType::EXPENSE;

    // Get amount
    std::cout << "Enter amount: ";
    std::string amountInput;
    std::getline(std::cin, amountInput);

    double amount;
    try {
        amount = std::stod(amountInput);
        if (amount <= 0) {
            std::cout << "Amount must be greater than zero. Transaction cancelled.\n";
            return;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Invalid amount. Transaction cancelled.\n";
        return;
    }

    // Get category
    std::cout << "Enter category: ";
    std::string category;
    std::getline(std::cin, category);

    if (category.empty()) {
        category = "Uncategorized";
    }

    // Get date or use current date
    std::cout << "Enter date (YYYY-MM-DD) or leave blank for current date: ";
    std::string dateInput;
    std::getline(std::cin, dateInput);

    time_t date;
    if (dateInput.empty()) {
        // Use current date
        date = time(nullptr);
    }
    else {
        try {
            // Validate and convert date string to time_t
            DateUtils::validateDateString(dateInput);
            date = DateUtils::stringToTime(dateInput);
        }
        catch (const std::exception& e) {
            std::cout << "Invalid date format: " << e.what() << ". Transaction cancelled.\n";
            return;
        }
    }

    // Create and add the transaction
    auto transaction = std::make_shared<Transaction>(amount, date, category, type);
    transactionManager->addTransaction(transaction);

    std::cout << "Transaction added successfully!\n";
    std::cout << "Details: " << transaction->getDisplayString() << std::endl;
}

void TransactionUI::displayTransactions(const std::vector<std::shared_ptr<Transaction>>& transactions) {
    // Calculate column widths based on content
    size_t dateWidth = 19;  // YYYY-MM-DDThh:mm:ss
    size_t typeWidth = 7;   // "Income" or "Expense"
    size_t amountWidth = 10;
    size_t categoryWidth = 15;

    // Find the maximum category length
    for (const auto& transaction : transactions) {
        categoryWidth = std::max(categoryWidth, transaction->getCategory().length());
    }

    // Display header
    std::cout << std::left << std::setw(dateWidth) << "Date"
        << std::setw(typeWidth) << "Type"
        << std::right << std::setw(amountWidth) << "Amount"
        << "  " << std::left << std::setw(categoryWidth) << "Category" << std::endl;

    // Display separator
    std::cout << std::string(dateWidth + typeWidth + amountWidth + categoryWidth + 2, '-') << std::endl;

    // Display each transaction
    for (const auto& transaction : transactions) {
        std::cout << std::left << std::setw(dateWidth) << transaction->getFormattedDate()
            << std::setw(typeWidth) << transaction->getTypeAsString()
            << std::right << std::setw(amountWidth) << transaction->getFormattedAmount()
            << "  " << std::left << transaction->getCategory() << std::endl;
    }

    // Display totals
    double totalIncome = 0.0;
    double totalExpenses = 0.0;

    for (const auto& transaction : transactions) {
        if (transaction->getType() == TransactionType::INCOME) {
            totalIncome += transaction->getAmount();
        }
        else {
            totalExpenses += transaction->getAmount();
        }
    }

    double netAmount = totalIncome - totalExpenses;

    std::cout << std::string(dateWidth + typeWidth + amountWidth + categoryWidth + 2, '-') << std::endl;
    std::cout << "Total Income: " << std::fixed << std::setprecision(2) << totalIncome << std::endl;
    std::cout << "Total Expenses: " << std::fixed << std::setprecision(2) << totalExpenses << std::endl;
    std::cout << "Net Amount: " << std::fixed << std::setprecision(2) << netAmount << std::endl;
}