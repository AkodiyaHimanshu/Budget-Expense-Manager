#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <stdexcept>
#include <regex>

class DateUtils {
public:
    /**
     * Validates a string in YYYY-MM format.
     *
     * @param yearMonth The string to validate in YYYY-MM format
     * @param minYear Minimum acceptable year (default: 1900)
     * @param maxYear Maximum acceptable year (default: 2100)
     * @throws std::invalid_argument if the format is invalid or the year/month is out of range
     * @return true if the format is valid, otherwise throws an exception
     */
    static bool validateYearMonth(const std::string& yearMonth, int minYear = 1900, int maxYear = 2100) {
        // Validate basic format (YYYY-MM)
        if (yearMonth.length() != 7 || yearMonth[4] != '-') {
            throw std::invalid_argument("Month format should be YYYY-MM");
        }

        // Parse year and month from input
        int year, month;
        try {
            year = std::stoi(yearMonth.substr(0, 4));
            month = std::stoi(yearMonth.substr(5, 2));
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Invalid year or month format");
        }

        // Validate year range
        if (year < minYear || year > maxYear) {
            throw std::invalid_argument("Year must be between " + std::to_string(minYear) +
                " and " + std::to_string(maxYear));
        }

        // Validate month value
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Month must be between 1 and 12");
        }

        return true;
    }
};

#endif // DATE_UTILS_H