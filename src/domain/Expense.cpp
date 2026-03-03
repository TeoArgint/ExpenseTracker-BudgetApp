#include "domain/Expense.h"
#include <utility>

Expense::Expense(std::string id, double amount, std::string category, std::string date, std::string description)
    : id(id), amount(amount), category(std::move(category)), date(std::move(date)), description(std::move(description)) {}

const std::string &Expense::getId() const { return id; }
double Expense::getAmount() const { return amount; }
const std::string& Expense::getCategory() const { return category; }
const std::string& Expense::getDate() const { return date; }
const std::string& Expense::getDescription() const { return description; }

void Expense::setAmount(double v) { amount = v; }
void Expense::setCategory(const std::string& v) { category = v; }
void Expense::setDate(const std::string& v) { date = v; }
void Expense::setDescription(const std::string& v) { description = v; }