#include "repo/IBudgetStateRepository.h"
#include "repo/InMemoryExpenseRepository.h"
#include "service/ExpenseService.h"
#include <cassert>

namespace {
class BudgetStateRepositoryStub : public IBudgetStateRepository {
private:
    mutable BudgetState state;

public:
    BudgetState load() const override {
        return state;
    }

    void save(const BudgetState& newState) const override {
        state = newState;
    }
};
}

int main() {
    InMemoryExpenseRepository expenseRepo;
    BudgetStateRepositoryStub stateRepo;
    ExpenseService service(expenseRepo, stateRepo);

    service.initializeIfNeeded(2026, 4, 5000);
    service.setMonthlySavingsContribution(500);
    service.addFixedBill("rent", 1000);

    BudgetState state = service.getState();
    assert(state.getSalary() == 5000);
    assert(state.getAvailable() == 3500);
    assert(state.getSavings() == 0);
    assert(state.getTotalFixedBills() == 1000);

    service.addExpense(1, 200, "food", "2026-04-10", "groceries");
    assert(service.totalSpentInMonth(2026, 4) == 200);
    assert(service.getState().getAvailable() == 3300);

    service.updateExpense(1, 300, "food", "2026-04-10", "bigger groceries");
    assert(service.totalSpentInMonth(2026, 4) == 300);
    assert(service.getState().getAvailable() == 3200);

    service.transferToSavings(200);
    state = service.getState();
    assert(state.getAvailable() == 3000);
    assert(state.getSavings() == 200);

    service.deleteExpense(1);
    assert(service.totalSpentInMonth(2026, 4) == 0);
    assert(service.getState().getAvailable() == 3300);

    service.initializeIfNeeded(2026, 5, 5000);
    state = service.getState();
    assert(state.getMonth() == 5);
    assert(state.getSavings() == 700);
    assert(state.getAvailable() == 3500);

    assert(service.totalSpentInYear(2026) == 0);
    assert(service.monthsToAfford(10000) == 2);

    service.addExpense(2, 400, "utilities", "2026-05-05", "electricity");
    assert(service.totalSpentInMonth(2026, 5) == 400);
    assert(service.getState().getAvailable() == 3100);

    service.removeFixedBill("rent");
    assert(service.getState().getTotalFixedBills() == 0);
    assert(service.getState().getAvailable() == 4100);

    return 0;
}
