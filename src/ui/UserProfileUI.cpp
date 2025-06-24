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

static bool readInt(int& out, int min, int max, const std::string& prompt) {
    std::cout << prompt;
    if (!(std::cin >> out) || out < min || out > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
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
        std::cout << "Invalid username format.\n";
        return;
    }

    if (profileManager->usernameExists(username)) {
        std::cout << "A profile with this username already exists.\n";
        return;
    }

    std::cout << "Enter display name: ";
    std::getline(std::cin, displayName);
    if (displayName.empty()) displayName = username;

    if (profileManager->createProfile(username, displayName)) {
        std::cout << "Profile created successfully!\n";

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

    int choice;
    if (!readInt(choice, 1, static_cast<int>(profiles.size()), "Enter the number of the profile to select: ")) return;

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

    if (profileManager->updateProfileDisplayName(profile->getUsername(), newDisplayName)) {
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

    int choice;
    if (!readInt(choice, 1, static_cast<int>(profiles.size()), "Enter the number of the profile to delete: ")) return;

    auto selectedProfile = profiles[choice - 1];

    std::cout << "Are you sure you want to delete profile '" << selectedProfile->getDisplayName()
        << "' and all its data? This action cannot be undone. (y/n): ";

    char confirm;
    if (!(std::cin >> confirm)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Profile deletion cancelled.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (std::tolower(confirm) != 'y') {
        std::cout << "Profile deletion cancelled.\n";
        return;
    }

    if (profileManager->deleteProfile(selectedProfile->getUsername())) {
        std::cout << "Profile deleted successfully!\n";
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

void UserProfileUI::listAllProfiles() const {
    auto profiles = profileManager->getAllProfiles();
    if (profiles.empty()) {
        std::cout << "No profiles found.\n";
        return;
    }

    displayProfileHeader("All Profiles");

    std::sort(profiles.begin(), profiles.end(), [](const auto& a, const auto& b) {
        return a->getLastAccessDate() > b->getLastAccessDate();
        });

    size_t usernameWidth = 8, displayNameWidth = 12;
    for (const auto& profile : profiles) {
        usernameWidth = std::max(usernameWidth, profile->getUsername().length());
        displayNameWidth = std::max(displayNameWidth, profile->getDisplayName().length());
    }

    std::cout << std::setw(4) << std::left << "#"
        << std::setw(usernameWidth + 2) << std::left << "Username"
        << std::setw(displayNameWidth + 2) << std::left << "Display Name"
        << std::setw(20) << std::left << "Created On"
        << std::setw(20) << std::left << "Last Access"
        << "Status\n";

    std::cout << std::string(4 + usernameWidth + 2 + displayNameWidth + 2 + 20 + 20 + 10, '-') << "\n";

    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];
        std::cout << std::setw(4) << std::left << (i + 1)
            << std::setw(usernameWidth + 2) << std::left << profile->getUsername()
            << std::setw(displayNameWidth + 2) << std::left << profile->getDisplayName()
            << std::setw(12) << std::left << profile->getCreatedDate().substr(0, 10)
            << std::setw(12) << std::left << profile->getLastAccessDate().substr(0, 10);

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
