#pragma once
#include "domain/Expense.h"
#include "domain/BudgetState.h"
#include <vector>
#include <map>

class IExpenseService {
public:
    virtual ~IExpenseService() = default;

    virtual void addExpense(int, double, const std::string&, const std::string&, const std::string&) = 0;
    virtual void updateExpense(int, double, const std::string&, const std::string&, const std::string&) = 0;
    virtual void deleteExpense(int) = 0;

    virtual std::vector<Expense> getAll() const = 0;

    virtual std::vector<Expense> filterByCategory(const std::string&) const = 0;
    virtual std::vector<Expense> filterByDateRange(const std::string&, const std::string&) const = 0;
    virtual std::vector<Expense> filterByAmountRange(double, double) const = 0;

    virtual std::map<std::string,double> totalByCategory() const = 0;
    virtual std::map<std::string,double> totalByMonth() const = 0;

    // 🔥 BUDGET
    virtual BudgetState getState() const = 0;
    virtual void initializeIfNeeded(int year, int month, double salary) = 0;
    virtual void setMonthlySalary(double) = 0;
    virtual void setMonthlySavingsContribution(double) = 0;
    virtual void addFixedBill(const std::string&, double) = 0;
    virtual void removeFixedBill(const std::string&) = 0;
    virtual void addGoal(const std::string&, double) = 0;
    virtual void removeGoal(const std::string&) = 0;
    virtual void markGoalNotified(const std::string&, bool) = 0;
    virtual void transferToSavings(double) = 0;

    virtual double totalSpentInMonth(int,int) const = 0;
    virtual double totalSpentInYear(int) const = 0;

    virtual int monthsToAfford(double) const = 0;

    // 🔥 EXTRA
    virtual std::vector<std::pair<std::string,double>> topCategories() const = 0;
    virtual std::map<std::string,double> graphData() const = 0;
    virtual void exportCSV(const std::string&) const = 0;
};
