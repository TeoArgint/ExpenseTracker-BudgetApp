#pragma once
#include "domain/Expense.h"
#include <vector>

class IExpenseRepository {
public:
    virtual ~IExpenseRepository() = default;

    virtual void add(const Expense& e) = 0;
    virtual void update(const Expense& e) = 0;
    virtual void remove(int id) = 0;

    virtual Expense findById(int id) const = 0;
    virtual std::vector<Expense> getAll() const = 0;
};