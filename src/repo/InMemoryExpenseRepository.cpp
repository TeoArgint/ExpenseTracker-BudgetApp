#include "repo/InMemoryExpenseRepository.h"
#include <stdexcept>

void InMemoryExpenseRepository::add(const Expense& e) {
    for (auto& x : data)
        if (x.getId() == e.getId())
            throw std::runtime_error("ID exists");
    data.push_back(e);
}

void InMemoryExpenseRepository::remove(int id) {
    for (auto it = data.begin(); it != data.end(); ++it)
        if (it->getId() == id) {
            data.erase(it);
            return;
        }
    throw std::runtime_error("Not found");
}

void InMemoryExpenseRepository::update(const Expense& e) {
    for (auto& x : data)
        if (x.getId() == e.getId()) {
            x = e;
            return;
        }
    throw std::runtime_error("Not found");
}

Expense InMemoryExpenseRepository::find(int id) const {
    for (auto& x : data)
        if (x.getId() == id) return x;
    throw std::runtime_error("Not found");
}

std::vector<Expense> InMemoryExpenseRepository::getAll() const {
    return data;
}