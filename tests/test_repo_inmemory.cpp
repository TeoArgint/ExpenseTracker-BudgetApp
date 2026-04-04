#include "repo/InMemoryExpenseRepository.h"
#include <cassert>
#include <stdexcept>

int main() {
    InMemoryExpenseRepository repo;

    repo.add(Expense(11, 10.5, "food", "2026-03-01", "pizza"));
    assert(repo.getAll().size() == 1);
    assert(repo.find(11).getAmount() == 10.5);

    repo.update(Expense(11, 20.0, "food", "2026-03-01", "pasta"));
    assert(repo.find(11).getAmount() == 20.0);
    assert(repo.find(11).getDescription() == "pasta");

    bool duplicateRejected = false;
    try {
        repo.add(Expense(11, 30.0, "transport", "2026-03-02", "bus"));
    } catch (const std::runtime_error&) {
        duplicateRejected = true;
    }
    assert(duplicateRejected);

    repo.remove(11);
    assert(repo.getAll().empty());

    bool missingRejected = false;
    try {
        repo.find(999);
    } catch (const std::runtime_error&) {
        missingRejected = true;
    }
    assert(missingRejected);

    return 0;
}
