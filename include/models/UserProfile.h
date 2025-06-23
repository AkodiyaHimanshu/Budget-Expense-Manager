#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include <string>

class UserProfile {
private:
    std::string username;
    std::string displayName;
    std::string dataDirectory;
    std::string createdDate;
    std::string lastAccessDate;

public:
    UserProfile(const std::string& username, const std::string& displayName);

    // Getters
    std::string getUsername() const;
    std::string getDisplayName() const;
    std::string getDataDirectory() const;
    std::string getCreatedDate() const;
    std::string getLastAccessDate() const;

    // Setters
    void setDisplayName(const std::string& displayName);
    void updateLastAccessDate();

    // File paths for this user's data
    std::string getTransactionsFilePath() const;
    std::string getBudgetsFilePath() const;
    std::string getSettingsFilePath() const;
};

#endif // USER_PROFILE_H