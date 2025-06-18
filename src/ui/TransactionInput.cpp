// TransactionInput.cpp
#include "../../include/ui/TransactionInput.h"
#include "../../include/utils/DateUtils.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <algorithm>

TransactionInput::TransactionInput(std::shared_ptr<TransactionManager> trManager,
    std::shared_ptr<CategoryManager> catManager)
    : transactionManager(trManager), categoryManager(catManager) {
}

void TransactionInput::addIncomeTransaction() {
    std::cout << "\n===== Add Income Transaction =====\n";

    // Get amount
    double amount;
    std::cout << "Enter amount: ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Please enter a valid positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear input buffer after reading amount
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get category
    std::string category = getCategoryFromUser();

    // Add the transaction
    if (addTransaction(amount, category, TransactionType::INCOME)) {
        std::cout << "\nIncome transaction added successfully!\n";
    }
    else {
        std::cout << "\nFailed to add income transaction. Please try again.\n";
    }
}

void TransactionInput::addExpenseTransaction() {
    std::cout << "\n===== Add Expense Transaction =====\n";

    // Get amount
    double amount;
    std::cout << "Enter amount: ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Please enter a valid positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear input buffer after reading amount
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get category
    std::string category = getCategoryFromUser();

    // Add the transaction
    if (addTransaction(amount, category, TransactionType::EXPENSE)) {
        std::cout << "\nExpense transaction added successfully!\n";
    }
    else {
        std::cout << "\nFailed to add expense transaction. Please try again.\n";
    }
}

bool TransactionInput::addTransaction(double amount, const std::string& category, TransactionType type) {
    try {
        // Create a new transaction with the current date and time
        auto transaction = std::make_shared<Transaction>(amount, std::time(nullptr), category, type);

        // Add the transaction to the manager
        transactionManager->addTransaction(transaction);

        // Display the added transaction
        std::cout << "\nTransaction details:\n";
        displayTransaction(transaction);

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding transaction: " << e.what() << std::endl;
        return false;
    }
}

std::string TransactionInput::getCategoryFromUser() {
    // Get all available categories
    auto categories = categoryManager->getAllCategories();

    // Display categories
    std::cout << "\nAvailable Categories:\n";
    int idx = 1;
    for (const auto& category : categories) {
        std::cout << idx << ". " << category << "\n";
        idx++;
    }

    // Add option for entering a custom category
    std::cout << idx << ". Enter a custom category\n";

    // Get user choice
    int choice;
    std::cout << "\nSelect a category (1-" << idx << "): ";
    while (!(std::cin >> choice) || choice < 1 || choice > idx) {
        std::cout << "Please enter a valid choice (1-" << idx << "): ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear input buffer after reading choice
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Return selected category or get custom category
    if (choice < idx) {
        return categories[choice - 1];
    }
    else {
        std::string customCategory;
        std::cout << "Enter custom category name: ";
        std::getline(std::cin, customCategory);

        // Add the custom category
        categoryManager->addCategory(customCategory);

        return customCategory;
    }
}

void TransactionInput::displayAllTransactions() const {
    std::cout << "\n===== All Transactions =====\n";

    auto transactions = transactionManager->getAllTransactions();

    if (transactions.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    // Display transactions
    for (const auto& transaction : transactions) {
        displayTransaction(transaction);
    }

    // Display totals
    double totalIncome = transactionManager->getTotalIncome();
    double totalExpenses = transactionManager->getTotalExpenses();
    double netAmount = transactionManager->getNetAmount();

    std::cout << "\nSummary:\n";
    std::cout << "Total Income: " << formatCurrency(totalIncome) << "\n";
    std::cout << "Total Expenses: " << formatCurrency(totalExpenses) << "\n";
    std::cout << "Net Amount: " << formatCurrency(netAmount) << "\n";
}

void TransactionInput::displayTransactionsByCategory() const {
    std::cout << "\n===== Transactions by Category =====\n";

    // Get all available categories
    auto categories = categoryManager->getAllCategories();

    if (categories.empty()) {
        std::cout << "No categories available.\n";
        return;
    }

    // Display categories
    std::cout << "Available Categories:\n";
    int idx = 1;
    for (const auto& category : categories) {
        std::cout << idx << ". " << category << "\n";
        idx++;
    }

    // Get user choice
    int choice;
    std::cout << "\nSelect a category (1-" << categories.size() << "): ";
    while (!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(categories.size())) {
        std::cout << "Please enter a valid choice (1-" << categories.size() << "): ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear input buffer after reading choice
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get selected category
    std::string selectedCategory = categories[choice - 1];

    // Get transactions for the selected category
    auto transactions = transactionManager->getTransactionsByCategory(selectedCategory);

    std::cout << "\nTransactions in category '" << selectedCategory << "':\n";

    if (transactions.empty()) {
        std::cout << "No transactions found in this category.\n";
        return;
    }

    // Display transactions
    for (const auto& transaction : transactions) {
        displayTransaction(transaction);
    }

    // Calculate totals for the category
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

    std::cout << "\nCategory Summary:\n";
    std::cout << "Total Income in '" << selectedCategory << "': " << formatCurrency(totalIncome) << "\n";
    std::cout << "Total Expenses in '" << selectedCategory << "': " << formatCurrency(totalExpenses) << "\n";
    std::cout << "Net Amount in '" << selectedCategory << "': " << formatCurrency(netAmount) << "\n";
}

void TransactionInput::displayTransactionsByType() const {
    std::cout << "\n===== Transactions by Type =====\n";
    std::cout << "1. Income Transactions\n";
    std::cout << "2. Expense Transactions\n";

    // Get user choice
    int choice;
    std::cout << "\nSelect an option (1-2): ";
    while (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cout << "Please enter a valid choice (1-2): ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear input buffer after reading choice
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get transactions by type
    TransactionType type = (choice == 1) ? TransactionType::INCOME : TransactionType::EXPENSE;
    std::string typeStr = (choice == 1) ? "Income" : "Expense";

    auto transactions = transactionManager->getTransactionsByType(type);

    std::cout << "\n" << typeStr << " Transactions:\n";

    if (transactions.empty()) {
        std::cout << "No " << typeStr << " transactions found.\n";
        return;
    }

    // Display transactions
    for (const auto& transaction : transactions) {
        displayTransaction(transaction);
    }

    // Calculate total for the type
    double total = 0.0;
    for (const auto& transaction : transactions) {
        total += transaction->getAmount();
    }

    std::cout << "\nTotal " << typeStr << ": " << formatCurrency(total) << "\n";
}

void TransactionInput::displaySummary() const {
    std::cout << "\n===== Financial Summary =====\n";

    // Get totals from the transaction manager
    double totalIncome = transactionManager->getTotalIncome();
    double totalExpenses = transactionManager->getTotalExpenses();
    double netAmount = transactionManager->getNetAmount();

    // Display summary information
    std::cout << "Total Income: " << formatCurrency(totalIncome) << "\n";
    std::cout << "Total Expenses: " << formatCurrency(totalExpenses) << "\n";
    std::cout << "Net Amount: " << formatCurrency(netAmount) << "\n";

    // Calculate savings rate if income is not zero
    if (totalIncome > 0) {
        double savingsRate = (netAmount / totalIncome) * 100;
        std::cout << "Savings Rate: " << std::fixed << std::setprecision(2) << savingsRate << "%\n";
    }

    // Get transactions by category for detailed breakdown
    auto categories = categoryManager->getAllCategories();

    if (!categories.empty()) {
        std::cout << "\nExpense Breakdown by Category:\n";

        // Sort categories by expense amount (descending)
        std::vector<std::pair<std::string, double>> categoryExpenses;

        for (const auto& category : categories) {
            auto transactions = transactionManager->getTransactionsByCategory(category);

            double categoryExpense = 0.0;
            for (const auto& transaction : transactions) {
                if (transaction->getType() == TransactionType::EXPENSE) {
                    categoryExpense += transaction->getAmount();
                }
            }

            if (categoryExpense > 0) {
                categoryExpenses.push_back({ category, categoryExpense });
            }
        }

        // Sort by expense amount (descending)
        std::sort(categoryExpenses.begin(), categoryExpenses.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        // Display category breakdown
        for (const auto& [category, amount] : categoryExpenses) {
            // Calculate percentage of total expenses
            double percentage = (amount / totalExpenses) * 100;

            std::cout << category << ": " << formatCurrency(amount)
                << " (" << std::fixed << std::setprecision(2) << percentage << "%)\n";
        }
    }
}

std::string TransactionInput::getMonthFromUser() {
    std::string monthStr;
    bool validMonth = false;

    while (!validMonth) {
        std::cout << "Enter month (YYYY-MM): ";
        std::getline(std::cin, monthStr);

        try {
            // Validate month format
            DateUtils::validateYearMonth(monthStr);
            validMonth = true;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << ". Please try again.\n";
        }
    }

    return monthStr;
}

void TransactionInput::displayMonthlyTransactions() const {
    std::cout << "\n===== Monthly Transactions =====\n";

    // Get month from user
    std::string monthStr = getMonthFromUser();

    // Get transactions for the month
    auto monthlyTransactions = transactionManager->getTransactionsByMonth();

    if (monthlyTransactions.find(monthStr) == monthlyTransactions.end() ||
        monthlyTransactions[monthStr].empty()) {
        std::cout << "No transactions found for " << monthStr << ".\n";
        return;
    }

    std::cout << "\nTransactions for " << monthStr << ":\n";

    // Display transactions for the month
    for (const auto& transaction : monthlyTransactions[monthStr]) {
        displayTransaction(transaction);
    }

    // Calculate monthly totals
    double monthlyIncome = 0.0;
    double monthlyExpenses = 0.0;

    for (const auto& transaction : monthlyTransactions[monthStr]) {
        if (transaction->getType() == TransactionType::INCOME) {
            monthlyIncome += transaction->getAmount();
        }
        else {
            monthlyExpenses += transaction->getAmount();
        }
    }

    double monthlyNet = monthlyIncome - monthlyExpenses;

    // Display monthly summary
    std::cout << "\nMonthly Summary for " << monthStr << ":\n";
    std::cout << "Total Income: " << formatCurrency(monthlyIncome) << "\n";
    std::cout << "Total Expenses: " << formatCurrency(monthlyExpenses) << "\n";
    std::cout << "Net Amount: " << formatCurrency(monthlyNet) << "\n";

    // Calculate savings rate if income is not zero
    if (monthlyIncome > 0) {
        double savingsRate = (monthlyNet / monthlyIncome) * 100;
        std::cout << "Savings Rate: " << std::fixed << std::setprecision(2) << savingsRate << "%\n";
    }
}

void TransactionInput::displayMonthlySummary() const {
    std::cout << "\n===== Monthly Summary =====\n";

    // Get month from user
    std::string monthStr = getMonthFromUser();

    // Get monthly summary
    auto monthlySummary = transactionManager->calculateMonthlySummary();

    if (monthlySummary.find(monthStr) == monthlySummary.end()) {
        std::cout << "No transactions found for " << monthStr << ".\n";
        return;
    }

    // Get the totals for the month
    auto [income, expense, net] = monthlySummary[monthStr];

    // Display monthly summary
    std::cout << "\nFinancial Summary for " << monthStr << ":\n";
    std::cout << "Total Income: " << formatCurrency(income) << "\n";
    std::cout << "Total Expenses: " << formatCurrency(expense) << "\n";
    std::cout << "Net Amount: " << formatCurrency(net) << "\n";

    // Calculate savings rate if income is not zero
    if (income > 0) {
        double savingsRate = (net / income) * 100;
        std::cout << "Savings Rate: " << std::fixed << std::setprecision(2) << savingsRate << "%\n";
    }

    // Get category breakdown for the month
    auto monthlyTransactions = transactionManager->getTransactionsByMonth();

    if (monthlyTransactions.find(monthStr) != monthlyTransactions.end() &&
        !monthlyTransactions[monthStr].empty()) {

        auto categories = categoryManager->getAllCategories();

        if (!categories.empty()) {
            std::cout << "\nExpense Breakdown for " << monthStr << ":\n";

            // Calculate expenses by category
            std::vector<std::pair<std::string, double>> categoryExpenses;

            for (const auto& category : categories) {
                double categoryExpense = 0.0;

                for (const auto& transaction : monthlyTransactions[monthStr]) {
                    if (transaction->getType() == TransactionType::EXPENSE &&
                        transaction->getCategory() == category) {
                        categoryExpense += transaction->getAmount();
                    }
                }

                if (categoryExpense > 0) {
                    categoryExpenses.push_back({ category, categoryExpense });
                }
            }

            // Sort by expense amount (descending)
            std::sort(categoryExpenses.begin(), categoryExpenses.end(),
                [](const auto& a, const auto& b) { return a.second > b.second; });

            // Display category breakdown
            for (const auto& [category, amount] : categoryExpenses) {
                // Calculate percentage of total expenses
                double percentage = (amount / expense) * 100;

                std::cout << category << ": " << formatCurrency(amount)
                    << " (" << std::fixed << std::setprecision(2) << percentage << "%)\n";
            }
        }
    }
}

void TransactionInput::displayAllMonthlySummaries() const {
    std::cout << "\n===== All Monthly Summaries =====\n";

    // Get monthly summary for all months
    auto monthlySummary = transactionManager->calculateMonthlySummary();

    if (monthlySummary.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    // Sort months chronologically (they should already be in YYYY-MM format)
    std::vector<std::string> months;
    for (const auto& [month, _] : monthlySummary) {
        months.push_back(month);
    }

    std::sort(months.begin(), months.end());

    // Display table header
    std::cout << std::left << std::setw(10) << "Month"
        << std::right << std::setw(15) << "Income"
        << std::setw(15) << "Expenses"
        << std::setw(15) << "Net"
        << std::setw(15) << "Savings Rate"
        << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    // Display each month's summary
    for (const auto& month : months) {
        auto [income, expense, net] = monthlySummary[month];

        // Calculate savings rate
        double savingsRate = (income > 0) ? (net / income) * 100 : 0.0;

        // Display the row
        std::cout << std::left << std::setw(10) << month
            << std::right << std::setw(15) << formatCurrency(income)
            << std::setw(15) << formatCurrency(expense)
            << std::setw(15) << formatCurrency(net)
            << std::setw(15) << std::fixed << std::setprecision(2) << savingsRate << "%"
            << std::endl;
    }

    // Calculate totals across all months
    double totalIncome = 0.0;
    double totalExpenses = 0.0;
    double totalNet = 0.0;

    for (const auto& [_, values] : monthlySummary) {
        auto [income, expense, net] = values;
        totalIncome += income;
        totalExpenses += expense;
        totalNet += net;
    }

    // Calculate overall savings rate
    double overallSavingsRate = (totalIncome > 0) ? (totalNet / totalIncome) * 100 : 0.0;

    // Display totals
    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::left << std::setw(10) << "TOTAL"
        << std::right << std::setw(15) << formatCurrency(totalIncome)
        << std::setw(15) << formatCurrency(totalExpenses)
        << std::setw(15) << formatCurrency(totalNet)
        << std::setw(15) << std::fixed << std::setprecision(2) << overallSavingsRate << "%"
        << std::endl;

    // Display additional insights
    std::cout << "\nAdditional Insights:\n";

    // Find month with highest income
    auto highestIncomeMonth = *std::max_element(months.begin(), months.end(),
        [&monthlySummary](const std::string& a, const std::string& b) {
            return std::get<0>(monthlySummary[a]) < std::get<0>(monthlySummary[b]);
        });

    std::cout << "Highest Income Month: " << highestIncomeMonth << " ("
        << formatCurrency(std::get<0>(monthlySummary[highestIncomeMonth])) << ")\n";

    // Find month with highest expenses
    auto highestExpenseMonth = *std::max_element(months.begin(), months.end(),
        [&monthlySummary](const std::string& a, const std::string& b) {
            return std::get<1>(monthlySummary[a]) < std::get<1>(monthlySummary[b]);
        });

    std::cout << "Highest Expense Month: " << highestExpenseMonth << " ("
        << formatCurrency(std::get<1>(monthlySummary[highestExpenseMonth])) << ")\n";

    // Find month with highest net (most profitable)
    auto highestNetMonth = *std::max_element(months.begin(), months.end(),
        [&monthlySummary](const std::string& a, const std::string& b) {
            return std::get<2>(monthlySummary[a]) < std::get<2>(monthlySummary[b]);
        });

    std::cout << "Most Profitable Month: " << highestNetMonth << " ("
        << formatCurrency(std::get<2>(monthlySummary[highestNetMonth])) << ")\n";

    // Calculate monthly averages
    double avgIncome = totalIncome / months.size();
    double avgExpenses = totalExpenses / months.size();
    double avgNet = totalNet / months.size();

    std::cout << "\nMonthly Averages:\n";
    std::cout << "Average Income: " << formatCurrency(avgIncome) << "\n";
    std::cout << "Average Expenses: " << formatCurrency(avgExpenses) << "\n";
    std::cout << "Average Net: " << formatCurrency(avgNet) << "\n";
}

void TransactionInput::displayTransaction(const std::shared_ptr<Transaction>& transaction) const {
    std::cout << "Date: " << transaction->getFormattedDate() << "\n";
    std::cout << "Type: " << transaction->getTypeAsString() << "\n";
    std::cout << "Category: " << transaction->getCategory() << "\n";
    std::cout << "Amount: " << formatCurrency(transaction->getAmount()) << "\n";
    std::cout << std::string(30, '-') << "\n";
}

std::string TransactionInput::formatCurrency(double amount) const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}