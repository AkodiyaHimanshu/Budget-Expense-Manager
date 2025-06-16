#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <sstream>
#include "../include/services/TransactionManager.h"
#include "../include/models/Transaction.h"
#include "../include/ui/TransactionInput.h"

// Mock class for testing TransactionInput
class MockTransactionManager : public TransactionManager {
public:
    MOCK_CONST_METHOD0(getMonthlyTransactionSummaries, const std::map<std::string, MonthlySummary>& ());
};

class MockCategoryManager : public CategoryManager {
    // Empty mock implementation for testing
};

// Test fixture for TransactionManager tests
class TransactionManagerTest : public ::testing::Test {
protected:
    TransactionManager manager;
    time_t baseTime;

    void SetUp() override {
        // Create a base time for consistent test results (2023-01-01)
        struct tm timeInfo = {};
        timeInfo.tm_year = 2023 - 1900;  // Years since 1900
        timeInfo.tm_mon = 0;             // 0-indexed month (January)
        timeInfo.tm_mday = 1;            // Day of month
        baseTime = mktime(&timeInfo);
    }

    // Helper to create transactions with relative months (0 = January 2023, 1 = February 2023, etc.)
    std::shared_ptr<Transaction> createTransaction(double amount, int monthOffset,
        TransactionType type, const std::string& category) {
        struct tm timeInfo = *localtime(&baseTime);
        timeInfo.tm_mon += monthOffset;
        mktime(&timeInfo);  // Normalize the time (handling year changes if month > 11)

        return std::make_shared<Transaction>(amount, mktime(&timeInfo), category, type);
    }
};

// Test fixture for TransactionInput tests
class TransactionInputTest : public ::testing::Test {
protected:
    ::testing::NiceMock<MockTransactionManager> mockManager;
    MockCategoryManager mockCategoryManager;
    TransactionInput transactionInput;
    std::stringstream testOutput;
    std::streambuf* originalCoutBuffer;

    // Constructor to initialize TransactionInput with mock managers
    TransactionInputTest() : transactionInput(mockManager, mockCategoryManager) {}

    void SetUp() override {
        // Redirect cout to our stringstream for testing output
        originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(testOutput.rdbuf());
    }

    void TearDown() override {
        // Restore the original cout buffer
        std::cout.rdbuf(originalCoutBuffer);
    }

    // Helper function to create a MonthlySummary for testing
    MonthlySummary createMonthlySummary(double income, double expenses) {
        MonthlySummary summary = { income, expenses, 0.0 };
        summary.updateNetAmount();
        return summary;
    }
};

// Test case: getMonthlyTransactionSummaries with no transactions
TEST_F(TransactionManagerTest, GetMonthlyTransactionSummaries_NoTransactions) {
    // Act: Get monthly summaries when there are no transactions
    const auto& summaries = manager.getMonthlyTransactionSummaries();

    // Assert: The result should be an empty map
    EXPECT_TRUE(summaries.empty());
}

// Test case: getMonthlyTransactionSummaries with single month of data
TEST_F(TransactionManagerTest, GetMonthlyTransactionSummaries_SingleMonth) {
    // Arrange: Add a few transactions for the same month
    manager.addTransaction(createTransaction(100.0, 0, TransactionType::INCOME, "Salary"));
    manager.addTransaction(createTransaction(50.0, 0, TransactionType::EXPENSE, "Groceries"));
    manager.addTransaction(createTransaction(25.0, 0, TransactionType::EXPENSE, "Utilities"));

    // Act: Get monthly summaries
    const auto& summaries = manager.getMonthlyTransactionSummaries();

    // Assert: Should have exactly one month with correct totals
    ASSERT_EQ(1, summaries.size());

    auto it = summaries.begin();
    EXPECT_EQ("2023-01", it->first);
    EXPECT_DOUBLE_EQ(100.0, it->second.totalIncome);
    EXPECT_DOUBLE_EQ(75.0, it->second.totalExpenses);
    EXPECT_DOUBLE_EQ(25.0, it->second.netAmount);
}

// Test case: getMonthlyTransactionSummaries with multiple months
TEST_F(TransactionManagerTest, GetMonthlyTransactionSummaries_MultipleMonths) {
    // Arrange: Add transactions for different months
    manager.addTransaction(createTransaction(1000.0, 0, TransactionType::INCOME, "Salary January"));   // Jan
    manager.addTransaction(createTransaction(500.0, 0, TransactionType::EXPENSE, "Rent January"));     // Jan

    manager.addTransaction(createTransaction(1000.0, 1, TransactionType::INCOME, "Salary February"));  // Feb
    manager.addTransaction(createTransaction(600.0, 1, TransactionType::EXPENSE, "Rent February"));     // Feb

    manager.addTransaction(createTransaction(1200.0, 2, TransactionType::INCOME, "Salary March"));     // Mar
    manager.addTransaction(createTransaction(600.0, 2, TransactionType::EXPENSE, "Rent March"));       // Mar

    // Act: Get monthly summaries
    const auto& summaries = manager.getMonthlyTransactionSummaries();

    // Assert: Should have three months with correct totals
    ASSERT_EQ(3, summaries.size());

    // Check January summary
    auto janIt = summaries.find("2023-01");
    ASSERT_NE(summaries.end(), janIt);
    EXPECT_DOUBLE_EQ(1000.0, janIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(500.0, janIt->second.totalExpenses);
    EXPECT_DOUBLE_EQ(500.0, janIt->second.netAmount);

    // Check February summary
    auto febIt = summaries.find("2023-02");
    ASSERT_NE(summaries.end(), febIt);
    EXPECT_DOUBLE_EQ(1000.0, febIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(600.0, febIt->second.totalExpenses);
    EXPECT_DOUBLE_EQ(400.0, febIt->second.netAmount);

    // Check March summary
    auto marIt = summaries.find("2023-03");
    ASSERT_NE(summaries.end(), marIt);
    EXPECT_DOUBLE_EQ(1200.0, marIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(600.0, marIt->second.totalExpenses);
    EXPECT_DOUBLE_EQ(600.0, marIt->second.netAmount);
}

// Test case: getMonthlyTransactionSummaries with cache invalidation
TEST_F(TransactionManagerTest, GetMonthlyTransactionSummaries_CacheInvalidation) {
    // Arrange: Add transactions and get summaries to populate cache
    manager.addTransaction(createTransaction(1000.0, 0, TransactionType::INCOME, "Salary"));
    manager.addTransaction(createTransaction(500.0, 0, TransactionType::EXPENSE, "Rent"));

    // First call to populate cache
    const auto& summaries1 = manager.getMonthlyTransactionSummaries();
    ASSERT_EQ(1, summaries1.size());

    // Act: Add a new transaction that affects the same month
    manager.addTransaction(createTransaction(200.0, 0, TransactionType::EXPENSE, "Groceries"));

    // Get summaries again - should reflect the new transaction
    const auto& summaries2 = manager.getMonthlyTransactionSummaries();

    // Assert: Should have updated totals
    ASSERT_EQ(1, summaries2.size());
    auto it = summaries2.begin();
    EXPECT_EQ("2023-01", it->first);
    EXPECT_DOUBLE_EQ(1000.0, it->second.totalIncome);
    EXPECT_DOUBLE_EQ(700.0, it->second.totalExpenses);  // 500 + 200
    EXPECT_DOUBLE_EQ(300.0, it->second.netAmount);      // 1000 - 700
}

// Test case: getMonthlyTransactionSummaries with new month added
TEST_F(TransactionManagerTest, GetMonthlyTransactionSummaries_NewMonthAdded) {
    // Arrange: Add transactions for January and get summaries
    manager.addTransaction(createTransaction(1000.0, 0, TransactionType::INCOME, "Salary January"));
    manager.addTransaction(createTransaction(500.0, 0, TransactionType::EXPENSE, "Rent January"));

    // First call to populate cache with January data
    const auto& summaries1 = manager.getMonthlyTransactionSummaries();
    ASSERT_EQ(1, summaries1.size());

    // Act: Add transactions for a different month
    manager.addTransaction(createTransaction(1100.0, 1, TransactionType::INCOME, "Salary February"));
    manager.addTransaction(createTransaction(550.0, 1, TransactionType::EXPENSE, "Rent February"));

    // Get summaries again - should include the new month
    const auto& summaries2 = manager.getMonthlyTransactionSummaries();

    // Assert: Should now have two months
    ASSERT_EQ(2, summaries2.size());

    // Check January summary (should be unchanged)
    auto janIt = summaries2.find("2023-01");
    ASSERT_NE(summaries2.end(), janIt);
    EXPECT_DOUBLE_EQ(1000.0, janIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(500.0, janIt->second.totalExpenses);
    EXPECT_DOUBLE_EQ(500.0, janIt->second.netAmount);

    // Check February summary (should be newly added)
    auto febIt = summaries2.find("2023-02");
    ASSERT_NE(summaries2.end(), febIt);
    EXPECT_DOUBLE_EQ(1100.0, febIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(550.0, febIt->second.totalExpenses);
    EXPECT_DOUBLE_EQ(550.0, febIt->second.netAmount);
}

// Test case: displayAllMonthlySummaries with no transaction data
TEST_F(TransactionInputTest, DisplayAllMonthlySummaries_NoTransactions) {
    // Arrange: Set up mock to return empty map
    std::map<std::string, MonthlySummary> emptyMap;
    EXPECT_CALL(mockManager, getMonthlyTransactionSummaries())
        .WillOnce(::testing::ReturnRef(emptyMap));

    // Act: Call displayAllMonthlySummaries
    transactionInput.displayAllMonthlySummaries();

    // Assert: Output should indicate no data
    EXPECT_TRUE(testOutput.str().find("No transaction data available") != std::string::npos);
}

// Test case: displayAllMonthlySummaries with single month of data
TEST_F(TransactionInputTest, DisplayAllMonthlySummaries_SingleMonth) {
    // Arrange: Set up mock to return a map with one month
    std::map<std::string, MonthlySummary> monthlyData;
    monthlyData["2023-01"] = createMonthlySummary(1000.0, 500.0);

    EXPECT_CALL(mockManager, getMonthlyTransactionSummaries())
        .WillOnce(::testing::ReturnRef(monthlyData));

    // Act: Call displayAllMonthlySummaries
    transactionInput.displayAllMonthlySummaries();

    // Assert: Output should contain the month and correct values
    std::string output = testOutput.str();
    EXPECT_TRUE(output.find("2023-01") != std::string::npos);
    EXPECT_TRUE(output.find("1000.00") != std::string::npos);  // Income
    EXPECT_TRUE(output.find("500.00") != std::string::npos);   // Expenses
    EXPECT_TRUE(output.find("Surplus") != std::string::npos);  // Status
}

// Test case: displayAllMonthlySummaries with multiple months including deficit
TEST_F(TransactionInputTest, DisplayAllMonthlySummaries_MultipleMonthsMixedStatus) {
    // Arrange: Set up mock to return a map with multiple months
    std::map<std::string, MonthlySummary> monthlyData;
    monthlyData["2023-01"] = createMonthlySummary(1000.0, 500.0);    // Surplus
    monthlyData["2023-02"] = createMonthlySummary(500.0, 800.0);     // Deficit
    monthlyData["2023-03"] = createMonthlySummary(700.0, 700.0);     // Balanced

    EXPECT_CALL(mockManager, getMonthlyTransactionSummaries())
        .WillOnce(::testing::ReturnRef(monthlyData));

    // Act: Call displayAllMonthlySummaries
    transactionInput.displayAllMonthlySummaries();

    // Assert: Output should contain all months and correct statuses
    std::string output = testOutput.str();

    // Check January (surplus)
    EXPECT_TRUE(output.find("2023-01") != std::string::npos);
    EXPECT_TRUE(output.find("Surplus") != std::string::npos);

    // Check February (deficit)
    EXPECT_TRUE(output.find("2023-02") != std::string::npos);
    EXPECT_TRUE(output.find("Deficit") != std::string::npos);

    // Check March (balanced)
    EXPECT_TRUE(output.find("2023-03") != std::string::npos);
    EXPECT_TRUE(output.find("Balanced") != std::string::npos);

    // Check totals (should sum all months)
    EXPECT_TRUE(output.find("TOTAL") != std::string::npos);
    EXPECT_TRUE(output.find("2200.00") != std::string::npos);  // Total income
    EXPECT_TRUE(output.find("2000.00") != std::string::npos);  // Total expenses
}

// Test case: Specific month cache invalidation when adding a transaction
TEST_F(TransactionManagerTest, CacheInvalidation_SpecificMonth) {
    // Arrange: Add transactions for January and February
    manager.addTransaction(createTransaction(1000.0, 0, TransactionType::INCOME, "Salary January"));   // Jan
    manager.addTransaction(createTransaction(500.0, 0, TransactionType::EXPENSE, "Rent January"));     // Jan
    manager.addTransaction(createTransaction(1200.0, 1, TransactionType::INCOME, "Salary February"));  // Feb
    manager.addTransaction(createTransaction(600.0, 1, TransactionType::EXPENSE, "Rent February"));    // Feb

    // Populate the cache with initial data
    const auto& initialSummaries = manager.getMonthlyTransactionSummaries();
    ASSERT_EQ(2, initialSummaries.size());

    // Verify initial data
    auto janIt = initialSummaries.find("2023-01");
    EXPECT_DOUBLE_EQ(1000.0, janIt->second.totalIncome);
    EXPECT_DOUBLE_EQ(500.0, janIt->second.totalExpenses);

    // Act: Add a new transaction for January only
    manager.addTransaction(createTransaction(200.0, 0, TransactionType::INCOME, "Bonus January"));

    // Fetch updated summaries
    const auto& updatedSummaries = manager.getMonthlyTransactionSummaries();

    // Assert: 
    // 1. January should be updated
    auto updatedJanIt = updatedSummaries.find("2023-01");
    EXPECT_DOUBLE_EQ(1200.0, updatedJanIt->second.totalIncome);  // 1000 + 200
    EXPECT_DOUBLE_EQ(500.0, updatedJanIt->second.totalExpenses);  // Unchanged

    // 2. February should remain the same
    auto febIt = updatedSummaries.find("2023-02");
    EXPECT_DOUBLE_EQ(1200.0, febIt->second.totalIncome);  // Unchanged
    EXPECT_DOUBLE_EQ(600.0, febIt->second.totalExpenses);  // Unchanged
}

// Test case: Edge case with empty cache and immediate transaction addition
TEST_F(TransactionManagerTest, Edge_EmptyCacheWithImmediateAddition) {
    // Initially verify cache is empty
    const auto& emptySummaries = manager.getMonthlyTransactionSummaries();
    EXPECT_TRUE(emptySummaries.empty());

    // Add a transaction
    manager.addTransaction(createTransaction(1000.0, 0, TransactionType::INCOME, "Salary"));

    // Get summaries - should contain the new transaction
    const auto& summaries = manager.getMonthlyTransactionSummaries();

    // Verify results
    ASSERT_EQ(1, summaries.size());
    auto it = summaries.begin();
    EXPECT_EQ("2023-01", it->first);
    EXPECT_DOUBLE_EQ(1000.0, it->second.totalIncome);
    EXPECT_DOUBLE_EQ(0.0, it->second.totalExpenses);
    EXPECT_DOUBLE_EQ(1000.0, it->second.netAmount);
}

// Additional mocks for input handling
class InputTestHelper {
public:
    // Set up the input for testing with a predefined response
    static void setNextInput(const std::string& input) {
        // Save original buffer
        std::cin.rdbuf(originalCinBuffer);

        // Clear any existing content in the input stream
        std::stringstream* ss = dynamic_cast<std::stringstream*>(&mockCin);
        if (ss) {
            ss->str("");
            ss->clear();
        }

        // Set the new mock input
        mockCin << input << std::endl;

        // Redirect cin to our mock stream
        std::cin.rdbuf(mockCin.rdbuf());
    }

    // Reset to original cin
    static void resetCin() {
        if (originalCinBuffer) {
            std::cin.rdbuf(originalCinBuffer);
        }
    }

    // Initialize the mock system
    static void initMockInput() {
        // Save the original input buffer if not already saved
        if (!originalCinBuffer) {
            originalCinBuffer = std::cin.rdbuf();
        }

        // Redirect cin to our mockCin
        std::cin.rdbuf(mockCin.rdbuf());
    }

private:
    static std::stringstream mockCin;
    static std::streambuf* originalCinBuffer;
};

std::stringstream InputTestHelper::mockCin;
std::streambuf* InputTestHelper::originalCinBuffer = nullptr;

// Test for displayMonthlySummary with valid input
TEST_F(TransactionInputTest, DisplayMonthlySummary_ValidInput) {
    // Initialize mock for cin
    InputTestHelper::initMockInput();

    // Set up the mock input to provide a valid yearMonth when prompted
    InputTestHelper::setNextInput("2023-01");

    // Mock the TransactionManager's calculateMonthlySummary to return a predefined summary
    MonthlySummary mockSummary = { 1500.0, 800.0, 700.0 };
    EXPECT_CALL(mockManager, calculateMonthlySummary(testing::StrEq("2023-01")))
        .WillOnce(testing::Return(mockSummary));

    // Act: Call the method that will prompt for input and display results
    transactionInput.displayMonthlySummary();

    // Reset cin to its original state
    InputTestHelper::resetCin();

    // Get the output and check it
    std::string output = testOutput.str();

    // Verify the output contains expected values
    EXPECT_TRUE(output.find("Monthly Summary for 2023-01") != std::string::npos);
    EXPECT_TRUE(output.find("1500.00") != std::string::npos);  // Income
    EXPECT_TRUE(output.find("800.00") != std::string::npos);   // Expenses
    EXPECT_TRUE(output.find("700.00") != std::string::npos);   // Net amount
    EXPECT_TRUE(output.find("Surplus") != std::string::npos);  // Status
}

// Test for YYYY-MM format validation in getMonthlyTransactionSummaries
TEST_F(TransactionManagerTest, FormatValidation_YearMonth) {
    // Non-integer Month - Should Throw
    try {
        manager.getTransactionsByMonth("2023-AB");
        FAIL() << "Expected std::invalid_argument for non-integer month";
    }
    catch (const std::invalid_argument& e) {
        EXPECT_TRUE(std::string(e.what()).find("Invalid year or month format") != std::string::npos);
    }
    catch (...) {
        FAIL() << "Expected std::invalid_argument, got different exception";
    }

    // Month out of range - Should Throw
    try {
        manager.getTransactionsByMonth("2023-13");
        FAIL() << "Expected std::invalid_argument for month > 12";
    }
    catch (const std::invalid_argument& e) {
        EXPECT_TRUE(std::string(e.what()).find("Month must be between") != std::string::npos);
    }
    catch (...) {
        FAIL() << "Expected std::invalid_argument, got different exception";
    }

    // Wrong format (missing hyphen) - Should Throw
    try {
        manager.getTransactionsByMonth("202301");
        FAIL() << "Expected std::invalid_argument for missing hyphen";
    }
    catch (const std::invalid_argument& e) {
        EXPECT_TRUE(std::string(e.what()).find("YYYY-MM") != std::string::npos);
    }
    catch (...) {
        FAIL() << "Expected std::invalid_argument, got different exception";
    }

    // Correct format - Should Not Throw
    try {
        manager.getTransactionsByMonth("2023-01");
        // Test passes if no exception is thrown
    }
    catch (...) {
        FAIL() << "No exception expected for valid format";
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}