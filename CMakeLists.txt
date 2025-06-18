# CMakeList.txt : CMake project for Budget-Expense-Manager, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Budget-Expense-Manager")

# Add source to this project's executable.
add_executable (Budget-Expense-Manager "src/main.cpp" "include/main.h" "include/models/Transaction.h" "src/models/Transaction.cpp" "include/services/TransactionManager.h" "src/services/TransactionManager.cpp" "include/ui/TransactionInput.h" "src/ui/TransactionInput.cpp" "include/services/CategoryManager.h" "src/services/CategoryManager.cpp" "include/ui/CategoryManagementUI.h" "src/ui/CategoryManagementUI.cpp" "include/utils/DateUtils.h" "include/utils/FileUtils.h" "include/ui/TransactionUI.h" "src/ui/TransactionUI.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Budget-Expense-Manager PROPERTY CXX_STANDARD 20)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/models/Transaction.cpp
    src/services/TransactionManager.cpp
    src/services/CategoryManager.cpp
    src/ui/TransactionInput.cpp
    src/ui/CategoryManagementUI.cpp
    # Note: FileUtils is header-only, so no .cpp file is needed
)