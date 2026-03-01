#include "repo/InMemoryExpenseRepository.h"
#include <cassert>

int main() {
    InMemoryExpenseRepository r;

    r.add(Expense(1, 10.5, "food", "2026-03-01", "pizza"));
    assert(r.getAll().size() == 1);
    assert(r.findById(1).getAmount() == 10.5);

    r.update(Expense(1, 20.0, "food", "2026-03-01", "pizza2"));
    assert(r.findById(1).getAmount() == 20.0);

    r.remove(1);
    assert(r.getAll().empty());

    return 0;
}