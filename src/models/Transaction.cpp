#include "../../include/models/Transaction.h"

// Constructors
Transaction::Transaction()
    : amount(0.0), date(time(nullptr)), category("Uncategorized"),
    type(TransactionType::EXPENSE), cachedMonthKey("") {
    updateMonthKeyCache();
}

Transaction::Transaction(double amount, time_t date, const std::string& category, TransactionType type)
    : amount(amount), date(date), category(category), type(type), cachedMonthKey("") {
    updateMonthKeyCache();
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
    // Date changed, so we need to update the cached month key
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

// Utility methods
std::string Transaction::getFormattedDate() const {
    struct tm* timeinfo = localtime(&date);
    std::ostringstream oss;
    // Using ISO-8601 format (YYYY-MM-DDThh:mm:ss) for better compatibility with spreadsheets and parsers
    oss << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S");
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

// Helper method to update the cached month key
void Transaction::updateMonthKeyCache() const {
    struct tm* timeinfo = localtime(&date);
    std::ostringstream oss;
    oss << std::setfill('0') << (timeinfo->tm_year + 1900) << "-"
        << std::setw(2) << (timeinfo->tm_mon + 1);
    cachedMonthKey = oss.str();
}

// Get month key in YYYY-MM format (uses caching to avoid repeated localtime() calls)
std::string Transaction::getMonthKey() const {
    // If the cached month key is empty, generate it
    if (cachedMonthKey.empty()) {
        updateMonthKeyCache();
    }
    return cachedMonthKey;
}