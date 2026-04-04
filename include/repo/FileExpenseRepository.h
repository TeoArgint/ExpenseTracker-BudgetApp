#pragma once
#include "repo/IExpenseRepository.h"
#include <string>

class FileExpenseRepository : public IExpenseRepository {
    std::string file;

    void load(std::vector<Expense>&) const;
    void save(const std::vector<Expense>&) const;

public:
    FileExpenseRepository(std::string file);

    void add(const Expense&) override;
    void remove(int) override;
    void update(const Expense&) override;
    Expense find(int) const override;
    std::vector<Expense> getAll() const override;
};