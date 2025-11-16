#include "../include/Contact.h"
#include "../include/Validator.h"
#include <iostream>
#include <vector>


using namespace std;
// Конструктор по умолчанию
Contact::Contact() 
    : firstName(""), lastName(""), patronymic(""), 
      address(""), dateOfBirth(""), email() {
    // Инициализируем все поля пустыми строками
    // phoneNumbers и так пустой по умолчанию
}

// Конструктор с параметрами
Contact::Contact(const string& first, const string& last, 
                 const string& patron, const string& addr,
                 const string& dob, const string& mail) {
    setFirstName(first);
    setLastName(last);
    setPatronymic(patron);
    setAddress(addr);
    setDateOfBirth(dob);
    setEmail(mail);
}


// Геттеры - просто возвращают значения
std::string Contact::getFirstName() const {
    return firstName;
}

std::string Contact::getLastName() const {
    return lastName;
}

std::string Contact::getPatronymic() const {
    return patronymic;
}

std::string Contact::getAddress() const {
    return address;
}

std::string Contact::getDateOfBirth() const {
    return dateOfBirth;
}

std::string Contact::getEmail() const {
    return email;
}

std::vector<std::string> Contact::getPhoneNumbers() const {
    return phoneNumbers;
}

// Сеттеры 
void Contact::setFirstName(const string& first) {
    ValidationResult result = Validator::validateName(first);
    if (!result.isValid) {
        throw invalid_argument("First name error: " + result.errorMessage);
    }
    firstName = Validator::trim(first);
}

void Contact::setLastName(const string& last) {
    ValidationResult result = Validator::validateName(last);
    if (!result.isValid) {
        throw invalid_argument("Last name error: " + result.errorMessage);
    }
    lastName = Validator::trim(last);
}

void Contact::setPatronymic(const string& patron) {
    if (!patron.empty()) {
        ValidationResult result = Validator::validateName(patron);
        if (!result.isValid) {
            throw invalid_argument("Patronymic error: " + result.errorMessage);
        }
        patronymic = Validator::trim(patron);
    } else {
        patronymic = "";
    }
}

void Contact::setAddress(const string& addr) {
    address = Validator::trim(addr);
}

void Contact::setDateOfBirth(const string& dob) {
    if (!dob.empty()) {
        ValidationResult result = Validator::validateDate(dob);
        if (!result.isValid) {
            throw invalid_argument("Date of birth error: " + result.errorMessage);
        }
        dateOfBirth = Validator::trim(dob);
    } else {
        dateOfBirth = "";
    }
}

void Contact::setEmail(const string& mail) {
    if (!mail.empty()) {
        ValidationResult result = Validator::validateEmail(mail);
        if (!result.isValid) {
            throw invalid_argument("Email error: " + result.errorMessage);
        }
        email = Validator::trim(mail);
    } else {
        email = "";
    }
}

void Contact::addPhoneNumber(const string& phone) {
    ValidationResult result = Validator::validatePhoneNumber(phone);
    if (!result.isValid) {
        throw invalid_argument("Phone number error: " + result.errorMessage);
    }
    phoneNumbers.push_back(Validator::trim(phone));
}


// Вывод информации о контакте (для отладки)
void Contact::print() const {
    std::cout << "Name: " << firstName << " " << lastName << " " << patronymic << std::endl;
    std::cout << "Address: " << address << std::endl;
    std::cout << "Birth Date: " << dateOfBirth << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "Phone Numbers: ";
    for (const auto& phone : phoneNumbers) {
        std::cout << phone << " ";
    }
    std::cout << std::endl << std::endl;
}

// Проверка валидности контакта
bool Contact::isValid() const {
    // Пока просто проверяем, что обязательные поля не пустые
    // Позже добавим полноценную валидацию
    return !firstName.empty() && !lastName.empty();
}