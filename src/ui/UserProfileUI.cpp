#include "../../include/ui/UserProfileUI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>

static std::string formatDate(const std::chrono::system_clock::time_point& tp) {
    auto t_c = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&t_c);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

UserProfileUI::UserProfileUI(std::shared_ptr<UserProfileManager> profileManager)
    : profileManager(profileManager) {
}

void UserProfileUI::displayProfileMenu() const {
    std::cout << "\n===== User Profile Management " << getActiveProfileDisplayLabel() << " =====\n";
    std::cout << "1. Create New Profile\n";
    std::cout << "2. Select Profile\n";
    std::cout << "3. View Profile Information\n";
    std::cout << "4. Update Profile Display Name\n";
    std::cout << "5. Delete Profile\n";
    std::cout << "6. List All Profiles\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-6): ";
}

void UserProfileUI::createProfile() {
    displayProfileHeader("Create New Profile");

    std::string username, displayName;

    std::cout << "Enter username (3-20 characters, alphanumeric, must start with a letter): ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (!profileManager->isValidUsername(username)) {
        std::cout << "Invalid username format. Username must be 3-20 characters, start with a letter, "
            << "and contain only alphanumeric characters, hyphens, and underscores.\n";
        return;
    }

    if (profileManager->usernameExists(username)) {
        std::cout << "A profile with this username already exists. Please choose a different username.\n";
        return;
    }

    std::cout << "Enter display name: ";
    std::getline(std::cin, displayName);

    if (displayName.empty()) {
        displayName = username; // Default to username if no display name provided
    }

    bool success = profileManager->createProfile(username, displayName);

    if (success) {
        std::cout << "Profile created successfully!\n";

        // If this is the first profile, set it as active automatically
        if (profileManager->getAllProfiles().size() == 1) {
            profileManager->setActiveProfile(username);
            std::cout << "This profile has been set as the active profile.\n";
        }
        else {
            std::cout << "Would you like to switch to this profile? (y/n): ";
            char choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice == 'y' || choice == 'Y') {
                profileManager->setActiveProfile(username);
                std::cout << "Switched to profile: " << displayName << "\n";
            }
        }
    }
    else {
        std::cout << "Failed to create profile.\n";
    }
}

void UserProfileUI::selectProfile() {
    auto profiles = profileManager->getAllProfiles();

    if (profiles.empty()) {
        std::cout << "No profiles found. Please create a profile first.\n";
        return;
    }

    displayProfileHeader("Select Profile");

    std::cout << "Available profiles:\n";
    for (size_t i = 0; i < profiles.size(); ++i) {
        std::cout << i + 1 << ". " << profiles[i]->getDisplayName()
            << " (" << profiles[i]->getUsername() << ")";

        if (profileManager->hasActiveProfile() &&
            profileManager->getActiveProfile()->getUsername() == profiles[i]->getUsername()) {
            std::cout << " [ACTIVE]";
        }

        std::cout << "\n";
    }

    std::cout << "Enter the number of the profile to select (1-" << profiles.size() << "): ";
    int choice;
    if (!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(profiles.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid selection.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto selectedProfile = profiles[choice - 1];
    profileManager->setActiveProfile(selectedProfile->getUsername());

    std::cout << "Switched to profile: " << selectedProfile->getDisplayName() << "\n";
}

void UserProfileUI::viewProfileInfo() {
    if (!profileManager->hasActiveProfile()) {
        std::cout << "No active profile. Please select a profile first.\n";
        return;
    }

    auto profile = profileManager->getActiveProfile();

    displayProfileHeader("Profile Information");

    std::cout << "Username: " << profile->getUsername() << "\n";
    std::cout << "Display Name: " << profile->getDisplayName() << "\n";
    std::cout << "Created On: " << profile->getCreatedDate() << "\n";
    std::cout << "Last Access: " << profile->getLastAccessDate() << "\n";
    std::cout << "Data Directory: " << profile->getDataDirectory() << "\n";
}

void UserProfileUI::updateProfileDisplayName() {
    if (!profileManager->hasActiveProfile()) {
        std::cout << "No active profile. Please select a profile first.\n";
        return;
    }

    auto profile = profileManager->getActiveProfile();

    displayProfileHeader("Update Profile Display Name");

    std::cout << "Current display name: " << profile->getDisplayName() << "\n";
    std::cout << "Enter new display name: ";

    std::string newDisplayName;
    std::getline(std::cin, newDisplayName);

    if (newDisplayName.empty()) {
        std::cout << "Display name cannot be empty.\n";
        return;
    }

    bool success = profileManager->updateProfileDisplayName(profile->getUsername(), newDisplayName);

    if (success) {
        std::cout << "Display name updated successfully!\n";
    }
    else {
        std::cout << "Failed to update display name.\n";
    }
}

void UserProfileUI::deleteProfile() {
    auto profiles = profileManager->getAllProfiles();

    if (profiles.empty()) {
        std::cout << "No profiles found.\n";
        return;
    }

    displayProfileHeader("Delete Profile");

    std::cout << "Available profiles:\n";
    for (size_t i = 0; i < profiles.size(); ++i) {
        std::cout << i + 1 << ". " << profiles[i]->getDisplayName()
            << " (" << profiles[i]->getUsername() << ")";

        if (profileManager->hasActiveProfile() &&
            profileManager->getActiveProfile()->getUsername() == profiles[i]->getUsername()) {
            std::cout << " [ACTIVE]";
        }

        std::cout << "\n";
    }

    std::cout << "Enter the number of the profile to delete (1-" << profiles.size() << "): ";
    int choice;
    if (!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(profiles.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid selection.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto selectedProfile = profiles[choice - 1];

    std::cout << "Are you sure you want to delete profile '" << selectedProfile->getDisplayName()
        << "' and all its data? This action cannot be undone. (y/n): ";

    char confirm;
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirm == 'y' || confirm == 'Y') {
        bool success = profileManager->deleteProfile(selectedProfile->getUsername());

        if (success) {
            std::cout << "Profile deleted successfully!\n";

            // If we deleted the active profile, try to set another one as active
            if (!profileManager->hasActiveProfile()) {
                auto remainingProfiles = profileManager->getAllProfiles();
                if (!remainingProfiles.empty()) {
                    profileManager->setActiveProfile(remainingProfiles[0]->getUsername());
                    std::cout << "Switched to profile: " << remainingProfiles[0]->getDisplayName() << "\n";
                }
            }
        }
        else {
            std::cout << "Failed to delete profile.\n";
        }
    }
    else {
        std::cout << "Profile deletion cancelled.\n";
    }
}

void UserProfileUI::listAllProfiles() const {
    auto profiles = profileManager->getAllProfiles();

    if (profiles.empty()) {
        std::cout << "No profiles found.\n";
        return;
    }

    displayProfileHeader("All Profiles");

    // Sort profiles by last access date (most recent first)
    std::sort(profiles.begin(), profiles.end(), [](const auto& a, const auto& b) {
        return a->getLastAccessDate() > b->getLastAccessDate();
        });

    // Calculate column widths
    size_t usernameWidth = 8; // Minimum width for "Username"
    size_t displayNameWidth = 12; // Minimum width for "Display Name"

    for (const auto& profile : profiles) {
        usernameWidth = std::max(usernameWidth, profile->getUsername().length());
        displayNameWidth = std::max(displayNameWidth, profile->getDisplayName().length());
    }

    // Header
    std::cout << std::setw(4) << std::left << "#"
        << std::setw(usernameWidth + 2) << std::left << "Username"
        << std::setw(displayNameWidth + 2) << std::left << "Display Name"
        << std::setw(20) << std::left << "Created On"
        << std::setw(20) << std::left << "Last Access"
        << "Status\n";

    std::cout << std::string(4 + usernameWidth + 2 + displayNameWidth + 2 + 20 + 20 + 10, '-') << "\n";

    // Print each profile
    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];

        std::cout << std::setw(4) << std::left << (i + 1)
            << std::setw(usernameWidth + 2) << std::left << profile->getUsername()
            << std::setw(displayNameWidth + 2) << std::left << profile->getDisplayName()
            << std::setw(12) << std::left << profile->getCreatedDate().substr(0, 10)
            << std::setw(12) << std::left << profile->getLastAccessDate().substr(0, 10);

        // Active status
        if (profileManager->hasActiveProfile() &&
            profileManager->getActiveProfile()->getUsername() == profile->getUsername()) {
            std::cout << "[ACTIVE]";
        }

        std::cout << "\n";
    }
}

void UserProfileUI::displayProfileHeader(const std::string& title) const {
    std::cout << "\n===== " << title << " " << getActiveProfileDisplayLabel() << " =====\n";
}

std::string UserProfileUI::getActiveProfileDisplayLabel() const {
    if (!profileManager->hasActiveProfile()) {
        return "(No Active Profile)";
    }
    return "(Active: " + profileManager->getActiveProfile()->getDisplayName() + ")";
}
