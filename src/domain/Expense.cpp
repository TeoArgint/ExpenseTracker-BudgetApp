#include "domain/Expense.h"

Expense::Expense(int id, double amount, std::string category,
                 std::string date, std::string description)
    : id(id), amount(amount), category(category),
      date(date), description(description) {}

int Expense::getId() const { return id; }
double Expense::getAmount() const { return amount; }
std::string Expense::getCategory() const { return category; }
std::string Expense::getDate() const { return date; }
std::string Expense::getDescription() const { return description; }