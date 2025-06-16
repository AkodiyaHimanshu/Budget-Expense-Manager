#include "../../include/services/TransactionManager.h"
#include "../../include/utils/DateUtils.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

void TransactionManager::addTransaction(std::shared_ptr<Transaction> transaction) {
    transactions.push_back(transaction);

    // Invalidate cache since data has changed
    cacheValid = false;
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
    // Validate the year-month format using the utility function
    DateUtils::validateYearMonth(yearMonth);

    // If cache is invalid, clear it
    if (!cacheValid) {
        clearCaches();
    }

    // Check if we have a cached result for this month
    auto cacheIt = monthlyTransactionsCache.find(yearMonth);
    if (cacheIt != monthlyTransactionsCache.end()) {
        // Cache hit - return the cached result
        return cacheIt->second;
    }

    // Cache miss - compute the result and cache it
    std::vector<std::shared_ptr<Transaction>> monthlyTransactions;

    for (const auto& transaction : transactions) {
        // Get the cached month key and compare directly with the input yearMonth
        if (transaction->getMonthKey() == yearMonth) {
            monthlyTransactions.push_back(transaction);
        }
    }

    // Store in cache for future requests
    monthlyTransactionsCache[yearMonth] = monthlyTransactions;

    return monthlyTransactions;
}

// Calculate monthly summary for a specific month (format: YYYY-MM)
MonthlySummary TransactionManager::calculateMonthlySummary(const std::string& yearMonth) const {
    // Validate the year-month format using the utility function
    DateUtils::validateYearMonth(yearMonth);

    // If cache is invalid, clear it
    if (!cacheValid) {
        clearCaches();
    }

    // Check if we have a cached result for this month
    auto cacheIt = monthlySummaryCache.find(yearMonth);
    if (cacheIt != monthlySummaryCache.end()) {
        // Cache hit - return the cached result
        return cacheIt->second;
    }

    // Cache miss - compute the result
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

    // Store in cache for future requests
    monthlySummaryCache[yearMonth] = summary;

    return summary;
}

// Group transactions by month and return monthly summaries
// Returns a const reference to avoid copying the entire map
const std::map<std::string, MonthlySummary>& TransactionManager::getMonthlyTransactionSummaries() const {
    // If the result is already cached and the cache is valid, we can return the cache directly
    if (cacheValid && !monthlySummaryCache.empty()) {
        return monthlySummaryCache;
    }

    // If cache is invalid, clear it
    if (!cacheValid) {
        clearCaches();
    }

    // We'll update the cache directly instead of creating a temporary map
    // This avoids an unnecessary copy when returning the result

    // First, aggregate all income and expenses by month
    for (const auto& transaction : transactions) {
        // Get the cached month key (YYYY-MM) from the transaction
        std::string monthKey = transaction->getMonthKey();

        // Create month entry in cache if it doesn't exist
        if (monthlySummaryCache.find(monthKey) == monthlySummaryCache.end()) {
            monthlySummaryCache[monthKey] = { 0.0, 0.0, 0.0 };
        }

        // Update the monthly income or expense total directly in the cache
        if (transaction->getType() == TransactionType::INCOME) {
            monthlySummaryCache[monthKey].totalIncome += transaction->getAmount();
        }
        else {
            monthlySummaryCache[monthKey].totalExpenses += transaction->getAmount();
        }
    }

    // Now calculate all net amounts in a separate pass using the dedicated method
    for (auto& [month, summary] : monthlySummaryCache) {
        summary.updateNetAmount();
    }

    return monthlySummaryCache;
}
