#pragma once
#include "domain/Expense.h"
#include <vector>

class IExpenseRepository {
public:
    virtual ~IExpenseRepository() = default;
    virtual void add(const Expense&) = 0;
    virtual void remove(int id) = 0;
    virtual void update(const Expense&) = 0;
    virtual Expense find(int id) const = 0;
    virtual std::vector<Expense> getAll() const = 0;
};