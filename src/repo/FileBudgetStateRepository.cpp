#include "repo/FileBudgetStateRepository.h"
#include <fstream>
#include <sstream>

FileBudgetStateRepository::FileBudgetStateRepository(const std::string& f)
    : file(f) {}

BudgetState FileBudgetStateRepository::load() const {
    std::ifstream fin(file);

    // dacă fișierul nu există → return state gol
    if (!fin.is_open()) {
        BudgetState st;
        return st;
    }

    BudgetState st;
    std::string line;

    int year = 0, month = 0;
    double salary = 0, available = 0, savings = 0, monthlySavingsContribution = 0;
    double transferredToSavingsThisMonth = 0;
    std::vector<std::pair<std::string, double>> fixedBills;
    std::vector<SavingsGoal> goals;

    while (getline(fin, line)) {
        std::stringstream ss(line);
        std::string key, value;

        getline(ss, key, '=');
        getline(ss, value);

        if (key == "year") year = std::stoi(value);
        else if (key == "month") month = std::stoi(value);
        else if (key == "salary") salary = std::stod(value);
        else if (key == "available") available = std::stod(value);
        else if (key == "savings") savings = std::stod(value);
        else if (key == "monthly_savings_contribution") monthlySavingsContribution = std::stod(value);
        else if (key == "transferred_to_savings_this_month") transferredToSavingsThisMonth = std::stod(value);
        else if (key == "fixed_bill") {
            const auto separator = value.find('|');
            if (separator != std::string::npos) {
                fixedBills.emplace_back(value.substr(0, separator), std::stod(value.substr(separator + 1)));
            }
        } else if (key == "goal") {
            std::stringstream goalStream(value);
            std::string name;
            std::string target;
            std::string notified;
            getline(goalStream, name, '|');
            getline(goalStream, target, '|');
            getline(goalStream, notified);
            if (!name.empty() && !target.empty()) {
                goals.push_back({name, std::stod(target), notified == "1"});
            }
        }
    }

    st.setYear(year);
    st.setMonth(month);
    st.setSalary(salary);
    st.setAvailable(available);
    st.setSavings(savings);
    st.setMonthlySavingsContribution(monthlySavingsContribution);
    st.setTransferredToSavingsThisMonth(transferredToSavingsThisMonth);
    st.setFixedBills(fixedBills);
    st.setGoals(goals);

    return st;
}

void FileBudgetStateRepository::save(const BudgetState& st) const {
    std::ofstream fout(file);

    fout << "year=" << st.getYear() << "\n";
    fout << "month=" << st.getMonth() << "\n";
    fout << "salary=" << st.getSalary() << "\n";
    fout << "available=" << st.getAvailable() << "\n";
    fout << "savings=" << st.getSavings() << "\n";
    fout << "monthly_savings_contribution=" << st.getMonthlySavingsContribution() << "\n";
    fout << "transferred_to_savings_this_month=" << st.getTransferredToSavingsThisMonth() << "\n";
    for (const auto& [name, amount] : st.getFixedBills()) {
        fout << "fixed_bill=" << name << "|" << amount << "\n";
    }
    for (const auto& goal : st.getGoals()) {
        fout << "goal=" << goal.name << "|" << goal.target << "|" << (goal.notified ? 1 : 0) << "\n";
    }
}
