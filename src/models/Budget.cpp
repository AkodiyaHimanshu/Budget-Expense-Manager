#include "../../include/models/Budget.h"
#include <iostream>

// Default constructor
Budget::Budget() : category(""), yearMonth(""), limitAmount(0.0) {}

// Parameterized constructor
Budget::Budget(const std::string& category, const std::string& yearMonth, double limitAmount)
    : category(category), yearMonth(yearMonth), limitAmount(limitAmount) {
}

// Getters and Setters
std::string Budget::getCategory() const {
    return category;
}

void Budget::setCategory(const std::string& category) {
    this->category = category;
}

std::string Budget::getYearMonth() const {
    return yearMonth;
}

void Budget::setYearMonth(const std::string& yearMonth) {
    this->yearMonth = yearMonth;
}

double Budget::getLimitAmount() const {
    return limitAmount;
}

void Budget::setLimitAmount(double limitAmount) {
    if (limitAmount < 0) {
        throw std::invalid_argument("Budget limit amount cannot be negative");
    }
    this->limitAmount = limitAmount;
}

// Format the limit amount with "$" symbol and 2 decimal places
std::string Budget::getFormattedAmount() const {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << limitAmount;
    return oss.str();
}

// Get a displayable string representation
std::string Budget::getDisplayString() const {
    return "Category: " + category + ", Month: " + yearMonth + ", Limit: " + getFormattedAmount();
}

// Static method to create a valid year-month string
std::string Budget::createYearMonthString(int year, int month) {
    // Validate month value
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }

    // Validate year (basic sanity check)
    if (year < 1900 || year > 2100) {
        throw std::invalid_argument("Year must be between 1900 and 2100");
    }

    std::ostringstream oss;
    oss << year << "-" << std::setw(2) << std::setfill('0') << month;
    return oss.str();
}