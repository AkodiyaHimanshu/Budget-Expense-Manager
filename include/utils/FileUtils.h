#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno> 

// Include appropriate headers for directory operations
#ifdef _WIN32
#include <direct.h>  // For Windows (provides mkdir)
#else
#include <sys/stat.h>  // For UNIX/Linux (provides mkdir)
#include <unistd.h>    // Additional UNIX/Linux utilities
#endif

#include "../models/Transaction.h"
#include "DateUtils.h"
#include <sys/stat.h>

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

    static std::vector<std::vector<std::string>> readCSV(const std::string& filePath) {
        std::vector<std::vector<std::string>> data;

        // Check if file exists first
        if (!fileExists(filePath)) {
            // Return empty data if file doesn't exist
            return data;
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << " for reading." << std::endl;
            return data;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Parse the CSV line
            std::vector<std::string> row = split(line, ',');
            data.push_back(row);
        }

        file.close();
        return data;
    }

    static bool writeCSV(const std::string& filePath, const std::vector<std::vector<std::string>>& data) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << " for writing." << std::endl;
            return false;
        }

        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                const std::string& field = row[i];

                // Check if field contains special characters that need quoting
                bool needsQuotes = field.find(',') != std::string::npos ||
                    field.find('"') != std::string::npos ||
                    field.find('\n') != std::string::npos;

                if (needsQuotes) {
                    // Add quotes and escape internal quotes by doubling them
                    file << '"';
                    for (char c : field) {
                        if (c == '"') {
                            file << '"' << '"'; // Escape quotes with double quotes
                        }
                        else {
                            file << c;
                        }
                    }
                    file << '"';
                }
                else {
                    file << field;
                }

                if (i < row.size() - 1) {
                    file << ',';
                }
            }
            file << '\n';
        }

        file.close();
        return true;
    }

    static bool createDirectories(const std::string& dirPath) {
        // Handle empty path
        if (dirPath.empty()) {
            return false;
        }

        // Create each directory in the path
        std::string path = dirPath;

        // Replace backslashes with forward slashes for consistency
        for (char& c : path) {
            if (c == '\\') {
                c = '/';
            }
        }

        // Add trailing slash if not present
        if (path.back() != '/') {
            path += '/';
        }

        // Create each directory level
        size_t pos = 0;
        while ((pos = path.find_first_of('/', pos + 1)) != std::string::npos) {
            std::string subdir = path.substr(0, pos);

            // Skip empty subdirectory
            if (subdir.empty()) {
                continue;
            }

#ifdef _WIN32
            // Windows implementation
            if (_mkdir(subdir.c_str()) != 0 && errno != EEXIST) {
                return false;
            }
#else
            // Unix implementation
            if (mkdir(subdir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 && errno != EEXIST) {
                return false;
            }
#endif
        }

        return true;
    }

    static std::string getFileExtension(const std::string& filePath) {
        size_t dotPos = filePath.find_last_of('.');
        if (dotPos == std::string::npos || dotPos == 0) {
            return "";
        }
        return filePath.substr(dotPos + 1);
    }

    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        // Simple CSV parser that handles quoted fields (which might contain the delimiter)
        bool inQuotes = false;
        std::string field;

        for (char c : str) {
            // Toggle quote state
            if (c == '"') {
                inQuotes = !inQuotes;
                continue;  // Don't include the quotes in the result
            }

            // If delimiter found and not in quotes, add field to result
            if (c == delimiter && !inQuotes) {
                tokens.push_back(field);
                field.clear();
                continue;
            }

            // Add character to current field
            field += c;
        }

        // Add the last field
        tokens.push_back(field);

        return tokens;
    }
};

#endif // FILE_UTILS_H