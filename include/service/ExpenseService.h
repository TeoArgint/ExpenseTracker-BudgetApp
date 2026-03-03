#pragma once
#include "service/IExpenseService.h"
#include "repo/IExpenseRepository.h"

class ExpenseService : public IExpenseService {
private:
    IExpenseRepository& repo;
    static std::string monthKey(const std::string& date); // YYYY-MM

public:
    explicit ExpenseService(IExpenseRepository& repo);

    void addExpense(const std::string& id, double amount, const std::string& category,
                    const std::string& date, const std::string& description) override;

    void updateExpense(const std::string& id, double amount, const std::string& category,
                       const std::string& date, const std::string& description) override;

    void deleteExpense(const std::string& id) override;

    std::vector<Expense> getAll() const override;

    std::vector<Expense> filterByCategory(const std::string& cat) const override;
    std::vector<Expense> filterByDateRange(const std::string& start, const std::string& end) const override;
    std::vector<Expense> filterByAmountRange(double minA, double maxA) const override;

    std::map<std::string, double> totalByCategory() const override;
    std::map<std::string, double> totalByMonth() const override;
};