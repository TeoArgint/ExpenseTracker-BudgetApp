#include "app/AppSupport.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <map>
#include <numeric>
#include <sstream>

namespace {

int daysInMonth(int year, int month) {
    static const std::array<int, 12> days{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        const bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        return leap ? 29 : 28;
    }
    return days.at(month - 1);
}

std::tm localNow() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    local = *std::localtime(&now);
#endif
    return local;
}

std::string monthLabel(int year, int month) {
    static const std::array<const char*, 12> names{
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    std::ostringstream out;
    out << names.at(month - 1) << " " << year;
    return out.str();
}

}

std::vector<Expense> AppSupport::expensesForMonth(const IExpenseService& service, int year, int month) {
    std::vector<Expense> result;
    const std::string prefix = std::to_string(year) + "-" + (month < 10 ? "0" : "") + std::to_string(month);
    for (const auto& expense : service.getAll()) {
        if (expense.getDate().rfind(prefix, 0) == 0) {
            result.push_back(expense);
        }
    }
    return result;
}

std::pair<int, int> AppSupport::currentYearMonth() {
    const std::tm now = localNow();
    return {now.tm_year + 1900, now.tm_mon + 1};
}

std::string AppSupport::isoDateToday() {
    const std::tm now = localNow();
    std::ostringstream out;
    out << std::put_time(&now, "%Y-%m-%d");
    return out.str();
}

void AppSupport::ensureDemoDataIfNeeded(IExpenseService& service) {
    if (!service.getAll().empty() || service.getState().getYear() != 0) {
        return;
    }

    const auto [year, month] = currentYearMonth();
    service.initializeIfNeeded(year, month, 5200);
    service.setMonthlySavingsContribution(650);
    service.addFixedBill("Rent", 1500);
    service.addFixedBill("Electricity", 180);
    service.addFixedBill("Gas", 120);
    service.addFixedBill("Internet", 65);
    service.addFixedBill("Phone", 55);
    service.addGoal("Emergency fund", 3000);
    service.addGoal("City break", 1800);

    auto addMonthExpense = [&](int id, int y, int m, int d, double amount, const std::string& category, const std::string& note) {
        std::ostringstream date;
        date << y << "-" << std::setw(2) << std::setfill('0') << m
             << "-" << std::setw(2) << std::setfill('0') << d;
        service.addExpense(id, amount, category, date.str(), note);
    };

    int currentYear = year;
    int currentMonth = month;
    int id = 1;
    for (int offset = 3; offset >= 0; --offset) {
        int y = currentYear;
        int m = currentMonth - offset;
        while (m <= 0) {
            m += 12;
            --y;
        }

        addMonthExpense(id++, y, m, 3, 340, "Groceries", "Weekly food shopping");
        addMonthExpense(id++, y, m, 7, 120, "Transport", "Fuel and parking");
        addMonthExpense(id++, y, m, 12, 85, "Coffee", "Coffee and snacks");
        addMonthExpense(id++, y, m, 16, 210, "Home", "Cleaning and household");
        addMonthExpense(id++, y, m, 22, 160 + offset * 20, "Fun", "Movies and going out");
    }
}

int AppSupport::nextExpenseId(const IExpenseService& service) {
    int nextId = 1;
    for (const auto& expense : service.getAll()) {
        nextId = std::max(nextId, expense.getId() + 1);
    }
    return nextId;
}

std::vector<AppSupport::MonthlyPoint> AppSupport::buildLastMonthsChart(const IExpenseService& service, int count, int endYear, int endMonth) {
    std::vector<MonthlyPoint> points;
    points.reserve(count);

    for (int offset = count - 1; offset >= 0; --offset) {
        int year = endYear;
        int month = endMonth - offset;
        while (month <= 0) {
            month += 12;
            --year;
        }

        points.push_back({monthLabel(year, month), year, month, service.totalSpentInMonth(year, month)});
    }

    return points;
}

std::vector<AppSupport::MonthlyPoint> AppSupport::availableMonths(const IExpenseService& service) {
    std::map<std::pair<int, int>, double> totalsByMonth;
    for (const auto& expense : service.getAll()) {
        const int year = std::stoi(expense.getDate().substr(0, 4));
        const int month = std::stoi(expense.getDate().substr(5, 2));
        totalsByMonth[{year, month}] += expense.getAmount();
    }

    std::vector<MonthlyPoint> months;
    for (const auto& [key, total] : totalsByMonth) {
        months.push_back({monthLabel(key.first, key.second), key.first, key.second, total});
    }

    std::sort(months.begin(), months.end(), [](const MonthlyPoint& left, const MonthlyPoint& right) {
        if (left.year == right.year) {
            return left.month > right.month;
        }
        return left.year > right.year;
    });

    if (months.empty()) {
        const auto [year, month] = currentYearMonth();
        months.push_back({monthLabel(year, month), year, month, 0});
    }

    return months;
}

AppSupport::DashboardMetrics AppSupport::calculateMetrics(const IExpenseService& service, int year, int month) {
    const auto state = service.getState();
    const std::tm now = localNow();

    DashboardMetrics metrics{};
    metrics.spentThisMonth = service.totalSpentInMonth(year, month);
    metrics.spentThisYear = service.totalSpentInYear(year);
    metrics.fixedBillsTotal = state.getTotalFixedBills();

    const auto [currentYear, currentMonth] = currentYearMonth();
    if (year == currentYear && month == currentMonth) {
        const int today = now.tm_mday;
        const int remainingDays = std::max(1, daysInMonth(year, month) - today + 1);
        metrics.safeToSpendPerDay = std::max(0.0, state.getAvailable()) / remainingDays;
    } else {
        const int totalDays = daysInMonth(year, month);
        metrics.safeToSpendPerDay = totalDays > 0 ? std::max(0.0, state.getSalary() - state.getTotalFixedBills() -
            state.getMonthlySavingsContribution() - metrics.spentThisMonth) / totalDays : 0;
    }

    const auto chart = buildLastMonthsChart(service, 6, year, month);
    double total = 0;
    for (const auto& point : chart) {
        total += point.total;
    }
    metrics.averageMonthlySpending = chart.empty() ? 0 : total / chart.size();
    return metrics;
}

std::vector<std::string> AppSupport::buildSavingIdeas(const IExpenseService& service, int year, int month) {
    const auto state = service.getState();
    const auto monthlyExpenses = AppSupport::expensesForMonth(service, year, month);
    std::map<std::string, double> categoryTotals;
    for (const auto& expense : monthlyExpenses) {
        categoryTotals[expense.getCategory()] += expense.getAmount();
    }

    std::vector<std::string> ideas;
    const double spentThisMonth = service.totalSpentInMonth(year, month);
    if (state.getSalary() > 0 && state.getTotalFixedBills() > state.getSalary() * 0.45) {
        ideas.push_back("Your repeating bills are using a big part of your salary. If you can renegotiate just one of them, your monthly budget will feel much lighter.");
    }

    if (!categoryTotals.empty() && spentThisMonth > 0) {
        const auto topCategory = std::max_element(categoryTotals.begin(), categoryTotals.end(),
            [](const auto& left, const auto& right) { return left.second < right.second; });
        if (topCategory->second > spentThisMonth * 0.30) {
            ideas.push_back("This month, most of your spending went to " + topCategory->first +
                            ". Putting a soft limit there would likely save money fastest.");
        }
    }

    if (state.getMonthlySavingsContribution() < state.getSalary() * 0.10) {
        ideas.push_back("Your automatic saving is still small. Even increasing it a little each month can build momentum without feeling harsh.");
    } else {
        ideas.push_back("Your automatic saving habit is already strong. Keeping that amount fixed after a salary raise is a smart baseline.");
    }

    if (state.getAvailable() > 700) {
        ideas.push_back("You still have a healthy amount left this month. Moving a small extra amount into savings now could protect it from impulse spending.");
    } else if (state.getAvailable() < 200) {
        ideas.push_back("Money left for this month is getting tight. A simple no-spend weekend could help you finish the month comfortably.");
    }

    if (ideas.size() < 4) {
        ideas.push_back("Check the monthly chart regularly. If one month jumps suddenly, look at what changed instead of only cutting random small costs.");
    }

    return ideas;
}
