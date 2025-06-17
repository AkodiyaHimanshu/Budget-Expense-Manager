#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <utility>
#include "../../include/models/Transaction.h"

namespace fs = std::filesystem;

/**
 * Represents the result of loading transactions from a CSV file.
 * Contains both successfully loaded transactions and any errors that occurred.
 */
struct LoadResult {
    // Successfully loaded transactions
    std::vector<std::shared_ptr<Transaction>> transactions;

    // Errors encountered during loading (line number and error message pairs)
    std::vector<std::pair<int, std::string>> errors;

    // Original content of lines that failed to parse
    std::vector<std::pair<int, std::string>> failedLines;

    // Total number of lines processed (excluding header)
    int totalLines = 0;

    // Quick access to number of transactions loaded and errors encountered
    size_t getSuccessCount() const { return transactions.size(); }
    size_t getErrorCount() const { return errors.size(); }

    // Check if there were any errors during loading
    bool hasErrors() const { return !errors.empty(); }

    // Get a formatted summary of the loading process
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Processed " << totalLines << " lines: ";
        oss << getSuccessCount() << " transactions loaded successfully, ";
        oss << getErrorCount() << " errors encountered.";
        return oss.str();
    }

    // Get detailed error report
    std::string getErrorReport() const {
        if (!hasErrors()) return "No errors encountered.";

        std::ostringstream oss;
        oss << "Encountered " << getErrorCount() << " errors while loading:\n";
        for (size_t i = 0; i < errors.size(); ++i) {
            oss << "Line " << errors[i].first << ": " << errors[i].second << "\n";
            oss << "  Content: \"" << failedLines[i].second << "\"\n";
        }
        return oss.str();
    }
};

class FileUtils {
public:
    /**
     * Loads transactions from a CSV file.
     *
     * @param filePath Path to the CSV file containing transaction data
     * @return LoadResult containing successfully loaded transactions and any errors
     * @throws std::runtime_error if file can't be opened
     */
    static LoadResult loadTransactionsFromCSV(const std::string& filePath) {
        LoadResult result;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
        int lineNumber = 0;
        bool headerProcessed = false;

        // Process the file line by line
        while (std::getline(file, line)) {
            lineNumber++;

            // Skip empty lines
            if (line.empty()) {
                std::cout << "Line " << lineNumber << ": Empty line skipped." << std::endl;
                continue;
            }

            // Process header line (first non-empty line)
            if (!headerProcessed) {
                // Validate header format
                if (line != "Amount,Date,Category,Type") {
                    std::cout << "Warning: Header line doesn't match expected format." << std::endl;
                    std::cout << "  Expected: Amount,Date,Category,Type" << std::endl;
                    std::cout << "  Found: " << line << std::endl;
                }
                headerProcessed = true;
                continue;
            }

            result.totalLines++;

            // Check for obviously malformed CSV (quick check before detailed parsing)
            int commaCount = 0;
            for (char c : line) {
                if (c == ',') commaCount++;
            }

            if (commaCount < 3) {
                std::string error = "Insufficient fields (need at least 4 fields, found " +
                    std::to_string(commaCount + 1) + ")";
                std::cout << "Line " << lineNumber << ": " << error << " - Skipped" << std::endl;
                result.errors.push_back({ lineNumber, error });
                result.failedLines.push_back({ lineNumber, line });
                continue;
            }

            // Attempt to parse the line into a transaction
            try {
                auto transaction = parseTransactionLine(line);
                if (transaction) {
                    result.transactions.push_back(transaction);
                }
            }
            catch (const std::exception& e) {
                std::string error = e.what();
                std::cout << "Line " << lineNumber << ": " << error << " - Skipped" << std::endl;
                result.errors.push_back({ lineNumber, error });
                result.failedLines.push_back({ lineNumber, line });
            }
        }

        // If no header was found, that's unusual
        if (!headerProcessed) {
            std::cout << "Warning: No header line found in CSV file." << std::endl;
        }

        return result;
    }

    /**
     * Saves transactions to a CSV file.
     *
     * @param transactions Vector of Transaction objects to save
     * @param filePath Path where the CSV file should be saved
     * @throws std::runtime_error if file can't be written
     * @return Number of transactions written
     */
    static int saveTransactionsToCSV(
        const std::vector<std::shared_ptr<Transaction>>& transactions,
        const std::string& filePath
    ) {
        // Ensure the directory exists
        fs::path path(filePath);
        fs::create_directories(path.parent_path());

        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filePath);
        }

        // Write header
        file << "Amount,Date,Category,Type\n";

        int count = 0;
        // Write data
        for (const auto& transaction : transactions) {
            file << transaction->getAmount() << ","
                << transaction->getFormattedDate() << ","
                << transaction->getCategory() << ","
                << static_cast<int>(transaction->getType()) << "\n";

            count++;
        }

        file.close();
        return count;
    }

private:
    /**
     * Parses a single line from the CSV file and creates a Transaction object.
     *
     * @param line A line from the CSV file in the format: amount,date,category,type
     * @return Shared pointer to a Transaction object
     * @throws std::invalid_argument if line format is invalid
     */
    static std::shared_ptr<Transaction> parseTransactionLine(const std::string& line) {
        std::istringstream ss(line);
        std::string amountStr, dateStr, category, typeStr;

        // Parse CSV fields and validate completeness
        if (!std::getline(ss, amountStr, ',')) {
            throw std::invalid_argument("Missing amount field");
        }
        if (!std::getline(ss, dateStr, ',')) {
            throw std::invalid_argument("Missing date field");
        }
        if (!std::getline(ss, category, ',')) {
            throw std::invalid_argument("Missing category field");
        }
        if (!std::getline(ss, typeStr, ',')) {
            throw std::invalid_argument("Missing transaction type field");
        }

        // Validate field content (check for empty fields)
        if (amountStr.empty()) {
            throw std::invalid_argument("Amount field is empty");
        }
        if (dateStr.empty()) {
            throw std::invalid_argument("Date field is empty");
        }
        if (category.empty()) {
            throw std::invalid_argument("Category field is empty");
        }
        if (typeStr.empty()) {
            throw std::invalid_argument("Transaction type field is empty");
        }

        // Convert and validate field values
        double amount;
        time_t date;
        TransactionType type;

        try {
            // Validate amount (must be a valid number)
            try {
                amount = std::stod(amountStr);
            }
            catch (const std::exception&) {
                throw std::invalid_argument("Amount '" + amountStr + "' is not a valid number");
            }

            // Validate date format (must be ISO: YYYY-MM-DDThh:mm:ss)
            struct tm tm = {};
            std::istringstream dateStream(dateStr);
            dateStream >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
            if (dateStream.fail()) {
                throw std::invalid_argument("Date '" + dateStr + "' is not in valid ISO format (YYYY-MM-DDThh:mm:ss)");
            }
            date = mktime(&tm);
            if (date == -1) {
                throw std::invalid_argument("Date '" + dateStr + "' could not be converted to a valid time");
            }

            // Validate transaction type (must be 0 or 1)
            int typeInt;
            try {
                typeInt = std::stoi(typeStr);
            }
            catch (const std::exception&) {
                throw std::invalid_argument("Transaction type '" + typeStr + "' is not a valid number");
            }

            if (typeInt == 0) {
                type = TransactionType::INCOME;
            }
            else if (typeInt == 1) {
                type = TransactionType::EXPENSE;
            }
            else {
                throw std::invalid_argument("Transaction type must be 0 (income) or 1 (expense), got: " + typeStr);
            }
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Error parsing transaction: " + std::string(e.what()));
        }

        // Create and return a new Transaction object
        return std::make_shared<Transaction>(amount, date, category, type);
    }
};

#endif // FILE_UTILS_H