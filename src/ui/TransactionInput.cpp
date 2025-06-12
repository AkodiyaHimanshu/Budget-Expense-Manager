#include "../../include/ui/TransactionInput.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>

// Constructor with CategoryManager
TransactionInput::TransactionInput(TransactionManager& tManager, CategoryManager& cManager)
    : transactionManager(tManager), categoryManager(cManager) {
}

// Get valid category using CategoryManager
std::string TransactionInput::getValidCategory(TransactionType type) {
    std::vector<std::string> categories = categoryManager.getAllCategories(type);
    std::string typeName = (type == TransactionType::INCOME) ? "Income" : "Expense";

    if (categories.empty()) {
        std::cout << "No " << typeName << " categories found. Using 'Other' as default.\n";
        return type == TransactionType::INCOME ? "Other Income" : "Other Expenses";
    }

    // Display all available categories
    std::cout << "\nAvailable " << typeName << " Categories:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        std::cout << (i + 1) << ". " << categories[i] << "\n";
    }

    // Option to add custom category
    std::cout << (categories.size() + 1) << ". Add Custom Category\n\n";

    // Get user selection
    int choice;
    std::cout << "Select category (1-" << (categories.size() + 1) << "): ";

    if (std::cin >> choice && choice >= 1 && choice <= static_cast<int>(categories.size() + 1)) {
        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Handle "Add Custom Category" option
        if (choice == static_cast<int>(categories.size() + 1)) {
            std::string customCategory;
            std::cout << "Enter new category name (3-25 chars, letters, numbers, spaces, &-_.()): ";
            std::getline(std::cin, customCategory);

            // Using same validation as in CategoryManagementUI
            std::regex pattern("^[a-zA-Z0-9 &\\-_\\.\\(\\)]{3,25}$");
            if (std::regex_match(customCategory, pattern)) {
                if (!categoryManager.categoryExists(customCategory, type)) {
                    if (categoryManager.addCategory(customCategory, type)) {
                        std::cout << "Added new category: " << customCategory << "\n";
                        return customCategory;
                    }
                    else {
                        std::cout << "Failed to add custom category. Using 'Other' instead.\n";
                        return type == TransactionType::INCOME ? "Other Income" : "Other Expenses";
                    }
                }
                else {
                    std::cout << "Category already exists. Using '" << customCategory << "'.\n";
                    return customCategory;
                }
            }
            else {
                std::cout << "Invalid category name. Using 'Other' instead.\n";
                return type == TransactionType::INCOME ? "Other Income" : "Other Expenses";
            }
        }
        else {
            // Return the selected category
            return categories[choice - 1];
        }
    }
    else {
        // Invalid input, clear buffer and use default
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid selection. Using 'Other' as default.\n";
        return type == TransactionType::INCOME ? "Other Income" : "Other Expenses";
    }
}

// Helper function to trim leading and trailing whitespace
std::string trimWhitespace(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";

    // Find the first non-whitespace character
    size_t start = str.find_first_not_of(whitespace);

    // If the string is all whitespace, return empty string
    if (start == std::string::npos) {
        return "";
    }

    // Find the last non-whitespace character
    size_t end = str.find_last_not_of(whitespace);

    // Return the trimmed substring
    return str.substr(start, end - start + 1);
}

double TransactionInput::getValidAmount() {
    double amount;
    bool validInput = false;
    std::string input;

    while (!validInput) {
        std::cout << "Enter amount: $";

        // Clear any newline left in the buffer from previous input
        if (std::cin.peek() == '\n') std::cin.get();

        // Get the entire line of input
        std::getline(std::cin, input);

        // Trim leading and trailing whitespace
        input = trimWhitespace(input);

        try {
            // Check for empty input
            if (input.empty()) {
                throw std::invalid_argument("Amount cannot be empty.");
            }

            // Check if the input contains only digits, a decimal point, and optionally a leading +
            bool validFormat = true;
            bool hasDecimal = false;
            bool hasDigits = false;
            size_t startIndex = 0;

            // Check for leading + sign and skip it for validation
            if (!input.empty() && input[0] == '+') {
                startIndex = 1;
            }
            else if (!input.empty() && input[0] == '-') {
                throw std::invalid_argument("Negative amounts are not allowed. Please enter a positive number.");
            }

            // Validate the rest of the string
            for (size_t i = startIndex; i < input.length(); ++i) {
                char c = input[i];

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

                hasDigits = true;
            }

            // Check if we actually found any digits
            if (!hasDigits) {
                throw std::invalid_argument("Amount must contain at least one digit.");
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

time_t TransactionInput::getValidDate() {
    // For simplicity, we'll just use the current date
    return std::time(nullptr);
}

// Update addIncomeTransaction to use the new method
void TransactionInput::addIncomeTransaction() {
    std::cout << "\n=== Add Income Transaction ===\n";

    // Get transaction amount
    double amount = getValidAmount();

    // Get transaction category
    std::string category = getValidCategory(TransactionType::INCOME);

    // Get date (using current date for simplicity)
    time_t date = getValidDate();

    // Create and add the transaction
    std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(
        amount, date, category, TransactionType::INCOME);
    transactionManager.addTransaction(transaction);

    std::cout << "\nIncome transaction added successfully:\n";
    std::cout << transaction->getDisplayString() << "\n\n";
}

// Update addExpenseTransaction to use the new method
void TransactionInput::addExpenseTransaction() {
    std::cout << "\n=== Add Expense Transaction ===\n";

    // Get transaction amount
    double amount = getValidAmount();

    // Get transaction category
    std::string category = getValidCategory(TransactionType::EXPENSE);

    // Get date (using current date for simplicity)
    time_t date = getValidDate();

    // Create and add the transaction
    std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(
        amount, date, category, TransactionType::EXPENSE);
    transactionManager.addTransaction(transaction);

    std::cout << "\nExpense transaction added successfully:\n";
    std::cout << transaction->getDisplayString() << "\n\n";
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