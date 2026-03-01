#pragma once
#include "service/IExpenseService.h"

class ConsoleUI {
private:
    IExpenseService& srv;

    static void printMenu();
    static void printExpenses(const std::vector<Expense>& v);

public:
    explicit ConsoleUI(IExpenseService& srv);
    void run();
};