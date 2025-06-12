#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

enum class TransactionType {
    INCOME,
    EXPENSE
};

class Transaction {
private:
    double amount;
    time_t date;
    std::string category;
    TransactionType type;

public:
    // Constructors
    Transaction();
    Transaction(double amount, time_t date, const std::string& category,
        TransactionType type);

    // Getters and Setters
    double getAmount() const;
    void setAmount(double amount);

    time_t getDate() const;
    void setDate(time_t date);

    std::string getCategory() const;
    void setCategory(const std::string& category);

    TransactionType getType() const;
    void setType(TransactionType type);

    // Utility methods
    std::string getFormattedDate() const;
    std::string getFormattedAmount() const;
    std::string getTypeAsString() const;
    std::string getDisplayString() const;
};

#endif // TRANSACTION_H