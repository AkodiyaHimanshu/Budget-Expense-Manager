#include "../../include/ui/TransactionInput.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>

TransactionInput::TransactionInput(TransactionManager& manager) : transactionManager(manager) {}

double TransactionInput::getValidAmount() {
    double amount;
    bool validInput = false;
    std::string input;

    while (!validInput) {
        std::cout << "Enter amount: $";
        std::cin >> input;

        // Clear any existing errors
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            // Check for empty input
            if (input.empty()) {
                throw std::invalid_argument("Amount cannot be empty.");
            }

            // Check if the input contains only digits, a decimal point, and optionally a leading +
            bool validFormat = true;
            bool hasDecimal = false;

            for (size_t i = 0; i < input.length(); ++i) {
                char c = input[i];

                // Allow + only at the beginning
                if (c == '+' && i == 0) {
                    continue;
                }

                // Allow only one decimal point
                if (c == '.') {
                    if (hasDecimal) {
                        validFormat = false;
                        break;
                    }
                    hasDecimal = true;
                    continue;
                }

                // Must be a digit
                if (!std::isdigit(c)) {
                    validFormat = false;
                    break;
                }
            }

            if (!validFormat) {
                throw std::invalid_argument("Amount must contain only digits and at most one decimal point.");
            }

            // Convert to double
            amount = std::stod(input);

            // Check for zero
            if (amount == 0) {
                throw std::invalid_argument("Amount cannot be zero.");
            }

            // Check for negative values
            if (amount < 0) {
                throw std::invalid_argument("Amount cannot be negative. Please enter a positive number.");
            }

            validInput = true;
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Error: " << e.what() << std::endl;
            std::cout << "Please try again." << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cout << "Error: The number is too large." << std::endl;
            std::cout << "Please enter a smaller amount." << std::endl;
        }
        catch (...) {
            std::cout << "Error: Invalid input format." << std::endl;
            std::cout << "Please enter a valid positive number (e.g., 123.45)." << std::endl;
        }
    }

    return amount;
}

std::string TransactionInput::getValidCategory() {
    std::string category;
    bool validInput = false;

    while (!validInput) {
        std::cout << "Enter category: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, category);

        if (!category.empty()) {
            validInput = true;
        }
        else {
            std::cout << "Category cannot be empty. Please enter a valid category.\n";
        }
    }

    return category;
}

time_t TransactionInput::getValidDate() {
    // For simplicity, we'll just use the current date
    return std::time(nullptr);
}

void TransactionInput::addIncomeTransaction() {
    // Clear any previous input errors
    std::cin.clear();

    std::cout << "\n=== Add Income Transaction ===\n";

    // Get amount
    double amount = getValidAmount();

    // Get category
    std::string category = getValidCategory();

    // Use current date for simplicity
    time_t date = getValidDate();

    // Create and add the transaction
    auto transaction = std::make_shared<Transaction>(
        amount,
        date,
        category,
        TransactionType::INCOME
    );

    transactionManager.addTransaction(transaction);

    std::cout << "\nIncome transaction added successfully!\n";
    std::cout << "Transaction details: " << transaction->getDisplayString() << "\n\n";
}

void TransactionInput::addExpenseTransaction() {
    // Clear any previous input errors
    std::cin.clear();

    std::cout << "\n=== Add Expense Transaction ===\n";

    // Get amount
    double amount = getValidAmount();

    // Get category
    std::string category = getValidCategory();

    // Use current date for simplicity
    time_t date = getValidDate();

    // Create and add the transaction
    auto transaction = std::make_shared<Transaction>(
        amount,
        date,
        category,
        TransactionType::EXPENSE
    );

    transactionManager.addTransaction(transaction);

    std::cout << "\nExpense transaction added successfully!\n";
    std::cout << "Transaction details: " << transaction->getDisplayString() << "\n\n";
}

void TransactionInput::displayAllTransactions() const {
    const auto& transactions = transactionManager.getAllTransactions();

    if (transactions.empty()) {
        std::cout << "\nNo transactions recorded yet.\n";
        return;
    }

    // Column widths for consistent formatting
    const int idWidth = 4;           // For transaction number
    const int dateWidth = 20;        // For date
    const int typeWidth = 10;        // For transaction type
    const int amountWidth = 15;      // For amount with currency symbol
    const int categoryWidth = 25;    // For category

    // Table header with separator line
    std::cout << "\n=== All Transactions ===\n\n";

    // Print header row with column labels
    std::cout << std::left
        << std::setw(idWidth) << "ID" << " | "
        << std::setw(dateWidth) << "Date & Time" << " | "
        << std::setw(typeWidth) << "Type" << " | "
        << std::setw(amountWidth) << "Amount" << " | "
        << std::setw(categoryWidth) << "Category"
        << std::endl;

    // Print separator line
    std::string separator(idWidth + dateWidth + typeWidth + amountWidth + categoryWidth + 12, '-');
    std::cout << separator << std::endl;

    // Print each transaction in tabular format
    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& transaction = transactions[i];

        std::cout << std::left
            << std::setw(idWidth) << (i + 1) << " | "
            << std::setw(dateWidth) << transaction->getFormattedDate() << " | "
            << std::setw(typeWidth) << transaction->getTypeAsString() << " | "
            << std::setw(amountWidth) << transaction->getFormattedAmount() << " | "
            << std::setw(categoryWidth) << transaction->getCategory()
            << std::endl;
    }

    // Print footer separator
    std::cout << separator << std::endl;
    std::cout << "\nTotal Entries: " << transactions.size() << std::endl << std::endl;
}

void TransactionInput::displaySummary() const {
    double totalIncome = transactionManager.calculateTotal(TransactionType::INCOME);
    double totalExpense = transactionManager.calculateTotal(TransactionType::EXPENSE);
    double balance = totalIncome - totalExpense;

    // Get count of each type of transaction
    size_t incomeCount = transactionManager.getTransactionsByType(TransactionType::INCOME).size();
    size_t expenseCount = transactionManager.getTransactionsByType(TransactionType::EXPENSE).size();

    // Column widths for consistent formatting
    const int categoryWidth = 20;      // For financial category
    const int countWidth = 15;         // For transaction count
    const int amountWidth = 20;        // For monetary values

    // Table header with separator line
    std::cout << "\n=== Financial Summary ===\n\n";

    // Print header row with column labels
    std::cout << std::left
        << std::setw(categoryWidth) << "Category" << " | "
        << std::setw(countWidth) << "Transactions" << " | "
        << std::setw(amountWidth) << "Amount"
        << std::endl;

    // Print separator line
    std::string separator(categoryWidth + countWidth + amountWidth + 6, '-');
    std::cout << separator << std::endl;

    // Format financial figures with currency symbol and 2 decimal places
    std::stringstream incomeStr, expenseStr, balanceStr;

    incomeStr << "$" << std::fixed << std::setprecision(2) << totalIncome;
    expenseStr << "$" << std::fixed << std::setprecision(2) << totalExpense;

    // Format balance with a sign to indicate positive/negative
    if (balance >= 0) {
        balanceStr << "+$" << std::fixed << std::setprecision(2) << balance;
    }
    else {
        balanceStr << "-$" << std::fixed << std::setprecision(2) << std::abs(balance);
    }

    // Print Income row
    std::cout << std::left
        << std::setw(categoryWidth) << "Total Income" << " | "
        << std::setw(countWidth) << incomeCount << " | "
        << std::setw(amountWidth) << incomeStr.str()
        << std::endl;

    // Print Expense row
    std::cout << std::left
        << std::setw(categoryWidth) << "Total Expenses" << " | "
        << std::setw(countWidth) << expenseCount << " | "
        << std::setw(amountWidth) << expenseStr.str()
        << std::endl;

    // Print separator before balance
    std::cout << separator << std::endl;

    // Print Balance row
    std::cout << std::left
        << std::setw(categoryWidth) << "Current Balance" << " | "
        << std::setw(countWidth) << (incomeCount + expenseCount) << " | "
        << std::setw(amountWidth) << balanceStr.str()
        << std::endl;

    // Print footer separator
    std::cout << separator << std::endl << std::endl;
}