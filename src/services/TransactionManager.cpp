#include "../../include/services/TransactionManager.h"
#include <algorithm>

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