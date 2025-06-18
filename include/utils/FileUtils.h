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
#include <algorithm>  // For std::find_if
#include <cctype>     // For std::isspace
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
     * Checks if a file exists.
     *
     * @param filePath Path to the file to check
     * @return true if the file exists, false otherwise
     */
    static bool fileExists(const std::string& filePath) {
        return fs::exists(filePath) && fs::is_regular_file(filePath);
    }

    /**
     * Creates a directory if it doesn't exist.
     *
     * @param dirPath Path to the directory to create
     * @return true if the directory was created or already exists, false otherwise
     */
    static bool createDirectoryIfNotExists(const std::string& dirPath) {
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            return true; // Directory already exists
        }

        try {
            return fs::create_directories(dirPath);
        }
        catch (const fs::filesystem_error&) {
            return false;
        }
    }

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

        // Skip header line if it exists
        std::getline(file, line);

        // Process all remaining lines
        while (std::getline(file, line)) {
            lineNumber++;
            result.totalLines++;

            // Skip empty lines
            if (line.empty() || std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); })) {
                continue;
            }

            // Parse line into transaction
            try {
                std::stringstream ss(line);
                std::string amountStr, dateStr, category, typeStr;

                // Parse CSV fields
                std::getline(ss, amountStr, ',');
                std::getline(ss, dateStr, ',');
                std::getline(ss, category, ',');
                std::getline(ss, typeStr, ',');

                // Trim whitespace from fields
                amountStr.erase(0, amountStr.find_first_not_of(" \t"));
                amountStr.erase(amountStr.find_last_not_of(" \t") + 1);
                dateStr.erase(0, dateStr.find_first_not_of(" \t"));
                dateStr.erase(dateStr.find_last_not_of(" \t") + 1);
                category.erase(0, category.find_first_not_of(" \t"));
                category.erase(category.find_last_not_of(" \t") + 1);
                typeStr.erase(0, typeStr.find_first_not_of(" \t"));
                typeStr.erase(typeStr.find_last_not_of(" \t") + 1);

                // Convert to appropriate types
                double amount = std::stod(amountStr);
                time_t date = std::stoll(dateStr);
                TransactionType type;

                // Convert type string to enum (case insensitive)
                std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::toupper);
                if (typeStr == "INCOME") {
                    type = TransactionType::INCOME;
                }
                else if (typeStr == "EXPENSE") {
                    type = TransactionType::EXPENSE;
                }
                else {
                    throw std::invalid_argument("Unknown transaction type: " + typeStr);
                }

                // Create and add the transaction
                auto transaction = std::make_shared<Transaction>(amount, date, category, type);
                result.transactions.push_back(transaction);
            }
            catch (const std::exception& e) {
                // Record error and failed line
                result.errors.emplace_back(lineNumber, e.what());
                result.failedLines.emplace_back(lineNumber, line);
            }
        }

        // Close the file
        file.close();
        return result;
    }

    /**
     * Saves transactions to a CSV file.
     *
     * @param transactions Vector of transactions to save
     * @param filePath Path to save the transactions CSV file
     * @return Number of transactions successfully saved
     * @throws std::runtime_error if file can't be opened
     */
    static int saveTransactionsToCSV(const std::vector<std::shared_ptr<Transaction>>& transactions, const std::string& filePath) {
        // Make sure parent directory exists
        fs::path path(filePath);
        createDirectoryIfNotExists(path.parent_path().string());

        // Open file for writing
        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filePath);
        }

        // Write header
        file << "Amount,Date,Category,Type\n";

        // Write transactions
        int savedCount = 0;
        for (const auto& transaction : transactions) {
            try {
                file << transaction->getAmount() << ","
                    << transaction->getDate() << ","
                    << transaction->getCategory() << ","
                    << (transaction->getType() == TransactionType::INCOME ? "INCOME" : "EXPENSE")
                    << "\n";

                savedCount++;
            }
            catch (const std::exception& e) {
                // Log error but continue with other transactions
                std::cerr << "Error saving transaction: " << e.what() << std::endl;
            }
        }

        // Close file
        file.close();
        return savedCount;
    }
};

#endif // FILE_UTILS_H