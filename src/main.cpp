/******************************************************************************
 * Budget & Expense Manager - CLI Application
 * Version: 2.3.1
 *
 * USAGE GUIDE:
 * ------------
 * This is a command-line personal finance manager that helps track your
 * transactions, manage monthly budgets, and view financial reports.
 *
 * HOW TO RUN:
 * - Interactive Mode:  ./budget
 * - Show Help:         ./budget --help   or   ./budget -h
 * - Show Version:      ./budget --version   or   ./budget -v
 *
 * FEATURES:
 * - Multi-user profile support
 * - Add, filter, and view transactions
 * - Set, update, and analyze budgets
 * - Generate reports and summaries
 *****************************************************************************/

#include <iostream>
#include <memory>
#include <limits>
#include <string>
#include <unordered_map>
#include <functional>

// App metadata (embedded)
#define APP_NAME "Budget & Expense Manager"
#define APP_VERSION "2.3.1"

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
    std::cout << "\n===== " << APP_NAME << " =====\n";
    std::cout << "1. Transaction Management\n";
    std::cout << "2. Budget Management\n";
    std::cout << "3. Financial Reports\n";
    std::cout << "4. User Profile Management\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice (0-4): ";
}

void showHelp() {
    std::cout << "Usage: budget [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h    Show this help message and exit\n";
    std::cout << "  --version, -v Show version info and exit\n";
    std::cout << "Available Top-Level Commands (enter at application prompt):\n";
    std::cout << "  1  Transaction Management\n";
    std::cout << "     1.1  View All Transactions\n";
    std::cout << "     1.2  View Transactions By Category\n";
    std::cout << "     1.3  View Transactions By Type (Income/Expense)\n";
    std::cout << "     1.4  View Transactions By Date Range\n";
    std::cout << "     1.5  View Transactions By Amount Range\n";
    std::cout << "     1.6  View Transactions By Month\n";
    std::cout << "     1.7  View Monthly Summary\n";
    std::cout << "     1.8  Add New Transaction\n";
    std::cout << "  2  Budget Management\n";
    std::cout << "     2.1  View All Budgets\n";
    std::cout << "     2.2  View Budgets By Category\n";
    std::cout << "     2.3  View Budgets By Month\n";
    std::cout << "     2.4  Set New Budget\n";
    std::cout << "     2.5  Update Budget\n";
    std::cout << "     2.6  Remove Budget\n";
    std::cout << "     2.7  View Budget Usage Report\n";
    std::cout << "  3  Financial Reports\n";
    std::cout << "     3.1  Monthly Summary\n";
    std::cout << "     3.2  Budget Utilization Report\n";
    std::cout << "  4  User Profile Management\n";
    std::cout << "     4.1  Create Profile\n";
    std::cout << "     4.2  Select Profile\n";
    std::cout << "     4.3  View Profile Info\n";
    std::cout << "     4.4  Update Profile Display Name\n";
    std::cout << "     4.5  Delete Profile\n";
    std::cout << "     4.6  List All Profiles\n";
    std::cout << "  0  Exit\n";
}

void showVersion() {
    std::cout << APP_NAME << " - Version " << APP_VERSION << "\n";
}

int main(int argc, char* argv[]) {
    std::unordered_map<std::string, std::function<void()>> flagHandlers{
        {"--help", showHelp}, {"-h", showHelp},
        {"--version", showVersion}, {"-v", showVersion}
    };

    bool handled = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto it = flagHandlers.find(arg);
        if (it != flagHandlers.end()) {
            it->second();
            handled = true;
        }
        else if (arg.rfind("--", 0) == 0 || arg.rfind("-", 0) == 0) {
            std::cerr << "Unknown option: " << arg << "\n";
            std::cerr << "Use --help to see available options.\n";
            return 1;
        }
    }

    if (handled) return 0;

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
        std::cout << "No active user profile. Exiting.\n";
        return 1;
    }

    int choice = -1;
    do {
        displayMainMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 0:
            std::cout << "Thank you for using Budget & Expense Manager. Goodbye!\n";
            break;
        case 1: {
            int tChoice = -1;
            do {
                if (!profileManager->hasActiveProfile()) {
                    std::cout << "No active user profile. Please select a profile first.\n";
                    profileUI->selectProfile();
                    if (profileManager->hasActiveProfile()) {
                        auto ap = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(ap);
                        budgetManager->setUserProfile(ap);
                    }
                    else break;
                }
                std::cout << "\n===== Transaction Management ("
                    << profileManager->getActiveProfile()->getDisplayName()
                    << ") =====\n";
                std::cout << "1. View All Transactions\n"
                    "2. View Transactions By Category\n"
                    "3. View Transactions By Type (Income/Expense)\n"
                    "4. View Transactions By Date Range\n"
                    "5. View Transactions By Amount Range\n"
                    "6. View Transactions By Month\n"
                    "7. View Monthly Summary\n"
                    "8. Add New Transaction\n"
                    "0. Back to Main Menu\n"
                    "Enter your choice (0-8): ";
                if (!(std::cin >> tChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switch (tChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1: transactionUI->showAllTransactions(); break;
                case 2: transactionUI->showTransactionsByCategory(); break;
                case 3: transactionUI->showTransactionsByType(); break;
                case 4: transactionUI->showTransactionsByDateRange(); break;
                case 5: transactionUI->showTransactionsByAmountRange(); break;
                case 6: transactionUI->showTransactionsByMonth(); break;
                case 7: transactionUI->showMonthlySummary(); break;
                case 8: transactionUI->addNewTransaction(); break;
                default: std::cout << "Invalid choice (0-8).\n";
                }
            } while (tChoice != 0);
            break;
        }
        case 2: {
            int bChoice = -1;
            do {
                if (!profileManager->hasActiveProfile()) {
                    std::cout << "No active user profile. Please select a profile first.\n";
                    profileUI->selectProfile();
                    if (profileManager->hasActiveProfile()) {
                        auto ap = profileManager->getActiveProfile();
                        transactionManager->setUserProfile(ap);
                        budgetManager->setUserProfile(ap);
                    }
                    else break;
                }
                std::cout << "\n===== Budget Management ("
                    << profileManager->getActiveProfile()->getDisplayName()
                    << ") =====\n";
                std::cout << "1. View All Budgets\n"
                    "2. View Budgets By Category\n"
                    "3. View Budgets By Month\n"
                    "4. Set New Budget\n"
                    "5. Update Budget\n"
                    "6. Remove Budget\n"
                    "7. View Budget Usage Report\n"
                    "0. Back to Main Menu\n"
                    "Enter your choice (0-7): ";
                if (!(std::cin >> bChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switch (bChoice) {
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                case 1: budgetUI->showAllBudgets(); break;
                case 2: budgetUI->showBudgetsByCategory(); break;
                case 3: budgetUI->showBudgetsByMonth(); break;
                case 4: budgetUI->setBudget(); break;
                case 5: budgetUI->updateBudget(); break;
                case 6: budgetUI->removeBudget(); break;
                case 7: budgetUI->showBudgetUsageReport(); break;
                default: std::cout << "Invalid choice (0-7).\n";
                }
            } while (bChoice != 0);
            break;
        }
        case 3: {
            int rChoice = -1;
            if (!profileManager->hasActiveProfile()) {
                std::cout << "No active user profile. Please select a profile first.\n";
                profileUI->selectProfile();
                if (profileManager->hasActiveProfile()) {
                    auto ap = profileManager->getActiveProfile();
                    transactionManager->setUserProfile(ap);
                    budgetManager->setUserProfile(ap);
                }
                else break;
            }
            std::cout << "\n===== Financial Reports ("
                << profileManager->getActiveProfile()->getDisplayName()
                << ") =====\n";
            std::cout << "1. Monthly Summary\n"
                "2. Budget Utilization Report\n"
                "0. Back to Main Menu\n"
                "Enter your choice (0-2): ";
            if (!(std::cin >> rChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (rChoice) {
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