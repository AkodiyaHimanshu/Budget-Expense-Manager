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
        // Skip header line if it exists
        std::getline(file, line);

        int lineNumber = 1; // Start from 1 for the header

        // Read data lines
        while (std::getline(file, line)) {
            lineNumber++;
            result.totalLines++;

            if (line.empty()) continue;

            try {
                auto transaction = parseTransactionLine(line);
                if (transaction) {
                    result.transactions.push_back(transaction);
                }
            }
            catch (const std::exception& e) {
                result.errors.push_back({ lineNumber, e.what() });
                result.failedLines.push_back({ lineNumber, line });
            }
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

        // Parse CSV fields
        std::getline(ss, amountStr, ',');
        std::getline(ss, dateStr, ',');
        std::getline(ss, category, ',');
        std::getline(ss, typeStr, ',');

        // Convert and validate fields
        double amount;
        time_t date;
        TransactionType type;

        try {
            amount = std::stod(amountStr);

            // Parse ISO date format (YYYY-MM-DDThh:mm:ss)
            struct tm tm = {};
            std::istringstream dateStream(dateStr);
            dateStream >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
            if (dateStream.fail()) {
                throw std::invalid_argument("Invalid date format");
            }
            date = mktime(&tm);

            // Parse transaction type
            int typeInt = std::stoi(typeStr);
            if (typeInt == 0) {
                type = TransactionType::INCOME;
            }
            else if (typeInt == 1) {
                type = TransactionType::EXPENSE;
            }
            else {
                throw std::invalid_argument("Invalid transaction type");
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