#include "../../include/models/Transaction.h"

// Constructors
Transaction::Transaction() : amount(0.0), date(time(nullptr)), category("Uncategorized"), type(TransactionType::EXPENSE) {}

Transaction::Transaction(double amount, time_t date, const std::string& category, TransactionType type)
    : amount(amount), date(date), category(category), type(type) {
}

// Getters and Setters
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

// Utility methods
std::string Transaction::getFormattedDate() const {
    struct tm* timeinfo = localtime(&date);
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Transaction::getFormattedAmount() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    return (type == TransactionType::INCOME ? "+" : "-") + oss.str();
}

std::string Transaction::getTypeAsString() const {
    return (type == TransactionType::INCOME) ? "Income" : "Expense";
}

std::string Transaction::getDisplayString() const {
    std::ostringstream oss;
    oss << "[" << getFormattedDate() << "] "
        << getTypeAsString() << ": "
        << getFormattedAmount() << " (" << category << ")";
    return oss.str();
}