#include <filesystem>
#include "repo/FileExpenseRepository.h"
#include "service/ExpenseService.h"
#include "ui/ConsoleUI.h"
#include "tests/temporaryTest.h"

int main() {
    std::filesystem::create_directories("data"); // creează data/ dacă lipsește
    testAddAndGetAll();
    FileExpenseRepository repo("data/expenses.csv");
    ExpenseService srv(repo);
    ConsoleUI ui(srv);
    ui.run();
    return 0;
}