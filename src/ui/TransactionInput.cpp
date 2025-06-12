#include "../../include/ui/TransactionInput.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>
#include <regex>

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

// Method to display all transactions
void TransactionInput::displayAllTransactions() const {
    const auto& transactions = transactionManager.getAllTransactions();
    displayTransactionsTabular(transactions, "All Transactions");
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

// Helper method to display transactions in a tabular format
void TransactionInput::displayTransactionsTabular(const std::vector<std::shared_ptr<Transaction>>& transactions, const std::string& title) const {
    // Define column widths for consistent formatting
    const int idWidth = 5;            // For row numbers
    const int dateWidth = 20;         // For date/time
    const int typeWidth = 10;         // For transaction type (Income/Expense)
    const int amountWidth = 15;       // For monetary values
    const int categoryWidth = 20;     // For category names

    // Print header with title
    std::cout << "\n=== " << title << " ===\n\n";

    // If no transactions, show message and return
    if (transactions.empty()) {
        std::cout << "No transactions found.\n\n";
        return;
    }

    // Print header row with column labels
    std::cout << std::left
        << std::setw(idWidth) << "ID" << " | "
        << std::setw(dateWidth) << "Date" << " | "
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

    // Calculate and show totals
    double totalAmount = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction->getType() == TransactionType::INCOME) {
            totalAmount += transaction->getAmount();
        }
        else {
            totalAmount -= transaction->getAmount();
        }
    }

    std::stringstream totalStr;
    if (totalAmount >= 0) {
        totalStr << "+$" << std::fixed << std::setprecision(2) << totalAmount;
    }
    else {
        totalStr << "-$" << std::fixed << std::setprecision(2) << std::abs(totalAmount);
    }

    std::cout << "\nTotal Entries: " << transactions.size() << std::endl;
    std::cout << "Net Amount: " << totalStr.str() << std::endl << std::endl;
}

// Method to display transactions filtered by category
void TransactionInput::displayTransactionsByCategory() const {
    // Get all available categories from both income and expense types
    std::vector<std::string> incomeCategories = categoryManager.getAllCategories(TransactionType::INCOME);
    std::vector<std::string> expenseCategories = categoryManager.getAllCategories(TransactionType::EXPENSE);

    // Combine categories for display
    std::vector<std::string> allCategories;
    allCategories.insert(allCategories.end(), incomeCategories.begin(), incomeCategories.end());
    allCategories.insert(allCategories.end(), expenseCategories.begin(), expenseCategories.end());

    // Remove duplicates (in case there are categories with same name in both income and expense)
    std::sort(allCategories.begin(), allCategories.end());
    allCategories.erase(std::unique(allCategories.begin(), allCategories.end()), allCategories.end());

    // If no categories, show message and return
    if (allCategories.empty()) {
        std::cout << "\nNo categories found. Please add categories first.\n";
        return;
    }

    // Display all available categories
    std::cout << "\n=== Filter Transactions by Category ===\n\n";
    std::cout << "Available Categories:\n";
    for (size_t i = 0; i < allCategories.size(); ++i) {
        std::cout << (i + 1) << ". " << allCategories[i] << "\n";
    }

    // Get user selection with improved error handling
    int choice;
    bool validInput = false;

    do {
        std::cout << "\nSelect category (1-" << allCategories.size() << ", 0 to cancel): ";

        // Try to read an integer
        if (std::cin >> choice) {
            // Integer read successfully, check if it's in range
            if (choice >= 0 && choice <= static_cast<int>(allCategories.size())) {
                validInput = true;
            }
            else {
                std::cout << "Error: Please enter a number between 0 and " << allCategories.size() << ".\n";
            }
        }
        else {
            // Input was not an integer
            std::cout << "Error: Please enter a valid number.\n";
            std::cin.clear(); // Clear the error state
        }

        // Clear any remaining input in the buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (!validInput);

    // Cancel if user chose 0
    if (choice == 0) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Get the selected category
    std::string selectedCategory = allCategories[choice - 1];

    // Get transactions for the selected category
    std::vector<std::shared_ptr<Transaction>> filteredTransactions =
        transactionManager.getTransactionsByCategory(selectedCategory);

    // Display the filtered transactions
    displayTransactionsTabular(filteredTransactions, "Transactions in Category: " + selectedCategory);

    // Calculate and display category-specific statistics
    double categoryTotal = 0.0;
    int incomeCount = 0, expenseCount = 0;
    double incomeTotal = 0.0, expenseTotal = 0.0;

    for (const auto& transaction : filteredTransactions) {
        if (transaction->getType() == TransactionType::INCOME) {
            incomeCount++;
            incomeTotal += transaction->getAmount();
            categoryTotal += transaction->getAmount();
        }
        else {
            expenseCount++;
            expenseTotal += transaction->getAmount();
            categoryTotal -= transaction->getAmount();
        }
    }

    // Display category statistics
    std::cout << "=== Category Statistics: " << selectedCategory << " ===\n\n";
    std::cout << "Income Transactions: " << incomeCount
        << " ($" << std::fixed << std::setprecision(2) << incomeTotal << ")\n";
    std::cout << "Expense Transactions: " << expenseCount
        << " ($" << std::fixed << std::setprecision(2) << expenseTotal << ")\n";

    std::stringstream netStr;
    if (categoryTotal >= 0) {
        netStr << "+$" << std::fixed << std::setprecision(2) << categoryTotal;
    }
    else {
        netStr << "-$" << std::fixed << std::setprecision(2) << std::abs(categoryTotal);
    }

    std::cout << "Net Category Impact: " << netStr.str() << "\n\n";
}