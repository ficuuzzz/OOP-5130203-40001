#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <regex>
#include <vector>

// Структура для детальной информации об ошибке
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
};

class Validator {
public:
    // Методы теперь возвращают детальную информацию
    static ValidationResult validateName(const std::string& name);
    static ValidationResult validatePhoneNumber(const std::string& phone);
    static ValidationResult validateEmail(const std::string& email);
    static ValidationResult validateDate(const std::string& date);
    
    // Вспомогательные методы
    static std::string trim(const std::string& str);

private:
    // Приватные регулярные выражения
    static const std::regex NAME_REGEX;
    static const std::regex PHONE_REGEX;
    static const std::regex EMAIL_REGEX;
    static const std::regex DATE_REGEX;
    
    // Вспомогательные приватные методы
    static bool isLeapYear(int year);
    static int daysInMonth(int month, int year);
};

#endif