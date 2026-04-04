#pragma once

#include "service/IExpenseService.h"
#include <vector>
#include "domain/Expense.h"

class ConsoleUI {
private:
    IExpenseService& srv;

    static void clearInput();
    static int readInt(const std::string&);
    static double readDouble(const std::string&);
    static std::string readToken(const std::string&);
    static std::string readLine(const std::string&);

    void printMainMenu();

    void printCrudMenu();
    void printFilterMenu();
    void printReportMenu();
    void printFinanceMenu();
    void printAnalyticsMenu();

    void printExpenses(const std::vector<Expense>& v);
    void printState() const;
    void printFixedBills() const;

public:
    explicit ConsoleUI(IExpenseService& s);

    void run();
};
