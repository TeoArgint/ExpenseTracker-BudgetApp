#include "repo/FileExpenseRepository.h"
#include "repo/FileBudgetStateRepository.h"
#include "service/ExpenseService.h"
#include "ui/ConsoleUI.h"

int main(){
    FileExpenseRepository repo("data/expense.csv");
    FileBudgetStateRepository state("data/budget_state.txt");

    ExpenseService srv(repo,state);
    ConsoleUI ui(srv);

    ui.run();
    return 0;
}
