#pragma once
#include "IBudgetStateRepository.h"
#include "domain/BudgetState.h"
#include <string>

class FileBudgetStateRepository : public IBudgetStateRepository {
private:
    std::string file;   // 🔥 ASTA LIPSEA

public:
    explicit FileBudgetStateRepository(const std::string& filename);

    BudgetState load() const;
    void save(const BudgetState&) const;
};