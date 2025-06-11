#include "../../include/ui/TransactionInput.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>

TransactionInput::TransactionInput(TransactionManager& manager) : transactionManager(manager) {}

double TransactionInput::getValidAmount() {
    double amount;
    bool validInput = false;

    while (!validInput) {
        std::cout << "Enter amount: $";
        if (std::cin >> amount && amount > 0) {
            validInput = true;
        }
        else {
            std::cout << "Invalid amount. Please enter a positive number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

void TransactionInput::displayAllTransactions() const {
    const auto& transactions = transactionManager.getAllTransactions();

    if (transactions.empty()) {
        std::cout << "\nNo transactions recorded yet.\n";
        return;
    }

    std::cout << "\n=== All Transactions ===\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        std::cout << (i + 1) << ". " << transactions[i]->getDisplayString() << "\n";
    }
    std::cout << std::endl;
}

void TransactionInput::displaySummary() const {
    double totalIncome = transactionManager.calculateTotal(TransactionType::INCOME);
    double totalExpense = transactionManager.calculateTotal(TransactionType::EXPENSE);
    double balance = totalIncome - totalExpense;

    std::cout << "\n=== Financial Summary ===\n";
    std::cout << "Total Income: $" << std::fixed << std::setprecision(2) << totalIncome << "\n";
    std::cout << "Total Expenses: $" << std::fixed << std::setprecision(2) << totalExpense << "\n";
    std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << balance << "\n\n";
}