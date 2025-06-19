#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <ctime>
#include <regex>
#include <sstream>
#include <iomanip>

class DateUtils {
public:
    /**
     * Validates a year-month string in YYYY-MM format
     *
     * @param yearMonth The year-month string to validate
     * @param minYear The minimum valid year (default: 1900)
     * @param maxYear The maximum valid year (default: 2100)
     * @return true if the year-month is valid, false otherwise
     */
    static bool validateYearMonth(const std::string& yearMonth, int minYear = 1900, int maxYear = 2100) {
        // Check format with regex (YYYY-MM)
        std::regex pattern("^([0-9]{4})-(0[1-9]|1[0-2])$");
        if (!std::regex_match(yearMonth, pattern)) {
            return false;
        }

        // Extract year
        int year = std::stoi(yearMonth.substr(0, 4));

        // Check year range
        return (year >= minYear && year <= maxYear);
    }

    /**
     * Validates a date string in YYYY-MM-DD format
     *
     * @param dateStr The date string to validate
     * @param minYear The minimum valid year (default: 1900)
     * @param maxYear The maximum valid year (default: 2100)
     * @return true if the date is valid, false otherwise
     */
    static bool validateDateString(const std::string& dateStr, int minYear = 1900, int maxYear = 2100) {
        // Check format with regex (YYYY-MM-DD)
        std::regex pattern("^([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$");
        if (!std::regex_match(dateStr, pattern)) {
            return false;
        }

        // Extract components
        int year = std::stoi(dateStr.substr(0, 4));
        int month = std::stoi(dateStr.substr(5, 2));
        int day = std::stoi(dateStr.substr(8, 2));

        // Check year range
        if (year < minYear || year > maxYear) {
            return false;
        }

        // Check day validity based on month and leap year
        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Adjust for leap year
        if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)) {
            daysInMonth[2] = 29;
        }

        return day <= daysInMonth[month];
    }

    /**
     * Converts a date string in YYYY-MM-DD format to time_t
     *
     * @param dateStr The date string to convert
     * @return time_t value representing the date (midnight)
     */
    static time_t stringToTime(const std::string& dateStr) {
        std::tm timeInfo = {};

        // Extract components
        int year = std::stoi(dateStr.substr(0, 4));
        int month = std::stoi(dateStr.substr(5, 2));
        int day = std::stoi(dateStr.substr(8, 2));

        // Fill timeInfo structure
        timeInfo.tm_year = year - 1900;  // Years since 1900
        timeInfo.tm_mon = month - 1;     // Months since January (0-11)
        timeInfo.tm_mday = day;          // Day of the month (1-31)
        timeInfo.tm_hour = 0;            // Set to midnight
        timeInfo.tm_min = 0;
        timeInfo.tm_sec = 0;

        // Convert to time_t
        return std::mktime(&timeInfo);
    }

    /**
     * Converts a time_t value to a date string in YYYY-MM-DD format
     *
     * @param time The time_t value to convert
     * @return Date string in YYYY-MM-DD format
     */
    static std::string timeToString(time_t time) {
        std::tm* timeInfo = std::localtime(&time);

        std::stringstream ss;
        ss << std::put_time(timeInfo, "%Y-%m-%d");
        return ss.str();
    }

    /**
     * Checks if a date falls within a given range (inclusive)
     *
     * @param date The date to check
     * @param startDate The start of the range
     * @param endDate The end of the range
     * @return true if the date is within the range, false otherwise
     */
    static bool isDateInRange(time_t date, time_t startDate, time_t endDate) {
        // Normalize to midnight for date comparison
        date = normalizeToMidnight(date);
        startDate = normalizeToMidnight(startDate);
        endDate = normalizeToMidnight(endDate);

        return (date >= startDate && date <= endDate);
    }

    /**
     * Checks if two dates are in the same month and year
     *
     * @param date1 The first date
     * @param date2 The second date
     * @return true if the dates are in the same month and year, false otherwise
     */
    static bool isSameMonth(time_t date1, time_t date2) {
        std::tm* tm1 = std::localtime(&date1);
        std::tm* tm2 = std::localtime(&date2);

        return (tm1->tm_year == tm2->tm_year && tm1->tm_mon == tm2->tm_mon);
    }

private:
    /**
     * Normalizes a time_t value to midnight (00:00:00) of the day
     *
     * @param time The time_t value to normalize
     * @return Normalized time_t value
     */
    static time_t normalizeToMidnight(time_t time) {
        std::tm* timeInfo = std::localtime(&time);

        // Set time to midnight
        timeInfo->tm_hour = 0;
        timeInfo->tm_min = 0;
        timeInfo->tm_sec = 0;

        // Convert back to time_t
        return std::mktime(timeInfo);
    }
};

#endif // DATE_UTILS_H