#include "../../include/models/UserProfile.h"
#include "../../include/utils/DateUtils.h"
#include <filesystem>
#include <iostream>

UserProfile::UserProfile(const std::string& username, const std::string& displayName)
    : username(username), displayName(displayName) {
    // Create data directory path for this user
    dataDirectory = "data/users/" + username + "/";

    // Set creation date
    createdDate = DateUtils::getCurrentDateStr();
    lastAccessDate = createdDate;

    // Ensure the user's data directory exists
    try {
        std::filesystem::create_directories(dataDirectory);
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating user directory: " << e.what() << std::endl;
    }
}

std::string UserProfile::getUsername() const {
    return username;
}

std::string UserProfile::getDisplayName() const {
    return displayName;
}

std::string UserProfile::getDataDirectory() const {
    return dataDirectory;
}

std::string UserProfile::getCreatedDate() const {
    return createdDate;
}

std::string UserProfile::getLastAccessDate() const {
    return lastAccessDate;
}

void UserProfile::setDisplayName(const std::string& newDisplayName) {
    displayName = newDisplayName;
}

void UserProfile::updateLastAccessDate() {
    lastAccessDate = DateUtils::getCurrentDateStr();
}

std::string UserProfile::getTransactionsFilePath() const {
    return dataDirectory + "transactions.csv";
}

std::string UserProfile::getBudgetsFilePath() const {
    return dataDirectory + "budgets.csv";
}

std::string UserProfile::getSettingsFilePath() const {
    return dataDirectory + "settings.csv";
}