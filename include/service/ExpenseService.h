#pragma once
#include "service/IExpenseService.h"
#include "repo/IExpenseRepository.h"
#include "repo/IBudgetStateRepository.h"

class ExpenseService : public IExpenseService {
private:
    IExpenseRepository& repo;
    IBudgetStateRepository& stateRepo;

    int getYear(const std::string&) const;
    int getMonth(const std::string&) const;
    double totalSpentForStateMonth(const BudgetState&) const;
    void recalculateAvailable(BudgetState&) const;

public:
    ExpenseService(IExpenseRepository&, IBudgetStateRepository&);

    void addExpense(int,double,const std::string&,const std::string&,const std::string&) override;
    void updateExpense(int,double,const std::string&,const std::string&,const std::string&) override;
    void deleteExpense(int) override;

    std::vector<Expense> getAll() const override;

    std::vector<Expense> filterByCategory(const std::string&) const override;
    std::vector<Expense> filterByDateRange(const std::string&, const std::string&) const override;
    std::vector<Expense> filterByAmountRange(double,double) const override;

    std::map<std::string,double> totalByCategory() const override;
    std::map<std::string,double> totalByMonth() const override;

    BudgetState getState() const override;
    void initializeIfNeeded(int,int,double) override;
    void setMonthlySalary(double) override;
    void setMonthlySavingsContribution(double) override;
    void addFixedBill(const std::string&, double) override;
    void removeFixedBill(const std::string&) override;
    void addGoal(const std::string&, double) override;
    void removeGoal(const std::string&) override;
    void markGoalNotified(const std::string&, bool) override;
    void transferToSavings(double) override;

    double totalSpentInMonth(int,int) const override;
    double totalSpentInYear(int) const override;

    int monthsToAfford(double) const override;

    std::vector<std::pair<std::string,double>> topCategories() const override;
    std::map<std::string,double> graphData() const override;
    void exportCSV(const std::string&) const override;
};
