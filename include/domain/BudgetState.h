#pragma once
#include <string>
#include <utility>
#include <vector>

struct SavingsGoal {
    std::string name;
    double target{};
    bool notified{};
};

class BudgetState {
    int year{}, month{};
    double salary{};
    double available{};
    double savings{};
    double monthlySavingsContribution{};
    double transferredToSavingsThisMonth{};
    std::vector<std::pair<std::string, double>> fixedBills;
    std::vector<SavingsGoal> goals;

public:
    BudgetState();

    int getYear() const;
    int getMonth() const;
    double getSalary() const;
    double getAvailable() const;
    double getSavings() const;
    double getMonthlySavingsContribution() const;
    double getTransferredToSavingsThisMonth() const;
    const std::vector<std::pair<std::string, double>>& getFixedBills() const;
    const std::vector<SavingsGoal>& getGoals() const;
    double getTotalFixedBills() const;

    void setYear(int);
    void setMonth(int);
    void setSalary(double);
    void setAvailable(double);
    void setSavings(double);
    void setMonthlySavingsContribution(double);
    void setTransferredToSavingsThisMonth(double);
    void setFixedBills(const std::vector<std::pair<std::string, double>>&);
    void setGoals(const std::vector<SavingsGoal>&);
    void addFixedBill(const std::string&, double);
    void removeFixedBill(const std::string&);
    void addGoal(const std::string&, double);
    void removeGoal(const std::string&);
    void markGoalNotified(const std::string&, bool);
};
