#pragma once
#include <string>

class Expense {
private:
    std::string id;
    double amount{};
    std::string category;
    std::string date;        // YYYY-MM-DD
    std::string description;

public:
    Expense() = default;
    Expense(std::string id, double amount, std::string category, std::string date, std::string description);
    const std::string& getId() const;
    double getAmount() const;
    const std::string& getCategory() const;
    const std::string& getDate() const;
    const std::string& getDescription() const;

    void setAmount(double v);
    void setCategory(const std::string& v);
    void setDate(const std::string& v);
    void setDescription(const std::string& v);
};