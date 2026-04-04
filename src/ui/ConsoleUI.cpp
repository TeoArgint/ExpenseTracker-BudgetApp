#include "ui/ConsoleUI.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>

ConsoleUI::ConsoleUI(IExpenseService& s) : srv(s) {}

static void getDate(int& y, int& m) {
    const time_t t = time(nullptr);
    tm* now = localtime(&t);
    y = now->tm_year + 1900;
    m = now->tm_mon + 1;
}

void ConsoleUI::clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int ConsoleUI::readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid number. Try again.\n";
        clearInput();
    }
}

double ConsoleUI::readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid value. Try again.\n";
        clearInput();
    }
}

std::string ConsoleUI::readToken(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::cin >> value;
    clearInput();
    return value;
}

std::string ConsoleUI::readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

void ConsoleUI::printMainMenu() {
    std::cout <<
    "\n===== MAIN MENU =====\n"
    "1. CRUD Operations\n"
    "2. Filters\n"
    "3. Reports\n"
    "4. Budget & Savings\n"
    "5. Analytics\n"
    "6. Export\n"
    "0. Exit\n";
}

void ConsoleUI::printCrudMenu() {
    std::cout <<
    "\n--- CRUD ---\n"
    "1. Add Expense\n"
    "2. Update Expense\n"
    "3. Delete Expense\n"
    "4. View All\n"
    "0. Back\n";
}

void ConsoleUI::printFilterMenu() {
    std::cout <<
    "\n--- FILTERS ---\n"
    "1. By Category\n"
    "2. By Date Range\n"
    "3. By Amount Range\n"
    "0. Back\n";
}

void ConsoleUI::printReportMenu() {
    std::cout <<
    "\n--- REPORTS ---\n"
    "1. Total by Category\n"
    "2. Total by Month\n"
    "3. Current Budget State\n"
    "0. Back\n";
}

void ConsoleUI::printFinanceMenu() {
    std::cout <<
    "\n--- BUDGET & SAVINGS ---\n"
    "1. Change Salary\n"
    "2. Set Automatic Monthly Savings\n"
    "3. Add Fixed Bill\n"
    "4. Remove Fixed Bill\n"
    "5. Transfer to Savings Now\n"
    "6. Show Budget State\n"
    "7. Show Fixed Bills\n"
    "0. Back\n";
}

void ConsoleUI::printAnalyticsMenu() {
    std::cout <<
    "\n--- ANALYTICS ---\n"
    "1. Monthly Total\n"
    "2. Yearly Total\n"
    "3. Months to Afford\n"
    "4. Top Categories\n"
    "5. Graph\n"
    "0. Back\n";
}

void ConsoleUI::printExpenses(const std::vector<Expense>& expenses) {
    if (expenses.empty()) {
        std::cout << "No expenses.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& expense : expenses) {
        std::cout << expense.getId() << " | "
                  << expense.getAmount() << " | "
                  << expense.getCategory() << " | "
                  << expense.getDate() << " | "
                  << expense.getDescription() << "\n";
    }
}

void ConsoleUI::printState() const {
    const auto state = srv.getState();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Tracked month: " << state.getYear() << "-" << std::setw(2) << std::setfill('0')
              << state.getMonth() << std::setfill(' ') << "\n";
    std::cout << "Salary: " << state.getSalary() << "\n";
    std::cout << "Available now: " << state.getAvailable() << "\n";
    std::cout << "Savings balance: " << state.getSavings() << "\n";
    std::cout << "Automatic monthly savings: " << state.getMonthlySavingsContribution() << "\n";
    std::cout << "Transferred to savings this month: " << state.getTransferredToSavingsThisMonth() << "\n";
    std::cout << "Fixed bills total: " << state.getTotalFixedBills() << "\n";
    std::cout << "Spent this month: " << srv.totalSpentInMonth(state.getYear(), state.getMonth()) << "\n";
}

void ConsoleUI::printFixedBills() const {
    const auto& bills = srv.getState().getFixedBills();
    if (bills.empty()) {
        std::cout << "No fixed bills configured.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& [name, amount] : bills) {
        std::cout << "- " << name << ": " << amount << "\n";
    }
}

void ConsoleUI::run() {
    int year;
    int month;
    getDate(year, month);

    const auto state = srv.getState();
    if (state.getYear() == 0) {
        std::cout << "Welcome! Let's configure your monthly budget.\n";
        const double salary = readDouble("Enter your initial monthly salary: ");
        srv.initializeIfNeeded(year, month, salary);
        const double monthlySavings = readDouble("Enter automatic monthly savings amount (0 if none): ");
        srv.setMonthlySavingsContribution(monthlySavings);
    } else {
        srv.initializeIfNeeded(year, month, state.getSalary());
    }

    std::cout << "\nBudget loaded for " << year << "-" << std::setw(2) << std::setfill('0')
              << month << std::setfill(' ') << ".\n";

    while (true) {
        printMainMenu();
        const int opt = readInt("Choose option: ");

        if (opt == 0) {
            break;
        }

        try {
            if (opt == 1) {
                while (true) {
                    printCrudMenu();
                    const int c = readInt("Choose option: ");
                    if (c == 0) {
                        break;
                    }

                    if (c == 1) {
                        const int id = readInt("Enter id: ");
                        const double amount = readDouble("Enter amount: ");
                        const std::string category = readLine("Enter category: ");
                        const std::string date = readToken("Enter date (YYYY-MM-DD): ");
                        const std::string description = readLine("Enter description: ");
                        srv.addExpense(id, amount, category, date, description);
                        std::cout << "Expense added.\n";
                    } else if (c == 2) {
                        const int id = readInt("Enter id to update: ");
                        const double amount = readDouble("Enter new amount: ");
                        const std::string category = readLine("Enter new category: ");
                        const std::string date = readToken("Enter new date (YYYY-MM-DD): ");
                        const std::string description = readLine("Enter new description: ");
                        srv.updateExpense(id, amount, category, date, description);
                        std::cout << "Expense updated.\n";
                    } else if (c == 3) {
                        const int id = readInt("Enter id to delete: ");
                        srv.deleteExpense(id);
                        std::cout << "Expense deleted.\n";
                    } else if (c == 4) {
                        printExpenses(srv.getAll());
                    }
                }
            } else if (opt == 2) {
                while (true) {
                    printFilterMenu();
                    const int c = readInt("Choose option: ");
                    if (c == 0) {
                        break;
                    }

                    if (c == 1) {
                        const std::string category = readLine("Enter category: ");
                        printExpenses(srv.filterByCategory(category));
                    } else if (c == 2) {
                        const std::string startDate = readToken("Enter start date (YYYY-MM-DD): ");
                        const std::string endDate = readToken("Enter end date (YYYY-MM-DD): ");
                        printExpenses(srv.filterByDateRange(startDate, endDate));
                    } else if (c == 3) {
                        const double minAmount = readDouble("Enter min amount: ");
                        const double maxAmount = readDouble("Enter max amount: ");
                        printExpenses(srv.filterByAmountRange(minAmount, maxAmount));
                    }
                }
            } else if (opt == 3) {
                while (true) {
                    printReportMenu();
                    const int c = readInt("Choose option: ");
                    if (c == 0) {
                        break;
                    }

                    if (c == 1) {
                        for (const auto& [category, total] : srv.totalByCategory()) {
                            std::cout << category << " " << total << "\n";
                        }
                    } else if (c == 2) {
                        for (const auto& [monthKey, total] : srv.totalByMonth()) {
                            std::cout << monthKey << " " << total << "\n";
                        }
                    } else if (c == 3) {
                        printState();
                    }
                }
            } else if (opt == 4) {
                while (true) {
                    printFinanceMenu();
                    const int c = readInt("Choose option: ");
                    if (c == 0) {
                        break;
                    }

                    if (c == 1) {
                        const double salary = readDouble("Enter new salary: ");
                        srv.setMonthlySalary(salary);
                        std::cout << "Salary updated.\n";
                    } else if (c == 2) {
                        const double amount = readDouble("Enter automatic monthly savings amount: ");
                        srv.setMonthlySavingsContribution(amount);
                        std::cout << "Automatic savings updated.\n";
                    } else if (c == 3) {
                        const std::string name = readLine("Enter fixed bill name: ");
                        const double amount = readDouble("Enter fixed bill amount: ");
                        srv.addFixedBill(name, amount);
                        std::cout << "Fixed bill saved.\n";
                    } else if (c == 4) {
                        const std::string name = readLine("Enter fixed bill name to remove: ");
                        srv.removeFixedBill(name);
                        std::cout << "Fixed bill removed.\n";
                    } else if (c == 5) {
                        const double amount = readDouble("Enter amount to transfer to savings now: ");
                        srv.transferToSavings(amount);
                        std::cout << "Transfer completed.\n";
                    } else if (c == 6) {
                        printState();
                    } else if (c == 7) {
                        printFixedBills();
                    }
                }
            } else if (opt == 5) {
                while (true) {
                    printAnalyticsMenu();
                    const int c = readInt("Choose option: ");
                    if (c == 0) {
                        break;
                    }

                    if (c == 1) {
                        const int selectedYear = readInt("Enter year: ");
                        const int selectedMonth = readInt("Enter month: ");
                        std::cout << srv.totalSpentInMonth(selectedYear, selectedMonth) << "\n";
                    } else if (c == 2) {
                        const int selectedYear = readInt("Enter year: ");
                        std::cout << srv.totalSpentInYear(selectedYear) << "\n";
                    } else if (c == 3) {
                        const double target = readDouble("Enter target amount: ");
                        const int monthsToTarget = srv.monthsToAfford(target);
                        if (monthsToTarget < 0) {
                            std::cout << "Target cannot be reached with the current monthly setup.\n";
                        } else {
                            std::cout << "Estimated months needed: " << monthsToTarget << "\n";
                        }
                    } else if (c == 4) {
                        for (const auto& [category, total] : srv.topCategories()) {
                            std::cout << category << " " << total << "\n";
                        }
                    } else if (c == 5) {
                        for (const auto& [category, total] : srv.graphData()) {
                            std::cout << category << " | ";
                            for (int i = 0; i < static_cast<int>(total / 10); i++) {
                                std::cout << "#";
                            }
                            std::cout << "\n";
                        }
                    }
                }
            } else if (opt == 6) {
                const std::string filename = readToken("Enter filename: ");
                srv.exportCSV(filename);
                std::cout << "Export completed.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }
}
