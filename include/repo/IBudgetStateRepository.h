#pragma once
#include "domain/BudgetState.h"

class IBudgetStateRepository {
public:
    virtual ~IBudgetStateRepository() = default;
    virtual BudgetState load() const = 0;
    virtual void save(const BudgetState& st) const = 0;
};