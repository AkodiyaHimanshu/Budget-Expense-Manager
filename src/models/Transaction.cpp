#include "../../include/models/Transaction.h"

Transaction::Transaction() :
    amount(0.0),
    date(std::time(nullptr)), // Current time
    category(""),
    type(TransactionType::EXPENSE),
    cachedMonthKey("") {
}

Transaction::Transaction(double amount, time_t date, const std::string& category, TransactionType type) :
    amount(amount),
    date(date),
    category(category),
    type(type),
    cachedMonthKey("") {
    // Initialize the month key cache
    updateMonthKeyCache();
}

void Transaction::updateMonthKeyCache() const {
    std::tm* timeInfo = std::localtime(&date);

    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y-%m");
    cachedMonthKey = ss.str();
}

double Transaction::getAmount() const {
    return amount;
}

void Transaction::setAmount(double amount) {
    this->amount = amount;
}

time_t Transaction::getDate() const {
    return date;
}

void Transaction::setDate(time_t date) {
    this->date = date;
    // Update month key cache when date changes
    updateMonthKeyCache();
}

std::string Transaction::getCategory() const {
    return category;
}

void Transaction::setCategory(const std::string& category) {
    this->category = category;
}

TransactionType Transaction::getType() const {
    return type;
}

void Transaction::setType(TransactionType type) {
    this->type = type;
}

std::string Transaction::getFormattedDate() const {
    std::tm* timeInfo = std::localtime(&date);

    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y-%m-%d");
    return ss.str();
}

std::string Transaction::getFormattedAmount() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << amount;

    if (type == TransactionType::INCOME) {
        return "$" + ss.str();
    }
    else {
        return "-$" + ss.str();
    }
}

std::string Transaction::getTypeAsString() const {
    return (type == TransactionType::INCOME) ? "Income" : "Expense";
}

std::string Transaction::getDisplayString() const {
    std::stringstream ss;
    ss << "[" << getFormattedDate() << "] ";
    ss << getTypeAsString() << ": ";
    ss << getCategory() << " - ";
    ss << getFormattedAmount();

    return ss.str();
}

std::string Transaction::getMonthKey() const {
    // Return the cached month key
    // This avoids repeated calls to localtime() which can be expensive
    if (cachedMonthKey.empty()) {
        updateMonthKeyCache();
    }

    return cachedMonthKey;
}