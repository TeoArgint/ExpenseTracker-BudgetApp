#include <cassert>
#include <iostream>
#include "repo/InMemoryExpenseRepository.h"
#include "service/ExpenseService.h"

void testAddAndGetAll() {
    InMemoryExpenseRepository repo;
    ExpenseService srv(repo);

    srv.addExpense("121", 300, "food", "2026-08-25", "hungry");

    auto all = srv.getAll();

    assert(all.size() == 1);
    assert(all[0].getId() == "121");
    assert(all[0].getAmount() == 300);

    std::cout << "Test add + getAll passed!\n";
}