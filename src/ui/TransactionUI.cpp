#include "../../include/ui/TransactionUI.h"

TransactionUI::TransactionUI(std::shared_ptr<TransactionManager> manager) : transactionManager(manager) {
    // Load existing transactions from storage
    transactionManager->loadTransactions();
}

void TransactionUI::displayTransactionsMenu() const {
    std::cout << "\n===== Transaction Management =====\n";
    std::cout << "1. Show All Transactions\n";
    std::cout << "2. Filter by Category\n";
    std::cout << "3. Filter by Type (Income/Expense)\n";
    std::cout << "4. Filter by Date Range\n";
    std::cout << "5. Filter by Amount Range\n";  // Added this option
    std::cout << "6. View by Month\n";
    std::cout << "7. View Monthly Summary\n";
    std::cout << "8. Add New Transaction\n";     // Moved this down by one
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Enter your choice (0-8): ";    // Updated range
}

void TransactionUI::displayTransactionHeader() const {
    std::cout << "\n";
    std::cout << std::left << std::setw(12) << "Date"
        << std::setw(12) << "Type"
        << std::setw(15) << "Category"
        << std::right << std::setw(15) << "Amount (₹)"
        << "\n";
    std::cout << std::string(55, '-') << "\n";
}

void TransactionUI::displayTransactions(const std::vector<std::shared_ptr<Transaction>>& transactions) const {
    for (const auto& transaction : transactions) {
        std::cout << std::left << std::setw(12) << transaction->getFormattedDate()
            << std::setw(12) << transaction->getTypeAsString()
            << std::setw(15) << transaction->getCategory()
            << std::right << std::setw(15) << transaction->getFormattedAmount()
            << "\n";
    }
    std::cout << std::string(55, '-') << "\n";
}

void TransactionUI::displayFinancialSummary() const {
    double totalIncome = transactionManager->getTotalIncome();
    double totalExpenses = transactionManager->getTotalExpenses();
    double netAmount = transactionManager->getNetAmount();

    std::cout << "\n===== Financial Summary =====\n";
    std::cout << "Total Income: ₹" << std::fixed << std::setprecision(2) << totalIncome << "\n";
    std::cout << "Total Expenses: ₹" << std::fixed << std::setprecision(2) << totalExpenses << "\n";
    std::cout << "Net Amount: ₹" << std::fixed << std::setprecision(2) << netAmount << "\n";

    if (netAmount > 0) {
        std::cout << "Status: You have a surplus of ₹" << std::fixed << std::setprecision(2) << netAmount << "\n";
    }
    else if (netAmount < 0) {
        std::cout << "Status: You have a deficit of ₹" << std::fixed << std::setprecision(2) << std::abs(netAmount) << "\n";
    }
    else {
        std::cout << "Status: Your budget is balanced (income equals expenses)\n";
    }
}

void TransactionUI::displayMonthlySummaryHeader() const {
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "Month"
        << std::right << std::setw(15) << "Income (₹)"
        << std::setw(15) << "Expenses (₹)"
        << std::setw(15) << "Net (₹)"
        << "\n";
    std::cout << std::string(60, '-') << "\n";
}

bool TransactionUI::validateDoubleInput(double& value, const std::string& prompt) const {
    std::cout << prompt;
    std::cin >> value;

    if (std::cin.fail() || value < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a positive number.\n";
        return false;
    }

    return true;
}

bool TransactionUI::validateDateInput(std::string& dateStr, const std::string& prompt) const {
    std::cout << prompt;
    std::cin >> dateStr;

    if (!DateUtils::validateDateString(dateStr)) {
        std::cout << "Invalid date format. Please use YYYY-MM-DD format.\n";
        return false;
    }

    return true;
}

bool TransactionUI::validateCategoryInput(std::string& category, const std::string& prompt) const {
    std::cout << prompt;
    std::cin.ignore(); // Clear previous input
    std::getline(std::cin, category);

    if (category.empty()) {
        std::cout << "Category cannot be empty.\n";
        return false;
    }

    return true;
}

std::shared_ptr<Transaction> TransactionUI::createTransaction(TransactionType type) {
    double amount = 0.0;
    std::string dateStr;
    std::string category;

    // Get and validate transaction amount
    if (!validateDoubleInput(amount, "Enter amount (₹): ")) {
        return nullptr;
    }

    // Get and validate transaction date
    if (!validateDateInput(dateStr, "Enter date (YYYY-MM-DD): ")) {
        return nullptr;
    }

    // Get and validate transaction category
    if (!validateCategoryInput(category, "Enter category: ")) {
        return nullptr;
    }

    // Create a new Transaction object
    time_t date = DateUtils::stringToTime(dateStr);
    return std::make_shared<Transaction>(amount, date, category, type);
}

void TransactionUI::showAllTransactions() const {
    auto transactions = transactionManager->getAllTransactions();

    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    displayTransactionHeader();
    displayTransactions(transactions);

    std::cout << "\nTotal Transactions: " << transactions.size() << "\n";
    displayFinancialSummary();
}

void TransactionUI::showTransactionsByCategory() const {
    std::string category;
    std::cout << "\nEnter category to filter by: ";
    std::cin.ignore(); // Clear previous input
    std::getline(std::cin, category);

    auto transactions = transactionManager->getTransactionsByCategory(category);

    if (transactions.empty()) {
        std::cout << "No transactions found for category '" << category << "'.\n";
        return;
    }

    displayTransactionHeader();
    displayTransactions(transactions);

    std::cout << "\nFound " << transactions.size() << " transaction(s) in category '" << category << "'.\n";
}

void TransactionUI::showTransactionsByType() const {
    int typeChoice;
    std::cout << "\n1. Income Transactions\n";
    std::cout << "2. Expense Transactions\n";
    std::cout << "Enter choice (1-2): ";
    std::cin >> typeChoice;

    if (std::cin.fail() || typeChoice < 1 || typeChoice > 2) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Operation cancelled.\n";
        return;
    }

    TransactionType type = (typeChoice == 1) ? TransactionType::INCOME : TransactionType::EXPENSE;
    auto transactions = transactionManager->getTransactionsByType(type);

    if (transactions.empty()) {
        std::cout << "No " << ((type == TransactionType::INCOME) ? "income" : "expense") << " transactions found.\n";
        return;
    }

    displayTransactionHeader();
    displayTransactions(transactions);

    std::cout << "\nFound " << transactions.size() << " "
        << ((type == TransactionType::INCOME) ? "income" : "expense")
        << " transaction(s).\n";
}

void TransactionUI::showTransactionsByDateRange() const {
    std::string startDateStr, endDateStr;

    std::cout << "\n===== Filter Transactions by Date Range =====\n";
    if (!validateDateInput(startDateStr, "Enter start date (YYYY-MM-DD): ") ||
        !validateDateInput(endDateStr, "Enter end date (YYYY-MM-DD): ")) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    time_t startDate = DateUtils::stringToTime(startDateStr);
    time_t endDate = DateUtils::stringToTime(endDateStr);

    if (startDate > endDate) {
        std::cout << "Error: Start date cannot be later than end date.\n";
        return;
    }

    auto transactions = transactionManager->getTransactionsByDateRange(startDate, endDate);

    if (transactions.empty()) {
        std::cout << "No transactions found between " << startDateStr << " and " << endDateStr << ".\n";
        return;
    }

    displayTransactionHeader();
    displayTransactions(transactions);

    std::cout << "\nFound " << transactions.size() << " transaction(s) between "
        << startDateStr << " and " << endDateStr << ".\n";
}

void TransactionUI::showTransactionsByAmountRange() {
    std::cout << "\n===== Filter Transactions by Amount Range =====\n";

    double minAmount, maxAmount;

    // Get minimum amount with validation
    while (true) {
        std::cout << "Enter minimum amount (₹): ";
        if (!(std::cin >> minAmount)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }
        if (minAmount < 0) {
            std::cout << "Minimum amount cannot be negative. Please try again.\n";
            continue;
        }
        break;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get maximum amount with validation
    while (true) {
        std::cout << "Enter maximum amount (₹): ";
        if (!(std::cin >> maxAmount)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }
        if (maxAmount < 0) {
            std::cout << "Maximum amount cannot be negative. Please try again.\n";
            continue;
        }
        if (maxAmount < minAmount) {
            std::cout << "Maximum amount must be greater than or equal to minimum amount (" << minAmount << "₹). Please try again.\n";
            continue;
        }
        break;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get filtered transactions
    std::vector<std::shared_ptr<Transaction>> filteredTransactions =
        transactionManager->getTransactionsByAmountRange(minAmount, maxAmount);

    if (filteredTransactions.empty()) {
        std::cout << "No transactions found in the range of ₹" << minAmount << " to ₹" << maxAmount << ".\n";
        return;
    }

    // Display the filtered transactions
    std::cout << "\n===== Transactions between ₹" << minAmount << " and ₹" << maxAmount << " =====\n";
    displayTransactions(filteredTransactions);
}

void TransactionUI::showTransactionsByMonth() const {
    std::string yearMonth;
    std::cout << "\nEnter month (YYYY-MM): ";
    std::cin >> yearMonth;

    if (!DateUtils::validateYearMonth(yearMonth)) {
        std::cout << "Invalid month format. Please use YYYY-MM format.\n";
        return;
    }

    // Get all transactions
    auto allTransactions = transactionManager->getAllTransactions();

    // Filter by month manually (assumes getMonthKey() returns YYYY-MM)
    std::vector<std::shared_ptr<Transaction>> monthTransactions;
    for (const auto& transaction : allTransactions) {
        if (transaction->getMonthKey() == yearMonth) {
            monthTransactions.push_back(transaction);
        }
    }

    if (monthTransactions.empty()) {
        std::cout << "No transactions found for month " << yearMonth << ".\n";
        return;
    }

    displayTransactionHeader();
    displayTransactions(monthTransactions);

    std::cout << "\nFound " << monthTransactions.size() << " transaction(s) for month " << yearMonth << ".\n";
}

void TransactionUI::showMonthlySummary() const {
    auto monthlySummary = transactionManager->calculateMonthlySummary();

    if (monthlySummary.empty()) {
        std::cout << "\nNo transaction data available for monthly summary.\n";
        return;
    }

    displayMonthlySummaryHeader();

    for (const auto& [month, summary] : monthlySummary) {
        double income = std::get<0>(summary);
        double expenses = std::get<1>(summary);
        double net = std::get<2>(summary);

        std::cout << std::left << std::setw(15) << month
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(15) << income
            << std::setw(15) << expenses
            << std::setw(15) << net
            << "\n";
    }

    std::cout << std::string(60, '-') << "\n";
}

void TransactionUI::addNewTransaction() {
    int typeChoice;
    std::cout << "\n1. Add Income\n";
    std::cout << "2. Add Expense\n";
    std::cout << "Enter choice (1-2): ";
    std::cin >> typeChoice;

    if (std::cin.fail() || typeChoice < 1 || typeChoice > 2) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Operation cancelled.\n";
        return;
    }

    if (typeChoice == 1) {
        addIncomeTransaction();
    }
    else {
        addExpenseTransaction();
    }
}

void TransactionUI::addIncomeTransaction() {
    std::cout << "\n===== Add Income Transaction =====\n";

    auto transaction = createTransaction(TransactionType::INCOME);
    if (transaction) {
        transactionManager->addTransaction(transaction);
        transactionManager->saveTransactions();
        std::cout << "Income transaction added successfully.\n";
    }
}

void TransactionUI::addExpenseTransaction() {
    std::cout << "\n===== Add Expense Transaction =====\n";

    auto transaction = createTransaction(TransactionType::EXPENSE);
    if (transaction) {
        transactionManager->addTransaction(transaction);
        transactionManager->saveTransactions();
        std::cout << "Expense transaction added successfully.\n";
    }
}