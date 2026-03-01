#include "ui/ConsoleUI.h"
#include <iostream>
#include <limits>

ConsoleUI::ConsoleUI(IExpenseService& srv) : srv(srv) {}

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ConsoleUI::printMenu() {
    std::cout << "\n=== Expense Tracker ===\n"
              << "1. Add expense\n"
              << "2. Update expense\n"
              << "3. Delete expense\n"
              << "4. List all\n"
              << "5. Filter by category\n"
              << "6. Filter by date range\n"
              << "7. Filter by amount range\n"
              << "8. Report: total by category\n"
              << "9. Report: total by month\n"
              << "0. Exit\n";
}

void ConsoleUI::printExpenses(const std::vector<Expense>& v) {
    if (v.empty()) { std::cout << "(no items)\n"; return; }
    for (const auto& e : v) {
        std::cout << "ID=" << e.getId()
                  << " | " << e.getAmount()
                  << " | " << e.getCategory()
                  << " | " << e.getDate()
                  << " | " << e.getDescription() << "\n";
    }
}

void ConsoleUI::run() {
    while (true) {
        printMenu();
        std::cout << "Choose: ";
        int cmd;
        if (!(std::cin >> cmd)) { clearInput(); continue; }
        clearInput();

        try {
            if (cmd == 0) break;

            if (cmd == 1) {
                int id; double amount;
                std::string cat, date, desc;

                std::cout << "id: "; std::cin >> id; clearInput();
                std::cout << "amount: "; std::cin >> amount; clearInput();
                std::cout << "category: "; std::getline(std::cin, cat);
                std::cout << "date (YYYY-MM-DD): "; std::getline(std::cin, date);
                std::cout << "description: "; std::getline(std::cin, desc);

                srv.addExpense(id, amount, cat, date, desc);
                std::cout << "Added.\n";
            } else if (cmd == 2) {
                int id; double amount;
                std::string cat, date, desc;

                std::cout << "id to update: "; std::cin >> id; clearInput();
                std::cout << "new amount: "; std::cin >> amount; clearInput();
                std::cout << "new category: "; std::getline(std::cin, cat);
                std::cout << "new date (YYYY-MM-DD): "; std::getline(std::cin, date);
                std::cout << "new description: "; std::getline(std::cin, desc);

                srv.updateExpense(id, amount, cat, date, desc);
                std::cout << "Updated.\n";
            } else if (cmd == 3) {
                int id;
                std::cout << "id to delete: "; std::cin >> id; clearInput();
                srv.deleteExpense(id);
                std::cout << "Deleted.\n";
            } else if (cmd == 4) {
                printExpenses(srv.getAll());
            } else if (cmd == 5) {
                std::string cat;
                std::cout << "category: "; std::getline(std::cin, cat);
                printExpenses(srv.filterByCategory(cat));
            } else if (cmd == 6) {
                std::string start, end;
                std::cout << "start date (YYYY-MM-DD): "; std::getline(std::cin, start);
                std::cout << "end date (YYYY-MM-DD): "; std::getline(std::cin, end);
                printExpenses(srv.filterByDateRange(start, end));
            } else if (cmd == 7) {
                double minA, maxA;
                std::cout << "min amount: "; std::cin >> minA; clearInput();
                std::cout << "max amount: "; std::cin >> maxA; clearInput();
                printExpenses(srv.filterByAmountRange(minA, maxA));
            } else if (cmd == 8) {
                auto m = srv.totalByCategory();
                for (const auto& [cat, sum] : m) std::cout << cat << " -> " << sum << "\n";
            } else if (cmd == 9) {
                auto m = srv.totalByMonth();
                for (const auto& [month, sum] : m) std::cout << month << " -> " << sum << "\n";
            } else {
                std::cout << "Unknown command.\n";
            }
        } catch (const std::exception& ex) {
            std::cout << "Error: " << ex.what() << "\n";
        }
    }
}