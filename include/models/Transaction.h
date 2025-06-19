#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * Enumeration defining transaction types
 */
enum class TransactionType {
    INCOME,
    EXPENSE
};

/**
 * Class representing a financial transaction
 *
 * Stores details about a single financial transaction including
 * amount, date, category, and type (income/expense)
 */
class Transaction {
private:
    double amount;
    time_t date;
    std::string category;
    TransactionType type;

    // Cached YYYY-MM string to avoid repeated localtime() calls
    mutable std::string cachedMonthKey;

    /**
     * Updates the cached month key when date changes
     */
    void updateMonthKeyCache() const;

public:
    /**
     * Default constructor
     */
    Transaction();

    /**
     * Parameterized constructor
     *
     * @param amount The transaction amount
     * @param date The transaction date as time_t
     * @param category The transaction category
     * @param type The transaction type (INCOME or EXPENSE)
     */
    Transaction(double amount, time_t date, const std::string& category,
        TransactionType type);

    // Core getters and setters
    double getAmount() const;
    void setAmount(double amount);
    time_t getDate() const;
    void setDate(time_t date);
    std::string getCategory() const;
    void setCategory(const std::string& category);
    TransactionType getType() const;
    void setType(TransactionType type);

    // Display and formatting methods

    /**
     * Gets the formatted date string (YYYY-MM-DD)
     *
     * @return Formatted date string
     */
    std::string getFormattedDate() const;

    /**
     * Gets the formatted amount string with currency symbol
     *
     * @return Formatted amount string
     */
    std::string getFormattedAmount() const;

    /**
     * Gets the transaction type as a string
     *
     * @return "Income" or "Expense"
     */
    std::string getTypeAsString() const;

    /**
     * Gets a complete display string for the transaction
     *
     * @return Formatted display string with all transaction details
     */
    std::string getDisplayString() const;

    /**
     * Gets the month key (YYYY-MM) for grouping by month
     * Uses caching to avoid repeated date calculations
     *
     * @return Month key in YYYY-MM format
     */
    std::string getMonthKey() const;
};

#endif // TRANSACTION_H