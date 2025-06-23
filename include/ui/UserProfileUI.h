#ifndef USER_PROFILE_UI_H
#define USER_PROFILE_UI_H

#include <memory>
#include "../services/UserProfileManager.h"

class UserProfileUI {
private:
    std::shared_ptr<UserProfileManager> profileManager;

public:
    UserProfileUI(std::shared_ptr<UserProfileManager> profileManager);

    // Menu display
    void displayProfileMenu() const;

    // Profile management operations
    void createProfile();
    void selectProfile();
    void viewProfileInfo();
    void updateProfileDisplayName();
    void deleteProfile();
    void listAllProfiles() const;

    // Helper methods
    void displayProfileHeader(const std::string& title) const;
    std::string getActiveProfileDisplayLabel() const;
};

#endif // USER_PROFILE_UI_H