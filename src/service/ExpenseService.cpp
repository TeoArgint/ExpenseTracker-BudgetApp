#include "service/ExpenseService.h"
#include "validation/Validators.h"
#include <algorithm>
#include <stdexcept>

ExpenseService::ExpenseService(IExpenseRepository& repo) : repo(repo) {}

std::string ExpenseService::monthKey(const std::string& date) {
    if (date.size() < 7) return "";
    return date.substr(0, 7);
}

void ExpenseService::addExpense(const std::string& id, double amount, const std::string& category,
                                const std::string& date, const std::string& description) {
    Validators::validateId(id);
    Validators::validateAmount(amount);
    Validators::validateCategory(category);
    Validators::validateDescription(description);
    if (!Validators::isValidDateYYYYMMDD(date)) throw std::runtime_error("Invalid date format (YYYY-MM-DD).");

    repo.add(Expense(id, amount, category, date, description));
}

void ExpenseService::updateExpense(const std::string& id, double amount, const std::string& category,
                                   const std::string& date, const std::string& description) {
    Validators::validateId(id);
    Validators::validateAmount(amount);
    Validators::validateCategory(category);
    Validators::validateDescription(description);
    if (!Validators::isValidDateYYYYMMDD(date)) throw std::runtime_error("Invalid date format (YYYY-MM-DD).");

    repo.update(Expense(id, amount, category, date, description));
}

void ExpenseService::deleteExpense(const std::string& id) {
    Validators::validateId(id);
    repo.remove(id);
}

std::vector<Expense> ExpenseService::getAll() const {
    return repo.getAll();
}

std::vector<Expense> ExpenseService::filterByCategory(const std::string& cat) const {
    auto v = repo.getAll();
    std::vector<Expense> out;
    std::copy_if(v.begin(), v.end(), std::back_inserter(out),
                 [&](const Expense& e){ return e.getCategory() == cat; });
    return out;
}

std::vector<Expense> ExpenseService::filterByDateRange(const std::string& start, const std::string& end) const {
    if (!Validators::isValidDateYYYYMMDD(start) || !Validators::isValidDateYYYYMMDD(end))
        throw std::runtime_error("Invalid start/end date (YYYY-MM-DD).");
    if (start > end) throw std::runtime_error("Start date must be <= end date.");

    auto v = repo.getAll();
    std::vector<Expense> out;
    std::copy_if(v.begin(), v.end(), std::back_inserter(out),
                 [&](const Expense& e){ return e.getDate() >= start && e.getDate() <= end; });
    return out;
}

std::vector<Expense> ExpenseService::filterByAmountRange(double minA, double maxA) const {
    if (minA > maxA) throw std::runtime_error("minAmount must be <= maxAmount.");
    auto v = repo.getAll();
    std::vector<Expense> out;
    std::copy_if(v.begin(), v.end(), std::back_inserter(out),
                 [&](const Expense& e){ return e.getAmount() >= minA && e.getAmount() <= maxA; });
    return out;
}

std::map<std::string, double> ExpenseService::totalByCategory() const {
    std::map<std::string, double> m;
    for (const auto& e : repo.getAll()) m[e.getCategory()] += e.getAmount();
    return m;
}

std::map<std::string, double> ExpenseService::totalByMonth() const {
    std::map<std::string, double> m;
    for (const auto& e : repo.getAll()) m[monthKey(e.getDate())] += e.getAmount();
    return m;
}