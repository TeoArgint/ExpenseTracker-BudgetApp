#include "repo/InMemoryExpenseRepository.h"
#include "service/ExpenseService.h"
#include <cassert>

int main() {
    InMemoryExpenseRepository r;
    ExpenseService s(r);

    s.addExpense("13", 50, "food", "2026-03-01", "kebab");
    s.addExpense("242", 100, "rent", "2026-03-02", "march");
    s.addExpense("322", 20, "food", "2026-03-03", "coffee");

    assert(s.filterByCategory("food").size() == 2);
    assert(s.filterByDateRange("2026-03-01", "2026-03-02").size() == 2);
    assert(s.filterByAmountRange(30, 120).size() == 2);

    auto byCat = s.totalByCategory();
    assert(byCat["food"] == 70);

    auto byMonth = s.totalByMonth();
    assert(byMonth["2026-03"] == 170);

    return 0;
}