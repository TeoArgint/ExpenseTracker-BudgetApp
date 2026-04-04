#include "repo/InMemoryExpenseRepository.h"
#include "repo/FileBudgetStateRepository.h"
#include "service/ExpenseService.h"

int main() {
    InMemoryExpenseRepository repo;
    FileBudgetStateRepository state("state.txt");

    ExpenseService srv(repo, state);

    srv.addExpense(121, 300, "food", "2026-08-25", "hungry");

    return 0;
}