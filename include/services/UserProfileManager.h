#ifndef USER_PROFILE_MANAGER_H
#define USER_PROFILE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "../models/UserProfile.h"

class UserProfileManager {
private:
    std::vector<std::shared_ptr<UserProfile>> profiles;
    std::shared_ptr<UserProfile> activeProfile;
    std::string profilesFilePath;

    void loadProfiles();
    void saveProfiles() const;

public:
    UserProfileManager();

    // Profile management
    bool createProfile(const std::string& username, const std::string& displayName);
    bool deleteProfile(const std::string& username);
    bool updateProfileDisplayName(const std::string& username, const std::string& newDisplayName);

    // Profile retrieval
    std::shared_ptr<UserProfile> getProfile(const std::string& username) const;
    std::vector<std::shared_ptr<UserProfile>> getAllProfiles() const;

    // Active profile management
    bool setActiveProfile(const std::string& username);
    std::shared_ptr<UserProfile> getActiveProfile() const;
    bool hasActiveProfile() const;

    // Username validation
    bool usernameExists(const std::string& username) const;
    bool isValidUsername(const std::string& username) const;
};

#endif // USER_PROFILE_MANAGER_H