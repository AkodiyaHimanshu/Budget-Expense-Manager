#include <iostream>
#include <memory>
#include <limits>
#include <string>

// Include models
#include "../include/models/Transaction.h"
#include "../include/models/Budget.h"
#include "../include/models/UserProfile.h"

// Include services
#include "../include/services/TransactionManager.h"
#include "../include/services/BudgetManager.h"
#include "../include/services/UserProfileManager.h"

// Include UI components
#include "../include/ui/TransactionUI.h"
#include "../include/ui/BudgetUI.h"
#include "../include/ui/UserProfileUI.h"

void displayMainMenu() {
    std::cout << "\n===== Budget & Expense Manager =====\n";
    std::cout << "1. Transaction Management\n";
    std::cout << "2. Budget Management\n";  // New option for budget management
    std::cout << "3. Financial Reports\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice (0-3): ";
}

int main() {
    std::cout << "===== Budget & Expense Manager =====\n";

    // Create the profile manager
    auto profileManager = std::make_shared<UserProfileManager>();

    // Create the profile UI
    auto profileUI = std::make_shared<UserProfileUI>(profileManager);

    // Initialize with null profiles since we don't have an active profile yet
    auto transactionManager = std::make_shared<TransactionManager>(nullptr);
    auto budgetManager = std::make_shared<BudgetManager>(nullptr);

    // Create UI components with managers
    auto transactionUI = std::make_shared<TransactionUI>(transactionManager, budgetManager);
    auto budgetUI = std::make_shared<BudgetUI>(budgetManager, transactionManager);

    // Before proceeding, ensure we have a user profile
    if (profileManager->getAllProfiles().empty()) {
        std::cout << "\nWelcome to Budget & Expense Manager!\n";
        std::cout << "No user profiles found. Let's create one to get started.\n";
        profileUI->createProfile();
    }
    else {
        // If we have profiles but none active, prompt to select one
        if (!profileManager->hasActiveProfile()) {
            std::cout << "\nPlease select a user profile to continue:\n";
            profileUI->selectProfile();
        }
    }

    // If we have an active profile now, set it in the managers
    if (profileManager->hasActiveProfile()) {
        auto activeProfile = profileManager->getActiveProfile();
        transactionManager->setUserProfile(activeProfile);
        budgetManager->setUserProfile(activeProfile);
        std::cout << "\nWelcome, " << activeProfile->getDisplayName() << "!\n";
    }
    else {
        // If still no active profile (user declined to create/select), exit
        std::cout << "No active user profile. Exiting.\n";
        return 1;
    }

    int choice = -1;

    do {
        std::cout << "\n===== Main Menu =====\n";
        std::cout << "1. Transaction Management\n";
        std::cout << "2. Budget Management\n";
        std::cout << "3. Financial Reports\n";
        std::cout << "4. User Profile Management\n";  // New menu option
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice (0-4): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice < 0 || choice > 4) {
            std::cout << "Invalid choice. Please try again (0-4).\n";
            continue;
        }

        switch (choice) {
        case 0:
            std::cout << "Thank you for using Budget & Expense Manager. Goodbye!\n";
            break;

        case 1: { // Transaction Management
            int transactionChoice = -1;

            do {
                // Check if we have an active profile
                if (!profileManager->hasActiveProfile()) {
                    std::cout << "No active user profile. Please select a profile first.\n";
                    profileUI->selectProfile();

                    if (profileManager->hasActiveProfile()) {
                        auto activeProfile = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(activeProfile);
                        budgetManager->setUserProfile(activeProfile);
                    }
                    else {
                        break; // Exit to main menu if no profile selected
                    }
                }

                std::cout << "\n===== Transaction Management ("
                    << profileManager->getActiveProfile()->getDisplayName()
                    << ") =====\n";
                std::cout << "1. View All Transactions\n";
                std::cout << "2. View Transactions By Category\n";
                std::cout << "3. View Transactions By Type (Income/Expense)\n";
                std::cout << "4. View Transactions By Date Range\n";
                std::cout << "5. View Transactions By Amount Range\n";
                std::cout << "6. View Transactions By Month\n";
                std::cout << "7. View Monthly Summary\n";
                std::cout << "8. Add New Transaction\n";
                std::cout << "0. Back to Main Menu\n";
                std::cout << "Enter your choice (0-8): ";

                if (!(std::cin >> transactionChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (transactionChoice < 0 || transactionChoice > 8) {
                    std::cout << "Invalid choice. Please try again (0-8).\n";
                    continue;
                }

                switch (transactionChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    transactionUI->showAllTransactions();
                    break;
                case 2:
                    transactionUI->showTransactionsByCategory();
                    break;
                case 3:
                    transactionUI->showTransactionsByType();
                    break;
                case 4:
                    transactionUI->showTransactionsByDateRange();
                    break;
                case 5:
                    transactionUI->showTransactionsByAmountRange();
                    break;
                case 6:
                    transactionUI->showTransactionsByMonth();
                    break;
                case 7:
                    transactionUI->showMonthlySummary();
                    break;
                case 8:
                    transactionUI->addNewTransaction();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-8).\n";
                }
            } while (transactionChoice != 0);

            break;
        }

        case 2: { // Budget Management
            int budgetChoice = -1;

            do {
                // Check if we have an active profile
                if (!profileManager->hasActiveProfile()) {
                    std::cout << "No active user profile. Please select a profile first.\n";
                    profileUI->selectProfile();

                    if (profileManager->hasActiveProfile()) {
                        auto activeProfile = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(activeProfile);
                        budgetManager->setUserProfile(activeProfile);
                    }
                    else {
                        break; // Exit to main menu if no profile selected
                    }
                }

                std::cout << "\n===== Budget Management ("
                    << profileManager->getActiveProfile()->getDisplayName()
                    << ") =====\n";
                std::cout << "1. View All Budgets\n";
                std::cout << "2. View Budgets By Category\n";
                std::cout << "3. View Budgets By Month\n";
                std::cout << "4. Set New Budget\n";
                std::cout << "5. Update Budget\n";
                std::cout << "6. Remove Budget\n";
                std::cout << "7. View Budget Usage Report\n";
                std::cout << "0. Back to Main Menu\n";
                std::cout << "Enter your choice (0-7): ";

                if (!(std::cin >> budgetChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (budgetChoice < 0 || budgetChoice > 7) {
                    std::cout << "Invalid choice. Please try again (0-7).\n";
                    continue;
                }

                switch (budgetChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    budgetUI->showAllBudgets();
                    break;
                case 2:
                    budgetUI->showBudgetsByCategory();
                    break;
                case 3:
                    budgetUI->showBudgetsByMonth();
                    break;
                case 4:
                    budgetUI->setBudget();
                    break;
                case 5:
                    budgetUI->updateBudget();
                    break;
                case 6:
                    budgetUI->removeBudget();
                    break;
                case 7:
                    budgetUI->showBudgetUsageReport();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-7).\n";
                }
            } while (budgetChoice != 0);

            break;
        }

        case 3: { // Financial Reports
            // Check if we have an active profile
            if (!profileManager->hasActiveProfile()) {
                std::cout << "No active user profile. Please select a profile first.\n";
                profileUI->selectProfile();

                if (profileManager->hasActiveProfile()) {
                    auto activeProfile = profileManager->getActiveProfile();
                    transactionManager->setUserProfile(activeProfile);
                    budgetManager->setUserProfile(activeProfile);
                }
                else {
                    break; // Exit to main menu if no profile selected
                }
            }

            std::cout << "\n===== Financial Reports ("
                << profileManager->getActiveProfile()->getDisplayName()
                << ") =====\n";
            std::cout << "1. Monthly Summary\n";
            std::cout << "2. Budget Utilization Report\n";
            std::cout << "0. Back to Main Menu\n";
            std::cout << "Enter your choice (0-2): ";

            int reportChoice;
            if (!(std::cin >> reportChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (reportChoice) {
            case 0:
                std::cout << "Returning to main menu...\n";
                break;
            case 1:
                transactionUI->showMonthlySummary();
                break;
            case 2:
                budgetUI->showBudgetUsageReport();
                break;
            default:
                std::cout << "Invalid choice. Please try again (0-2).\n";
            }

            break;
        }

        case 4: { // User Profile Management - NEW
            int profileChoice = -1;

            do {
                profileUI->displayProfileMenu();

                if (!(std::cin >> profileChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (profileChoice < 0 || profileChoice > 6) {
                    std::cout << "Invalid choice. Please try again (0-6).\n";
                    continue;
                }

                switch (profileChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1:
                    profileUI->createProfile();
                    break;
                case 2:
                    profileUI->selectProfile();
                    // If the profile was changed, update the managers
                    if (profileManager->hasActiveProfile()) {
                        auto activeProfile = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(activeProfile);
                        budgetManager->setUserProfile(activeProfile);
                    }
                    break;
                case 3:
                    profileUI->viewProfileInfo();
                    break;
                case 4:
                    profileUI->updateProfileDisplayName();
                    break;
                case 5:
                    profileUI->deleteProfile();
                    // If the profile was changed, update the managers
                    if (profileManager->hasActiveProfile()) {
                        auto activeProfile = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(activeProfile);
                        budgetManager->setUserProfile(activeProfile);
                    }
                    else {
                        // If no profile is active after deletion, prompt to create/select one
                        if (profileManager->getAllProfiles().empty()) {
                            std::cout << "No profiles remaining. Let's create a new one:\n";
                            profileUI->createProfile();
                        }
                        else {
                            std::cout << "Please select a profile to continue:\n";
                            profileUI->selectProfile();
                        }

                        // Update managers with the new active profile if one was chosen
                        if (profileManager->hasActiveProfile()) {
                            auto activeProfile = profileManager->getActiveProfile();
                            transactionManager->setUserProfile(activeProfile);
                            budgetManager->setUserProfile(activeProfile);
                        }
                    }
                    break;
                case 6:
                    profileUI->listAllProfiles();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again (0-6).\n";
                }
            } while (profileChoice != 0);

            break;
        }

        default:
            std::cout << "Invalid choice. Please try again (0-4).\n";
        }
    } while (choice != 0);

    return 0;
}