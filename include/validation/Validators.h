#pragma once
#include <string>

namespace Validators {
    bool isValidDateYYYYMMDD(const std::string& date);
    void validateId(int id);
    void validateAmount(double amount);
    void validateCategory(const std::string& cat);
    void validateDescription(const std::string& desc);
}