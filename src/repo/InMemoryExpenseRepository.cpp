#include "repo/InMemoryExpenseRepository.h"
#include <stdexcept>

void InMemoryExpenseRepository::add(const Expense& e) {
    for (const auto& x : all)
        if (x.getId() == e.getId())
            throw std::runtime_error("Expense with this id already exists.");
    all.push_back(e);
}

void InMemoryExpenseRepository::update(const Expense& e) {
    for (auto& x : all) {
        if (x.getId() == e.getId()) { x = e; return; }
    }
    throw std::runtime_error("Expense not found for update.");
}

void InMemoryExpenseRepository::remove(const std::string& id) {
    for (size_t i = 0; i < all.size(); i++) {
        if (all[i].getId() == id) {
            all.erase(all.begin() + static_cast<long>(i));
            return;
        }
    }
    throw std::runtime_error("Expense not found for delete.");
}

Expense InMemoryExpenseRepository::findById(const std::string& id) const {
    for (const auto& x : all)
        if (x.getId() == id) return x;
    throw std::runtime_error("Expense not found.");
}

std::vector<Expense> InMemoryExpenseRepository::getAll() const {
    return all;
}