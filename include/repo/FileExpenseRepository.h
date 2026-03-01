#pragma once
#include "repo/IExpenseRepository.h"
#include <string>

class FileExpenseRepository : public IExpenseRepository {
private:
    std::string filename;
    mutable std::vector<Expense> cache; // încărcat din fișier

    void load() const;
    void save() const;

public:
    explicit FileExpenseRepository(std::string filename);

    void add(const Expense& e) override;
    void update(const Expense& e) override;
    void remove(int id) override;

    Expense findById(int id) const override;
    std::vector<Expense> getAll() const override;
};