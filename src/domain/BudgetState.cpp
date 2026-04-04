#include "domain/BudgetState.h"
#include <algorithm>
#include <stdexcept>

BudgetState::BudgetState()
    : year(0), month(0), salary(0), available(0), savings(0), monthlySavingsContribution(0),
      transferredToSavingsThisMonth(0) {}

int BudgetState::getYear() const { return year; }
int BudgetState::getMonth() const { return month; }
double BudgetState::getSalary() const { return salary; }
double BudgetState::getAvailable() const { return available; }
double BudgetState::getSavings() const { return savings; }
double BudgetState::getMonthlySavingsContribution() const { return monthlySavingsContribution; }
double BudgetState::getTransferredToSavingsThisMonth() const { return transferredToSavingsThisMonth; }
const std::vector<std::pair<std::string, double>>& BudgetState::getFixedBills() const { return fixedBills; }
const std::vector<SavingsGoal>& BudgetState::getGoals() const { return goals; }

double BudgetState::getTotalFixedBills() const {
    double total = 0;
    for (const auto& [_, amount] : fixedBills) {
        total += amount;
    }
    return total;
}

void BudgetState::setYear(int v) { year = v; }
void BudgetState::setMonth(int v) { month = v; }
void BudgetState::setSalary(double v) { salary = v; }
void BudgetState::setAvailable(double v) { available = v; }
void BudgetState::setSavings(double v) { savings = v; }
void BudgetState::setMonthlySavingsContribution(double v) { monthlySavingsContribution = v; }
void BudgetState::setTransferredToSavingsThisMonth(double v) { transferredToSavingsThisMonth = v; }
void BudgetState::setFixedBills(const std::vector<std::pair<std::string, double>>& v) { fixedBills = v; }
void BudgetState::setGoals(const std::vector<SavingsGoal>& v) { goals = v; }

void BudgetState::addFixedBill(const std::string& name, double amount) {
    auto it = std::find_if(fixedBills.begin(), fixedBills.end(),
                           [&](const auto& bill) { return bill.first == name; });
    if (it != fixedBills.end()) {
        it->second = amount;
        return;
    }
    fixedBills.emplace_back(name, amount);
}

void BudgetState::removeFixedBill(const std::string& name) {
    auto it = std::remove_if(fixedBills.begin(), fixedBills.end(),
                             [&](const auto& bill) { return bill.first == name; });
    if (it == fixedBills.end()) {
        throw std::runtime_error("Fixed bill not found.");
    }
    fixedBills.erase(it, fixedBills.end());
}

void BudgetState::addGoal(const std::string& name, double target) {
    auto it = std::find_if(goals.begin(), goals.end(),
                           [&](const SavingsGoal& goal) { return goal.name == name; });
    if (it != goals.end()) {
        it->target = target;
        it->notified = false;
        return;
    }
    goals.push_back({name, target, false});
}

void BudgetState::removeGoal(const std::string& name) {
    auto it = std::remove_if(goals.begin(), goals.end(),
                             [&](const SavingsGoal& goal) { return goal.name == name; });
    if (it == goals.end()) {
        throw std::runtime_error("Goal not found.");
    }
    goals.erase(it, goals.end());
}

void BudgetState::markGoalNotified(const std::string& name, bool notified) {
    auto it = std::find_if(goals.begin(), goals.end(),
                           [&](const SavingsGoal& goal) { return goal.name == name; });
    if (it == goals.end()) {
        throw std::runtime_error("Goal not found.");
    }
    it->notified = notified;
}
