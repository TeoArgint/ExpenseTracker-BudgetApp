#pragma once
#include <string>

class Expense {
    int id;
    double amount;
    std::string category, date, description;

public:
    Expense() = default;
    Expense(int id, double amount, std::string category,
            std::string date, std::string description);

    int getId() const;
    double getAmount() const;
    std::string getCategory() const;
    std::string getDate() const;
    std::string getDescription() const;
};