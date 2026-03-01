#pragma once
#include "repo/IExpenseRepository.h"

class InMemoryExpenseRepository : public IExpenseRepository {
private:
    std::vector<Expense> all;

public:
    void add(const Expense& e) override;
    void update(const Expense& e) override;
    void remove(int id) override;

    Expense findById(int id) const override;
    std::vector<Expense> getAll() const override;
};