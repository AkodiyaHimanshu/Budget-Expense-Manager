#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <stdexcept>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>

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

    /**
     * Validates a date string in YYYY-MM-DD format.
     *
     * @param dateStr The string to validate in YYYY-MM-DD format
     * @param minYear Minimum acceptable year (default: 1900)
     * @param maxYear Maximum acceptable year (default: 2100)
     * @throws std::invalid_argument if the format is invalid or the date is out of range
     * @return true if the format is valid, otherwise throws an exception
     */
    static bool validateDateString(const std::string& dateStr, int minYear = 1900, int maxYear = 2100) {
        // Validate basic format (YYYY-MM-DD)
        if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
            throw std::invalid_argument("Date format should be YYYY-MM-DD");
        }

        // Parse year, month, and day from input
        int year, month, day;
        try {
            year = std::stoi(dateStr.substr(0, 4));
            month = std::stoi(dateStr.substr(5, 2));
            day = std::stoi(dateStr.substr(8, 2));
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Invalid date format");
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

        // Validate day value based on month
        int daysInMonth = 31; // Default for most months

        if (month == 4 || month == 6 || month == 9 || month == 11) {
            daysInMonth = 30;
        }
        else if (month == 2) {
            // Check for leap year
            bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            daysInMonth = isLeapYear ? 29 : 28;
        }

        if (day < 1 || day > daysInMonth) {
            throw std::invalid_argument("Invalid day for the given month");
        }

        return true;
    }

    /**
     * Converts a date string in YYYY-MM-DD format to time_t.
     *
     * @param dateStr The string to convert in YYYY-MM-DD format
     * @return time_t value representing the date at 00:00:00
     * @throws std::invalid_argument if the date string is invalid
     */
    static time_t stringToTime(const std::string& dateStr) {
        // Validate the date string format first
        validateDateString(dateStr);

        // Parse date components
        int year = std::stoi(dateStr.substr(0, 4));
        int month = std::stoi(dateStr.substr(5, 2));
        int day = std::stoi(dateStr.substr(8, 2));

        // Create tm struct and fill with date components
        struct tm timeinfo = {};
        timeinfo.tm_year = year - 1900; // Years since 1900
        timeinfo.tm_mon = month - 1;    // Months since January (0-11)
        timeinfo.tm_mday = day;         // Day of month (1-31)
        timeinfo.tm_hour = 0;           // Set to beginning of the day
        timeinfo.tm_min = 0;
        timeinfo.tm_sec = 0;

        // Convert to time_t (seconds since epoch)
        time_t time = mktime(&timeinfo);
        if (time == -1) {
            throw std::invalid_argument("Failed to convert date string to time");
        }

        return time;
    }

    /**
     * Converts time_t to a date string in YYYY-MM-DD format.
     *
     * @param time The time_t value to convert
     * @return Date string in YYYY-MM-DD format
     */
    static std::string timeToDateString(time_t time) {
        struct tm* timeinfo = localtime(&time);
        std::ostringstream oss;
        oss << std::setfill('0')
            << (timeinfo->tm_year + 1900) << "-"
            << std::setw(2) << (timeinfo->tm_mon + 1) << "-"
            << std::setw(2) << timeinfo->tm_mday;
        return oss.str();
    }

    /**
     * Checks if a date (as time_t) is within a given date range (inclusive).
     *
     * @param date The date to check
     * @param startDate The start of the range (inclusive)
     * @param endDate The end of the range (inclusive)
     * @return true if the date is within the range, false otherwise
     */
    static bool isDateInRange(time_t date, time_t startDate, time_t endDate) {
        // Normalize dates to beginning of day for accurate comparison
        date = normalizeDateToStartOfDay(date);
        startDate = normalizeDateToStartOfDay(startDate);
        endDate = normalizeDateToStartOfDay(endDate);

        // Inclusive range check (date >= startDate && date <= endDate)
        return (difftime(date, startDate) >= 0 && difftime(endDate, date) >= 0);
    }

private:
    /**
     * Normalizes a time_t value to the start of its day (00:00:00).
     * This ensures consistent date comparisons regardless of time components.
     *
     * @param time The time_t value to normalize
     * @return A time_t value representing the same date at 00:00:00
     */
    static time_t normalizeDateToStartOfDay(time_t time) {
        struct tm* timeinfo = localtime(&time);
        timeinfo->tm_hour = 0;
        timeinfo->tm_min = 0;
        timeinfo->tm_sec = 0;
        return mktime(timeinfo);
    }
};

#endif // DATE_UTILS_H