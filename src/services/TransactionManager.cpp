#include "../../include/services/TransactionManager.h"
#include "../../include/utils/FileUtils.h"
#include "../../include/utils/DateUtils.h"
#include <algorithm>
#include <numeric>
#include <iostream>

TransactionManager::TransactionManager() {
    loadTransactions();
}

TransactionManager::~TransactionManager() {
    // Ensure transactions are saved before destroying the manager
    try {
        saveTransactions();
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving transactions during cleanup: " << e.what() << std::endl;
    }
}

void TransactionManager::addTransaction(const std::shared_ptr<Transaction>& transaction) {
    transactions.push_back(transaction);
    // Sort transactions by date (newest first)
    std::sort(transactions.begin(), transactions.end(),
        [](const std::shared_ptr<Transaction>& a, const std::shared_ptr<Transaction>& b) {
            return a->getDate() > b->getDate();
        });
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getAllTransactions() const {
    return transactions;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Transaction>> result;

    for (const auto& t : transactions) {
        if (t->getCategory() == category) {
            result.push_back(t);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByType(TransactionType type) const {
    std::vector<std::shared_ptr<Transaction>> result;

    for (const auto& t : transactions) {
        if (t->getType() == type) {
            result.push_back(t);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByDateRange(time_t startDate, time_t endDate) const {
    std::vector<std::shared_ptr<Transaction>> result;

    for (const auto& t : transactions) {
        if (DateUtils::isDateInRange(t->getDate(), startDate, endDate)) {
            result.push_back(t);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByAmountRange(double minAmount, double maxAmount) const {
    std::vector<std::shared_ptr<Transaction>> filteredTransactions;
    for (const auto& transaction : transactions) {
        double amount = transaction->getAmount();
        if (amount >= minAmount && amount <= maxAmount) {
            filteredTransactions.push_back(transaction);
        }
    }
    return filteredTransactions;
}

std::map<std::string, std::vector<std::shared_ptr<Transaction>>> TransactionManager::getTransactionsByMonth() const {
    std::map<std::string, std::vector<std::shared_ptr<Transaction>>> monthlyTransactions;

    for (const auto& t : transactions) {
        std::string monthKey = t->getMonthKey();
        monthlyTransactions[monthKey].push_back(t);
    }

    return monthlyTransactions;
}

std::map<std::string, std::tuple<double, double, double>> TransactionManager::calculateMonthlySummary() const {
    auto monthlyTransactions = getTransactionsByMonth();
    std::map<std::string, std::tuple<double, double, double>> monthlySummary;

    for (const auto& [month, transactions] : monthlyTransactions) {
        double income = 0.0;
        double expenses = 0.0;

        for (const auto& t : transactions) {
            if (t->getType() == TransactionType::INCOME) {
                income += t->getAmount();
            }
            else {
                expenses += t->getAmount();
            }
        }

        double netAmount = income - expenses;
        monthlySummary[month] = std::make_tuple(income, expenses, netAmount);
    }

    return monthlySummary;
}

void TransactionManager::saveTransactions() {
    try {
        int savedCount = FileUtils::saveTransactionsToCSV(transactions, dataFilePath);
        std::cout << "Saved " << savedCount << " transactions to " << dataFilePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving transactions: " << e.what() << std::endl;
        throw; // Rethrow to allow caller to handle the error
    }
}

void TransactionManager::loadTransactions() {
    try {
        // Ensure the data directory exists
        std::string dataDir = dataFilePath.substr(0, dataFilePath.find_last_of('/'));
        FileUtils::createDirectoryIfNotExists(dataDir);

        // Check if file exists before attempting to load
        if (!FileUtils::fileExists(dataFilePath)) {
            std::cout << "No transaction data file found. A new file will be created when transactions are added.\n";
            return;
        }

        // Load transactions from file
        auto loadResult = FileUtils::loadTransactionsFromCSV(dataFilePath);
        transactions = loadResult.transactions;

        // Log any errors that occurred during loading
        if (loadResult.hasErrors()) {
            std::cerr << "Warning: " << loadResult.getErrorCount() << " errors encountered while loading transactions.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading transactions: " << e.what() << std::endl;
    }
}

double TransactionManager::getTotalIncome() const {
    return std::accumulate(transactions.begin(), transactions.end(), 0.0,
        [](double sum, const std::shared_ptr<Transaction>& t) {
            return sum + (t->getType() == TransactionType::INCOME ? t->getAmount() : 0.0);
        });
}

double TransactionManager::getTotalExpenses() const {
    return std::accumulate(transactions.begin(), transactions.end(), 0.0,
        [](double sum, const std::shared_ptr<Transaction>& t) {
            return sum + (t->getType() == TransactionType::EXPENSE ? t->getAmount() : 0.0);
        });
}

double TransactionManager::getNetAmount() const {
    return getTotalIncome() - getTotalExpenses();
}

bool TransactionManager::checkBudgetExceeded(const std::shared_ptr<Transaction>& transaction,
    const std::shared_ptr<BudgetManager>& budgetManager,
    std::string& warningMessage) const {
    // Only check for expense transactions
    if (transaction->getType() != TransactionType::EXPENSE) {
        return false;
    }

    std::string category = transaction->getCategory();
    std::string monthKey = transaction->getMonthKey();
    double amount = transaction->getAmount();

    // Get the budget for this category and month
    auto budget = budgetManager->getBudgetByMonthAndCategory(monthKey, category);
    if (!budget) {
        return false; // No budget set for this category/month
    }

    // Calculate current spending for this category in this month
    double currentSpending = 0.0;
    for (const auto& t : transactions) {
        if (t->getType() == TransactionType::EXPENSE &&
            t->getCategory() == category &&
            t->getMonthKey() == monthKey) {
            currentSpending += t->getAmount();
        }
    }

    // Add the new transaction amount
    double newTotal = currentSpending + amount;
    double limit = budget->getAmount();

    // Check if it exceeds the budget
    if (newTotal > limit) {
        double percentExceeded = ((newTotal - limit) / limit) * 100.0;
        warningMessage = "WARNING: This expense will exceed your budget for " +
            category + " in " + monthKey + " by $" +
            std::to_string(newTotal - limit) +
            " (" + std::to_string(static_cast<int>(percentExceeded)) + "%).";
        return true;
    }

    // Check if it's close to the budget (90% or more)
    if (newTotal >= 0.9 * limit) {
        double percentUsed = (newTotal / limit) * 100.0;
        warningMessage = "CAUTION: This expense will bring you to " +
            std::to_string(static_cast<int>(percentUsed)) +
            "% of your budget for " + category + " in " + monthKey + ".";
        return true;
    }

    return false;
}