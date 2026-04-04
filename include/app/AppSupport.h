#pragma once

#include "service/IExpenseService.h"
#include <string>
#include <utility>
#include <vector>

namespace AppSupport {

struct MonthlyPoint {
    std::string label;
    int year{};
    int month{};
    double total{};
};

struct DashboardMetrics {
    double spentThisMonth{};
    double spentThisYear{};
    double fixedBillsTotal{};
    double safeToSpendPerDay{};
    double averageMonthlySpending{};
};

std::pair<int, int> currentYearMonth();
std::string isoDateToday();
void ensureDemoDataIfNeeded(IExpenseService&);
int nextExpenseId(const IExpenseService&);
std::vector<MonthlyPoint> buildLastMonthsChart(const IExpenseService&, int count, int endYear, int endMonth);
std::vector<MonthlyPoint> availableMonths(const IExpenseService&);
std::vector<Expense> expensesForMonth(const IExpenseService&, int year, int month);
DashboardMetrics calculateMetrics(const IExpenseService&, int year, int month);
std::vector<std::string> buildSavingIdeas(const IExpenseService&, int year, int month);

}
