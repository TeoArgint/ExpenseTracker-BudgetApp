#include "repo/FileExpenseRepository.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

// CSV: id,amount,category,date,description (description fără virgule, ca să fie simplu)
void FileExpenseRepository::load() const {
    cache.clear();

    std::ifstream fin(filename);
    if (!fin.is_open()) return; // tratăm ca gol

    std::string line;
    while (std::getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        int id = std::stoi(trim(token));

        std::getline(ss, token, ',');
        double amount = std::stod(trim(token));

        std::getline(ss, token, ',');
        std::string category = trim(token);

        std::getline(ss, token, ',');
        std::string date = trim(token);

        std::getline(ss, token);
        std::string description = trim(token);

        cache.emplace_back(id, amount, category, date, description);
    }
}

void FileExpenseRepository::save() const {
    std::ofstream fout(filename);
    if (!fout.is_open()) throw std::runtime_error("Cannot open file for writing.");

    for (const auto& e : cache) {
        fout << e.getId() << ","
             << e.getAmount() << ","
             << e.getCategory() << ","
             << e.getDate() << ","
             << e.getDescription() << "\n";
    }
}

FileExpenseRepository::FileExpenseRepository(std::string filename)
    : filename(std::move(filename)) {
    load();
    save(); // creează fișierul dacă lipsește
}

void FileExpenseRepository::add(const Expense& e) {
    load();
    for (const auto& x : cache)
        if (x.getId() == e.getId())
            throw std::runtime_error("Expense with this id already exists.");
    cache.push_back(e);
    save();
}

void FileExpenseRepository::update(const Expense& e) {
    load();
    for (auto& x : cache) {
        if (x.getId() == e.getId()) { x = e; save(); return; }
    }
    throw std::runtime_error("Expense not found for update.");
}

void FileExpenseRepository::remove(int id) {
    load();
    for (size_t i = 0; i < cache.size(); i++) {
        if (cache[i].getId() == id) {
            cache.erase(cache.begin() + static_cast<long>(i));
            save();
            return;
        }
    }
    throw std::runtime_error("Expense not found for delete.");
}

Expense FileExpenseRepository::findById(int id) const {
    load();
    for (const auto& x : cache)
        if (x.getId() == id) return x;
    throw std::runtime_error("Expense not found.");
}

std::vector<Expense> FileExpenseRepository::getAll() const {
    load();
    return cache;
}