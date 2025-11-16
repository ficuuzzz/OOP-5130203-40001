// Защита от множественного включения файла
// Если этот файл еще не включали, define PHONEBOOK_CONTACT_H
#ifndef PHONEBOOK_CONTACT_H
#define PHONEBOOK_CONTACT_H

#include <string>
#include <vector>

class Contact {
private:
    // Приватные поля
    // Внешний код не может напрямую менять эти значения
    std::string firstName;
    std::string lastName;
    std::string patronymic;
    std::string address;
    std::string dateOfBirth;
    std::string email;
    std::vector<std::string> phoneNumbers;

public:
    // Конструктор по умолчанию
    Contact();
    
    // Конструктор с параметрами
    Contact(const std::string& first, const std::string& last, 
            const std::string& patron, const std::string& addr,
            const std::string& dob, const std::string& mail);
    
    // Геттеры 
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getPatronymic() const;
    std::string getAddress() const;
    std::string getDateOfBirth() const;
    std::string getEmail() const;
    std::vector<std::string> getPhoneNumbers() const;
    
    // Сеттеры 
    void setFirstName(const std::string& first);
    void setLastName(const std::string& last);
    void setPatronymic(const std::string& patron);
    void setAddress(const std::string& addr);
    void setDateOfBirth(const std::string& dob);
    void setEmail(const std::string& mail);
    void addPhoneNumber(const std::string& phone);
    
    // Вспомогательные методы
    void print() const; // для отладки
    bool isValid() const; // проверка валидности всех полей
};

#endif // PHONEBOOK_CONTACT_H