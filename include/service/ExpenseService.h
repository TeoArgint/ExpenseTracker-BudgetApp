#pragma once
#include "service/IExpenseService.h"
#include "repo/IExpenseRepository.h"

class ExpenseService : public IExpenseService {
private:
    IExpenseRepository& repo;
    static std::string monthKey(const std::string& date); // YYYY-MM

public:
    explicit ExpenseService(IExpenseRepository& repo);

    void addExpense(int id, double amount, const std::string& category,
                    const std::string& date, const std::string& description) override;

    void updateExpense(int id, double amount, const std::string& category,
                       const std::string& date, const std::string& description) override;

    void deleteExpense(int id) override;

    std::vector<Expense> getAll() const override;

    std::vector<Expense> filterByCategory(const std::string& cat) const override;
    std::vector<Expense> filterByDateRange(const std::string& start, const std::string& end) const override;
    std::vector<Expense> filterByAmountRange(double minA, double maxA) const override;

    std::map<std::string, double> totalByCategory() const override;
    std::map<std::string, double> totalByMonth() const override;
};