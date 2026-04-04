#pragma once
#include "repo/IExpenseRepository.h"

class InMemoryExpenseRepository : public IExpenseRepository {
    std::vector<Expense> data;

public:
    void add(const Expense&) override;
    void remove(int) override;
    void update(const Expense&) override;
    Expense find(int) const override;
    std::vector<Expense> getAll() const override;
};