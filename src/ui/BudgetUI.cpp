#include "../../include/ui/BudgetUI.h"
#include "../../include/services/BudgetManager.h"
#include "../../include/services/TransactionManager.h"
#include "../../include/models/Budget.h"
#include "../../include/models/Transaction.h"
#include "../../include/utils/DateUtils.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <limits>
#include <algorithm>
#include <vector>

BudgetUI::BudgetUI(const std::shared_ptr<BudgetManager>& budgetManager,
    const std::shared_ptr<TransactionManager>& transactionManager)
    : budgetManager(budgetManager), transactionManager(transactionManager) {
}

void BudgetUI::displayBudgetMenu() {
    std::cout << "\n===== Budget Management Menu =====\n";
    std::cout << "1. View All Budgets\n";
    std::cout << "2. View Budgets by Category\n";
    std::cout << "3. View Budgets by Month\n";
    std::cout << "4. Set New Budget\n";
    std::cout << "5. Update Existing Budget\n";
    std::cout << "6. Remove Budget\n";
    std::cout << "7. Budget Usage Report\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-7): ";
}

void BudgetUI::displayBudget(const std::shared_ptr<Budget>& budget) {
    if (!budget) return;

    std::cout << budget->getDisplayString() << std::endl;
}

void BudgetUI::displayBudgets(const std::vector<std::shared_ptr<Budget>>& budgets) {
    if (budgets.empty()) {
        std::cout << "No budgets found.\n";
        return;
    }

    std::cout << "\n===== Budget Limits =====\n";
    for (const auto& budget : budgets) {
        displayBudget(budget);
    }
}

void BudgetUI::showAllBudgets() {
    std::cout << "\n===== All Budget Limits =====\n";

    auto allBudgets = budgetManager->getAllBudgets();
    displayBudgets(allBudgets);
}

void BudgetUI::showBudgetsByCategory() {
    std::string category;
    std::cout << "\n===== View Budgets by Category =====\n";
    std::cout << "Enter category: ";
    std::getline(std::cin, category);

    auto budgets = budgetManager->getBudgetsByCategory(category);

    if (budgets.empty()) {
        std::cout << "No budgets found for category '" << category << "'.\n";
        return;
    }

    std::cout << "\n===== Budgets for Category: " << category << " =====\n";
    displayBudgets(budgets);
}

void BudgetUI::showBudgetsByMonth() {
    std::string yearMonth;
    std::cout << "\n===== View Budgets by Month =====\n";

    // Get and validate year-month
    while (true) {
        std::cout << "Enter year-month (YYYY-MM): ";
        std::getline(std::cin, yearMonth);

        if (DateUtils::validateYearMonth(yearMonth)) {
            break;
        }

        std::cout << "Invalid year-month format. Please use YYYY-MM (e.g., 2023-06).\n";
    }

    auto budgets = budgetManager->getBudgetsByYearMonth(yearMonth);

    if (budgets.empty()) {
        std::cout << "No budgets found for month '" << yearMonth << "'.\n";
        return;
    }

    std::cout << "\n===== Budgets for Month: " << yearMonth << " =====\n";
    displayBudgets(budgets);
}

void BudgetUI::setBudget() {
    std::string category, yearMonth;
    double limitAmount;

    std::cout << "\n===== Set New Budget Limit =====\n";

    // Get category
    std::cout << "Enter category: ";
    std::getline(std::cin, category);

    // Get and validate year-month
    while (true) {
        std::cout << "Enter year-month (YYYY-MM): ";
        std::getline(std::cin, yearMonth);

        if (DateUtils::validateYearMonth(yearMonth)) {
            break;
        }

        std::cout << "Invalid year-month format. Please use YYYY-MM (e.g., 2023-06).\n";
    }

    // Get and validate limit amount
    while (true) {
        std::cout << "Enter budget limit amount ($): ";

        if (!(std::cin >> limitAmount)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        if (limitAmount < 0) {
            std::cout << "Budget limit amount cannot be negative. Please try again.\n";
            continue;
        }

        break;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Check if budget already exists
    if (budgetManager->hasBudget(category, yearMonth)) {
        char choice;
        std::cout << "A budget already exists for " << category << " in " << yearMonth << ".\n";
        std::cout << "Do you want to update it? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::tolower(choice) != 'y') {
            std::cout << "Budget setting canceled.\n";
            return;
        }
    }

    // Create and add/update budget
    auto budget = std::make_shared<Budget>(category, yearMonth, limitAmount);
    budgetManager->addBudget(budget);

    std::cout << "Budget successfully set: " << budget->getDisplayString() << std::endl;
}

void BudgetUI::updateBudget() {
    std::string category, yearMonth;
    double newLimit;

    std::cout << "\n===== Update Existing Budget =====\n";

    // Get category
    std::cout << "Enter category: ";
    std::getline(std::cin, category);

    // Get and validate year-month
    while (true) {
        std::cout << "Enter year-month (YYYY-MM): ";
        std::getline(std::cin, yearMonth);

        if (DateUtils::validateYearMonth(yearMonth)) {
            break;
        }

        std::cout << "Invalid year-month format. Please use YYYY-MM (e.g., 2023-06).\n";
    }

    // Check if the budget exists
    auto existingBudget = budgetManager->getBudget(category, yearMonth);
    if (!existingBudget) {
        std::cout << "No budget found for " << category << " in " << yearMonth << ".\n";

        char choice;
        std::cout << "Would you like to create a new budget instead? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::tolower(choice) != 'y') {
            std::cout << "Budget update canceled.\n";
            return;
        }
    }
    else {
        std::cout << "Current budget: " << existingBudget->getDisplayString() << std::endl;
    }

    // Get new limit amount
    while (true) {
        std::cout << "Enter new budget limit amount ($): ";

        if (!(std::cin >> newLimit)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        if (newLimit < 0) {
            std::cout << "Budget limit amount cannot be negative. Please try again.\n";
            continue;
        }

        break;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Update the budget
    budgetManager->updateBudget(category, yearMonth, newLimit);

    std::cout << "Budget successfully updated.\n";

    // Display the updated budget
    auto updatedBudget = budgetManager->getBudget(category, yearMonth);
    if (updatedBudget) {
        std::cout << "New budget: " << updatedBudget->getDisplayString() << std::endl;
    }
}

void BudgetUI::removeBudget() {
    std::string category, yearMonth;

    std::cout << "\n===== Remove Budget =====\n";

    // Get category
    std::cout << "Enter category: ";
    std::getline(std::cin, category);

    // Get and validate year-month
    while (true) {
        std::cout << "Enter year-month (YYYY-MM): ";
        std::getline(std::cin, yearMonth);

        if (DateUtils::validateYearMonth(yearMonth)) {
            break;
        }

        std::cout << "Invalid year-month format. Please use YYYY-MM (e.g., 2023-06).\n";
    }

    // Check if the budget exists
    auto existingBudget = budgetManager->getBudget(category, yearMonth);
    if (!existingBudget) {
        std::cout << "No budget found for " << category << " in " << yearMonth << ".\n";
        return;
    }

    // Confirm removal
    char choice;
    std::cout << "Are you sure you want to remove this budget? (y/n): ";
    std::cout << existingBudget->getDisplayString() << std::endl;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (std::tolower(choice) != 'y') {
        std::cout << "Budget removal canceled.\n";
        return;
    }

    // Remove the budget
    if (budgetManager->removeBudget(category, yearMonth)) {
        std::cout << "Budget successfully removed.\n";
    }
    else {
        std::cout << "Failed to remove budget.\n";
    }
}

void BudgetUI::displayBudgetUsage(const std::shared_ptr<Budget>& budget) {
    if (!budget) return;

    // Get the category and year-month from the budget
    std::string category = budget->getCategory();
    std::string yearMonth = budget->getYearMonth();
    double budgetLimit = budget->getLimitAmount();

    // Calculate total expenses for this category and month
    double totalExpenses = 0.0;

    // Get all expense transactions
    // TransactionType is an enum class defined in Transaction.h
    auto allTransactions = transactionManager->getTransactionsByType(TransactionType::EXPENSE);

    // Filter by category and month
    for (const auto& transaction : allTransactions) {
        if (transaction->getCategory() == category &&
            transaction->getMonthKey() == yearMonth) {
            totalExpenses += transaction->getAmount();
        }
    }

    // Calculate usage percentage
    double usagePercentage = (budgetLimit > 0) ? (totalExpenses / budgetLimit) * 100.0 : 0.0;

    // Calculate remaining amount
    double remainingAmount = budgetLimit - totalExpenses;

    // Display budget usage information
    std::cout << "Budget: " << budget->getDisplayString() << std::endl;
    std::cout << "Total Spent: $" << std::fixed << std::setprecision(2) << totalExpenses << std::endl;
    std::cout << "Remaining: $" << std::fixed << std::setprecision(2) << remainingAmount << std::endl;
    std::cout << "Usage: " << std::fixed << std::setprecision(1) << usagePercentage << "%" << std::endl;

    // Visual representation of the budget usage
    std::cout << "Usage: [";

    int barWidth = 20;
    int usedWidth = static_cast<int>((usagePercentage > 100 ? 100 : usagePercentage) * barWidth / 100);

    for (int i = 0; i < barWidth; ++i) {
        if (i < usedWidth) {
            std::cout << (usagePercentage > 100 ? "!" : "#");
        }
        else {
            std::cout << " ";
        }
    }

    std::cout << "] " << std::fixed << std::setprecision(1) << usagePercentage << "%" << std::endl;

    // Status message based on budget usage
    if (usagePercentage > 100) {
        std::cout << "Status: OVER BUDGET by $"
            << std::fixed << std::setprecision(2) << -remainingAmount << std::endl;
    }
    else if (usagePercentage >= 90) {
        std::cout << "Status: NEAR LIMIT ($"
            << std::fixed << std::setprecision(2) << remainingAmount << " remaining)" << std::endl;
    }
    else if (usagePercentage >= 75) {
        std::cout << "Status: MODERATE USAGE" << std::endl;
    }
    else {
        std::cout << "Status: WITHIN BUDGET" << std::endl;
    }

    std::cout << std::string(40, '-') << std::endl;
}

void BudgetUI::showBudgetUsageReport() {
    std::string yearMonth;

    std::cout << "\n===== Budget Usage Report =====\n";

    // Default to current month
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    std::string defaultYearMonth = Budget::createYearMonthString(
        localTime->tm_year + 1900, localTime->tm_mon + 1);

    // Get and validate year-month, with option to use default
    std::cout << "Enter year-month (YYYY-MM) or press Enter for current month ("
        << defaultYearMonth << "): ";
    std::getline(std::cin, yearMonth);

    if (yearMonth.empty()) {
        yearMonth = defaultYearMonth;
    }
    else {
        while (!DateUtils::validateYearMonth(yearMonth)) {
            std::cout << "Invalid year-month format. Please use YYYY-MM (e.g., 2023-06): ";
            std::getline(std::cin, yearMonth);

            if (yearMonth.empty()) {
                yearMonth = defaultYearMonth;
                break;
            }
        }
    }

    // Get all budgets for the specified month
    auto budgets = budgetManager->getBudgetsByYearMonth(yearMonth);

    if (budgets.empty()) {
        std::cout << "No budgets found for month " << yearMonth << ".\n";
        return;
    }

    std::cout << "\n===== Budget Usage Report for " << yearMonth << " =====\n";

    // Display usage for each budget
    for (const auto& budget : budgets) {
        displayBudgetUsage(budget);
    }
}