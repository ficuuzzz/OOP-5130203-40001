#include "../include/Validator.h"
#include <iostream>
#include <algorithm>
#include <locale>
#include <ctime>

using namespace std;

// Инициализация статических регулярных выражений
const regex Validator::NAME_REGEX(R"(^[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ0-9\s\-]*[a-zA-Zа-яА-ЯёЁ0-9]$)");
const regex Validator::PHONE_REGEX(R"(^(\+7|8)[\s\-]?\(?\d{3}\)?[\s\-]?\d{3}[\s\-]?\d{2}[\s\-]?\d{2}$)");
const regex Validator::EMAIL_REGEX(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
const regex Validator::DATE_REGEX(R"(^(\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$)");

// Детальная проверка имени
ValidationResult Validator::validateName(const string& name) {
    setlocale(LC_ALL, "Russian");

    string cleaned = trim(name);

    if (cleaned.empty()) {
        return {false, "Name cannot be empty"};
    }
    
    if (cleaned.length() < 2) {
        return {false, "Name must be at least 2 characters long"};
    }
    
    // УПРОЩЕННАЯ ПРОВЕРКА ПЕРВОГО СИМВОЛА
    unsigned char firstChar = cleaned[0];
    bool isFirstLetter = ((firstChar >= 'A' && firstChar <= 'Z') ||
                         (firstChar >= 'a' && firstChar <= 'z') ||
                         (firstChar >= 128)); // русские и другие Unicode символы
    
    if (!isFirstLetter) {
        return {false, "Name must start with a letter"};
    }
    
    // Проверка последнего символа
    unsigned char lastChar = cleaned.back();
    bool isLastValid = isFirstLetter || 
                      (lastChar >= '0' && lastChar <= '9'); // или цифра
    
    if (!isLastValid || lastChar == '-') {
        return {false, "Name must end with a letter or digit"};
    }
    
    // Проверка на двойные дефисы и пробелы
    if (cleaned.find("--") != string::npos) {
        return {false, "Name cannot contain consecutive hyphens"};
    }
    
    if (cleaned.find("  ") != string::npos) {
        return {false, "Name cannot contain consecutive spaces"};
    }
    
    // УПРОЩЕННАЯ ПРОВЕРКА СИМВОЛОВ
    for (size_t i = 0; i < cleaned.length(); ++i) {
        unsigned char c = cleaned[i];
        
        // Разрешаем:
        bool isValid = ((c >= 'A' && c <= 'Z') ||
                       (c >= 'a' && c <= 'z') ||
                       (c >= '0' && c <= '9') ||
                       (c == ' ') || 
                       (c == '-') ||
                       (c >= 128)); // все русские и Unicode символы
        
        if (!isValid) {
            return {false, "Name contains invalid character"};
        }
        
        // Дополнительные проверки для специальных символов
        if (c == '-' || c == ' ') {
            // Дефис/пробел не может быть в начале или конце (уже проверили)
            // Не может быть два подряд (уже проверили)
            // Не может быть перед другим дефисом/пробелом
            if (i > 0 && (cleaned[i-1] == '-' || cleaned[i-1] == ' ')) {
                return {false, "Name cannot have consecutive spaces or hyphens"};
            }
        }
    }
    
    return {true, ""};
}

// Детальная проверка телефонного номера
ValidationResult Validator::validatePhoneNumber(const string& phone) {
    string trimmed = trim(phone);
    
    if (trimmed.empty()) {
        return {false, "Phone number cannot be empty"};
    }
    
    // Расширяем regex для поддержки большего количества форматов
    // Теперь поддерживаем: 
    // +7XXXXXXXXXX, 8XXXXXXXXXX, +7(XXX)XXXXXXX, 8(XXX)XXXXXXX и т.д.
    if (!regex_match(trimmed, PHONE_REGEX)) {
        return {false, "Phone number format is invalid. Examples: +79161234567, 8(912)123-45-67, 88005553535"};
    }
    
    // Удаляем все нецифровые символы для проверки длины
    string digitsOnly;
    for (char c : trimmed) {
        if (isdigit(c)) {
            digitsOnly += c;
        }
    }
    
    // Проверяем длину 
    if (digitsOnly.length() != 11) {
        return {false, "Phone number must contain exactly 11 digits"};
    }
    
    string operatorCode = digitsOnly.substr(1, 3); // берем код оператора (первые 3 цифры после 7 или 8)
    
    // Проверяем только что код оператора состоит из цифр
    bool allDigits = true;
    for (char c : operatorCode) {
        if (!isdigit(c)) {
            allDigits = false;
            break;
        }
    }
    
    if (!allDigits) {
        return {false, "Invalid operator code"};
    }
    
    return {true, ""};
}

// Детальная проверка email
ValidationResult Validator::validateEmail(const string& email) {
    string trimmed = trim(email);
    
    if (trimmed.empty()) {
        return {false, "Email cannot be empty"};
    }
    
    // Проверяем наличие @
    size_t atPos = trimmed.find('@');
    if (atPos == string::npos) {
        return {false, "Email must contain @ symbol"};
    }
    
    if (atPos == 0) {
        return {false, "Email must have username before @"};
    }
    
    if (atPos == trimmed.length() - 1) {
        return {false, "Email must have domain after @"};
    }
    
    // ДОПОЛНИТЕЛЬНАЯ ПРОВЕРКА: убедимся что нет пробелов вокруг @
    // Это специальная проверка, так как trim уже убрал пробелы по краям,
    // но если был пробел ВОКРУГ @, например "user @mail.com", 
    // то после trim получится "user@mail.com" - что валидно!
    // Но по условию задачи пробелы вокруг @ недопустимы.
    
    // Проверяем исходную строку на пробелы вокруг @
    size_t originalAtPos = email.find('@');
    if (originalAtPos != string::npos) {
        // Проверяем есть ли пробел перед @
        if (originalAtPos > 0 && isspace(email[originalAtPos - 1])) {
            return {false, "Email cannot have spaces before @ symbol"};
        }
        // Проверяем есть ли пробел после @
        if (originalAtPos < email.length() - 1 && isspace(email[originalAtPos + 1])) {
            return {false, "Email cannot have spaces after @ symbol"};
        }
    }
    
    // Проверяем регулярным выражением
    if (!regex_match(trimmed, EMAIL_REGEX)) {
        string username = trimmed.substr(0, atPos);
        string domain = trimmed.substr(atPos + 1);
        
        // Проверяем username
        for (char c : username) {
            if (!isalnum(c) && c != '.' && c != '_' && c != '%' && c != '+') {
                return {false, "Username contains invalid character: " + string(1, c)};
            }
        }
        
        // Проверяем domain
        if (domain.find('.') == string::npos) {
            return {false, "Domain must contain a dot (.)"};
        }
        
        return {false, "Email format is invalid. Example: user@example.com"};
    }
    
    return {true, ""};
}

// Детальная проверка даты
ValidationResult Validator::validateDate(const string& date) {
    string trimmed = trim(date);
    
    if (trimmed.empty()) {
        return {false, "Date cannot be empty"};
    }
    
    // Проверяем формат с помощью regex
    if (!regex_match(trimmed, DATE_REGEX)) {
        return {false, "Date format must be YYYY-MM-DD. Example: 1990-05-15"};
    }
    
    // Парсим дату
    int year, month, day;
    if (sscanf(trimmed.c_str(), "%d-%d-%d", &year, &month, &day) != 3) {
        return {false, "Failed to parse date"};
    }
    
    // Проверяем год
    if (year < 1900) {
        return {false, "Year must be 1900 or later"};
    }
    
    // Проверяем месяц
    if (month < 1 || month > 12) {
        return {false, "Month must be between 01 and 12"};
    }
    
    // Проверяем дни в месяце
    int maxDays = daysInMonth(month, year);
    if (day < 1 || day > maxDays) {
        return {false, "Day must be between 01 and " + to_string(maxDays) + " for this month/year"};
    }
    
    // Проверяем что дата не в будущем
    time_t now = time(0);
    tm* current_time = localtime(&now);
    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_day = current_time->tm_mday;
    
    if (year > current_year) {
        return {false, "Date cannot be in the future"};
    }
    if (year == current_year && month > current_month) {
        return {false, "Date cannot be in the future"};
    }
    if (year == current_year && month == current_month && day > current_day) {
        return {false, "Date cannot be in the future"};
    }
    
    return {true, ""};
}

string Validator::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool Validator::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Validator::daysInMonth(int month, int year) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 0;
    }
}
