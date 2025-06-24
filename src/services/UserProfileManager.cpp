#include "../../include/services/UserProfileManager.h"
#include "../../include/utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <filesystem>

UserProfileManager::UserProfileManager() : profilesFilePath("data/profiles.csv") {
    // Ensure data directory exists
    try {
        std::filesystem::create_directories("data");
        std::filesystem::create_directories("data/users");
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating data directories: " << e.what() << std::endl;
    }

    loadProfiles();
}

void UserProfileManager::loadProfiles() {
    profiles.clear();
    activeProfile = nullptr;

    if (!std::filesystem::exists(profilesFilePath)) {
        return; // The profiles file doesn't exist yet
    }

    std::vector<std::vector<std::string>> data = FileUtils::readCSV(profilesFilePath);
    for (const auto& row : data) {
        // Format: username,displayName,createdDate,lastAccessDate
        if (row.size() >= 4) {
            std::string username = row[0];
            std::string displayName = row[1];

            // Create the profile with loaded data
            auto profile = std::make_shared<UserProfile>(username, displayName);
            profiles.push_back(profile);
        }
    }
}

void UserProfileManager::saveProfiles() const {
    std::ofstream file(profilesFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open profiles file for writing." << std::endl;
        return;
    }

    for (const auto& profile : profiles) {
        // Format: username,displayName,createdDate,lastAccessDate
        file << profile->getUsername() << ","
            << profile->getDisplayName() << ","
            << profile->getCreatedDate() << ","
            << profile->getLastAccessDate() << "\n";
    }

    file.close();
}

bool UserProfileManager::createProfile(const std::string& username, const std::string& displayName) {
    // Check if a profile with this username already exists
    if (usernameExists(username)) {
        return false;
    }

    // Check if the username is valid
    if (!isValidUsername(username)) {
        return false;
    }

    // Create a new profile
    auto profile = std::make_shared<UserProfile>(username, displayName);
    profiles.push_back(profile);

    // Save profiles to file
    saveProfiles();

    return true;
}

bool UserProfileManager::deleteProfile(const std::string& username) {
    for (auto it = profiles.begin(); it != profiles.end(); ++it) {
        if ((*it)->getUsername() == username) {
            // Check if this is the active profile
            if (activeProfile && activeProfile->getUsername() == username) {
                activeProfile = nullptr; // Clear active profile
            }

            // Remove the profile from the list
            profiles.erase(it);

            // Save the updated profiles
            saveProfiles();

            // Option: Delete user's data files as well
            std::string userDir = "data/users/" + username;
            try {
                std::filesystem::remove_all(userDir);
            }
            catch (const std::exception& e) {
                std::cerr << "Error removing user directory: " << e.what() << std::endl;
                // Continue even if directory deletion fails
            }

            return true;
        }
    }

    return false; // Profile not found
}

bool UserProfileManager::updateProfileDisplayName(const std::string& username, const std::string& newDisplayName) {
    for (auto& profile : profiles) {
        if (profile->getUsername() == username) {
            profile->setDisplayName(newDisplayName);
            saveProfiles();
            return true;
        }
    }

    return false; // Profile not found
}

std::shared_ptr<UserProfile> UserProfileManager::getProfile(const std::string& username) const {
    for (const auto& profile : profiles) {
        if (profile->getUsername() == username) {
            return profile;
        }
    }

    return nullptr; // Profile not found
}

std::vector<std::shared_ptr<UserProfile>> UserProfileManager::getAllProfiles() const {
    return profiles;
}

bool UserProfileManager::setActiveProfile(const std::string& username) {
    auto profile = getProfile(username);
    if (profile) {
        activeProfile = profile;
        activeProfile->updateLastAccessDate();
        saveProfiles(); // Save updated last access date
        return true;
    }

    return false;
}

std::shared_ptr<UserProfile> UserProfileManager::getActiveProfile() const {
    return activeProfile;
}

bool UserProfileManager::hasActiveProfile() const {
    return activeProfile != nullptr;
}

bool UserProfileManager::usernameExists(const std::string& username) const {
    return getProfile(username) != nullptr;
}

bool UserProfileManager::isValidUsername(const std::string& username) const {
    // Username must:
    // - Be 3-20 characters long
    // - Contain only alphanumeric characters, hyphens, and underscores
    // - Start with a letter

    if (username.length() < 3 || username.length() > 20) {
        return false;
    }

    std::regex validUsernameRegex("^[a-zA-Z][a-zA-Z0-9_-]*$");
    return std::regex_match(username, validUsernameRegex);
}