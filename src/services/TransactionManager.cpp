#include "../../include/services/TransactionManager.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

void TransactionManager::addTransaction(std::shared_ptr<Transaction> transaction) {
    transactions.push_back(transaction);
}

const std::vector<std::shared_ptr<Transaction>>& TransactionManager::getAllTransactions() const {
    return transactions;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByType(TransactionType type) const {
    std::vector<std::shared_ptr<Transaction>> result;
    std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(result),
        [type](const std::shared_ptr<Transaction>& t) { return t->getType() == type; });
    return result;
}

std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Transaction>> result;
    std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(result),
        [&category](const std::shared_ptr<Transaction>& t) { return t->getCategory() == category; });
    return result;
}

double TransactionManager::calculateTotal(TransactionType type) const {
    double total = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction->getType() == type) {
            total += transaction->getAmount();
        }
    }
    return total;
}

// Calculate net amount (income - expenses) across all transactions
double TransactionManager::calculateNetTotal() const {
    double totalIncome = calculateTotal(TransactionType::INCOME);
    double totalExpenses = calculateTotal(TransactionType::EXPENSE);
    return calculateNetAmount(totalIncome, totalExpenses);
}

// Get transactions for a specific month (format: YYYY-MM)
std::vector<std::shared_ptr<Transaction>> TransactionManager::getTransactionsByMonth(const std::string& yearMonth) const {
    // Validate input format (YYYY-MM)
    if (yearMonth.length() != 7 || yearMonth[4] != '-') {
        throw std::invalid_argument("Month format should be YYYY-MM");
    }

    // Parse year and month from input
    int year, month;
    try {
        year = std::stoi(yearMonth.substr(0, 4));
        month = std::stoi(yearMonth.substr(5, 2));
    }
    catch (const std::exception& e) {
        throw std::invalid_argument("Invalid year or month format");
    }

    // Validate month value
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }

    std::vector<std::shared_ptr<Transaction>> monthlyTransactions;

    for (const auto& transaction : transactions) {
        time_t transactionDate = transaction->getDate();
        struct tm* timeinfo = localtime(&transactionDate);

        // Check if transaction is in the specified month and year
        if ((timeinfo->tm_year + 1900) == year && (timeinfo->tm_mon + 1) == month) {
            monthlyTransactions.push_back(transaction);
        }
    }

    return monthlyTransactions;
}

// Calculate monthly summary for a specific month (format: YYYY-MM)
MonthlySummary TransactionManager::calculateMonthlySummary(const std::string& yearMonth) const {
    MonthlySummary summary = { 0.0, 0.0, 0.0 }; // Initialize with zeros

    // Get all transactions for the specified month
    auto monthlyTransactions = getTransactionsByMonth(yearMonth);

    // Calculate income and expenses
    for (const auto& transaction : monthlyTransactions) {
        if (transaction->getType() == TransactionType::INCOME) {
            summary.totalIncome += transaction->getAmount();
        }
        else {
            summary.totalExpenses += transaction->getAmount();
        }
    }

    // Calculate net amount using the dedicated method
    summary.updateNetAmount();

    return summary;
}

// Group transactions by month and return monthly summaries
std::map<std::string, MonthlySummary> TransactionManager::getMonthlyTransactionSummaries() const {
    std::map<std::string, MonthlySummary> monthlySummaries;

    // First, aggregate all income and expenses by month
    for (const auto& transaction : transactions) {
        time_t date = transaction->getDate();
        struct tm* timeinfo = localtime(&date);

        // Create month key in YYYY-MM format
        std::ostringstream oss;
        oss << std::setfill('0') << (timeinfo->tm_year + 1900) << "-"
            << std::setw(2) << (timeinfo->tm_mon + 1);
        std::string monthKey = oss.str();

        // Create month entry if it doesn't exist
        if (monthlySummaries.find(monthKey) == monthlySummaries.end()) {
            monthlySummaries[monthKey] = { 0.0, 0.0, 0.0 };
        }

        // Update the monthly income or expense total
        if (transaction->getType() == TransactionType::INCOME) {
            monthlySummaries[monthKey].totalIncome += transaction->getAmount();
        }
        else {
            monthlySummaries[monthKey].totalExpenses += transaction->getAmount();
        }

        // Net amount will be calculated outside the loop
    }

    // Now calculate all net amounts in a separate pass using the dedicated method
    for (auto& [month, summary] : monthlySummaries) {
        summary.updateNetAmount();
    }

    return monthlySummaries;
}
