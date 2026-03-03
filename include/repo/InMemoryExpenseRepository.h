#pragma once
#include "repo/IExpenseRepository.h"

class InMemoryExpenseRepository : public IExpenseRepository {
private:
    std::vector<Expense> all;

public:
    void add(const Expense& e) override;
    void update(const Expense& e) override;
    void remove(const std::string& id) override;

    Expense findById(const std::string& id) const override;
    std::vector<Expense> getAll() const override;
};