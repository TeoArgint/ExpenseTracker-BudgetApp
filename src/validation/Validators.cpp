#include "validation/Validators.h"
#include <stdexcept>
#include <cctype>

static bool isLeap(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

bool Validators::isValidDateYYYYMMDD(const std::string& date) {
    if (date.size() != 10) throw std::runtime_error("Date must be a string and exactly 10 digits.");
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) { if (date[i] != '-') throw std::runtime_error("There must be a '-' between the YYYY MM DD"); }
        else { if (!std::isdigit(static_cast<unsigned char>(date[i]))) throw std::runtime_error("Date must be a number."); }
    }
    int y = std::stoi(date.substr(0,4));
    int m = std::stoi(date.substr(5,2));
    int d = std::stoi(date.substr(8,2));
    if (y < 1900 || y > 3000) throw std::runtime_error("Year must be between 1900 and 3000");
    if (m < 1 || m > 12)  throw std::runtime_error("Month must be between 1 and 12");
//TODO  there needs to be a validator for day too but to keep in mind the days not all are 31 and in bisec year thre are 28-29!!!
    int mdays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int maxd = mdays[m];
    if (m == 2 && isLeap(y)) maxd = 29;
    return d >= 1 && d <= maxd;
}

void Validators::validateId(const std::string& id) {
    if (id.empty())
        throw std::runtime_error("Id cannot be empty.");

    if (!std::all_of(id.begin(), id.end(), ::isdigit))
        throw std::runtime_error("Id must contain only digits.");

    int numericId = std::stoi(id);

    if (numericId <= 0)
        throw std::runtime_error("Id must be positive.");

}

void Validators::validateAmount(double amount) {
    if (amount <= 0.0)
        throw std::runtime_error("Amount must be greater than 0.");
}

void Validators::validateCategory(const std::string& cat) {
    if (cat.empty()) throw std::runtime_error("Category cannot be empty.");
}

void Validators::validateDescription(const std::string& desc) {
    if (desc.empty()) throw std::runtime_error("Description cannot be empty.");
}