#include "repo/FileExpenseRepository.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

FileExpenseRepository::FileExpenseRepository(std::string f) : file(f) {}

void FileExpenseRepository::load(std::vector<Expense>& v) const {
    std::ifstream fin(file);
    if (!fin.is_open()) {
        return;
    }
    std::string line;

    while (getline(fin, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream ss(line);
        std::string id, amount, cat, date, desc;

        getline(ss, id, ',');
        getline(ss, amount, ',');
        getline(ss, cat, ',');
        getline(ss, date, ',');
        getline(ss, desc);

        v.emplace_back(std::stoi(id), std::stod(amount), cat, date, desc);
    }
}

void FileExpenseRepository::save(const std::vector<Expense>& v) const {
    std::ofstream fout(file);
    for (auto& e : v)
        fout << e.getId() << "," << e.getAmount() << ","
             << e.getCategory() << "," << e.getDate() << ","
             << e.getDescription() << "\n";
}

std::vector<Expense> FileExpenseRepository::getAll() const {
    std::vector<Expense> v;
    load(v);
    return v;
}

void FileExpenseRepository::add(const Expense& e) {
    auto v = getAll();
    v.push_back(e);
    save(v);
}

void FileExpenseRepository::remove(int id) {
    auto v = getAll();
    v.erase(std::remove_if(v.begin(), v.end(),
                           [&](auto& x){ return x.getId()==id; }), v.end());
    save(v);
}

void FileExpenseRepository::update(const Expense& e) {
    auto v = getAll();
    for (auto& x : v)
        if (x.getId() == e.getId()) x = e;
    save(v);
}

Expense FileExpenseRepository::find(int id) const {
    for (auto& e : getAll())
        if (e.getId() == id) return e;
    throw std::runtime_error("Not found");
}
