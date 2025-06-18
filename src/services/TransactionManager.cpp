#include "../../include/services/TransactionManager.h"
#include "../../include/utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

TransactionManager::TransactionManager() {
    loadTransactions();
}

TransactionManager::~TransactionManager() {
    saveTransactions();
}

void TransactionManager::addTransaction(const std::shared_ptr<Transaction>& transaction) {
    transactions.push_back(transaction);
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getAllTransactions() const {
    return transactions;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Transaction>> result;

    // Lambda function to check if transaction matches the category
    auto matchesCategory = [&category](const std::shared_ptr<Transaction>& t) {
        return t->getCategory() == category;
        };

    // Copy transactions that match the category to the result vector
    std::copy_if(transactions.begin(), transactions.end(),
        std::back_inserter(result), matchesCategory);

    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByType(TransactionType type) const {
    std::vector<std::shared_ptr<Transaction>> result;

    // Lambda function to check if transaction matches the type
    auto matchesType = [&type](const std::shared_ptr<Transaction>& t) {
        return t->getType() == type;
        };

    // Copy transactions that match the type to the result vector
    std::copy_if(transactions.begin(), transactions.end(),
        std::back_inserter(result), matchesType);

    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByDateRange(time_t startDate, time_t endDate) const {
    std::vector<std::shared_ptr<Transaction>> result;

    // Lambda function to check if transaction's date is within the range (inclusive)
    auto isInDateRange = [startDate, endDate](const std::shared_ptr<Transaction>& t) {
        return DateUtils::isDateInRange(t->getDate(), startDate, endDate);
        };

    // Copy transactions that are within the date range to the result vector
    std::copy_if(transactions.begin(), transactions.end(),
        std::back_inserter(result), isInDateRange);

    return result;
}

std::map<std::string, std::vector<std::shared_ptr<Transaction>>> TransactionManager::getTransactionsByMonth() const {
    std::map<std::string, std::vector<std::shared_ptr<Transaction>>> monthlyTransactions;

    for (const auto& transaction : transactions) {
        // Get month key in YYYY-MM format
        std::string monthKey = transaction->getMonthKey();

        // Add transaction to the appropriate month's vector
        monthlyTransactions[monthKey].push_back(transaction);
    }

    return monthlyTransactions;
}

std::map<std::string, std::tuple<double, double, double>> TransactionManager::calculateMonthlySummary() const {
    std::map<std::string, std::tuple<double, double, double>> summary;

    // Get transactions grouped by month
    auto monthlyTransactions = getTransactionsByMonth();

    // Process each month
    for (const auto& [month, transactionList] : monthlyTransactions) {
        double totalIncome = 0.0;
        double totalExpenses = 0.0;

        // Calculate total income and expenses for the month
        for (const auto& transaction : transactionList) {
            if (transaction->getType() == TransactionType::INCOME) {
                totalIncome += transaction->getAmount();
            }
            else {
                totalExpenses += transaction->getAmount();
            }
        }

        // Calculate net amount (income - expenses)
        double netAmount = totalIncome - totalExpenses;

        // Store the summary for this month
        summary[month] = std::make_tuple(totalIncome, totalExpenses, netAmount);
    }

    return summary;
}

void TransactionManager::saveTransactions() {
    saveTransactionsToFile("data/transactions.csv");
}

void TransactionManager::loadTransactions() {
    loadTransactionsFromFile("data/transactions.csv");
}

bool TransactionManager::saveTransactionsToFile(const std::string& filename) const {
    try {
        // Make sure the data directory exists
        FileUtils::createDirectoryIfNotExists("data");

        // Open the file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
            return false;
        }

        // Write header
        file << "Amount,Date,Category,Type\n";

        // Write each transaction
        for (const auto& transaction : transactions) {
            file << transaction->getAmount() << ","
                << transaction->getDate() << ","
                << transaction->getCategory() << ","
                << (transaction->getType() == TransactionType::INCOME ? "INCOME" : "EXPENSE")
                << "\n";
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving transactions: " << e.what() << std::endl;
        return false;
    }
}

bool TransactionManager::loadTransactionsFromFile(const std::string& filename) {
    try {
        // Clear existing transactions
        transactions.clear();

        // Check if the file exists
        if (!FileUtils::fileExists(filename)) {
            // It's okay if the file doesn't exist yet
            return true;
        }

        // Open the file for reading
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for reading" << std::endl;
            return false;
        }

        // Skip header line
        std::string line;
        std::getline(file, line);

        // Read each transaction line
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string amountStr, dateStr, category, typeStr;

            // Parse CSV values
            std::getline(iss, amountStr, ',');
            std::getline(iss, dateStr, ',');
            std::getline(iss, category, ',');
            std::getline(iss, typeStr, ',');

            // Convert to appropriate types
            double amount = std::stod(amountStr);
            time_t date = std::stoll(dateStr);
            TransactionType type = (typeStr == "INCOME") ? TransactionType::INCOME : TransactionType::EXPENSE;

            // Create and add the transaction
            auto transaction = std::make_shared<Transaction>(amount, date, category, type);
            transactions.push_back(transaction);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading transactions: " << e.what() << std::endl;
        return false;
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