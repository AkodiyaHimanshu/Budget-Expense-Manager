#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include "../../include/models/Transaction.h"

namespace fs = std::filesystem;

class FileUtils {
public:
    /**
     * Loads transactions from a CSV file.
     *
     * @param filePath Path to the CSV file containing transaction data
     * @return Vector of shared pointers to Transaction objects
     * @throws std::runtime_error if file can't be opened or data is invalid
     */
    static std::vector<std::shared_ptr<Transaction>> loadTransactionsFromCSV(const std::string& filePath) {
        std::vector<std::shared_ptr<Transaction>> transactions;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
        // Skip header line if it exists
        std::getline(file, line);

        // Read data lines
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            try {
                auto transaction = parseTransactionLine(line);
                if (transaction) {
                    transactions.push_back(transaction);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << std::endl;
                std::cerr << "Reason: " << e.what() << std::endl;
                // Continue processing the rest of the file
            }
        }

        return transactions;
    }

    /**
     * Saves transactions to a CSV file.
     *
     * @param transactions Vector of Transaction objects to save
     * @param filePath Path where the CSV file should be saved
     * @throws std::runtime_error if file can't be written
     */
    static void saveTransactionsToCSV(
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

        // Write data
        for (const auto& transaction : transactions) {
            file << transaction->getAmount() << ","
                << transaction->getFormattedDate() << ","
                << transaction->getCategory() << ","
                << static_cast<int>(transaction->getType()) << "\n";
        }

        file.close();
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