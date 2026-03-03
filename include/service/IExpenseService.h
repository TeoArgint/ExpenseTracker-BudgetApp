#pragma once
#include "domain/Expense.h"
#include <map>
#include <vector>
#include <string>

class IExpenseService {
public:
    virtual ~IExpenseService() = default;

    virtual void addExpense(const std::string& id, double amount, const std::string& category,
                            const std::string& date, const std::string& description) = 0;

    virtual void updateExpense(const std::string& id, double amount, const std::string& category,
                               const std::string& date, const std::string& description) = 0;

    virtual void deleteExpense(const std::string& id) = 0;

    virtual std::vector<Expense> getAll() const = 0;

    virtual std::vector<Expense> filterByCategory(const std::string& cat) const = 0;
    virtual std::vector<Expense> filterByDateRange(const std::string& start, const std::string& end) const = 0;
    virtual std::vector<Expense> filterByAmountRange(double minA, double maxA) const = 0;

    virtual std::map<std::string, double> totalByCategory() const = 0;
    virtual std::map<std::string, double> totalByMonth() const = 0;
};