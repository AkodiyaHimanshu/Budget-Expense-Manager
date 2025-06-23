#pragma once

#include <string>
#include <ctime>
#include <memory>
#include <iomanip>
#include <sstream>

class Budget {
private:
    std::string category;     // Category name (e.g., "Food", "Entertainment")
    std::string yearMonth;    // Year and month in YYYY-MM format (e.g., "2023-06")
    double limitAmount;       // Budget limit amount for the specified category and month

public:
    // Constructors
    Budget();
    Budget(const std::string& category, const std::string& yearMonth, double limitAmount);

    // Getters and Setters
    std::string getCategory() const;
    void setCategory(const std::string& category);

    std::string getYearMonth() const;
    void setYearMonth(const std::string& yearMonth);

    double getLimitAmount() const;
    void setLimitAmount(double limitAmount);

    // Utility methods
    std::string getFormattedAmount() const;
    std::string getDisplayString() const;

    // Static method to create a valid year-month string
    static std::string createYearMonthString(int year, int month);
};