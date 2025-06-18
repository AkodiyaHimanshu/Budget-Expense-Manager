#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>

// Include appropriate headers for directory operations
#ifdef _WIN32
#include <direct.h>  // For Windows (provides mkdir)
#else
#include <sys/stat.h>  // For UNIX/Linux (provides mkdir)
#include <unistd.h>    // Additional UNIX/Linux utilities
#endif

#include "../models/Transaction.h"
#include "DateUtils.h"

class FileUtils {
public:
    /**
     * Result structure for storing transactions loaded from a file
     * along with any errors encountered during loading
     */
    struct LoadResult {
        std::vector<std::shared_ptr<Transaction>> transactions;
        std::vector<std::pair<int, std::string>> errors;
        std::vector<std::pair<int, std::string>> failedLines;
        int totalLines = 0;

        bool hasErrors() const { return !errors.empty(); }
        size_t getErrorCount() const { return errors.size(); }
    };

    /**
     * Checks if a file exists
     *
     * @param filePath The path to the file to check
     * @return true if the file exists, false otherwise
     */
    static bool fileExists(const std::string& filePath) {
        struct stat buffer;
        return (stat(filePath.c_str(), &buffer) == 0);
    }

    /**
     * Creates a directory if it doesn't already exist
     *
     * @param dirPath The path to the directory to create
     * @return true if the directory was created or already exists, false otherwise
     */
    static bool createDirectoryIfNotExists(const std::string& dirPath) {
        // If directory already exists, return true
        if (fileExists(dirPath)) {
            return true;
        }

        // Create directory (system-dependent)
#ifdef _WIN32
        int result = _mkdir(dirPath.c_str());  // Note: Windows uses _mkdir
#else
        int result = mkdir(dirPath.c_str(), 0777);  // Unix/Linux uses mkdir with permissions
#endif

        return (result == 0);
    }

    /**
     * Loads transactions from a CSV file
     *
     * @param filePath The path to the CSV file
     * @return LoadResult containing loaded transactions and any errors
     */
    static LoadResult loadTransactionsFromCSV(const std::string& filePath) {
        LoadResult result;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            result.errors.push_back({ 0, "Failed to open file: " + filePath });
            return result;
        }

        std::string line;
        int lineNum = 0;

        // Read and process each line
        while (std::getline(file, line)) {
            lineNum++;
            result.totalLines++;

            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            try {
                // Parse CSV line: amount,date,category,type
                std::stringstream ss(line);
                std::string amountStr, dateStr, category, typeStr;

                // Read CSV fields
                if (!std::getline(ss, amountStr, ',') ||
                    !std::getline(ss, dateStr, ',') ||
                    !std::getline(ss, category, ',') ||
                    !std::getline(ss, typeStr)) {
                    throw std::runtime_error("Invalid CSV format - missing fields");
                }

                // Parse transaction data
                double amount = std::stod(amountStr);
                time_t date = DateUtils::stringToTime(dateStr);
                TransactionType type = (typeStr == "INCOME") ?
                    TransactionType::INCOME :
                    TransactionType::EXPENSE;

                // Create and add transaction
                auto transaction = std::make_shared<Transaction>(amount, date, category, type);
                result.transactions.push_back(transaction);
            }
            catch (const std::exception& e) {
                result.errors.push_back({ lineNum, std::string("Error parsing line: ") + e.what() });
                result.failedLines.push_back({ lineNum, line });
            }
        }

        file.close();
        return result;
    }

    /**
     * Saves transactions to a CSV file
     *
     * @param transactions The transactions to save
     * @param filePath The path to the CSV file
     * @return The number of transactions saved
     */
    static int saveTransactionsToCSV(const std::vector<std::shared_ptr<Transaction>>& transactions,
        const std::string& filePath) {
        std::ofstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filePath);
        }

        int count = 0;

        // Write each transaction as a CSV line
        for (const auto& t : transactions) {
            file << t->getAmount() << ","
                << DateUtils::timeToString(t->getDate()) << ","
                << t->getCategory() << ","
                << (t->getType() == TransactionType::INCOME ? "INCOME" : "EXPENSE")
                << std::endl;

            count++;
        }

        file.close();
        return count;
    }
};

#endif // FILE_UTILS_H