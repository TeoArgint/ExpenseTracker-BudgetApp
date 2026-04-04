cmake_minimum_required(VERSION 3.20)
project(ExpenseTracker_BudgetApp LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(expense_core
        src/app/AppSupport.cpp
        src/domain/Expense.cpp
        src/domain/BudgetState.cpp
        src/repo/InMemoryExpenseRepository.cpp
        src/repo/FileExpenseRepository.cpp
        src/repo/FileBudgetStateRepository.cpp
        src/service/ExpenseService.cpp
        src/ui/ConsoleUI.cpp
        src/validation/Validators.cpp
)

target_include_directories(expense_core PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(ExpenseTracker_BudgetApp MACOSX_BUNDLE src/app/main.mm)
target_compile_definitions(ExpenseTracker_BudgetApp PRIVATE APP_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_link_libraries(ExpenseTracker_BudgetApp PRIVATE expense_core "-framework AppKit" "-framework Foundation")

enable_testing()

add_executable(test_repo_inmemory tests/test_repo_inmemory.cpp)
target_link_libraries(test_repo_inmemory PRIVATE expense_core)
add_test(NAME test_repo_inmemory COMMAND test_repo_inmemory)

add_executable(test_service tests/test_service.cpp)
target_link_libraries(test_service PRIVATE expense_core)
add_test(NAME test_service COMMAND test_service)
